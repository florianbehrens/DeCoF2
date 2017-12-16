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

#include <functional>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <decof/all.h>
#include <decof/client_context/client_context.h>

#include "test_helpers.h"

BOOST_AUTO_TEST_SUITE(parameter_observation)

using decof::value_t;

struct fixture
{
    struct my_context_t : public decof::client_context
    {
        using decof::client_context::client_context;
        
        void observe(const std::string& uri, decof::client_read_interface::signal_type::slot_type slot) {
            decof::client_context::observe(uri, slot);
        }

        void unobserve(const std::string& uri) {
            decof::client_context::unobserve(uri);
        }

        void set_parameter(const std::string &uri, const value_t &value, char separator = ':')
        {
            decof::client_context::set_parameter(uri, value, separator);
        }

        void tick()
        {
            decof::client_context::tick();
        }
    };

    struct external_readonly_parameter_t : public decof::external_readonly_parameter<bool>
    {
        using decof::external_readonly_parameter<bool>::external_readonly_parameter;

        bool external_value() const override
        {
            return m_value;
        }

        bool m_value = false;
    };

    struct external_readwrite_parameter_t : public decof::external_readwrite_parameter<bool>
    {
        using decof::external_readwrite_parameter<bool>::external_readwrite_parameter;

        bool external_value(const bool &value) override
        {
            m_value = value;
            return true;
        }

        bool external_value() const override
        {
            return m_value;
        }

        bool m_value = false;
    };

    fixture() :
        obj_dict("root"),
        managed_readonly_parameter("managed_readonly_parameter", &obj_dict, false),
        managed_readwrite_parameter("managed_readwrite_parameter", &obj_dict, false),
        external_readonly_parameter("external_readonly_parameter", &obj_dict),
        external_readwrite_parameter("external_readwrite_parameter", &obj_dict),
        my_context(new my_context_t(obj_dict))
    {}

    void notify(const std::string &uri, const value_t &value) {
        notified_uri = uri;
        notified_value = value;
    }

    std::string notified_uri;
    value_t notified_value;

    decof::object_dictionary obj_dict;
    decof::managed_readonly_parameter<bool> managed_readonly_parameter;
    decof::managed_readwrite_parameter<bool> managed_readwrite_parameter;
    external_readonly_parameter_t external_readonly_parameter;
    external_readwrite_parameter_t external_readwrite_parameter;
    std::shared_ptr<my_context_t> my_context;
};

BOOST_FIXTURE_TEST_CASE(observe_managed_readonly_parameter, fixture)
{
    my_context->observe("root:managed_readonly_parameter",
                        std::bind(&fixture::notify, this, std::placeholders::_1, std::placeholders::_2));
    managed_readonly_parameter.value(true);

    BOOST_REQUIRE_EQUAL(notified_uri, "root:managed_readonly_parameter");
    BOOST_REQUIRE_EQUAL(notified_value, value_t(true));

    my_context->unobserve("root:managed_readonly_parameter");
    managed_readonly_parameter.value(false);

    BOOST_REQUIRE_NE(notified_value, value_t{ false });
}

BOOST_FIXTURE_TEST_CASE(observe_managed_readwrite_parameter, fixture)
{
    my_context->observe("root:managed_readwrite_parameter",
                        std::bind(&fixture::notify, this, std::placeholders::_1, std::placeholders::_2));
    my_context->set_parameter("root:managed_readwrite_parameter", true);

    BOOST_REQUIRE_EQUAL(notified_uri, "root:managed_readwrite_parameter");
    BOOST_REQUIRE_EQUAL(notified_value, value_t{ true });

    my_context->unobserve("root:managed_readwrite_parameter");
    my_context->set_parameter("root:managed_readwrite_parameter", false);

    BOOST_REQUIRE_NE(notified_value, value_t{ false });
}

BOOST_FIXTURE_TEST_CASE(observe_external_readonly_parameter, fixture)
{
    my_context->observe("root:external_readonly_parameter",
                        std::bind(&fixture::notify, this, std::placeholders::_1, std::placeholders::_2));
    external_readonly_parameter.m_value = true;
    my_context->tick();

    BOOST_REQUIRE_EQUAL(notified_uri, "root:external_readonly_parameter");
    BOOST_REQUIRE_EQUAL(notified_value, value_t{ true });

    my_context->unobserve("root:external_readonly_parameter");
    external_readonly_parameter.m_value = false;
    my_context->tick();

    BOOST_REQUIRE_NE(notified_value, value_t{ false });
}

BOOST_FIXTURE_TEST_CASE(observe_external_readwrite_parameter, fixture)
{
    my_context->observe("root:external_readwrite_parameter",
                        std::bind(&fixture::notify, this, std::placeholders::_1, std::placeholders::_2));
    my_context->set_parameter("root:external_readwrite_parameter", true);

    BOOST_REQUIRE_EQUAL(notified_uri, "root:external_readwrite_parameter");
    BOOST_REQUIRE_EQUAL(notified_value, value_t{ true });

    my_context->unobserve("root:external_readwrite_parameter");
    my_context->set_parameter("root:external_readwrite_parameter", false);

    BOOST_REQUIRE_NE(notified_value, value_t{ false });
}

BOOST_AUTO_TEST_SUITE_END()
