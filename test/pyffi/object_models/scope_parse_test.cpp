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

BOOST_AUTO_TEST_SUITE(ast_parse_test_suite)

BOOST_AUTO_TEST_CASE(ast_parse_class_test)
{
    std::istringstream is("class Int\n");
    Scope scope;
    BOOST_CHECK_EQUAL(scope.parse(is), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    BOOST_CHECK_EQUAL(get<Class>(scope[0]).name, "Int");
    BOOST_CHECK(!get<Class>(scope[0]).base_name);
    BOOST_CHECK(!get<Class>(scope[0]).scope);
}

BOOST_AUTO_TEST_CASE(ast_parse_base_name_test)
{
    std::istringstream is("class Int(Object)\n");
    Scope scope;
    BOOST_CHECK_EQUAL(scope.parse(is), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    BOOST_CHECK_EQUAL(get<Class>(scope[0]).name, "Int");
    BOOST_CHECK_EQUAL(get<Class>(scope[0]).base_name.get(), "Object");
    BOOST_CHECK(!get<Class>(scope[0]).scope);
}

BOOST_AUTO_TEST_CASE(ast_parse_attr_test)
{
    std::istringstream is("Int x\n");
    Scope scope;
    BOOST_CHECK_EQUAL(scope.parse(is), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    BOOST_CHECK_EQUAL(get<Attr>(scope[0]).class_name, "Int");
    BOOST_CHECK_EQUAL(get<Attr>(scope[0]).name, "x");
}

BOOST_AUTO_TEST_CASE(ast_parse_class_attr_test)
{
    std::istringstream is("class Int\nInt x\n");
    Scope scope;
    BOOST_CHECK_EQUAL(scope.parse(is), true);
    BOOST_CHECK_EQUAL(scope.size(), 2);
    BOOST_CHECK_EQUAL(get<Class>(scope[0]).name, "Int");
    BOOST_CHECK(!get<Class>(scope[0]).base_name);
    BOOST_CHECK(!get<Class>(scope[0]).scope);
    BOOST_CHECK_EQUAL(get<Attr>(scope[1]).class_name, "Int");
    BOOST_CHECK_EQUAL(get<Attr>(scope[1]).name, "x");
}

BOOST_AUTO_TEST_CASE(ast_parse_class_scope_test)
{
    std::istringstream is("class Vector:\n    Int x\n");
    Scope scope;
    BOOST_CHECK_EQUAL(scope.parse(is), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    BOOST_CHECK_EQUAL(get<Class>(scope[0]).name, "Vector");
    BOOST_CHECK(!get<Class>(scope[0]).base_name);
    BOOST_CHECK(get<Class>(scope[0]).scope);
    Scope & class_scope = get<Class>(scope[0]).scope.get();
    BOOST_CHECK_EQUAL(class_scope.size(), 1);
    BOOST_CHECK_EQUAL(get<Attr>(class_scope[0]).class_name, "Int");
    BOOST_CHECK_EQUAL(get<Attr>(class_scope[0]).name, "x");
}

BOOST_AUTO_TEST_CASE(ast_parse_if_test)
{
    std::istringstream is("if false:\n    Float angle\n");
    Scope scope;
    BOOST_CHECK_EQUAL(scope.parse(is), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    IfElifsElse & ifelifselse = get<IfElifsElse>(scope[0]);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_.size(), 1);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_[0].expr, false);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_[0].scope.size(), 1);
    Attr & attr = get<Attr>(ifelifselse.ifs_[0].scope[0]);
    BOOST_CHECK_EQUAL(attr.class_name, "Float");
    BOOST_CHECK_EQUAL(attr.name, "angle");
    BOOST_CHECK(!ifelifselse.else_);
}

BOOST_AUTO_TEST_CASE(ast_parse_if_else_test)
{
    std::istringstream is("if true:\n    Int64 size\nelse:\n    Int32 offset\n");
    Scope scope;
    BOOST_CHECK_EQUAL(scope.parse(is), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    IfElifsElse & ifelifselse = get<IfElifsElse>(scope[0]);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_.size(), 1);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_[0].expr, true);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_[0].scope.size(), 1);
    Attr & attr1 = get<Attr>(ifelifselse.ifs_[0].scope[0]);
    BOOST_CHECK_EQUAL(attr1.class_name, "Int64");
    BOOST_CHECK_EQUAL(attr1.name, "size");
    BOOST_CHECK(ifelifselse.else_);
    BOOST_CHECK_EQUAL(ifelifselse.else_.get().size(), 1);
    Attr & attr2 = get<Attr>(ifelifselse.else_.get()[0]);
    BOOST_CHECK_EQUAL(attr2.class_name, "Int32");
    BOOST_CHECK_EQUAL(attr2.name, "offset");
}

