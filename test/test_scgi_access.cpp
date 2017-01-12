/*
 * Copyright (c) 2015 Florian Behrens
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/asio.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <decof/all.h>
#include <decof/client_context/generic_tcp_server.h>
#include <decof/scgi/scgi_context.h>

#include <scgi/bencode_string_parser.h>

BOOST_AUTO_TEST_SUITE(scgi_access)

using namespace decof;
namespace asio = boost::asio;

struct fixture
{
    fixture() :
        io_service(new asio::io_service()),
        client_sock(*io_service),
        od("test"),
        conn_mgr(od, io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 0)),
        is(&buf)
    {
        // Setup server
        conn_mgr.preload();

        // Connect with server
        client_sock.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), conn_mgr.port()));
        io_service->poll();
    }

    ~fixture()
    {
        client_sock.close();
    }

    std::shared_ptr<asio::io_service> io_service;
    asio::ip::tcp::socket client_sock;

    object_dictionary od;
    generic_tcp_server<scgi::scgi_context> conn_mgr;

    asio::streambuf buf;
    std::istream is;
    std::string str;
    std::stringstream ss;
};

struct scgi_request
{
    typedef std::map<std::string, std::string> attributes_type;

    scgi_request(std::initializer_list<attributes_type::value_type> il, std::string d = std::string()) :
        attributes(il),
        data(d)
    {}

    attributes_type attributes;
    std::string data;
};

std::ostream& operator<<(std::ostream &os, const scgi_request &r)
{
    // Sum up lengths of attributes
    int netstring_length = 0;
    for (const auto &elem : r.attributes)
        netstring_length += elem.first.size() + 1 + elem.second.size() + 1;

    // Output netstring
    os << netstring_length << ":";

    // Start with CONTENT_LENGTH, and SCPI
    os << "CONTENT_LENGTH" << '\0';
    try {
        os << r.attributes.at("CONTENT_LENGTH") << '\0';
    } catch (std::out_of_range&) {
        os << "0" << '\0';
    }
    os << "SCGI" << '\0';
    try {
        os << r.attributes.at("SCGI") << '\0';
    } catch (std::out_of_range&) {
        os << "1" << '\0';
    }

    for (const auto &elem : r.attributes) {
        if (elem.first != "CONTENT_LENGTH" && elem.first != "SCGI")
            os << elem.first << '\0' << elem.second << '\0';
    }
    os << "," << r.data << std::flush;

    return os;
}

struct test_event_type : public event
{
    using event::event;

    virtual void signal() {
        invoked = true;
    }

    bool invoked = false;
};

BOOST_FIXTURE_TEST_CASE(post_event, fixture)
{
    test_event_type test_event("event", &od);

    ss << scgi_request({
        { "CONTENT_LENGTH",         "0" },
        { "SCGI",                   "1" },
        { "REMOTE_PORT",            "12345" },
        { "REMOTE_ADDR",            "127.0.0.1" },
        { "REQUEST_URI",            "/test/event" },
        { "REQUEST_METHOD",         "POST" }
    });

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    BOOST_REQUIRE_EQUAL(test_event.invoked, true);
}

BOOST_FIXTURE_TEST_CASE(get_boolean, fixture)
{
    managed_readonly_parameter<decof::boolean> boolean_ro("boolean_ro", &od, true);

    ss << scgi_request({
        { "CONTENT_LENGTH",         "0" },
        { "SCGI",                   "1" },
        { "REMOTE_PORT",            "12345" },
        { "REMOTE_ADDR",            "127.0.0.1" },
        { "REQUEST_URI",            "/test/boolean_ro" },
        { "REQUEST_METHOD",         "GET" },
        { "CONTENT_TYPE",           "vnd/com.toptica.decof.boolean" }
    });

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    // Skip rest of header
    do {
        std::getline(is, str, '\n');
    } while (str != "\r");

    // Read response body
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "true");
}

BOOST_FIXTURE_TEST_CASE(put_boolean, fixture)
{
    managed_readwrite_parameter<decof::boolean> boolean_rw("boolean_rw", &od, true);

    ss << scgi_request(
        {
            { "CONTENT_LENGTH",         "5" },
            { "SCGI",                   "1" },
            { "REMOTE_PORT",            "12345" },
            { "REMOTE_ADDR",            "127.0.0.1" },
            { "REQUEST_URI",            "/test/boolean_rw" },
            { "REQUEST_METHOD",         "PUT" },
            { "CONTENT_TYPE",           "vnd/com.toptica.decof.boolean" }
        },
        "false"
    );

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    BOOST_REQUIRE_EQUAL(boolean_rw.value(), false);
}

BOOST_FIXTURE_TEST_CASE(get_integer, fixture)
{
    managed_readonly_parameter<decof::integer> integer_ro("integer_ro", &od, -12345);

    ss << scgi_request({
        { "CONTENT_LENGTH",         "0" },
        { "SCGI",                   "1" },
        { "REMOTE_PORT",            "12345" },
        { "REMOTE_ADDR",            "127.0.0.1" },
        { "REQUEST_URI",            "/test/integer_ro" },
        { "REQUEST_METHOD",         "GET" },
        { "CONTENT_TYPE",           "vnd/com.toptica.decof.integer" }
    });

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    // Skip rest of header
    do {
        std::getline(is, str, '\n');
    } while (str != "\r");

    // Read response body
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "-12345");
}

BOOST_FIXTURE_TEST_CASE(put_integer, fixture)
{
    managed_readwrite_parameter<decof::integer> integer_rw("integer_rw", &od, 0);

    ss << scgi_request(
        {
            { "CONTENT_LENGTH",         "6" },
            { "SCGI",                   "1" },
            { "REMOTE_PORT",            "12345" },
            { "REMOTE_ADDR",            "127.0.0.1" },
            { "REQUEST_URI",            "/test/integer_rw" },
            { "REQUEST_METHOD",         "PUT" },
            { "CONTENT_TYPE",           "vnd/com.toptica.decof.integer" }
        },
        "-12345"
    );

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));

    BOOST_REQUIRE_EQUAL(integer_rw.value(), -12345);
}

BOOST_FIXTURE_TEST_CASE(get_real, fixture)
{
    managed_readonly_parameter<decof::real> real_ro("real_ro", &od, -123.45);

    ss << scgi_request({
        { "CONTENT_LENGTH",         "0" },
        { "SCGI",                   "1" },
        { "REMOTE_PORT",            "12345" },
        { "REMOTE_ADDR",            "127.0.0.1" },
        { "REQUEST_URI",            "/test/real_ro" },
        { "REQUEST_METHOD",         "GET" },
        { "CONTENT_TYPE",           "vnd/com.toptica.decof.real" }
    });

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    // Skip rest of header
    do {
        std::getline(is, str, '\n');
    } while (str != "\r");

    // Read response body
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "-123.45");
}

BOOST_FIXTURE_TEST_CASE(put_real, fixture)
{
    managed_readwrite_parameter<decof::real> real_rw("real_rw", &od, 0);

    ss << scgi_request(
        {
            { "CONTENT_LENGTH",         "7" },
            { "SCGI",                   "1" },
            { "REMOTE_PORT",            "12345" },
            { "REMOTE_ADDR",            "127.0.0.1" },
            { "REQUEST_URI",            "/test/real_rw" },
            { "REQUEST_METHOD",         "PUT" },
            { "CONTENT_TYPE",           "vnd/com.toptica.decof.real" }
        },
        "-123.45"
    );

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));

    BOOST_REQUIRE_EQUAL(real_rw.value(), -123.45);
}

BOOST_FIXTURE_TEST_CASE(get_string, fixture)
{
    managed_readonly_parameter<decof::string> string_ro("string_ro", &od, "Hello\nWorld");

    ss << scgi_request({
        { "CONTENT_LENGTH",         "0" },
        { "SCGI",                   "1" },
        { "REMOTE_PORT",            "12345" },
        { "REMOTE_ADDR",            "127.0.0.1" },
        { "REQUEST_URI",            "/test/string_ro" },
        { "REQUEST_METHOD",         "GET" },
        { "CONTENT_TYPE",           "vnd/com.toptica.decof.string" }
    });

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    // Skip rest of header
    do {
        std::getline(is, str, '\n');
    } while (str != "\r");

    // Read response body
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "Hello\nWorld");
}

BOOST_FIXTURE_TEST_CASE(put_string, fixture)
{
    managed_readwrite_parameter<decof::string> string_rw("string_rw", &od);

    ss << scgi_request(
        {
            { "CONTENT_LENGTH",         "11" },
            { "SCGI",                   "1" },
            { "REMOTE_PORT",            "12345" },
            { "REMOTE_ADDR",            "127.0.0.1" },
            { "REQUEST_URI",            "/test/string_rw" },
            { "REQUEST_METHOD",         "PUT" },
            { "CONTENT_TYPE",           "vnd/com.toptica.decof.string" }
        },
        "Hello\nWorld"
    );

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));

    BOOST_REQUIRE_EQUAL(string_rw.value(), "Hello\nWorld");
}

BOOST_FIXTURE_TEST_CASE(get_binary, fixture)
{
    managed_readonly_parameter<decof::binary> binary_ro("binary_ro", &od, "Hello\nWorld");

    ss << scgi_request({
        { "CONTENT_LENGTH",         "0" },
        { "SCGI",                   "1" },
        { "REMOTE_PORT",            "12345" },
        { "REMOTE_ADDR",            "127.0.0.1" },
        { "REQUEST_URI",            "/test/binary_ro" },
        { "REQUEST_METHOD",         "GET" },
        { "CONTENT_TYPE",           "vnd/com.toptica.decof.binary" }
    });

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    // Skip rest of header
    do {
        std::getline(is, str, '\n');
    } while (str != "\r");

    // Read response body
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "Hello\nWorld");
}

BOOST_FIXTURE_TEST_CASE(put_binary, fixture)
{
    managed_readwrite_parameter<decof::binary> binary_rw("binary_rw", &od);

    ss << scgi_request(
        {
            { "CONTENT_LENGTH",         "11" },
            { "SCGI",                   "1" },
            { "REMOTE_PORT",            "12345" },
            { "REMOTE_ADDR",            "127.0.0.1" },
            { "REQUEST_URI",            "/test/binary_rw" },
            { "REQUEST_METHOD",         "PUT" },
            { "CONTENT_TYPE",           "vnd/com.toptica.decof.binary" }
        },
        "Hello\nWorld"
    );

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));

    BOOST_REQUIRE_EQUAL(binary_rw.value(), "Hello\nWorld");
}

BOOST_FIXTURE_TEST_CASE(get_boolean_seq, fixture)
{
    managed_readonly_parameter<decof::boolean_seq> boolean_seq_ro("boolean_seq_ro", &od, decof::boolean_seq{ true, false, true });

    ss << scgi_request({
        { "CONTENT_LENGTH",         "0" },
        { "SCGI",                   "1" },
        { "REMOTE_PORT",            "12345" },
        { "REMOTE_ADDR",            "127.0.0.1" },
        { "REQUEST_URI",            "/test/boolean_seq_ro" },
        { "REQUEST_METHOD",         "GET" },
        { "CONTENT_TYPE",           "vnd/com.toptica.decof.boolean_seq" }
    });

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    // Skip rest of header
    do {
        std::getline(is, str, '\n');
    } while (str != "\r");

    // Read response body
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, std::string("\x01\x00\x01", 3));
}

BOOST_FIXTURE_TEST_CASE(put_boolean_seq, fixture)
{
    managed_readwrite_parameter<decof::boolean_seq> boolean_seq_rw("boolean_seq_rw", &od);

    ss << scgi_request(
        {
            { "CONTENT_LENGTH",         "3" },
            { "SCGI",                   "1" },
            { "REMOTE_PORT",            "12345" },
            { "REMOTE_ADDR",            "127.0.0.1" },
            { "REQUEST_URI",            "/test/boolean_seq_rw" },
            { "REQUEST_METHOD",         "PUT" },
            { "CONTENT_TYPE",           "vnd/com.toptica.decof.boolean_seq" }
        },
        { "\x01\x00\x01", 3 }
    );

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    decof::boolean_seq nominal{ true, false, true };
    decof::boolean_seq actual = boolean_seq_rw.value();
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        actual.cbegin(),
        actual.cend(),
        nominal.cbegin(),
        nominal.cend());
}

BOOST_FIXTURE_TEST_CASE(get_integer_seq, fixture)
{
    using integer_t = int32_t;
    using sequence_t = std::vector<integer_t>;

    managed_readonly_parameter<sequence_t> integer_seq_ro("integer_seq_ro", &od, sequence_t{
        std::numeric_limits<integer_t>::max(),
        0,
        std::numeric_limits<integer_t>::min()
    });

    ss << scgi_request({
        { "CONTENT_LENGTH",         "0" },
        { "SCGI",                   "1" },
        { "REMOTE_PORT",            "12345" },
        { "REMOTE_ADDR",            "127.0.0.1" },
        { "REQUEST_URI",            "/test/integer_seq_ro" },
        { "REQUEST_METHOD",         "GET" },
        { "CONTENT_TYPE",           "vnd/com.toptica.decof.integer_seq" }
    });

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    // Skip rest of header
    do {
        std::getline(is, str, '\n');
    } while (str != "\r");

    // Read response body
    std::getline(is, str, '\r');

    auto actual = reinterpret_cast<const std::int32_t*>(str.data());
    auto size = integer_seq_ro.value().size();
    auto nominal = integer_seq_ro.value();
    BOOST_REQUIRE_EQUAL(str.length(), size * sizeof(std::int32_t));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        nominal.cbegin(),
        nominal.cend(),
        actual,
        actual + size);
}

BOOST_FIXTURE_TEST_CASE(put_integer_seq, fixture)
{
    managed_readwrite_parameter<decof::integer_seq> integer_seq_rw("integer_seq_rw", &od);

    decof::integer data[] = {
        std::numeric_limits<decof::integer>::max(),
        0,
        std::numeric_limits<decof::integer>::min()
    };

    ss << scgi_request(
        {
            { "CONTENT_LENGTH",         std::to_string(sizeof(data)) },
            { "SCGI",                   "1" },
            { "REMOTE_PORT",            "12345" },
            { "REMOTE_ADDR",            "127.0.0.1" },
            { "REQUEST_URI",            "/test/integer_seq_rw" },
            { "REQUEST_METHOD",         "PUT" },
            { "CONTENT_TYPE",           "vnd/com.toptica.decof.integer_seq" }
        },
        { reinterpret_cast<char *>(data), sizeof(data) }
    );

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    decof::integer_seq actual = integer_seq_rw.value();
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        data,
        data + sizeof(data) / sizeof(data[0]),
        actual.cbegin(),
        actual.cend());
}

BOOST_FIXTURE_TEST_CASE(get_real_seq, fixture)
{
    managed_readonly_parameter<decof::real_seq> real_seq_ro("real_seq_ro", &od, decof::real_seq{
        std::numeric_limits<decof::real>::max(),
        0,
        std::numeric_limits<decof::real>::lowest()
    });

    ss << scgi_request({
        { "CONTENT_LENGTH",         "0" },
        { "SCGI",                   "1" },
        { "REMOTE_PORT",            "12345" },
        { "REMOTE_ADDR",            "127.0.0.1" },
        { "REQUEST_URI",            "/test/real_seq_ro" },
        { "REQUEST_METHOD",         "GET" },
        { "CONTENT_TYPE",           "vnd/com.toptica.decof.real_seq" }
    });

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    // Skip rest of header
    do {
        std::getline(is, str, '\n');
    } while (str != "\r");

    // Read response body
    std::getline(is, str, '\r');

    auto actual = reinterpret_cast<const double*>(str.data());
    auto size = real_seq_ro.value().size();
    auto nominal = real_seq_ro.value();
    BOOST_REQUIRE_EQUAL(str.length(), size * sizeof(double));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        nominal.cbegin(),
        nominal.cend(),
        actual,
        actual + size);
}

BOOST_FIXTURE_TEST_CASE(put_real_seq, fixture)
{
    managed_readwrite_parameter<decof::real_seq> real_seq_rw("real_seq_rw", &od);

    double data[] = {
        std::numeric_limits<decof::real>::max(),
        0,
        std::numeric_limits<decof::real>::lowest()
    };

    ss << scgi_request(
        {
            { "CONTENT_LENGTH",         std::to_string(sizeof(data)) },
            { "SCGI",                   "1" },
            { "REMOTE_PORT",            "12345" },
            { "REMOTE_ADDR",            "127.0.0.1" },
            { "REQUEST_URI",            "/test/real_seq_rw" },
            { "REQUEST_METHOD",         "PUT" },
            { "CONTENT_TYPE",           "vnd/com.toptica.decof.real_seq" }
        },
        { reinterpret_cast<char *>(data), sizeof(data) }
    );

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    decof::real_seq actual = real_seq_rw.value();
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        data,
        data + sizeof(data) / sizeof(data[0]),
        actual.cbegin(),
        actual.cend());
}

BOOST_FIXTURE_TEST_CASE(get_string_seq, fixture)
{
    managed_readonly_parameter<decof::string_seq> string_seq_ro("string_seq_ro", &od, decof::string_seq{
        "Line1\r\n",
        "Line2"
    });

    ss << scgi_request({
        { "CONTENT_LENGTH",         "0" },
        { "SCGI",                   "1" },
        { "REMOTE_PORT",            "12345" },
        { "REMOTE_ADDR",            "127.0.0.1" },
        { "REQUEST_URI",            "/test/string_seq_ro" },
        { "REQUEST_METHOD",         "GET" },
        { "CONTENT_TYPE",           "vnd/com.toptica.decof.string_seq" }
    });

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    // Skip rest of header
    do {
        std::getline(is, str, '\n');
    } while (str != "\r");

    // Read and parse response body
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::vector<std::string> actual;
    for (std::istreambuf_iterator<char> it(&buf); it != std::istreambuf_iterator<char>(); ) {
        decof::scgi::bencode_string_parser parser;
        decof::scgi::bencode_string_parser::result_type result;
        std::tie(result, it) = parser.parse(it, std::istreambuf_iterator<char>());
        BOOST_REQUIRE_EQUAL(result, decof::scgi::bencode_string_parser::good);
        actual.emplace_back(std::move(parser.data));

        // Read CR+LF
        BOOST_REQUIRE_EQUAL(*it++, '\r');
        BOOST_REQUIRE_EQUAL(*it++, '\n');
    }

    const auto &nominal = string_seq_ro.value();
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        nominal.cbegin(),
        nominal.cend(),
        actual.cbegin(),
        actual.cend());
}

BOOST_FIXTURE_TEST_CASE(put_string_seq, fixture)
{
    managed_readwrite_parameter<decof::string_seq> string_seq_rw("string_seq_rw", &od);

    ss << scgi_request(
        {
            { "CONTENT_LENGTH",         "18" },
            { "SCGI",                   "1" },
            { "REMOTE_PORT",            "12345" },
            { "REMOTE_ADDR",            "127.0.0.1" },
            { "REQUEST_URI",            "/test/string_seq_rw" },
            { "REQUEST_METHOD",         "PUT" },
            { "CONTENT_TYPE",           "vnd/com.toptica.decof.string_seq" }
        },
        { "7:Line1\r\n\r\n5:Line2" }
    );

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    decof::string_seq actual = string_seq_rw.value();
    std::string nominal[] = { "Line1\r\n", "Line2" };

    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        nominal,
        nominal + sizeof(nominal) / sizeof(nominal[0]),
        actual.cbegin(),
        actual.cend());
}

BOOST_FIXTURE_TEST_CASE(get_binary_seq, fixture)
{
    managed_readonly_parameter<decof::binary_seq> binary_seq_ro("binary_seq_ro", &od, decof::binary_seq{
        "Line1\r\n",
        "Line2"
    });

    ss << scgi_request({
        { "CONTENT_LENGTH",         "0" },
        { "SCGI",                   "1" },
        { "REMOTE_PORT",            "12345" },
        { "REMOTE_ADDR",            "127.0.0.1" },
        { "REQUEST_URI",            "/test/binary_seq_ro" },
        { "REQUEST_METHOD",         "GET" },
        { "CONTENT_TYPE",           "vnd/com.toptica.decof.binary_seq" }
    });

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    // Skip rest of header
    do {
        std::getline(is, str, '\n');
    } while (str != "\r");

    // Read and parse response body
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::vector<std::string> actual;
    for (std::istreambuf_iterator<char> it(&buf); it != std::istreambuf_iterator<char>(); ) {
        decof::scgi::bencode_string_parser parser;
        decof::scgi::bencode_string_parser::result_type result;
        std::tie(result, it) = parser.parse(it, std::istreambuf_iterator<char>());
        BOOST_REQUIRE_EQUAL(result, decof::scgi::bencode_string_parser::good);
        actual.emplace_back(std::move(parser.data));

        // Read CR+LF
        BOOST_REQUIRE_EQUAL(*it++, '\r');
        BOOST_REQUIRE_EQUAL(*it++, '\n');
    }

    const auto &nominal = binary_seq_ro.value();
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        nominal.cbegin(),
        nominal.cend(),
        actual.cbegin(),
        actual.cend());
}

BOOST_FIXTURE_TEST_CASE(put_binary_seq, fixture)
{
    managed_readwrite_parameter<decof::binary_seq> binary_seq_rw("binary_seq_rw", &od);

    ss << scgi_request(
        {
            { "CONTENT_LENGTH",         "18" },
            { "SCGI",                   "1" },
            { "REMOTE_PORT",            "12345" },
            { "REMOTE_ADDR",            "127.0.0.1" },
            { "REQUEST_URI",            "/test/binary_seq_rw" },
            { "REQUEST_METHOD",         "PUT" },
            { "CONTENT_TYPE",           "vnd/com.toptica.decof.binary_seq" }
        },
        { "7:Line1\r\n\r\n5:Line2" }
    );

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");

    decof::binary_seq actual = binary_seq_rw.value();
    std::string nominal[] = { "Line1\r\n", "Line2" };

    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        nominal,
        nominal + sizeof(nominal) / sizeof(nominal[0]),
        actual.cbegin(),
        actual.cend());
}

BOOST_FIXTURE_TEST_CASE(browse, fixture)
{
    ss << scgi_request({
        { "CONTENT_LENGTH",         "0" },
        { "SCGI",                   "1" },
        { "REMOTE_PORT",            "12345" },
        { "REMOTE_ADDR",            "127.0.0.1" },
        { "REQUEST_URI",            "/browse" },
        { "REQUEST_METHOD",         "GET" }
    });

    client_sock.write_some(asio::buffer(ss.str()));
    io_service->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));
    std::getline(is, str, '\n');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK\r");

    // Read rest of header
    std::unordered_map<std::string, std::string> headers;
    while (true) {
        std::vector<std::string> header;
        std::getline(is, str, '\n');
        std::string trimmed_str = boost::trim_copy(str);

        if (trimmed_str.empty())
            break;

        boost::split(header, trimmed_str, boost::is_any_of(": "), boost::token_compress_on);
        headers[header[0]] = header[1];
    }

    BOOST_REQUIRE_EQUAL(headers["Content-Type"], "text/xml");
    BOOST_REQUIRE_GT(std::stoi(headers["Content-Length"]), 0);
}

BOOST_AUTO_TEST_SUITE_END()
