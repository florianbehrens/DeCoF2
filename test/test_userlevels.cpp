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

BOOST_AUTO_TEST_SUITE(userlevels)

using namespace decof;

struct fixture
{
    struct my_event_t : public event
    {
        using event::event;

        void signal() override
        {
        }
    };

    struct my_context_t : public client_context
    {
        using client_context::client_context;

        void get_parameter(const std::string& uri, char separator = ':')
        {
            client_context::get_parameter(uri, separator);
        }

        void set_parameter(const std::string& uri, const value_t& value, char separator = ':')
        {
            client_context::set_parameter(uri, value, separator);
        }

        void signal_event(const std::string& uri, char separator = ':')
        {
            client_context::signal_event(uri, separator);
        }

        void observe(const std::string& uri, client_read_interface::value_change_slot_t slot, char separator = ':')
        {
            client_context::observe(uri, slot, separator);
        }
    };

    fixture()
      : managed_readwrite_param("param", &obj_dict, Forbidden, Internal),
        event("event", &obj_dict),
        my_context(new my_context_t(obj_dict))
    {
    }

    object_dictionary                 obj_dict;
    managed_readwrite_parameter<bool> managed_readwrite_param;
    my_event_t                        event;
    std::shared_ptr<my_context_t>     my_context;
};

BOOST_FIXTURE_TEST_CASE(read_access_denied, fixture)
{
    managed_readwrite_param.readlevel(Forbidden);

    for (userlevel_t ul = Normal; ul <= Internal; ul = static_cast<userlevel_t>(ul + 1)) {
        bool read_failed = false;
        try {
            my_context->userlevel(ul);
            my_context->get_parameter("root:param");
        } catch (access_denied_error&) {
            read_failed = true;
        }

        BOOST_REQUIRE_EQUAL(read_failed, true);
    }
}

BOOST_FIXTURE_TEST_CASE(read_access_allowed, fixture)
{
    my_context->userlevel(Internal);

    for (userlevel_t ul = Internal; ul >= Normal; ul = static_cast<userlevel_t>(ul - 1)) {
        try {
            managed_readwrite_param.readlevel(ul);
            my_context->get_parameter("root:param");
        } catch (access_denied_error& ex) {
            BOOST_FAIL(ex.what());
        }
    }
}

BOOST_FIXTURE_TEST_CASE(observation_denied, fixture)
{
    managed_readwrite_param.readlevel(Forbidden);

    for (userlevel_t ul = Normal; ul <= Internal; ul = static_cast<userlevel_t>(ul + 1)) {
        bool observation_failed = false;
        try {
            my_context->userlevel(ul);
            my_context->observe("root:param", [](const std::string&, const value_t&) {});
        } catch (access_denied_error&) {
            observation_failed = true;
        }

        BOOST_REQUIRE_EQUAL(observation_failed, true);
    }
}

BOOST_FIXTURE_TEST_CASE(observation_allowed, fixture)
{
    my_context->userlevel(Internal);

    for (userlevel_t ul = Internal; ul >= Normal; ul = static_cast<userlevel_t>(ul - 1)) {
        try {
            managed_readwrite_param.readlevel(ul);
            my_context->observe("root:param", [](const std::string&, const value_t&) {});
        } catch (access_denied_error& ex) {
            BOOST_FAIL(ex.what());
        }
    }
}

BOOST_FIXTURE_TEST_CASE(write_access_denied, fixture)
{
    managed_readwrite_param.writelevel(Forbidden);

    for (userlevel_t ul = Normal; ul <= Internal; ul = static_cast<userlevel_t>(ul + 1)) {
        bool write_failed = false;
        try {
            my_context->userlevel(ul);
            my_context->set_parameter("root:param", value_t(true));
        } catch (access_denied_error&) {
            write_failed = true;
        }

        BOOST_REQUIRE_EQUAL(write_failed, true);
    }
}

BOOST_FIXTURE_TEST_CASE(write_access_allowed, fixture)
{
    my_context->userlevel(Internal);

    for (userlevel_t ul = Internal; ul >= Normal; ul = static_cast<userlevel_t>(ul - 1)) {
        try {
            managed_readwrite_param.writelevel(ul);
            my_context->set_parameter("root:param", value_t(true));
        } catch (access_denied_error& ex) {
            BOOST_FAIL(ex.what());
        }
    }
}

BOOST_FIXTURE_TEST_CASE(signal_access_denied, fixture)
{
    event.writelevel(Forbidden);

    for (userlevel_t ul = Normal; ul <= Internal; ul = static_cast<userlevel_t>(ul + 1)) {
        bool write_failed = false;
        try {
            my_context->userlevel(ul);
            my_context->signal_event("root:event");
        } catch (access_denied_error&) {
            write_failed = true;
        }

        BOOST_REQUIRE_EQUAL(write_failed, true);
    }
}

BOOST_FIXTURE_TEST_CASE(signal_access_allowed, fixture)
{
    my_context->userlevel(Internal);

    for (userlevel_t ul = Internal; ul >= Normal; ul = static_cast<userlevel_t>(ul - 1)) {
        try {
            event.writelevel(ul);
            my_context->signal_event("root:event");
        } catch (access_denied_error& ex) {
            BOOST_FAIL(ex.what());
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