BOOST_AUTO_TEST_CASE(ast_parse_if_elifs_else_test)
{
    std::istringstream is("if false:\n    Int x1\nelif true:\n    Int x2\nelif false:\n    Int x3\nelif true:\n    Int x4\nelse:\n    Int another_attribute\n");
    Scope scope;
    BOOST_CHECK_EQUAL(scope.parse(is), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    IfElifsElse & ifelifselse = get<IfElifsElse>(scope[0]);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_.size(), 4);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_[0].expr, false);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_[1].expr, true);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_[2].expr, false);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_[3].expr, true);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_[0].scope.size(), 1);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_[1].scope.size(), 1);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_[2].scope.size(), 1);
    BOOST_CHECK_EQUAL(ifelifselse.ifs_[3].scope.size(), 1);
    Attr & attr0 = get<Attr>(ifelifselse.ifs_[0].scope[0]);
    Attr & attr1 = get<Attr>(ifelifselse.ifs_[1].scope[0]);
    Attr & attr2 = get<Attr>(ifelifselse.ifs_[2].scope[0]);
    Attr & attr3 = get<Attr>(ifelifselse.ifs_[3].scope[0]);
    BOOST_CHECK_EQUAL(attr0.class_name, "Int");
    BOOST_CHECK_EQUAL(attr1.class_name, "Int");
    BOOST_CHECK_EQUAL(attr2.class_name, "Int");
    BOOST_CHECK_EQUAL(attr3.class_name, "Int");
    BOOST_CHECK_EQUAL(attr0.name, "x1");
    BOOST_CHECK_EQUAL(attr1.name, "x2");
    BOOST_CHECK_EQUAL(attr2.name, "x3");
    BOOST_CHECK_EQUAL(attr3.name, "x4");
    BOOST_CHECK(ifelifselse.else_);
    BOOST_CHECK_EQUAL(ifelifselse.else_.get().size(), 1);
    Attr & attr4 = get<Attr>(ifelifselse.else_.get()[0]);
    BOOST_CHECK_EQUAL(attr4.class_name, "Int");
    BOOST_CHECK_EQUAL(attr4.name, "another_attribute");
}

BOOST_AUTO_TEST_CASE(ast_parse_doc_oneline_test)
{
    std::istringstream is("class Int:\n    \"\"\"A 32-bit integer.\"\"\"\n");
    Scope scope;
    BOOST_CHECK_EQUAL(scope.parse(is), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    BOOST_CHECK_EQUAL(get<Class>(scope[0]).name, "Int");
    BOOST_CHECK(get<Class>(scope[0]).scope);
    Scope & inner_scope = get<Class>(scope[0]).scope.get();
    BOOST_CHECK_EQUAL(inner_scope.size(), 1);
    Doc & doc = get<Doc>(inner_scope[0]);
    BOOST_CHECK_EQUAL(doc.size(), 1);
    BOOST_CHECK_EQUAL(doc.front(), "A 32-bit integer.");
}

BOOST_AUTO_TEST_CASE(ast_parse_doc_multiline_test_1)
{
    std::istringstream is("class Int:\n    \"\"\"A 32-bit integer.\n    Indeed!\"\"\"\n");
    Scope scope;
    BOOST_CHECK_EQUAL(scope.parse(is), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    BOOST_CHECK_EQUAL(get<Class>(scope[0]).name, "Int");
    BOOST_CHECK(get<Class>(scope[0]).scope);
    Scope & inner_scope = get<Class>(scope[0]).scope.get();
    BOOST_CHECK_EQUAL(inner_scope.size(), 1);
    Doc & doc = get<Doc>(inner_scope[0]);
    BOOST_CHECK_EQUAL(doc.size(), 2);
    BOOST_CHECK_EQUAL(doc.front(), "A 32-bit integer.");
    doc.pop_front();
    BOOST_CHECK_EQUAL(doc.front(), "Indeed!");
}

BOOST_AUTO_TEST_CASE(ast_parse_doc_multiline_test_2)
{
    std::istringstream is("class Int:\n    \"\"\"A 32-bit integer.\n    Indeed!\n    \"\"\"\n");
    Scope scope;
    BOOST_CHECK_EQUAL(scope.parse(is), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    BOOST_CHECK_EQUAL(get<Class>(scope[0]).name, "Int");
    BOOST_CHECK(get<Class>(scope[0]).scope);
    Scope & inner_scope = get<Class>(scope[0]).scope.get();
    BOOST_CHECK_EQUAL(inner_scope.size(), 1);
    Doc & doc = get<Doc>(inner_scope[0]);
    BOOST_CHECK_EQUAL(doc.size(), 2);
    BOOST_CHECK_EQUAL(doc.front(), "A 32-bit integer.");
    doc.pop_front();
    BOOST_CHECK_EQUAL(doc.front(), "Indeed!");
}

BOOST_AUTO_TEST_CASE(ast_parse_doc_multiline_test_3)
{
    std::istringstream is("class Int:\n    \"\"\"A 32-bit integer.\n    Indeed!\n    \n    \"\"\"\n");
    Scope scope;
    BOOST_CHECK_EQUAL(scope.parse(is), true);
    BOOST_CHECK_EQUAL(scope.size(), 1);
    BOOST_CHECK_EQUAL(get<Class>(scope[0]).name, "Int");
    BOOST_CHECK(get<Class>(scope[0]).scope);
    Scope & inner_scope = get<Class>(scope[0]).scope.get();
    BOOST_CHECK_EQUAL(inner_scope.size(), 1);
    Doc & doc = get<Doc>(inner_scope[0]);
    BOOST_CHECK_EQUAL(doc.size(), 2);
    BOOST_CHECK_EQUAL(doc.front(), "A 32-bit integer.");
    doc.pop_front();
    BOOST_CHECK_EQUAL(doc.front(), "Indeed!");
}

BOOST_AUTO_TEST_SUITE_END()
