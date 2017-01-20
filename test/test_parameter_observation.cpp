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

#include <boost/any.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <decof/all.h>
#include <decof/client_context/client_context.h>

BOOST_AUTO_TEST_SUITE(parameter_observation)

struct fixture
{
    struct my_context_t : public decof::client_context
    {
        using decof::client_context::client_context;
        
        void observe(const std::string& uri, decof::client_read_interface::value_change_slot_t slot) {
            decof::client_context::observe(uri, slot);
        }

        void unobserve(const std::string& uri) {
            decof::client_context::unobserve(uri);
        }

        void set_parameter(const std::string &uri, const boost::any &any_value, char separator = ':')
        {
            decof::client_context::set_parameter(uri, any_value, separator);
        }

        void tick()
        {
            decof::client_context::tick();
        }
    };

    struct external_readonly_parameter_t : public decof::external_readonly_parameter<decof::boolean>
    {
        using decof::external_readonly_parameter<decof::boolean>::external_readonly_parameter;

        decof::boolean external_value() const override
        {
            return m_value;
        }

        size_t num_slots() {
            return signal_.num_slots();
        }

        bool m_value = false;
    };

    struct external_readwrite_parameter_t : public decof::external_readwrite_parameter<decof::boolean>
    {
        using decof::external_readwrite_parameter<decof::boolean>::external_readwrite_parameter;

        bool external_value(const decof::boolean &value) override
        {
            m_value = value;
            return true;
        }

        decof::boolean external_value() const override
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

    void notify(const std::string &uri, const boost::any &value) {
        notified_uri = uri;
        notified_value = value;
    }

    std::string notified_uri;
    boost::any notified_value;

    decof::object_dictionary obj_dict;
    decof::managed_readonly_parameter<decof::boolean> managed_readonly_parameter;
    decof::managed_readwrite_parameter<decof::boolean> managed_readwrite_parameter;
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
    BOOST_REQUIRE_EQUAL(boost::any_cast<bool>(notified_value), true);

    my_context->unobserve("root:managed_readonly_parameter");
    managed_readonly_parameter.value(false);

    BOOST_REQUIRE_NE(boost::any_cast<bool>(notified_value), false);
}

BOOST_FIXTURE_TEST_CASE(observe_managed_readwrite_parameter, fixture)
{
    my_context->observe("root:managed_readwrite_parameter",
                        std::bind(&fixture::notify, this, std::placeholders::_1, std::placeholders::_2));
    my_context->set_parameter("root:managed_readwrite_parameter", boost::any(true));

    BOOST_REQUIRE_EQUAL(notified_uri, "root:managed_readwrite_parameter");
    BOOST_REQUIRE_EQUAL(boost::any_cast<bool>(notified_value), true);

    my_context->unobserve("root:managed_readwrite_parameter");
    my_context->set_parameter("root:managed_readwrite_parameter", boost::any(false));

    BOOST_REQUIRE_NE(boost::any_cast<bool>(notified_value), false);
}

BOOST_FIXTURE_TEST_CASE(observe_external_readonly_parameter, fixture)
{
    BOOST_REQUIRE_EQUAL(external_readonly_parameter.num_slots(), 0);

    my_context->observe("root:external_readonly_parameter",
                        std::bind(&fixture::notify, this, std::placeholders::_1, std::placeholders::_2));

    BOOST_REQUIRE_EQUAL(external_readonly_parameter.num_slots(), 1);

    external_readonly_parameter.m_value = true;
    my_context->tick();

    BOOST_REQUIRE_EQUAL(notified_uri, "root:external_readonly_parameter");
    BOOST_REQUIRE_EQUAL(boost::any_cast<bool>(notified_value), true);

    my_context->unobserve("root:external_readonly_parameter");

    BOOST_REQUIRE_EQUAL(external_readonly_parameter.num_slots(), 0);

    external_readonly_parameter.m_value = false;
    my_context->tick();

    BOOST_REQUIRE_NE(boost::any_cast<bool>(notified_value), false);
}

BOOST_FIXTURE_TEST_CASE(observe_external_readwrite_parameter, fixture)
{
    my_context->observe("root:external_readwrite_parameter",
                        std::bind(&fixture::notify, this, std::placeholders::_1, std::placeholders::_2));
    my_context->set_parameter("root:external_readwrite_parameter", boost::any(true));

    BOOST_REQUIRE_EQUAL(notified_uri, "root:external_readwrite_parameter");
    BOOST_REQUIRE_EQUAL(boost::any_cast<bool>(notified_value), true);

    my_context->unobserve("root:external_readwrite_parameter");
    my_context->set_parameter("root:external_readwrite_parameter", boost::any(false));

    BOOST_REQUIRE_NE(boost::any_cast<bool>(notified_value), false);
}

BOOST_AUTO_TEST_SUITE_END()
