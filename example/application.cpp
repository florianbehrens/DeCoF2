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
#include "background_worker.h"
#include "composite.h"
#include <decof/all.h>
#include <decof/asio_tick/asio_tick.h>
#include <decof/cli/clisrv_context.h>
#include <decof/cli/pubsub_context.h>
#include <decof/client_context/generic_tcp_server.h>
#include <decof/scgi/scgi_context.h>
#include <boost/asio/steady_timer.hpp>
#include <boost/bind.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

using namespace decof;

namespace {

DECOF_DECLARE_MANAGED_READWRITE_PARAMETER(my_managed_readwrite_parameter, std::string);
DECOF_DECLARE_MANAGED_READONLY_PARAMETER(my_managed_readonly_parameter, std::string);
DECOF_DECLARE_EXTERNAL_READONLY_PARAMETER(current_context_endpoint_parameter, std::string);
DECOF_DECLARE_EXTERNAL_READONLY_PARAMETER(time_parameter, std::string);
DECOF_DECLARE_EVENT(exit_event);
DECOF_DECLARE_WRITEONLY_PARAMETER(cout_parameter, std::string);
typedef std::tuple<bool, int, float, std::string> cout_tuple_parameter_type;
DECOF_DECLARE_WRITEONLY_PARAMETER(cout_tuple_parameter, cout_tuple_parameter_type);

struct spin_count_parameter : public managed_readonly_parameter<long long>
{
    spin_count_parameter(node* parent) : managed_readonly_parameter<long long>("spin-count", parent, 0)
    {
        application::instance().strand().post(std::bind(&spin_count_parameter::increment, this));
    }

    void increment()
    {
        value(value() + 1);
        application::instance().strand().post(std::bind(&spin_count_parameter::increment, this));
    }
};

void my_managed_readwrite_parameter::verify(const std::string& value)
{
    if (value != "true" && value != "false")
        throw invalid_value_error();

    std::cout << "my_managed_readwrite_parameter was changed to " << value << std::endl;
}

current_context_endpoint_parameter::value_type current_context_endpoint_parameter::external_value() const
{
    auto cc = get_object_dictionary()->current_context();
    if (cc != nullptr)
        return cc->connection_type() + "://" + cc->remote_endpoint();

    return "Unknown";
}

std::string time_parameter::external_value() const
{
    const size_t max_length = 40;
    char         str[max_length];
    std::time_t  now = std::time(nullptr);
    std::strftime(str, sizeof(str), "%c", std::localtime(&now));
    return str;
}

void exit_event::signal()
{
    application::instance().strand().get_io_service().stop();
}

void cout_parameter::value(const std::string& value)
{
    std::cout << value << std::endl;
}

void cout_tuple_parameter::value(const cout_tuple_parameter_type& value)
{
    std::cout << "Boolean value: " << std::get<0>(value) << std::endl
              << "Integer value: " << std::get<1>(value) << std::endl
              << "Real value: " << std::get<2>(value) << std::endl
              << "String value: " << std::get<3>(value) << std::endl;
}

struct ip_address_parameter : public external_readwrite_parameter<std::string>
{
    ip_address_parameter(const char* name, node* parent = nullptr)
      : external_readwrite_parameter<std::string>(name, parent)
    {
    }

  private:
    bool external_value(const value_type& value) override
    {
        std::fstream file(filename_, std::ios_base::out | std::ios_base::trunc);
        if (file.rdstate() & std::ios_base::failbit)
            return false;

        file << value;
        return true;
    }

