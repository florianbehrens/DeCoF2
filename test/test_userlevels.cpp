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

#include <boost/test/unit_test.hpp>

#include "client_context.h"
#include "decof.h"

BOOST_AUTO_TEST_SUITE(userlevels)

struct fixture
{
    struct my_context_t : public decof::client_context
    {
        using decof::client_context::client_context;

        void get_parameter(const std::string &uri, char separator = ':')
        {
            decof::client_context::get_parameter(uri, separator);
        }

        void set_parameter(const std::string &uri, const boost::any &any_value, char separator = ':')
        {
            decof::client_context::set_parameter(uri, any_value, separator);
        }
    };

    fixture() :
        managed_readwrite_parameter("param", &obj_dict, decof::Forbidden, decof::Internal),
        my_context(new my_context_t(obj_dict, std::shared_ptr<decof::connection>()))
    {}

    decof::object_dictionary obj_dict;
    decof::managed_readwrite_parameter<decof::boolean> managed_readwrite_parameter;
    std::shared_ptr<my_context_t> my_context;
};

BOOST_FIXTURE_TEST_CASE(read_access_denied, fixture)
{
    managed_readwrite_parameter.readlevel(decof::Forbidden);

    for (decof::userlevel_t ul = decof::Normal; ul <= decof::Internal; ul = static_cast<decof::userlevel_t>(ul + 1)) {
        bool read_failed = false;
        try {
            my_context->userlevel(ul);
            my_context->get_parameter("root:param");
        } catch (decof::access_denied_error &) {
            read_failed = true;
        }

        BOOST_REQUIRE_EQUAL(read_failed, true);
    }
}

BOOST_FIXTURE_TEST_CASE(read_access_allowed, fixture)
{
    my_context->userlevel(decof::Internal);

    for (decof::userlevel_t ul = decof::Internal; ul >= decof::Normal; ul = static_cast<decof::userlevel_t>(ul - 1)) {
        try {
            managed_readwrite_parameter.readlevel(ul);
            my_context->get_parameter("root:param");
        } catch (decof::access_denied_error &ex) {
            BOOST_FAIL(ex.what());
        }
    }
}

BOOST_FIXTURE_TEST_CASE(write_access_denied, fixture)
{
    managed_readwrite_parameter.writelevel(decof::Forbidden);

    for (decof::userlevel_t ul = decof::Normal; ul <= decof::Internal; ul = static_cast<decof::userlevel_t>(ul + 1)) {
        bool write_failed = false;
        try {
            my_context->userlevel(ul);
            my_context->set_parameter("root:param", boost::any(true));
        } catch (decof::access_denied_error &) {
            write_failed = true;
        }

        BOOST_REQUIRE_EQUAL(write_failed, true);
    }
}

BOOST_FIXTURE_TEST_CASE(write_access_allowed, fixture)
{
    my_context->userlevel(decof::Internal);

    for (decof::userlevel_t ul = decof::Internal; ul >= decof::Normal; ul = static_cast<decof::userlevel_t>(ul - 1)) {
        try {
            managed_readwrite_parameter.writelevel(ul);
            my_context->set_parameter("root:param", boost::any(true));
        } catch (decof::access_denied_error &ex) {
            BOOST_FAIL(ex.what());
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
