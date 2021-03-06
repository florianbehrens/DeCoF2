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

BOOST_AUTO_TEST_SUITE(parameter_access)

using decof::value_t;

struct fixture
{
    struct my_context_t : public decof::client_context
    {
        using decof::client_context::client_context;

        void get_parameter(const std::string& uri, char separator = ':')
        {
            const auto obj = object_dictionary_.find_object(uri, separator);
            decof::client_context::get_parameter(obj);
        }

        void set_parameter(const std::string& uri, const value_t& value, char separator = ':')
        {
            const auto obj = object_dictionary_.find_object(uri, separator);
            decof::client_context::set_parameter(obj, value);
        }
    };

    struct external_readonly_parameter_t : public decof::external_readonly_parameter<bool>
    {
        using decof::external_readonly_parameter<bool>::external_readonly_parameter;

        bool external_value() const override
        {
            return true;
        }
    };

    struct external_readwrite_parameter_t : public decof::external_readwrite_parameter<bool>
    {
        using decof::external_readwrite_parameter<bool>::external_readwrite_parameter;

        bool external_value(const bool& value) override
        {
            return true;
        }

        bool external_value() const override
        {
            return true;
        }
    };

    struct writeonly_parameter_t : public decof::writeonly_parameter<bool>
    {
        using decof::writeonly_parameter<bool>::writeonly_parameter;

        void value(const bool& value) override
        {
        }
    };

    fixture()
      : managed_readwrite_parameter("managed_readwrite_parameter", &obj_dict),
        managed_readonly_parameter("managed_readonly_parameter", &obj_dict),
        external_readwrite_parameter("external_readwrite_parameter", &obj_dict),
        external_readonly_parameter("external_readonly_parameter", &obj_dict),
        writeonly_parameter("writeonly_parameter", &obj_dict),
        my_context(new my_context_t(obj_dict))
    {
    }

    decof::object_dictionary                 obj_dict;
    decof::managed_readwrite_parameter<bool> managed_readwrite_parameter;
    decof::managed_readonly_parameter<bool>  managed_readonly_parameter;
    external_readwrite_parameter_t           external_readwrite_parameter;
    external_readonly_parameter_t            external_readonly_parameter;
    writeonly_parameter_t                    writeonly_parameter;
    std::shared_ptr<my_context_t>            my_context;
};

BOOST_FIXTURE_TEST_CASE(read_managed_readwrite_parameter, fixture)
{
    try {
        my_context->get_parameter("root:managed_readwrite_parameter");
    } catch (std::exception& ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_FIXTURE_TEST_CASE(write_managed_readwrite_parameter, fixture)
{
    try {
        my_context->set_parameter("root:managed_readwrite_parameter", false);
    } catch (std::exception& ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_FIXTURE_TEST_CASE(read_managed_readonly_parameter, fixture)
{
    try {
        my_context->get_parameter("root:managed_readonly_parameter");
    } catch (std::exception& ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_FIXTURE_TEST_CASE(write_managed_readonly_parameter, fixture)
{
    bool write_failed = false;

    try {
        my_context->set_parameter("root:managed_readonly_parameter", false);
    } catch (decof::invalid_parameter_error&) {
        write_failed = true;
    }

    BOOST_REQUIRE_EQUAL(write_failed, true);
}

BOOST_FIXTURE_TEST_CASE(read_external_readwrite_parameter, fixture)
{
    try {
        my_context->get_parameter("root:external_readwrite_parameter");
    } catch (std::exception& ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_FIXTURE_TEST_CASE(write_external_readwrite_parameter, fixture)
{
    try {
        my_context->set_parameter("root:external_readwrite_parameter", false);
    } catch (std::exception& ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_FIXTURE_TEST_CASE(read_external_readonly_parameter, fixture)
{
    try {
        my_context->get_parameter("root:external_readonly_parameter");
    } catch (std::exception& ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_FIXTURE_TEST_CASE(write_external_readonly_parameter, fixture)
{
    bool write_failed = false;

    try {
        my_context->set_parameter("root:external_readonly_parameter", false);
    } catch (decof::invalid_parameter_error&) {
        write_failed = true;
    }

    BOOST_REQUIRE_EQUAL(write_failed, true);
}

BOOST_FIXTURE_TEST_CASE(read_writeonly_parameter, fixture)
{
    bool read_failed = false;

    try {
        my_context->get_parameter("root:writeonly_parameter");
    } catch (decof::invalid_parameter_error&) {
        read_failed = true;
    }

    BOOST_REQUIRE_EQUAL(read_failed, true);
}

BOOST_FIXTURE_TEST_CASE(write_writeonly_parameter, fixture)
{
    try {
        my_context->set_parameter("root:writeonly_parameter", false);
    } catch (std::exception& ex) {
        BOOST_FAIL(ex.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception occurred");
    }
}

BOOST_AUTO_TEST_SUITE_END()
