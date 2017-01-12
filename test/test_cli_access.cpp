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

#include <sstream>
#include <string>

#include <boost/algorithm/string.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <decof/all.h>
#include <decof/client_context/generic_tcp_server.h>
#include <decof/cli/clisrv_context.h>

BOOST_AUTO_TEST_SUITE(cli_access)

using namespace decof;
namespace cli = decof::cli;
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

        // Connect with server and wait for prompt
        client_sock.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), conn_mgr.port()));
        io_service->poll();
        asio::read_until(client_sock, buf, std::string("\n> "));
        buf.consume(buf.size());
    }

    ~fixture()
    {
        client_sock.close();
    }

    std::shared_ptr<asio::io_service> io_service;
    boost::asio::ip::tcp::socket client_sock;

    object_dictionary od;
    generic_tcp_server<cli::clisrv_context> conn_mgr;

    asio::streambuf buf;
    std::istream is;
    std::string str;
};

BOOST_FIXTURE_TEST_CASE(change_userlevel, fixture)
{
    int userlevel = decof::Forbidden;
    managed_readonly_parameter<decof::boolean> dummy("dummy", &od, true);

    cli::cli_context_base::install_userlevel_callback([](const decof::client_context&, userlevel_t, const std::string&) {
        return true;
    });

    {
        // Read initial userlevel
        client_sock.write_some(asio::buffer(std::string("(param-ref 'ul)\n")));
        io_service->poll();

        asio::streambuf buf;
        buf.commit(asio::read_until(client_sock, buf, std::string("> ")));
        std::istream(&buf) >> userlevel;

        BOOST_REQUIRE_EQUAL(userlevel, decof::Normal);
    }

    {
        // Change userlevel to Internal
        int retval;
        std::stringstream out;
        out << "(exec 'change-ul " << decof::Internal << " \"passwd\")\n";

        client_sock.write_some(asio::buffer(out.str()));
        io_service->poll();

        asio::streambuf buf;
        buf.commit(asio::read_until(client_sock, buf, std::string("> ")));
        std::istream(&buf) >> retval;

        BOOST_REQUIRE_EQUAL(retval, 0);
    }

    {
        // Read userlevel after change
        client_sock.write_some(asio::buffer(std::string("(param-ref 'ul)\n")));
        io_service->poll();

        asio::streambuf buf;
        buf.commit(asio::read_until(client_sock, buf, std::string("> ")));
        std::istream(&buf) >> userlevel;

        BOOST_REQUIRE_EQUAL(userlevel, decof::Internal);
    }
}

