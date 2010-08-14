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

#include "pyffi/object_models/ast.hpp"

using boost::get;
using namespace pyffi;
using namespace pyffi::object_models;

BOOST_AUTO_TEST_SUITE(ast_parse_test_suite)

BOOST_AUTO_TEST_CASE(ast_parse_class_test)
{
    std::istringstream is("class Int\n");
    Scope scope;
    BOOST_CHECK_EQUAL(parse(is, scope), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    BOOST_CHECK_EQUAL(get<Class>(scope[0]).name, "Int");
    BOOST_CHECK(!get<Class>(scope[0]).base_name);
    BOOST_CHECK(!get<Class>(scope[0]).scope);
}

BOOST_AUTO_TEST_CASE(ast_parse_attr_test)
{
    std::istringstream is("Int x\n");
    Scope scope;
    BOOST_CHECK_EQUAL(parse(is, scope), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    BOOST_CHECK_EQUAL(get<Attr>(scope[0]).class_name, "Int");
    BOOST_CHECK_EQUAL(get<Attr>(scope[0]).name, "x");
}

BOOST_AUTO_TEST_SUITE_END()
