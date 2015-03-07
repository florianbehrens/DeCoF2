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

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "decof.h"
#include "tcp_connection_manager.h"
#include "textproto/textproto_clisrv.h"

BOOST_AUTO_TEST_SUITE(parameter_access)

using namespace decof;
namespace asio = boost::asio;

struct fixture
{
    fixture() :
        od("test"),
        conn_mgr(od, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 0)),
        client_sock(*od.io_service().get()),
        is(&buf),
        scalars("scalars", &od)
    {
        // Setup server
        conn_mgr.preload<textproto_clisrv>();

        // Connect with server and wait for prompt
        client_sock.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), conn_mgr.port()));
        od.io_service()->poll();
        asio::read_until(client_sock, buf, std::string("\n> "));
//        od.io_service()->poll();
        buf.consume(buf.size());
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

    node scalars;
};

BOOST_FIXTURE_TEST_CASE(boolean_readonly, fixture)
{
    managed_readonly_parameter<decof::boolean> boolean_ro("boolean_ro", &scalars, true);
    client_sock.write_some(asio::buffer(std::string("get test:scalars:boolean_ro\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "#t");

    client_sock.write_some(asio::buffer(std::string("set test:scalars:boolean_ro #f\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(boolean_ro.value(), true);
}

BOOST_FIXTURE_TEST_CASE(boolean_readwrite, fixture)
{
    managed_readwrite_parameter<decof::boolean> boolean_rw("boolean_rw", &scalars, false);
    client_sock.write_some(asio::buffer(std::string("set test:scalars:boolean_rw #t\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(boolean_rw.value(), true);
}

BOOST_FIXTURE_TEST_CASE(integer_readonly, fixture)
{
    managed_readonly_parameter<decof::integer> integer_ro("integer_ro", &scalars, -42);
    client_sock.write_some(asio::buffer(std::string("get test:scalars:integer_ro\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "-42");

    client_sock.write_some(asio::buffer(std::string("set test:scalars:integer_ro 0\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(integer_ro.value(), -42);
}

BOOST_FIXTURE_TEST_CASE(integer_readwrite, fixture)
{
    managed_readwrite_parameter<decof::integer> integer_rw("integer_rw", &scalars, 0);
    client_sock.write_some(asio::buffer(std::string("set test:scalars:integer_rw -42\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(integer_rw.value(), -42);
}

BOOST_FIXTURE_TEST_CASE(real_readonly, fixture)
{
    managed_readonly_parameter<decof::real> real_ro("real_ro", &scalars, -0.123456);
    client_sock.write_some(asio::buffer(std::string("get test:scalars:real_ro\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "-0.123456");

    client_sock.write_some(asio::buffer(std::string("set test:scalars:real_ro 0\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(real_ro.value(), -0.123456);
}

BOOST_FIXTURE_TEST_CASE(real_readwrite, fixture)
{
    managed_readwrite_parameter<decof::real> real_rw("real_rw", &scalars, 0.0);
    client_sock.write_some(asio::buffer(std::string("set test:scalars:real_rw -0.123456\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(real_rw.value(), -0.123456);
}

BOOST_FIXTURE_TEST_CASE(string_readonly, fixture)
{
    managed_readonly_parameter<decof::string> string_ro("string_ro", &scalars, "decof");
    client_sock.write_some(asio::buffer(std::string("get test:scalars:string_ro\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "\"decof\"");

    client_sock.write_some(asio::buffer(std::string("set test:scalars:string_ro \"nope\"\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(string_ro.value(), "decof");
}

BOOST_FIXTURE_TEST_CASE(string_readwrite, fixture)
{
    managed_readwrite_parameter<decof::string> string_rw("string_rw", &scalars, "");
    client_sock.write_some(asio::buffer(std::string("set test:scalars:string_rw \"decof\"\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(string_rw.value(), "decof");
}

BOOST_FIXTURE_TEST_CASE(binary_readonly, fixture)
{
    managed_readonly_parameter<binary> binary_ro("binary_ro", &scalars, "decof");
    client_sock.write_some(asio::buffer(std::string("get test:scalars:binary_ro\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    std::getline(is, str);
    BOOST_REQUIRE_EQUAL(str, "&ZGVjb2Y=");

    client_sock.write_some(asio::buffer(std::string("set test:scalars:binary_ro &bm9wZQo=\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(binary_ro.value(), "decof");
}

BOOST_FIXTURE_TEST_CASE(binary_readwrite, fixture)
{
    managed_readwrite_parameter<binary> binary_rw("binary_rw", &scalars);
    client_sock.write_some(asio::buffer(std::string("set test:scalars:binary_rw &ZGVjb2Y=\n")));
    od.io_service()->poll();
    asio::read_until(client_sock, buf, std::string("\n"));
    BOOST_REQUIRE_EQUAL(binary_rw.value(), "decof");
}

BOOST_AUTO_TEST_SUITE_END()
