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

#include <memory>

#include <boost/test/unit_test.hpp>

#include "decof.h"
#include "client_context.h"

BOOST_AUTO_TEST_SUITE(object_dictionary)

struct fixture
{
    struct my_context_t : public decof::client_context
    {
        using decof::client_context::client_context;
        
        std::string connection_type() const override
        {
            return std::string();
        }
        
        std::string remote_endpoint() const override
        {
            return std::string();
        }    
        
        void preload() override
        {}
        
        void get_parameter(const std::string &uri, char separator = ':')
        {
            decof::client_context::get_parameter(uri, separator);
        }
    };

    fixture() :
        obj_dict("root"),
        node1("node1", &obj_dict),
        node2("node2", &node1),
        param("param", &node2, "Hello World"),
        my_context(new my_context_t(obj_dict, std::shared_ptr<decof::connection>()))
    {}

    decof::object_dictionary obj_dict;
    decof::node node1, node2;
    decof::managed_readonly_parameter<decof::string> param;
    std::shared_ptr<my_context_t> my_context;
};

BOOST_FIXTURE_TEST_CASE(find_root_object, fixture)
{
    try {
        my_context->get_parameter("root");
    } catch (std::exception &ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_FIXTURE_TEST_CASE(find_child_object, fixture)
{
    try {
        my_context->get_parameter("root:node1:node2:param");
    } catch (std::exception &ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_FIXTURE_TEST_CASE(find_child_object_with_custom_separator, fixture)
{
    try {
        my_context->get_parameter("root&node1&node2&param", '&');
    } catch (std::exception &ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_AUTO_TEST_SUITE_END()
