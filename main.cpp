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
#include "scheme_protocol.h"
#include "scheme_monitor_protocol.h"
#include "tcp_connection_manager.h"
#include "tree_element.h"

using namespace decof;

object_dictionary obj_dict;

struct my_managed_readwrite_parameter : public managed_readwrite_parameter<std::string>
{
    my_managed_readwrite_parameter(std::string name, node *parent = nullptr, std::string value = std::string())
     : managed_readwrite_parameter<std::string>(name, parent, value)
    {}

    virtual void verify(const std::string& value) override {
        if (value != "true" && value != "false")
            throw invalid_value_error();

        std::cout << "my_managed_readwrite_parameter was changed to " << value << std::endl;
    }
};

struct my_managed_readonly_parameter : public managed_readonly_parameter<std::string>
{
    my_managed_readonly_parameter(std::string name, node *parent = nullptr, std::string value = std::string())
     : managed_readonly_parameter<std::string>(name, parent, value)
    {}

    virtual bool notify(const std::string& value) {
        std::cout << "my_managed_readonly_parameter was changed to " << value << std::endl;
        return true;
    }
};

struct time_parameter : public external_readonly_parameter<std::string>
{
    time_parameter(std::string name, node *parent = nullptr)
     : external_readonly_parameter<std::string>(name, parent)
    {}

private:
    std::string get_external_value() override {
        const size_t max_length = 40;
        char str[max_length];
        std::time_t now = std::time(nullptr);
        std::strftime(str, sizeof(str), "%c", std::localtime(&now));
        return std::string(str);
    }
};

struct ip_address_parameter : public external_readwrite_parameter<std::string>
{
    ip_address_parameter(std::string name, node *parent = nullptr)
     : external_readwrite_parameter<std::string>(name, parent)
    {}

private:
    bool set_external_value(const value_type &value) override {
        std::fstream file(filename_, std::ios_base::out | std::ios_base::trunc);
        if (file.rdstate() & std::ios_base::failbit)
            return false;

        file << value;
        return true;
    }

    std::string get_external_value() override {
        std::string str;
        std::fstream file_(filename_, std::ios_base::in);
        std::getline(file_, str);
        return str;
    }

    std::string filename_ = "C:\\Users\\willy\\data.txt";
};

int main()
{
    // Output some diagnostics:
    std::cout << "sizeof(node) = " << sizeof(node) << std::endl;
    std::cout << "sizeof(managed_readonly_parameter<std::string>) = " << sizeof(managed_readonly_parameter<std::string>) << std::endl;
    std::cout << "sizeof(managed_readwrite_parameter<std::string>) = " << sizeof(managed_readwrite_parameter<std::string>) << std::endl;
    std::cout << "sizeof(external_readonly_parameter<std::string>) = " << sizeof(external_readonly_parameter<std::string>) << std::endl;
    std::cout << "sizeof(external_readwrite_parameter<std::string>) = " << sizeof(external_readwrite_parameter<std::string>) << std::endl;

    std::cout << "sizeof(boost::asio::steady_timer) = " << sizeof(boost::asio::steady_timer) << std::endl;
    std::cout << "sizeof(boost::signals2::connection) = " << sizeof(boost::signals2::connection) << std::endl;

    // Setup object dictionary
    // root
    //  |-- enabled: string (rw)
    //  |-- subnode: node (r)
    //  | |-- time: string (r)
    //  | |-- leaf2: stringlist (rw)
    //  | |-- ip-address: string (rw)
    new my_managed_readwrite_parameter("enabled", &obj_dict, "false");
    node* subnode = new node("subnode", &obj_dict);
    new time_parameter("time", subnode);
    stringlist sl = { "value1", "value2", "value3" };
    new managed_readwrite_parameter<stringlist>("leaf2", subnode, sl);
    new ip_address_parameter("ip-address", subnode);

    // Setup scheme command line connection manager
    boost::asio::ip::tcp::endpoint cmd_endpoint(boost::asio::ip::tcp::v4(), 1998);
    tcp_connection_manager cmd_conn_manager(obj_dict, cmd_endpoint, &scheme_protocol::handle_connect);
    cmd_conn_manager.preload();

    // Setup scheme monitoring line connection manager
    boost::asio::ip::tcp::endpoint mon_endpoint(boost::asio::ip::tcp::v4(), 1999);
    tcp_connection_manager mon_conn_manager(obj_dict, mon_endpoint, &scheme_monitor_protocol::handle_connect);
    mon_conn_manager.preload();

    obj_dict.get_io_service().run();

    return 0;
}
