/*
 * Copyright (c) 2017 Florian Behrens
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

#include "application.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <thread>

#include <boost/asio/steady_timer.hpp>
#include <boost/bind.hpp>

#include <decof/all.h>
#include <decof/asio_tick/asio_tick.h>
#include <decof/cli/clisrv_context.h>
#include <decof/cli/pubsub_context.h>
#include <decof/client_context/generic_tcp_server.h>
#include <decof/scgi/scgi_context.h>

#include "application.h"
#include "background_worker.h"
#include "composite.h"

namespace
{

DECOF_DECLARE_MANAGED_READWRITE_PARAMETER(my_managed_readwrite_parameter, decof::string);
DECOF_DECLARE_MANAGED_READONLY_PARAMETER(my_managed_readonly_parameter, decof::string);
DECOF_DECLARE_EXTERNAL_READONLY_PARAMETER(current_context_endpoint_parameter, decof::string);
DECOF_DECLARE_EXTERNAL_READONLY_PARAMETER(time_parameter, decof::string);
DECOF_DECLARE_EVENT(exit_event);
DECOF_DECLARE_WRITEONLY_PARAMETER(cout_parameter, decof::string);
typedef decof::tuple<decof::boolean, decof::integer, decof::real, decof::string, decof::binary> cout_tuple_parameter_type;
DECOF_DECLARE_WRITEONLY_PARAMETER(cout_tuple_parameter, cout_tuple_parameter_type);

struct spin_count_parameter : public decof::managed_readonly_parameter<decof::integer>
{
    spin_count_parameter(decof::node *parent) :
        decof::managed_readonly_parameter<decof::integer>("spin-count", parent, 0)
    {
        application::instance().strand().post(std::bind(&spin_count_parameter::increment, this));
    }

    void increment() {
        value(value() + 1);
        application::instance().strand().post(std::bind(&spin_count_parameter::increment, this));
    }
};

void my_managed_readwrite_parameter::verify(const decof::string& value)
{
    if (value != "true" && value != "false")
        throw decof::invalid_value_error();

    std::cout << "my_managed_readwrite_parameter was changed to " << value << std::endl;
}

current_context_endpoint_parameter::value_type current_context_endpoint_parameter::external_value() const
{
    const std::shared_ptr<decof::client_context> cc = get_object_dictionary()->current_context();
    if (cc != nullptr)
        return cc->connection_type() + "://" + cc->remote_endpoint();

    return "Unknown";
}

decof::string time_parameter::external_value() const
{
    const size_t max_length = 40;
    char str[max_length];
    std::time_t now = std::time(nullptr);
    std::strftime(str, sizeof(str), "%c", std::localtime(&now));
    return decof::string(str);
}

void exit_event::signal()
{
    application::instance().strand().get_io_service().stop();
}

void cout_parameter::value(const decof::string &value)
{
    std::cout << value << std::endl;
}

void cout_tuple_parameter::value(const cout_tuple_parameter_type &value)
{
    std::cout << "Boolean value: " << std::get<0>(value) << std::endl
              << "Integer value: " << std::get<1>(value) << std::endl
              << "Real value: " << std::get<2>(value) << std::endl
              << "String value: " << std::get<3>(value) << std::endl
              << "Binary value: " << std::get<4>(value) << std::endl;
}

struct ip_address_parameter : public decof::external_readwrite_parameter<decof::string>
{
    ip_address_parameter(std::string name, decof::node *parent = nullptr)
     : decof::external_readwrite_parameter<decof::string>(name, parent)
    {}

private:
    bool external_value(const value_type &value) override {
        std::fstream file(filename_, std::ios_base::out | std::ios_base::trunc);
        if (file.rdstate() & std::ios_base::failbit)
            return false;

        file << value;
        return true;
    }

    decof::string external_value() const override {
        decof::string str;
        std::fstream file_(filename_, std::ios_base::in);
        std::getline(file_, str);
        return str;
    }

    std::string filename_ = "C:\\Users\\willy\\data.txt";
};

// Setup object dictionary
// root
//  |-- enabled: string (rw)
//  |-- spin-count: int (r)
//  |-- current-context: node (r)
//  | |-- endpoint: string (r)
//  |-- subnode: node (r)
//  | |-- time: string (r)
//  | |-- leaf2: string_vector (rw)
//  | |-- ip-address: string (rw)
//  | |-- boolean: bool (rw)
//  | |-- integer: int (rw)
//  | |-- double: double (rw)
//  | |-- string: string (rw)
//  | |-- binary: binary (rw)
//  | |-- boolean_seq: boolean_seq (rw)
//  | |-- integer_seq: integer_seq (rw)
//  | |-- real_seq: real_seq (rw)
//  | |-- string_seq: string_seq (rw)
//  | |-- binary_seq: binary_seq (rw)
//  |-- tuples: node (r)
//  | |-- tuple2: tuple<boolean, integer> (ro)
//  |-- events: node (r)
//  | |-- exit: event
//  |-- writeonly: node (r)
//  | |-- string: string (w)
//  | |-- tuple: tuple (w)
//  |-- composite
//  | |-- summand1
//  | |-- summand2
//  | |-- sum

decof::object_dictionary obj_dict("example");
my_managed_readwrite_parameter enable_param("enabled", &obj_dict, "false");
spin_count_parameter spin_count_param(&obj_dict);
decof::node current_context_node("current-context", &obj_dict);
current_context_endpoint_parameter endpoint_param("endpoint", &current_context_node);
decof::node subnode("subnode", &obj_dict);
time_parameter time_param("time", &subnode);
decof::string_seq sl = { "value1", "value2", "value3" };
decof::managed_readwrite_parameter<decof::string_seq> leaf2_param("leaf2", &subnode, decof::Normal, decof::Normal, sl);
ip_address_parameter ipo_address_param("ip-address", &subnode);
decof::managed_readwrite_parameter<decof::boolean> boolean_param("boolean", &subnode);
decof::managed_readwrite_parameter<decof::integer> integer_param("integer", &subnode);
decof::managed_readwrite_parameter<decof::real> real_param("real", &subnode);
decof::managed_readwrite_parameter<decof::string> string_param("string", &subnode);
decof::managed_readwrite_parameter<decof::binary> binary_param("binary", &subnode);
decof::managed_readwrite_parameter<decof::boolean_seq> boolean_seq_param("boolean_seq", &subnode);
decof::managed_readwrite_parameter<decof::integer_seq> integer_seq_param("integer_seq", &subnode);
decof::managed_readwrite_parameter<decof::real_seq> real_seq_param("real_seq", &subnode);
decof::managed_readwrite_parameter<decof::string_seq> string_seq_param("string_seq", &subnode);
decof::managed_readwrite_parameter<decof::binary_seq> binary_seq_param("binary_seq", &subnode);
decof::node tuples_node("tuples", &obj_dict);
decof::managed_readwrite_parameter<decof::tuple<decof::boolean, decof::integer, decof::real, decof::string, decof::binary>> scalar_tuple("scalar_tuple", &tuples_node);
decof::node events_node("events", &obj_dict);
exit_event exit_ev("exit", &events_node);
decof::node writeonly_node("writeonly", &obj_dict);
cout_parameter cout_param("string", &writeonly_node);
cout_tuple_parameter cout_tuple_param("tuple", &writeonly_node);
composite comp("composite", &obj_dict);
auto background_work = background_worker::create("background_work", &obj_dict);

} // Anonymous namespace

application&application::instance()
{
    static application inst;
    return inst;
}

int application::run(int, char*[])
{
    // Install userlevel change callback
    using decof::cli::cli_context_base;
    cli_context_base::install_userlevel_callback([](const decof::client_context&, decof::userlevel_t userlevel, const std::string& passwd) {
        if (userlevel == decof::Normal || userlevel == decof::Readonly)
            return true;
        else if (userlevel == decof::Maintenance && passwd == "maintenance")
            return true;
        else if (userlevel == decof::Service && passwd == "service")
            return true;
        else if (userlevel == decof::Internal && passwd == "internal")
            return true;
        return false;
    });

    // Setup request/respone CLI context
    boost::asio::ip::tcp::endpoint cmd_endpoint(boost::asio::ip::tcp::v4(), 1998);
    decof::generic_tcp_server<decof::cli::clisrv_context> conn_mgr_cmd(obj_dict, strand_, cmd_endpoint);
    conn_mgr_cmd.preload();

    // Setup publish/subscribe CLI context
    boost::asio::ip::tcp::endpoint mon_endpoint(boost::asio::ip::tcp::v4(), 1999);
    decof::generic_tcp_server<decof::cli::pubsub_context> conn_mgr_mon(obj_dict, strand_, mon_endpoint);
    conn_mgr_mon.preload();

    // Setup SCGI context
    boost::asio::ip::tcp::endpoint scgi_endpoint(boost::asio::ip::tcp::v4(), 8081);
    decof::generic_tcp_server<decof::scgi::scgi_context> scgi_conn_mgr(obj_dict, strand_, scgi_endpoint);
    scgi_conn_mgr.preload();

    // Setup asio_tick context
    decof::asio_tick::asio_tick_context tick_ctx(obj_dict, strand_);
    tick_ctx.preload();

    // Start two threads to show that handlers are serialized by strands
    std::thread t([this]() { io_service_.run(); });
    io_service_.run();
    t.join();

    return 0;
}

void application::stop()
{
    io_service_.stop();
}

boost::asio::io_service::strand&application::strand()
{
    return strand_;
}

decof::asio_executor&application::executor()
{
    return executor_;
}

application::application() :
    io_service_(),
    strand_(io_service_),
    executor_(strand_)
{}
