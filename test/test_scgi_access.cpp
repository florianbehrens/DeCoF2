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

#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>

#include "decof.h"
#include "tcp_connection_manager.h"
#include "scgi/context.h"

BOOST_AUTO_TEST_SUITE(scgi_access)

using namespace decof;
namespace asio = boost::asio;

struct fixture
{
    fixture() :
        od("test"),
        conn_mgr(od, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 0)),
        client_sock(*od.io_service().get()),
        is(&buf)
    {
        // Setup server
        conn_mgr.preload<scgi::scgi_context>();

        // Connect with server
        client_sock.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), conn_mgr.port()));
        od.io_service()->poll();
    }

    ~fixture()
    {
        client_sock.close();
    }

    object_dictionary od;
    tcp_connection_manager conn_mgr;
    boost::asio::ip::tcp::socket client_sock;

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
    od.io_service()->poll();

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
    od.io_service()->poll();

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
    od.io_service()->poll();

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
    od.io_service()->poll();

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
    od.io_service()->poll();

    // Read response header
    asio::read_until(client_sock, buf, std::string("\r\n"));
    std::getline(is, str, '\r');
    BOOST_REQUIRE_EQUAL(str, "HTTP/1.1 200 OK");
    asio::read_until(client_sock, buf, std::string("\r\n\r\n"));

    BOOST_REQUIRE_EQUAL(integer_rw.value(), -12345);
}

BOOST_AUTO_TEST_SUITE_END()
