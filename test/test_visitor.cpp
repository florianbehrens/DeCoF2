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

BOOST_AUTO_TEST_SUITE(visitor_pattern)

struct test_visitor : decof::object_visitor
{
    void visit(decof::object*) override
    {
        counter += 1;
    }

    size_t counter = 0;
};

struct my_context_t : public decof::client_context
{
    using decof::client_context::client_context;

    void browse(decof::object_visitor *visitor, const std::string &root_uri = std::string())
    {
        decof::client_context::browse(visitor, root_uri);
    }
};

BOOST_AUTO_TEST_CASE(base_functions)
{
    decof::object_dictionary obj_dict("root");
    decof::handler_event event("event", &obj_dict);
    decof::managed_readonly_parameter<decof::boolean> boolean_param("boolean", &obj_dict);
    decof::managed_readonly_parameter<decof::integer> integer_param("integer", &obj_dict);
    decof::managed_readonly_parameter<decof::real> real_param("real", &obj_dict);
    decof::managed_readonly_parameter<decof::string> string_param("string", &obj_dict);
    decof::managed_readonly_parameter<decof::binary> binary_param("binary", &obj_dict);
    decof::managed_readonly_parameter<decof::boolean_seq> boolean_seq_param("boolean_seq", &obj_dict);
    decof::managed_readonly_parameter<decof::integer_seq> integer_seq_param("integer_seq", &obj_dict);
    decof::managed_readonly_parameter<decof::real_seq> real_seq_param("real_seq", &obj_dict);
    decof::managed_readonly_parameter<decof::string_seq> string_seq_param("string_seq", &obj_dict);
    decof::managed_readonly_parameter<decof::binary_seq> binary_seq_param("binary_seq", &obj_dict);

    auto my_context = std::make_shared<my_context_t>(obj_dict);
    test_visitor visitor;
    my_context->browse(&visitor);
    BOOST_REQUIRE_EQUAL(visitor.counter, 12);
}

BOOST_AUTO_TEST_CASE(tree_visitor)
{
    decof::object_dictionary obj_dict("root");
    decof::handler_event event("event", &obj_dict);
    decof::managed_readonly_parameter<decof::boolean> boolean_param("boolean", &obj_dict);
    decof::managed_readonly_parameter<decof::integer> integer_param("integer", &obj_dict);
    decof::managed_readonly_parameter<decof::real> real_param("real", &obj_dict);
    decof::managed_readonly_parameter<decof::string> string_param("string", &obj_dict);
    decof::managed_readonly_parameter<decof::binary> binary_param("binary", &obj_dict);
    decof::managed_readwrite_parameter<decof::boolean_seq> boolean_seq_param("boolean_seq", &obj_dict);
    decof::managed_readwrite_parameter<decof::integer_seq> integer_seq_param("integer_seq", &obj_dict);
    decof::managed_readwrite_parameter<decof::real_seq> real_seq_param("real_seq", &obj_dict);
    decof::managed_readwrite_parameter<decof::string_seq> string_seq_param("string_seq", &obj_dict);
    decof::managed_readwrite_parameter<decof::binary_seq> binary_seq_param("binary_seq", &obj_dict);

    auto my_context = std::make_shared<my_context_t>(obj_dict);

    std::ostringstream out;
    decof::cli::tree_visitor visitor(out);
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
root:binary_seq PARAM RW BINARY_SEQ
)lit";

    BOOST_REQUIRE_EQUAL(out.str(), nominal);
}


BOOST_AUTO_TEST_SUITE_END()
