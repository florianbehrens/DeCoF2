#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/bind.hpp>

#include "decof.h"
#include "tcp_connection_manager.h"
#include "textproto_clisrv.h"
#include "textproto_pubsub.h"
#include "webservice.h"

namespace
{

DECOF_DECLARE_MANAGED_READWRITE_PARAMETER(my_managed_readwrite_parameter, decof::string);
DECOF_DECLARE_MANAGED_READONLY_PARAMETER(my_managed_readonly_parameter, decof::string);
DECOF_DECLARE_EXTERNAL_READONLY_PARAMETER(current_context_endpoint_parameter, decof::string);
DECOF_DECLARE_EXTERNAL_READONLY_PARAMETER(time_parameter, decof::string);
DECOF_DECLARE_EVENT(exit_event);

void my_managed_readwrite_parameter::verify(const decof::string& value)
{
    if (value != "true" && value != "false")
        throw decof::invalid_value_error();

    std::cout << "my_managed_readwrite_parameter was changed to " << value << std::endl;
}

current_context_endpoint_parameter::value_type current_context_endpoint_parameter::get_external_value()
{
    const std::shared_ptr<decof::client_context> cc = get_object_dictionary()->current_context();
    if (cc != nullptr) {
        const decof::connection* c = cc->connnection();
        return c->type() + "://" + c->remote_endpoint();
    }

    return "Unknown";
}

decof::string time_parameter::get_external_value()
{
    const size_t max_length = 40;
    char str[max_length];
    std::time_t now = std::time(nullptr);
    std::strftime(str, sizeof(str), "%c", std::localtime(&now));
    return decof::string(str);
}

void exit_event::signal()
{
    get_object_dictionary()->get_io_service().stop();
}

struct ip_address_parameter : public decof::external_readwrite_parameter<decof::string>
{
    ip_address_parameter(std::string name, decof::node *parent = nullptr)
     : decof::external_readwrite_parameter<decof::string>(name, parent)
    {}

private:
    bool set_external_value(const value_type &value) override {
        std::fstream file(filename_, std::ios_base::out | std::ios_base::trunc);
        if (file.rdstate() & std::ios_base::failbit)
            return false;

        file << value;
        return true;
    }

    decof::string get_external_value() override {
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
//  |-- current-context: node (r)
//  | |-- endpoint: string (r)
//  |-- subnode: node (r)
//  | |-- time: string (r)
//  | |-- leaf2: string_vector (rw)
//  | |-- ip-address: string (rw)
//  | |-- bool: bool (rw)
//  | |-- integer: int (rw)
//  | |-- double: double (rw)
//  | |-- string: string (rw)
//  | |-- binary: binary (rw)
//  | |-- boolean_seq: boolean_seq (rw)
//  | |-- integer_seq: integer_seq (rw)
//  | |-- real_seq: real_seq (rw)
//  | |-- string_seq: string_seq (rw)
//  | |-- binary_seq: binary_seq (rw)
//  |-- events: node (r)
//    |-- exit: event
decof::object_dictionary obj_dict;
my_managed_readwrite_parameter enable_param("enabled", &obj_dict, "false");
decof::node current_context_node("current-context", &obj_dict);
current_context_endpoint_parameter endpoint_param("endpoint", &current_context_node);
decof::node subnode("subnode", &obj_dict);
time_parameter time_param("time", &subnode);
decof::string_seq sl = { "value1", "value2", "value3" };
decof::managed_readwrite_parameter<decof::string_seq> leaf2_param("leaf2", &subnode, decof::Readonly, decof::Normal, sl);
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
decof::node events_node("events", &obj_dict);
exit_event exit_ev("exit", &events_node);

} // Anonymous namespace

int main()
{
    // Output some diagnostics:
    std::cout << "sizeof(node) = " << sizeof(decof::node) << std::endl;
    std::cout << "sizeof(managed_readonly_parameter<decof::integer>) = " << sizeof(decof::managed_readonly_parameter<decof::integer>) << std::endl;
    std::cout << "sizeof(managed_readwrite_parameter<decof::integer>) = " << sizeof(decof::managed_readwrite_parameter<decof::integer>) << std::endl;
    std::cout << "sizeof(external_readonly_parameter<decof::integer>) = " << sizeof(decof::external_readonly_parameter<decof::integer>) << std::endl;
    std::cout << "sizeof(external_readwrite_parameter<decof::integer>) = " << sizeof(decof::external_readwrite_parameter<decof::integer>) << std::endl;

    std::cout << "sizeof(boost::asio::steady_timer) = " << sizeof(boost::asio::steady_timer) << std::endl;
    std::cout << "sizeof(boost::signals2::connection) = " << sizeof(boost::signals2::connection) << std::endl;

    // Setup scheme command line connection manager
    boost::asio::ip::tcp::endpoint cmd_endpoint(boost::asio::ip::tcp::v4(), 1998);
    decof::tcp_connection_manager conn_mgr_cmd(obj_dict, cmd_endpoint);
    conn_mgr_cmd.preload<decof::textproto_clisrv>();

    // Setup scheme monitoring line connection manager
    boost::asio::ip::tcp::endpoint mon_endpoint(boost::asio::ip::tcp::v4(), 1999);
    decof::tcp_connection_manager conn_mgr_mon(obj_dict, mon_endpoint);
    conn_mgr_mon.preload<decof::textproto_pubsub>();

    // Setup webservice
    boost::asio::ip::tcp::endpoint websvc_endpoint(boost::asio::ip::tcp::v4(), 8080);
    decof::tcp_connection_manager conn_mgr_websvc(obj_dict, websvc_endpoint);
    conn_mgr_websvc.preload<decof::webservice>();

    obj_dict.get_io_service().run();

    return 0;
}
