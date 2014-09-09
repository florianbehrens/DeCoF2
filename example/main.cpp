#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/bind.hpp>

#include "exceptions.h"
#include "external_readonly_parameter.h"
#include "external_readwrite_parameter.h"
#include "object_dictionary.h"
#include "managed_readwrite_parameter.h"
#include "managed_readonly_parameter.h"
#include "tcp_connection_manager.h"
#include "tree_element.h"
#include "textproto_clisrv.h"
#include "textproto_pubsub.h"
#include "types.h"

decof::object_dictionary obj_dict;

struct current_context_endpoint_parameter : public decof::external_readonly_parameter<decof::string>
{
    current_context_endpoint_parameter(std::string name, decof::node *parent = nullptr) :
        decof::external_readonly_parameter<decof::string>(name, parent)
    {}

    virtual value_type get_external_value() {
        const decof::connection* c = get_object_dictionary()->current_context()->connnection();
        return c->type() + "://" + c->remote_endpoint();
    }
};

struct my_managed_readwrite_parameter : public decof::managed_readwrite_parameter<decof::string>
{
    my_managed_readwrite_parameter(std::string name, decof::node *parent = nullptr, decof::string value = decof::string())
     : decof::managed_readwrite_parameter<decof::string>(name, parent, value)
    {}

    virtual void verify(const decof::string& value) override {
        if (value != "true" && value != "false")
            throw decof::invalid_value_error();

        std::cout << "my_managed_readwrite_parameter was changed to " << value << std::endl;
    }
};

struct my_managed_readonly_parameter : public decof::managed_readonly_parameter<decof::string>
{
    my_managed_readonly_parameter(std::string name, decof::node *parent = nullptr, decof::string value = decof::string())
     : decof::managed_readonly_parameter<decof::string>(name, parent, value)
    {}

    virtual bool notify(const decof::string& value) {
        std::cout << "my_managed_readonly_parameter was changed to " << value << std::endl;
        return true;
    }
};

struct time_parameter : public decof::external_readonly_parameter<decof::string>
{
    time_parameter(std::string name, decof::node *parent = nullptr)
     : decof::external_readonly_parameter<decof::string>(name, parent)
    {}

private:
    decof::string get_external_value() override {
        const size_t max_length = 40;
        char str[max_length];
        std::time_t now = std::time(nullptr);
        std::strftime(str, sizeof(str), "%c", std::localtime(&now));
        return decof::string(str);
    }
};

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
    //  | |-- bool_vector: bool_vector (rw)
    //  | |-- int_vector: int_vector (rw)
    //  | |-- double_vector: double_vector (rw)
    new my_managed_readwrite_parameter("enabled", &obj_dict, "false");
    decof::node* current_context_node = new decof::node("current-context", &obj_dict);
    new current_context_endpoint_parameter("endpoint", current_context_node);
    decof::node* subnode = new decof::node("subnode", &obj_dict);
    new time_parameter("time", subnode);
    decof::string_seq sl = { "value1", "value2", "value3" };
    new decof::managed_readwrite_parameter<decof::string_seq>("leaf2", subnode, sl);
    new ip_address_parameter("ip-address", subnode);
    new decof::managed_readwrite_parameter<decof::boolean>("boolean", subnode);
    new decof::managed_readwrite_parameter<decof::integer>("integer", subnode);
    new decof::managed_readwrite_parameter<decof::real>("real", subnode);
    new decof::managed_readwrite_parameter<decof::boolean_seq>("boolean_seq", subnode);
    new decof::managed_readwrite_parameter<decof::integer_seq>("integer_seq", subnode);
    new decof::managed_readwrite_parameter<decof::real_seq>("real_seq", subnode);

    // Setup scheme command line connection manager
    boost::asio::ip::tcp::endpoint cmd_endpoint(boost::asio::ip::tcp::v4(), 1998);
    decof::tcp_connection_manager conn_mgr_cmd(obj_dict, cmd_endpoint);
    conn_mgr_cmd.preload<decof::textproto_clisrv>();

    // Setup scheme monitoring line connection manager
    boost::asio::ip::tcp::endpoint mon_endpoint(boost::asio::ip::tcp::v4(), 1999);
    decof::tcp_connection_manager conn_mgr_mon(obj_dict, mon_endpoint);
    conn_mgr_mon.preload<decof::textproto_pubsub>();

    obj_dict.get_io_service().run();

    return 0;
}
