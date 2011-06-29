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

#include "pyffi/object_models/scope.hpp"

using boost::get;
using namespace pyffi;
using namespace pyffi::object_models;

class ParseFixture
{
public:
    ParseFixture() : scope() {};
    void parse_check_equal(std::string const & str) const {
        std::istringstream is(str);
        Scope scope_parsed;
        BOOST_CHECK_EQUAL(scope_parsed.parse(is), true);
        //BOOST_CHECK_EQUAL(scope_parsed, scope);
        BOOST_CHECK(scope_parsed == scope);
    };
    Scope scope;
};

BOOST_AUTO_TEST_SUITE(ast_parse_test_suite)

BOOST_FIXTURE_TEST_CASE(ast_parse_doc_multiline_test, ParseFixture)
{
    Class int_("Int");
    Doc doc;
    doc.push_back("A 32-bit integer.");
    doc.push_back("Indeed!");
    int_.doc = doc;
    scope.push_back(int_);
    parse_check_equal("class Int\n    \"\"\"A 32-bit integer.\n    Indeed!\"\"\"\n");
    parse_check_equal("class Int\n    \"\"\"A 32-bit integer.\n    Indeed!\n    \n    \"\"\"\n");
}

BOOST_FIXTURE_TEST_CASE(ast_parse_blank_lines_test_1, ParseFixture)
{
    scope.push_back(Class("Vector"));
    parse_check_equal("class Vector");
    parse_check_equal("\nclass Vector");
    parse_check_equal(" \n  \n     \n\n   \nclass Vector");
    parse_check_equal("class Vector   ");
    parse_check_equal("class Vector   \n");
    parse_check_equal("class Vector\n  \n     \n\n   \n");
}

BOOST_FIXTURE_TEST_CASE(ast_parse_blank_lines_test_2, ParseFixture)
{
    Class vec("Vector");
    vec.scope = Scope();
    vec.scope.get().push_back(Attr("Int", "x"));
    vec.scope.get().push_back(Attr("Int", "y"));
    vec.scope.get().push_back(Attr("Int", "z"));
    scope.push_back(vec);
    parse_check_equal("class Vector\n\n    Int x\n\n  \n    Int y\n      \n    Int z");
}

BOOST_AUTO_TEST_SUITE_END()
