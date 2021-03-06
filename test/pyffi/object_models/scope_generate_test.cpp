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

class GenerateParseFixture
{
public:
    GenerateParseFixture(Scope const & scope, std::string const & str) : scope(scope), str(str) {
        std::ostringstream os;
        BOOST_CHECK_EQUAL(scope.generate(os), true);
        BOOST_CHECK_EQUAL(os.str(), str);
        std::istringstream is(str);
        Scope scope_parsed;
        BOOST_CHECK_EQUAL(scope_parsed.parse(is), true);
        //BOOST_CHECK_EQUAL(scope_parsed, scope);
        BOOST_CHECK(scope_parsed == scope);
    };
    Scope scope;
    std::string str;
};

BOOST_AUTO_TEST_SUITE(ast_generate_test_suite)

BOOST_AUTO_TEST_CASE(ast_generate_class_test)
{
    Scope scope;
    Class int_("Int");
    scope.push_back(int_);
    GenerateParseFixture(scope, "class Int\n");
}

BOOST_AUTO_TEST_CASE(ast_generate_base_name_test)
{
    Scope scope;
    Class int_("Int");
    int_.base_name = "Object";
    scope.push_back(int_);
    GenerateParseFixture(scope, "class Int(Object)\n");
}

BOOST_AUTO_TEST_CASE(ast_generate_attr_test)
{
    Scope scope;
    Attr x("Int", "x");
    scope.push_back(x);
    GenerateParseFixture(scope, "Int x\n");
}

BOOST_AUTO_TEST_CASE(ast_parse_attr_doc_test)
{
    Scope scope;
    Attr x("Int", "x");
    Doc doc;
    doc.push_back("Hello world.");
    x.doc = doc;
    scope.push_back(x);
    GenerateParseFixture(scope, "Int x\n    \"\"\"Hello world.\"\"\"\n");
}

BOOST_AUTO_TEST_CASE(ast_generate_class_attr_test)
{
    Scope scope;
    Class int_("Int");
    Attr x("Int", "x");
    scope.push_back(int_);
    scope.push_back(x);
    GenerateParseFixture(scope, "class Int\nInt x\n");
}

BOOST_AUTO_TEST_CASE(ast_generate_class_scope_test)
{
    Class vec("Vector");
    Attr x("Int", "x");
    vec.scope = Scope();
    vec.scope.get().push_back(x);
    BOOST_CHECK_EQUAL(vec.scope.get().size(), 1);
    Scope scope;
    scope.push_back(vec);
    GenerateParseFixture(scope, "class Vector\n    Int x\n");
}

BOOST_AUTO_TEST_CASE(ast_generate_if_test)
{
    IfElifsElse ifelifselse;
    ifelifselse.ifs_.resize(1);
    ifelifselse.ifs_[0].expr = false;
    ifelifselse.ifs_[0].scope.push_back(Attr("Float", "angle"));
    Scope scope;
    scope.push_back(ifelifselse);
    GenerateParseFixture(scope, "if false\n    Float angle\n");
}

BOOST_AUTO_TEST_CASE(ast_generate_if_else_test)
{
    IfElifsElse ifelifselse;
    ifelifselse.ifs_.resize(1);
    ifelifselse.ifs_[0].expr = true;
    ifelifselse.ifs_[0].scope.push_back(Attr("Int64", "size"));
    ifelifselse.else_ = Scope();
    ifelifselse.else_.get().push_back(Attr("Int32", "offset"));
    Scope scope;
    scope.push_back(ifelifselse);
    GenerateParseFixture(scope, "if true\n    Int64 size\nelse\n    Int32 offset\n");
}

BOOST_AUTO_TEST_CASE(ast_generate_if_elifs_else_test)
{
    IfElifsElse ifelifselse;
    ifelifselse.ifs_.resize(4);
    ifelifselse.ifs_[0].expr = false;
    ifelifselse.ifs_[0].scope.push_back(Attr("Int", "x1"));
    ifelifselse.ifs_[1].expr = true;
    ifelifselse.ifs_[1].scope.push_back(Attr("Int", "x2"));
    ifelifselse.ifs_[2].expr = false;
    ifelifselse.ifs_[2].scope.push_back(Attr("Int", "x3"));
    ifelifselse.ifs_[3].expr = true;
    ifelifselse.ifs_[3].scope.push_back(Attr("Int", "x4"));
    ifelifselse.else_ = Scope();
    ifelifselse.else_.get().push_back(Attr("Int", "another_attribute"));
    Scope scope;
    scope.push_back(ifelifselse);
    GenerateParseFixture(scope, "if false\n    Int x1\nelif true\n    Int x2\nelif false\n    Int x3\nelif true\n    Int x4\nelse\n    Int another_attribute\n");
}

BOOST_AUTO_TEST_CASE(ast_generate_doc_oneline_test)
{
    Scope scope;
    Class int_("Int");
    Doc doc;
    doc.push_back("A 32-bit integer.");
    int_.doc = doc;
    scope.push_back(int_);
    GenerateParseFixture(scope, "class Int\n    \"\"\"A 32-bit integer.\"\"\"\n");
}

BOOST_AUTO_TEST_CASE(ast_generate_doc_multiline_test)
{
    Scope scope;
    Class int_("Int");
    Doc doc;
    doc.push_back("A 32-bit integer.");
    doc.push_back("Indeed!");
    int_.doc = doc;
    scope.push_back(int_);
    GenerateParseFixture(scope, "class Int\n    \"\"\"A 32-bit integer.\n    Indeed!\n    \"\"\"\n");
}

BOOST_AUTO_TEST_SUITE_END()
