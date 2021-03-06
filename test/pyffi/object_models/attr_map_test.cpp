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
#include <boost/test/unit_test.hpp>

#include "pyffi/object_models/attr_map.hpp"

using boost::get;
using namespace pyffi;
using namespace pyffi::object_models;

BOOST_AUTO_TEST_SUITE(attr_map_test_suite)

BOOST_AUTO_TEST_CASE(attr_map_simple_test)
{
    Attr x("Int", "x");
    Attr y("Int", "y");
    Attr x2("Int", "x");

    AttrMap map;
    BOOST_CHECK_EQUAL(map.size(), 0);
    map.push_back(x);
    map.push_back(y);
    BOOST_CHECK_EQUAL(map.size(), 2);
    BOOST_CHECK_EQUAL(&map["x"], &x);
    BOOST_CHECK_EQUAL(&map["y"], &y);
    BOOST_CHECK_THROW(&map["z"], std::runtime_error);

    AttrMap::const_iterator iter = map.begin();
    BOOST_CHECK_EQUAL(&(*iter), &x);
    BOOST_CHECK_NO_THROW(++iter);
    BOOST_CHECK_EQUAL(&(*iter), &y);
    BOOST_CHECK_NO_THROW(++iter);
    BOOST_CHECK(iter == map.end()); // cout << iter does not work, so use CHECK rather than CHECK_EQUAL

    map.push_back(x2); // same as "x", does nothing
    BOOST_CHECK_EQUAL(map.size(), 2);
    BOOST_CHECK_EQUAL(&map["x"], &x);
}

BOOST_AUTO_TEST_SUITE_END()
