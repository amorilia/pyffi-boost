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

BOOST_AUTO_TEST_SUITE(ast_test_suite)

BOOST_AUTO_TEST_CASE(ast_attr_test)
{
    Attr x("Int", "x");
    BOOST_CHECK_EQUAL(x.name, "x");
    BOOST_CHECK_EQUAL(x.class_name, "Int");
}

BOOST_AUTO_TEST_CASE(ast_class_test)
{
    Class int_("Int");
    BOOST_CHECK_EQUAL(int_.name, "Int");
    BOOST_CHECK(!int_.base_name);
    BOOST_CHECK(!int_.scope);
    int_.base_name = "Object";
    int_.scope = Scope();
    int_.scope.get().push_back(Attr("Bool", "negate"));
    BOOST_CHECK_EQUAL(int_.base_name.get(), "Object");
    BOOST_CHECK_EQUAL(int_.scope.get().size(), 1);
    BOOST_CHECK_EQUAL(get<Attr>(int_.scope.get()[0]).name, "negate");
    BOOST_CHECK_EQUAL(get<Attr>(int_.scope.get()[0]).class_name, "Bool");
}

BOOST_AUTO_TEST_CASE(ast_ifelifelse_test)
{
    IfElifsElse ifelifselse;
    ifelifselse.if_.expr = false;
    ifelifselse.if_.scope.push_back(Attr("Int", "x1"));
    ifelifselse.elifs_.resize(2);
    ifelifselse.elifs_[0].expr = true;
    ifelifselse.elifs_[0].scope.push_back(Attr("Int", "x2"));
    ifelifselse.elifs_[1].expr = true;
    ifelifselse.elifs_[1].scope.push_back(Attr("Int", "x3"));
    ifelifselse.else_.push_back(Attr("Int", "x4"));
}

BOOST_AUTO_TEST_CASE(ast_scope_test)
{
    Scope scope;
    Class int_("Int");
    scope.push_back(int_);
    Class intvec("IVector");
    Attr x("Int", "x");
    Attr y("Int", "y");
    Attr z("Int", "z");
    intvec.scope = Scope();
    intvec.scope.get().push_back(x);
    intvec.scope.get().push_back(y);
    intvec.scope.get().push_back(z);
    scope.push_back(intvec);
    IfElifsElse ifelifselse;
    ifelifselse.if_.expr = true;
    ifelifselse.if_.scope.push_back(Attr("IVector", "position"));
    scope.push_back(ifelifselse);
}

BOOST_AUTO_TEST_CASE(ast_generate_class_test)
{
    Scope scope;
    Class int_("Int");
    scope.push_back(int_);
    std::ostringstream os;
    generate(os, scope);
    BOOST_CHECK_EQUAL(os.str(), "class Int\n");
}

BOOST_AUTO_TEST_CASE(ast_generate_base_name_test)
{
    Scope scope;
    Class int_("Int");
    int_.base_name = "Object";
    scope.push_back(int_);
    std::ostringstream os;
    generate(os, scope);
    BOOST_CHECK_EQUAL(os.str(), "class Int(Object)\n");
}

BOOST_AUTO_TEST_CASE(ast_generate_attr_test)
{
    Scope scope;
    Attr x("Int", "x");
    scope.push_back(x);
    std::ostringstream os;
    generate(os, scope);
    BOOST_CHECK_EQUAL(os.str(), "Int x\n");
}

BOOST_AUTO_TEST_CASE(ast_generate_class_attr_test)
{
    Scope scope;
    Class int_("Int");
    Attr x("Int", "x");
    scope.push_back(int_);
    scope.push_back(x);
    std::ostringstream os;
    generate(os, scope);
    BOOST_CHECK_EQUAL(os.str(), "class Int\nInt x\n");
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
    std::ostringstream os;
    generate(os, scope);
    BOOST_CHECK_EQUAL(os.str(), "class Vector:\n    Int x\n");
}

BOOST_AUTO_TEST_SUITE_END()
