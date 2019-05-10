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

#define BOOST_TEST_DYN_LINK

#include <decof/all.h>
#include <decof/client_context/generic_tcp_server.h>
#include <decof/cli/clisrv_context.h>

#include <boost/algorithm/string.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/test/unit_test.hpp>

#include <sstream>
#include <string>

BOOST_AUTO_TEST_SUITE(cli_access)

using namespace decof;
namespace asio = boost::asio;

struct fixture
{
    fixture() :
        io_service(),
        strand(io_service),
        client_sock(io_service),
        od("test"),
        server(od, strand, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 0)),
        is(&buf)
    {
        // Setup server
        server.preload();

        // Connect with server and wait for prompt
        client_sock.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), server.port()));
        io_service.poll();
        asio::read_until(client_sock, buf, std::string("\n> "));
        buf.consume(buf.size());
    }

    ~fixture()
    {
        client_sock.close();
    }

    asio::io_service io_service;
    asio::io_service::strand strand;
    boost::asio::ip::tcp::socket client_sock;

    object_dictionary od;
    generic_tcp_server<cli::clisrv_context> server;

    asio::streambuf buf;
    std::istream is;
    std::string str;
};

BOOST_FIXTURE_TEST_CASE(change_userlevel, fixture)
{
    int userlevel = Forbidden;
    managed_readonly_parameter<bool> dummy("dummy", &od, true);

    cli::cli_context_base::install_userlevel_callback([](const client_context&, userlevel_t, const std::string&) {
        return true;
    });

    {
        // Read initial userlevel
        client_sock.write_some(asio::buffer(std::string("(param-ref 'ul)\n")));
        io_service.poll();

        asio::streambuf buf;
        buf.commit(asio::read_until(client_sock, buf, std::string("> ")));
        std::istream(&buf) >> userlevel;

        BOOST_REQUIRE_EQUAL(userlevel, Normal);
    }

    {
        // Change userlevel to Internal
        int retval;
        std::stringstream out;
        out << "(exec 'change-ul " << Internal << " \"passwd\")\n";

        client_sock.write_some(asio::buffer(out.str()));
        io_service.poll();

        asio::streambuf buf;
        buf.commit(asio::read_until(client_sock, buf, std::string("> ")));
        std::istream(&buf) >> retval;

        BOOST_REQUIRE_EQUAL(retval, 0);
    }

    {
        // Read userlevel after change
        client_sock.write_some(asio::buffer(std::string("(param-ref 'ul)\n")));
        io_service.poll();

        asio::streambuf buf;
        buf.commit(asio::read_until(client_sock, buf, std::string("> ")));
        std::istream(&buf) >> userlevel;

        BOOST_REQUIRE_EQUAL(userlevel, Internal);
    }
}

