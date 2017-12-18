/*
 * Copyright (c) 2016 Florian Behrens
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

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <decof/all.h>
#include <decof/client_context/generic_tcp_server.h>

#include "cli/tree_visitor.h"

using namespace decof;

BOOST_AUTO_TEST_SUITE(visitor_pattern)

struct test_visitor : object_visitor
{
    void visit(object*) override
    {
        counter += 1;
    }

    size_t counter = 0;
};

struct my_context_t : public client_context
{
    using client_context::client_context;

    void browse(object_visitor *visitor, const std::string &root_uri = std::string())
    {
        client_context::browse(visitor, root_uri);
    }
};

BOOST_AUTO_TEST_CASE(base_functions)
{
    object_dictionary obj_dict("root");
    handler_event event("event", &obj_dict);
    managed_readonly_parameter<bool> boolean_param("boolean", &obj_dict);
    managed_readonly_parameter<int> integer_param("integer", &obj_dict);
    managed_readonly_parameter<float> real_param("real", &obj_dict);
    managed_readonly_parameter<std::string> string_param("string", &obj_dict);
    managed_readonly_parameter<std::string, encoding_hint::binary> binary_param("binary", &obj_dict);
    managed_readonly_parameter<std::vector<bool>> boolean_seq_param("boolean_seq", &obj_dict);
    managed_readonly_parameter<std::vector<int>> integer_seq_param("integer_seq", &obj_dict);
    managed_readonly_parameter<std::vector<float>> real_seq_param("real_seq", &obj_dict);
    managed_readonly_parameter<std::vector<std::string>> string_seq_param("string_seq", &obj_dict);
    managed_readonly_parameter<std::tuple<bool, int, float, std::string>> tuple_param("tuple", &obj_dict);

    auto my_context = std::make_shared<my_context_t>(obj_dict);
    test_visitor visitor;
    my_context->browse(&visitor);
    BOOST_REQUIRE_EQUAL(visitor.counter, 12);
}

BOOST_AUTO_TEST_CASE(tree_visitor)
{
    object_dictionary obj_dict("root");
    handler_event event("event", &obj_dict);
    managed_readonly_parameter<bool> boolean_param("boolean", &obj_dict);
    managed_readonly_parameter<int> integer_param("integer", &obj_dict);
    managed_readonly_parameter<float> real_param("real", &obj_dict);
    managed_readonly_parameter<std::string> string_param("string", &obj_dict);
    managed_readonly_parameter<std::string, encoding_hint::binary> binary_param("binary", &obj_dict);
    managed_readwrite_parameter<std::vector<bool>> boolean_seq_param("boolean_seq", &obj_dict);
    managed_readwrite_parameter<std::vector<int>> integer_seq_param("integer_seq", &obj_dict);
    managed_readwrite_parameter<std::vector<float>> real_seq_param("real_seq", &obj_dict);
    managed_readwrite_parameter<std::vector<std::string>> string_seq_param("string_seq", &obj_dict);

    auto my_context = std::make_shared<my_context_t>(obj_dict);

    std::ostringstream out;
    cli::tree_visitor visitor(out);
    my_context->browse(&visitor);

    auto nominal = R"lit(root NODE
root:event EVENT
root:boolean PARAM RO BOOLEAN
root:integer PARAM RO INTEGER
root:real PARAM RO REAL
root:string PARAM RO STRING
root:binary PARAM RO BINARY
root:boolean_seq PARAM RW BOOLEAN_SEQ
root:integer_seq PARAM RW INTEGER_SEQ
root:real_seq PARAM RW REAL_SEQ
root:string_seq PARAM RW STRING_SEQ
)lit";

    BOOST_REQUIRE_EQUAL(out.str(), nominal);
}

BOOST_AUTO_TEST_SUITE_END()