BOOST_FIXTURE_TEST_CASE(omit_root_node_name, fixture)
{
    managed_readonly_parameter<decof::boolean> dummy("dummy", &od, true);
    client_sock.write_some(asio::buffer(std::string("get dummy\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "#t");
}

BOOST_FIXTURE_TEST_CASE(boolean_readonly, fixture)
{
    managed_readonly_parameter<decof::boolean> boolean_ro("boolean_ro", &od, true);
    client_sock.write_some(asio::buffer(std::string("get test:boolean_ro\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "#t");

    client_sock.write_some(asio::buffer(std::string("set test:boolean_ro #f\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(boolean_ro.value(), true);
}

BOOST_FIXTURE_TEST_CASE(boolean_readwrite, fixture)
{
    managed_readwrite_parameter<decof::boolean> boolean_rw("boolean_rw", &od, false);
    client_sock.write_some(asio::buffer(std::string("set test:boolean_rw #t\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(boolean_rw.value(), true);
}

BOOST_FIXTURE_TEST_CASE(integer_readonly, fixture)
{
    managed_readonly_parameter<decof::integer> integer_ro("integer_ro", &od, -42);
    client_sock.write_some(asio::buffer(std::string("get test:integer_ro\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "-42");

    client_sock.write_some(asio::buffer(std::string("set test:integer_ro 0\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(integer_ro.value(), -42);
}

BOOST_FIXTURE_TEST_CASE(integer_readwrite, fixture)
{
    managed_readwrite_parameter<decof::integer> integer_rw("integer_rw", &od, 0);
    client_sock.write_some(asio::buffer(std::string("set test:integer_rw -42\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(integer_rw.value(), -42);
}

BOOST_FIXTURE_TEST_CASE(real_readonly, fixture)
{
    managed_readonly_parameter<decof::real> real_ro("real_ro", &od, -0.123456);
    client_sock.write_some(asio::buffer(std::string("get test:real_ro\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "-0.123456");

    client_sock.write_some(asio::buffer(std::string("set test:real_ro 0\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(real_ro.value(), -0.123456);
}

BOOST_FIXTURE_TEST_CASE(real_readwrite, fixture)
{
    managed_readwrite_parameter<decof::real> real_rw("real_rw", &od, 0.0);
    client_sock.write_some(asio::buffer(std::string("set test:real_rw -0.123456\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(real_rw.value(), -0.123456);
}

BOOST_FIXTURE_TEST_CASE(string_readonly, fixture)
{
    managed_readonly_parameter<decof::string> string_ro("string_ro", &od, "decof");
    client_sock.write_some(asio::buffer(std::string("get test:string_ro\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "\"decof\"");

    client_sock.write_some(asio::buffer(std::string("set test:string_ro \"nope\"\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(string_ro.value(), "decof");
}

BOOST_FIXTURE_TEST_CASE(string_readwrite, fixture)
{
    managed_readwrite_parameter<decof::string> string_rw("string_rw", &od, "");
    client_sock.write_some(asio::buffer(std::string("set test:string_rw \"decof\"\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(string_rw.value(), "decof");
}

BOOST_FIXTURE_TEST_CASE(binary_readonly, fixture)
{
    managed_readonly_parameter<binary> binary_ro("binary_ro", &od, "decof");
    client_sock.write_some(asio::buffer(std::string("get test:binary_ro\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "&ZGVjb2Y=");

    client_sock.write_some(asio::buffer(std::string("set test:binary_ro &bm9wZQo=\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(binary_ro.value(), "decof");
}

BOOST_FIXTURE_TEST_CASE(binary_readwrite, fixture)
{
    managed_readwrite_parameter<binary> binary_rw("binary_rw", &od);
    client_sock.write_some(asio::buffer(std::string("set test:binary_rw &ZGVjb2Y=\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(binary_rw.value(), "decof");
}

BOOST_FIXTURE_TEST_CASE(boolean_seq_readonly, fixture)
{
    managed_readonly_parameter<decof::boolean_seq> boolean_seq_ro("boolean_seq_ro", &od, boolean_seq({ true, false, true, false }));
    client_sock.write_some(asio::buffer(std::string("get test:boolean_seq_ro\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "[#t,#f,#t,#f]");

    client_sock.write_some(asio::buffer(std::string("set test:boolean_seq_ro [#f,#t,#f,#t]\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const boolean_seq current = boolean_seq_ro.value();
    const boolean_seq expected = { true, false, true, false };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(boolean_seq_readwrite, fixture)
{
    managed_readwrite_parameter<decof::boolean_seq> boolean_seq_rw("boolean_seq_rw", &od, boolean_seq({ true, false, true, false }));
    client_sock.write_some(asio::buffer(std::string("set test:boolean_seq_rw [#f,#t,#f,#t]\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const boolean_seq current = boolean_seq_rw.value();
    const boolean_seq expected = { false, true, false, true };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(integer_seq_readonly, fixture)
{
    using integer_t = int32_t;
    using sequence_t = std::vector<integer_t>;

    managed_readonly_parameter<sequence_t> integer_seq_ro("integer_seq_ro", &od, sequence_t({
        std::numeric_limits<integer_t>::min(), std::numeric_limits<integer_t>::max()
    }));
    client_sock.write_some(asio::buffer(std::string("get test:integer_seq_ro\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "[-2147483648,2147483647]");

    client_sock.write_some(asio::buffer(std::string("set test:integer_seq_ro []\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const sequence_t current = integer_seq_ro.value();
    const sequence_t expected = {
        std::numeric_limits<integer_t>::min(), std::numeric_limits<integer_t>::max()
    };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(integer_seq_readwrite, fixture)
{
    using integer_t = int32_t;
    using sequence_t = std::vector<integer_t>;

    managed_readwrite_parameter<sequence_t> integer_seq_rw("integer_seq_rw", &od);
    client_sock.write_some(asio::buffer(std::string("set test:integer_seq_rw [-2147483648,2147483647]\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const sequence_t current = integer_seq_rw.value();
    const sequence_t expected = {
        std::numeric_limits<integer_t>::min(), std::numeric_limits<integer_t>::max()
    };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(real_seq_readonly, fixture)
{
    managed_readonly_parameter<decof::real_seq> real_seq_ro("real_seq_ro", &od, real_seq({ -1.23, 1.23 }));
    client_sock.write_some(asio::buffer(std::string("get test:real_seq_ro\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "[-1.23,1.23]");

    client_sock.write_some(asio::buffer(std::string("set test:real_seq_ro []\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const real_seq current = real_seq_ro.value();
    const real_seq expected = { -1.23, 1.23 };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(real_seq_readwrite, fixture)
{
    managed_readwrite_parameter<decof::real_seq> real_seq_rw("real_seq_rw", &od);
    client_sock.write_some(asio::buffer(std::string("set test:real_seq_rw [-1.23,1.23]\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const real_seq current = real_seq_rw.value();
    const real_seq expected = { -1.23, 1.23 };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(string_seq_readonly, fixture)
{
    managed_readonly_parameter<decof::string_seq> string_seq_ro("string_seq_ro", &od, string_seq({ "Hello", "World" }));
    client_sock.write_some(asio::buffer(std::string("get test:string_seq_ro\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "[\"Hello\",\"World\"]");

    client_sock.write_some(asio::buffer(std::string("set test:string_seq_ro []\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const string_seq current = string_seq_ro.value();
    const string_seq expected = { "Hello", "World" };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(string_seq_readwrite, fixture)
{
    managed_readwrite_parameter<decof::string_seq> string_seq_rw("string_seq_rw", &od);
    client_sock.write_some(asio::buffer(std::string("set test:string_seq_rw [\"Hello\",\"World\"]\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const string_seq current = string_seq_rw.value();
    const string_seq expected = { "Hello", "World" };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(binary_seq_readonly, fixture)
{
    managed_readonly_parameter<decof::binary_seq> binary_seq_ro("binary_seq_ro", &od, binary_seq({ "Hello", "World" }));
    client_sock.write_some(asio::buffer(std::string("get test:binary_seq_ro\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "[&SGVsbG8=,&V29ybGQ=]");

    client_sock.write_some(asio::buffer(std::string("set test:binary_seq_ro []\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const binary_seq current = binary_seq_ro.value();
    const binary_seq expected = { "Hello", "World" };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

BOOST_FIXTURE_TEST_CASE(binary_seq_readwrite, fixture)
{
    managed_readwrite_parameter<decof::binary_seq> binary_seq_rw("binary_seq_rw", &od);
    client_sock.write_some(asio::buffer(std::string("set test:binary_seq_rw [&SGVsbG8=,&V29ybGQ=]\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const binary_seq current = binary_seq_rw.value();
    const binary_seq expected = { "Hello", "World" };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        current.cbegin(),
        current.cend(),
        expected.cbegin(),
        expected.cend());
}

typedef decof::tuple<decof::boolean, decof::integer, decof::real, decof::string, decof::binary> full_tuple;

BOOST_FIXTURE_TEST_CASE(tuple_readonly, fixture)
{
    managed_readonly_parameter<full_tuple> tuple_ro("tuple_ro", &od, std::make_tuple(true, -1, -1.23, "Hello", "World"));
    client_sock.write_some(asio::buffer(std::string("get test:tuple_ro\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "{#t,-1,-1.23,\"Hello\",&V29ybGQ=}");

    client_sock.write_some(asio::buffer(std::string("set test:tuple_ro {#f,1,1.23,\"\",&}\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const full_tuple current = tuple_ro.value();
    const full_tuple expected = std::make_tuple(true, -1, -1.23, "Hello", "World");
    BOOST_REQUIRE(current == expected);
}

BOOST_FIXTURE_TEST_CASE(tuple_readwrite, fixture)
{
    managed_readwrite_parameter<full_tuple> tuple_rw("tuple_rw", &od);
    client_sock.write_some(asio::buffer(std::string("set test:tuple_rw {#t,-1,-1.23,\"Hello\",&V29ybGQ=}\n")));
    io_service->poll();
    asio::read_until(client_sock, buf, std::string("\n"));

    const full_tuple current = tuple_rw.value();
    const full_tuple expected = std::make_tuple(true, -1, -1.23, "Hello", "World");
    BOOST_REQUIRE(current == expected);
}

BOOST_AUTO_TEST_SUITE_END()
