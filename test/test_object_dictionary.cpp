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
#include <decof/client_context/client_context.h>
#include <boost/test/unit_test.hpp>
#include <memory>

BOOST_AUTO_TEST_SUITE(object_dictionary)

struct fixture
{
    struct my_context_t : public decof::client_context
    {
        using decof::client_context::client_context;

        void get_parameter(const std::string& uri, char separator = ':')
        {
            decof::client_context::get_parameter(uri, separator);
        }
    };

    fixture()
      : obj_dict("root"),
        node1(new decof::node("node1", &obj_dict)),
        node2(new decof::node("node2", node1.get())),
        param("param", node2.get(), "Hello World"),
        my_context(new my_context_t(obj_dict))
    {
    }

    decof::object_dictionary                       obj_dict;
    std::unique_ptr<decof::node>                   node1, node2;
    decof::managed_readonly_parameter<std::string> param;
    std::shared_ptr<my_context_t>                  my_context;
};

BOOST_FIXTURE_TEST_CASE(find_root_object, fixture)
{
    try {
        my_context->get_parameter("root");
    } catch (std::exception& ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_FIXTURE_TEST_CASE(find_child_object, fixture)
{
    try {
        my_context->get_parameter("root:node1:node2:param");
    } catch (std::exception& ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_FIXTURE_TEST_CASE(find_child_object_with_custom_separator, fixture)
{
    try {
        my_context->get_parameter("root&node1&node2&param", '&');
    } catch (std::exception& ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_FIXTURE_TEST_CASE(delete_middle_node, fixture)
{
    node1.reset();
    node2.reset();
}

BOOST_FIXTURE_TEST_CASE(set_parameter_value, fixture)
{
    std::string str("Hello");
    param.value(str);
    BOOST_REQUIRE_EQUAL(str, param.value());
}

BOOST_FIXTURE_TEST_CASE(move_parameter_value, fixture)
{
    std::string str("Hello");
    std::string str2(str);
    param.value(std::move(str2));
    BOOST_REQUIRE_EQUAL(str, param.value());
    BOOST_REQUIRE_NE(str2, param.value());
}

BOOST_FIXTURE_TEST_CASE(get_object_dictionary, fixture)
{
    auto od = param.get_object_dictionary();
    BOOST_REQUIRE(od != nullptr);
    BOOST_REQUIRE_EQUAL(od->name(), std::string("root"));
}

BOOST_FIXTURE_TEST_CASE(get_const_object_dictionary, fixture)
{
    const decof::managed_readonly_parameter<std::string>& rparam = param;
    auto                                                  od     = rparam.get_object_dictionary();
    BOOST_REQUIRE(od != nullptr);
    BOOST_REQUIRE_EQUAL(od->name(), std::string("root"));
}

BOOST_AUTO_TEST_CASE(add_and_remove_child_to_node)
{
    decof::node                             node("node");
    decof::managed_readonly_parameter<bool> parameter("parameter", nullptr);

    node.add_child(&parameter);

    BOOST_REQUIRE_EQUAL(node.children().size(), 1);
    BOOST_REQUIRE_EQUAL(parameter.parent(), &node);

    node.remove_child(&parameter);

    BOOST_REQUIRE_EQUAL(node.children().size(), 0);
    BOOST_REQUIRE(parameter.parent() == nullptr);
}

BOOST_AUTO_TEST_CASE(set_and_reset_parent)
{
    decof::node                             node("node");
    decof::managed_readonly_parameter<bool> parameter("parameter", nullptr);

    parameter.reset_parent(&node);

    BOOST_REQUIRE_EQUAL(node.children().size(), 1);
    BOOST_REQUIRE_EQUAL(parameter.parent(), &node);

    parameter.reset_parent();

    BOOST_REQUIRE_EQUAL(node.children().size(), 0);
    BOOST_REQUIRE(parameter.parent() == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()