    std::string external_value() const override
    {
        std::string  str;
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
//  | |-- boolean_seq: sequence<boolean> (rw)
//  | |-- integer_seq: sequence<int> (rw)
//  | |-- real_seq: sequence<real> (rw)
//  | |-- string_seq: sequence<string> (rw)
//  | |-- binary_seq: sequence<string> (rw)
//  |-- tuples: node (r)
//  | |-- scalar_tuple: tuple<boolean, int, real, string> (rw)
//  |-- events: node (r)
//  | |-- exit: event
//  |-- writeonly: node (r)
//  | |-- string: string (w)
//  | |-- tuple: tuple (w)
//  |-- composite
//  | |-- summand1
//  | |-- summand2
//  | |-- sum

object_dictionary                  obj_dict("example");
my_managed_readwrite_parameter     enable_param("enabled", &obj_dict, "false");
spin_count_parameter               spin_count_param(&obj_dict);
node                               current_context_node("current-context", &obj_dict);
current_context_endpoint_parameter endpoint_param("endpoint", &current_context_node);
node                               subnode("subnode", &obj_dict);
time_parameter                     time_param("time", &subnode);
managed_readwrite_parameter<std::vector<std::string>>
                                                                       leaf2_param("leaf2", &subnode, Normal, Normal, {"value1", "value2", "value3"});
ip_address_parameter                                                   ipo_address_param("ip-address", &subnode);
managed_readwrite_parameter<bool>                                      boolean_param("boolean", &subnode);
managed_readwrite_parameter<std::uint16_t>                             integer_param("integer", &subnode);
managed_readwrite_parameter<float>                                     real_param("real", &subnode);
managed_readwrite_parameter<std::string>                               string_param("string", &subnode);
managed_readwrite_parameter<std::string, encoding_hint::binary>        binary_param("binary", &subnode);
managed_readwrite_parameter<std::vector<bool>>                         boolean_seq_param("boolean_seq", &subnode);
managed_readwrite_parameter<std::vector<int>>                          integer_seq_param("integer_seq", &subnode);
managed_readwrite_parameter<std::vector<float>>                        real_seq_param("real_seq", &subnode);
managed_readwrite_parameter<std::vector<std::string>>                  string_seq_param("string_seq", &subnode);
node                                                                   tuples_node("tuples", &obj_dict);
managed_readwrite_parameter<std::tuple<bool, int, float, std::string>> scalar_tuple("scalar_tuple", &tuples_node);
node                                                                   events_node("events", &obj_dict);
exit_event                                                             exit_ev("exit", &events_node);
node                                                                   writeonly_node("writeonly", &obj_dict);
cout_parameter                                                         cout_param("string", &writeonly_node);
cout_tuple_parameter                                                   cout_tuple_param("tuple", &writeonly_node);
composite                                                              comp("composite", &obj_dict);
auto background_work = background_worker::create("background_work", &obj_dict);

} // Anonymous namespace

application& application::instance()
{
    static application inst;
    return inst;
}

int application::run(int, char*[])
{
    // Install userlevel change callback
    using cli::cli_context_base;
    cli_context_base::install_userlevel_callback(
        [](const client_context&, userlevel_t userlevel, const std::string& passwd) {
            if (userlevel == Normal || userlevel == Readonly)
                return true;
            else if (userlevel == Maintenance && passwd == "maintenance")
                return true;
            else if (userlevel == Service && passwd == "service")
                return true;
            else if (userlevel == Internal && passwd == "internal")
                return true;
            return false;
        });

    // Setup request/respone CLI context
    boost::asio::ip::tcp::endpoint          cmd_endpoint(boost::asio::ip::tcp::v4(), 1998);
    generic_tcp_server<cli::clisrv_context> conn_mgr_cmd(obj_dict, strand_, cmd_endpoint);
    conn_mgr_cmd.preload();

    // Setup publish/subscribe CLI context
    boost::asio::ip::tcp::endpoint          mon_endpoint(boost::asio::ip::tcp::v4(), 1999);
    generic_tcp_server<cli::pubsub_context> conn_mgr_mon(obj_dict, strand_, mon_endpoint);
    conn_mgr_mon.preload();

    // Setup SCGI context
    boost::asio::ip::tcp::endpoint         scgi_endpoint(boost::asio::ip::tcp::v4(), 8081);
    generic_tcp_server<scgi::scgi_context> scgi_conn_mgr(obj_dict, strand_, scgi_endpoint);
    scgi_conn_mgr.preload();

    // Setup asio_tick context
    asio_tick::asio_tick_context tick_ctx(obj_dict, strand_);
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

boost::asio::io_service::strand& application::strand()
{
    return strand_;
}

asio_executor& application::executor()
{
    return executor_;
}

application::application() : io_service_(), strand_(io_service_), executor_(strand_)
{
}