BOOST_FIXTURE_TEST_CASE(omit_root_node_name, fixture)
{
    managed_readonly_parameter<bool> dummy("dummy", &od, true);
    client_sock.write_some(asio::buffer(std::string("get dummy\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "#t");
}

BOOST_FIXTURE_TEST_CASE(boolean_readonly, fixture)
{
    managed_readonly_parameter<bool> boolean_ro("boolean_ro", &od, true);
    client_sock.write_some(asio::buffer(std::string("get test:boolean_ro\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "#t");

    client_sock.write_some(asio::buffer(std::string("set test:boolean_ro #f\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(boolean_ro.value(), true);
}

BOOST_FIXTURE_TEST_CASE(boolean_readwrite, fixture)
{
    managed_readwrite_parameter<bool> boolean_rw("boolean_rw", &od, false);
    client_sock.write_some(asio::buffer(std::string("set test:boolean_rw #t\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(boolean_rw.value(), true);
}

BOOST_FIXTURE_TEST_CASE(integer_readonly, fixture)
{
    managed_readonly_parameter<int> integer_ro("integer_ro", &od, -42);
    client_sock.write_some(asio::buffer(std::string("get test:integer_ro\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "-42");

    client_sock.write_some(asio::buffer(std::string("set test:integer_ro 0\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(integer_ro.value(), -42);
}

BOOST_FIXTURE_TEST_CASE(integer_readwrite, fixture)
{
    managed_readwrite_parameter<int> integer_rw("integer_rw", &od, 0);
    client_sock.write_some(asio::buffer(std::string("set test:integer_rw -42\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(integer_rw.value(), -42);
}

BOOST_FIXTURE_TEST_CASE(real_readonly, fixture)
{
    managed_readonly_parameter<double> real_ro("real_ro", &od, -0.123456);
    client_sock.write_some(asio::buffer(std::string("get test:real_ro\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "-0.123456");

    client_sock.write_some(asio::buffer(std::string("set test:real_ro 0\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(real_ro.value(), -0.123456);
}

BOOST_FIXTURE_TEST_CASE(real_readwrite, fixture)
{
    managed_readwrite_parameter<double> real_rw("real_rw", &od, 0.0);
    client_sock.write_some(asio::buffer(std::string("set test:real_rw -0.123456\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(real_rw.value(), -0.123456);
}

BOOST_FIXTURE_TEST_CASE(string_readonly, fixture)
{
    managed_readonly_parameter<std::string> string_ro("string_ro", &od, "decof");
    client_sock.write_some(asio::buffer(std::string("get test:string_ro\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "\"decof\"");

    client_sock.write_some(asio::buffer(std::string("set test:string_ro \"nope\"\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(string_ro.value(), "decof");
}

BOOST_FIXTURE_TEST_CASE(string_readwrite, fixture)
{
    managed_readwrite_parameter<std::string> string_rw("string_rw", &od, "");
    client_sock.write_some(asio::buffer(std::string("set test:string_rw \"decof\"\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(string_rw.value(), "decof");
}

BOOST_FIXTURE_TEST_CASE(binary_readonly, fixture)
{
    managed_readonly_parameter<std::string, encoding_hint::binary> binary_ro("binary_ro", &od, "decof");
    client_sock.write_some(asio::buffer(std::string("get test:binary_ro\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "&ZGVjb2Y=");

    client_sock.write_some(asio::buffer(std::string("set test:binary_ro &bm9wZQo=\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(binary_ro.value(), "decof");
}

BOOST_FIXTURE_TEST_CASE(binary_readwrite, fixture)
{
    managed_readwrite_parameter<std::string, encoding_hint::binary> binary_rw("binary_rw", &od);
    client_sock.write_some(asio::buffer(std::string("set test:binary_rw &ZGVjb2Y=\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(binary_rw.value(), "decof");
}

BOOST_FIXTURE_TEST_CASE(boolean_seq_readonly, fixture)
{
    managed_readonly_parameter<std::vector<bool>> boolean_seq_ro("boolean_seq_ro", &od, { true, false, true, false });
    client_sock.write_some(asio::buffer(std::string("get test:boolean_seq_ro\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "[#t,#f,#t,#f]");

    client_sock.write_some(asio::buffer(std::string("set test:boolean_seq_ro [#f,#t,#f,#t]\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const std::vector<bool> current = boolean_seq_ro.value();
    const std::vector<bool> expected = { true, false, true, false };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(boolean_seq_readwrite, fixture)
{
    managed_readwrite_parameter<std::vector<bool>> boolean_seq_rw("boolean_seq_rw", &od, { true, false, true, false });
    client_sock.write_some(asio::buffer(std::string("set test:boolean_seq_rw [#f,#t,#f,#t]\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const std::vector<bool> current = boolean_seq_rw.value();
    const std::vector<bool> expected = { false, true, false, true };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(integer_seq_readonly, fixture)
{
    managed_readonly_parameter<std::vector<int>> integer_seq_ro("integer_seq_ro", &od, {
        std::numeric_limits<int>::min(), std::numeric_limits<int>::max()
    });

    client_sock.write_some(asio::buffer(std::string("get test:integer_seq_ro\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "[-2147483648,2147483647]");

    client_sock.write_some(asio::buffer(std::string("set test:integer_seq_ro []\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const std::vector<int> current = integer_seq_ro.value();
    const std::vector<int> expected = {
        std::numeric_limits<int>::min(), std::numeric_limits<int>::max()
    };

    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(integer_seq_readwrite, fixture)
{
    managed_readwrite_parameter<std::vector<int>> integer_seq_rw("integer_seq_rw", &od);

    client_sock.write_some(asio::buffer(std::string("set test:integer_seq_rw [-2147483648,2147483647]\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const std::vector<int> current = integer_seq_rw.value();
    const std::vector<int> expected = {
        std::numeric_limits<int>::min(), std::numeric_limits<int>::max()
    };

    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(real_seq_readonly, fixture)
{
    managed_readonly_parameter<std::vector<double>> real_seq_ro("real_seq_ro", &od, { -1.23, 1.23 });
    client_sock.write_some(asio::buffer(std::string("get test:real_seq_ro\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "[-1.23,1.23]");

    client_sock.write_some(asio::buffer(std::string("set test:real_seq_ro []\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const std::vector<double> current = real_seq_ro.value();
    const std::vector<double> expected = { -1.23, 1.23 };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(real_seq_readwrite, fixture)
{
    managed_readwrite_parameter<std::vector<double>> real_seq_rw("real_seq_rw", &od);
    client_sock.write_some(asio::buffer(std::string("set test:real_seq_rw [-1.23,1.23]\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const std::vector<double> current = real_seq_rw.value();
    const std::vector<double> expected = { -1.23, 1.23 };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(string_seq_readonly, fixture)
{
    managed_readonly_parameter<std::vector<std::string>> string_seq_ro("string_seq_ro", &od, { "Hello", "World" });
    client_sock.write_some(asio::buffer(std::string("get test:string_seq_ro\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "[\"Hello\",\"World\"]");

    client_sock.write_some(asio::buffer(std::string("set test:string_seq_ro []\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const std::vector<std::string> current = string_seq_ro.value();
    const std::vector<std::string> expected = { "Hello", "World" };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(string_seq_readwrite, fixture)
{
    managed_readwrite_parameter<std::vector<std::string>> string_seq_rw("string_seq_rw", &od);
    client_sock.write_some(asio::buffer(std::string("set test:string_seq_rw [\"Hello\",\"World\"]\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const std::vector<std::string> current = string_seq_rw.value();
    const std::vector<std::string> expected = { "Hello", "World" };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

typedef std::tuple<bool, int, double, std::string> full_tuple;

BOOST_FIXTURE_TEST_CASE(tuple_readonly, fixture)
{
    managed_readonly_parameter<full_tuple> tuple_ro("tuple_ro", &od, std::make_tuple(true, -1, -1.23, "Hello World"));
    client_sock.write_some(asio::buffer(std::string("get test:tuple_ro\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "{#t,-1,-1.23,\"Hello World\"}");

    client_sock.write_some(asio::buffer(std::string("set test:tuple_ro {#f,1,1.23,\"\"}\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const full_tuple current = tuple_ro.value();
    const full_tuple expected = std::make_tuple(true, -1, -1.23, "Hello World");
    BOOST_REQUIRE(current == expected);
}

BOOST_FIXTURE_TEST_CASE(tuple_readwrite, fixture)
{
    managed_readwrite_parameter<full_tuple> tuple_rw("tuple_rw", &od);
    client_sock.write_some(asio::buffer(std::string("set test:tuple_rw {#t,-1,-1.23,\"Hello World\"}\n")));
    io_service.poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const full_tuple current = tuple_rw.value();
    const full_tuple expected = std::make_tuple(true, -1, -1.23, "Hello World");
    BOOST_REQUIRE(current == expected);
}

BOOST_AUTO_TEST_SUITE_END()
