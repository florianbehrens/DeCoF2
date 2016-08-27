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
#include <boost/test/unit_test.hpp>

#include <decof/automatic_ptr.h>

BOOST_AUTO_TEST_SUITE(automatic_pointer)

struct target_t : public decof::automatic_ptr_target<target_t>
{
    size_t test = 0xDEADBEEF;
};

struct fixture
{
    fixture() :
        tgt_uptr(new target_t),
        tgt_aptr(tgt_uptr.get())
    {}

    std::unique_ptr<target_t> tgt_uptr;
    decof::automatic_ptr<target_t> tgt_aptr;
};

BOOST_AUTO_TEST_CASE(default_contructed_automatic_ptr)
{
    decof::automatic_ptr<target_t> tgt_aptr2;
    BOOST_REQUIRE_EQUAL(tgt_aptr2, false);
}

BOOST_FIXTURE_TEST_CASE(simple_automatic_ptr, fixture)
{
    BOOST_REQUIRE_EQUAL(tgt_aptr, true);
    BOOST_REQUIRE(tgt_aptr.get() != nullptr);
    BOOST_REQUIRE_EQUAL(tgt_aptr->test, 0xDEADBEEF);
    BOOST_REQUIRE_EQUAL((*tgt_aptr).test, 0xDEADBEEF);

    tgt_uptr.reset();
    BOOST_REQUIRE_EQUAL(tgt_aptr, false);
}

BOOST_FIXTURE_TEST_CASE(copy_construction, fixture)
{
    decof::automatic_ptr<target_t> tgt_aptr2 = tgt_aptr;
    BOOST_REQUIRE_EQUAL(tgt_aptr, true);
    BOOST_REQUIRE_EQUAL(tgt_aptr2, true);
    BOOST_REQUIRE_EQUAL(tgt_aptr.get(), tgt_aptr2.get());

    tgt_aptr.reset();
    BOOST_REQUIRE_EQUAL(tgt_aptr, false);
    BOOST_REQUIRE_EQUAL(tgt_aptr2, true);
}

BOOST_FIXTURE_TEST_CASE(assignment, fixture)
{
    decof::automatic_ptr<target_t> tgt_aptr2;
    tgt_aptr2 = tgt_aptr;
    BOOST_REQUIRE_EQUAL(tgt_aptr, true);
    BOOST_REQUIRE_EQUAL(tgt_aptr2, true);
    BOOST_REQUIRE_EQUAL(tgt_aptr.get(), tgt_aptr2.get());

    tgt_aptr.reset();
    BOOST_REQUIRE_EQUAL(tgt_aptr, false);
    BOOST_REQUIRE_EQUAL(tgt_aptr2, true);
}

BOOST_FIXTURE_TEST_CASE(target_copy, fixture)
{
    std::unique_ptr<target_t> tgt_uptr2(new target_t(*tgt_uptr));
    BOOST_REQUIRE_EQUAL(tgt_aptr.get(), tgt_uptr.get());
}

BOOST_FIXTURE_TEST_CASE(target_move, fixture)
{
    std::unique_ptr<target_t> tgt_uptr2(new target_t(std::move(*tgt_uptr)));
    BOOST_REQUIRE_EQUAL(tgt_aptr, false);
}

BOOST_AUTO_TEST_SUITE_END()
