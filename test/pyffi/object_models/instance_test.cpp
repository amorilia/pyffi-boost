/*

Copyright (c) 2007-2010, Python File Format Interface
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above
     copyright notice, this list of conditions and the following
     disclaimer in the documentation and/or other materials provided
     with the distribution.

   * Neither the name of the Python File Format Interface
     project nor the names of its contributors may be used to endorse
     or promote products derived from this software without specific
     prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include "pyffi/object_models/instance.hpp"

using namespace pyffi;
using namespace pyffi::object_models;

BOOST_AUTO_TEST_SUITE(instance_test_suite)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    Class Int("Int");
    // cannot instantiate without init method
    BOOST_CHECK_THROW(Instance x(Int), std::runtime_error);
    Int.set_type<int>();
    BOOST_CHECK_NO_THROW(Instance x(Int));
}

BOOST_AUTO_TEST_CASE(get_test)
{
    Class Short("Short");
    Short.set_type<short>();

    // check that get returns assigned value
    Instance obj_short(Short);
    BOOST_CHECK_EQUAL(obj_short.get<short>(), 0);

    // check bad type casts
    BOOST_CHECK_THROW(obj_short.get<int>(), std::runtime_error);
    BOOST_CHECK_THROW(obj_short.get<float>(), std::runtime_error);
    BOOST_CHECK_THROW(obj_short.get<char>(), std::runtime_error);
    BOOST_CHECK_THROW(obj_short.get<std::string>(), std::runtime_error);

    // check assignment via get
    obj_short.get<short>() = 10;
    BOOST_CHECK_EQUAL(obj_short.get<short>(), 10);
}

BOOST_AUTO_TEST_CASE(write_test)
{
    Class Int("Int");
    Int.set_type<int>();
    Instance x(Int);
    x.get<int>() = 1145258561;
    std::ostringstream os;
    x.write(os);
    BOOST_CHECK_EQUAL(os.str(), "ABCD");
}

BOOST_AUTO_TEST_CASE(read_test)
{
    Class Int("Int");
    Int.set_type<int>();
    Instance x(Int);
    x.get<int>() = 1145258561;
    std::istringstream is("ABCD");
    x.read(is);
    BOOST_CHECK_EQUAL(x.get<int>(), 1145258561);
}

BOOST_AUTO_TEST_SUITE_END()
