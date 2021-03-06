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
    ifelifselse.ifs_.resize(3);
    ifelifselse.ifs_[0].expr = false;
    ifelifselse.ifs_[0].scope.push_back(Attr("Int", "x1"));
    ifelifselse.ifs_[1].expr = true;
    ifelifselse.ifs_[1].scope.push_back(Attr("Int", "x2"));
    ifelifselse.ifs_[2].expr = true;
    ifelifselse.ifs_[2].scope.push_back(Attr("Int", "x3"));
    ifelifselse.else_ = Scope();
    ifelifselse.else_.get().push_back(Attr("Int", "x4"));
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
    ifelifselse.ifs_.resize(1);
    ifelifselse.ifs_[0].expr = true;
    ifelifselse.ifs_[0].scope.push_back(Attr("IVector", "position"));
    scope.push_back(ifelifselse);
}

BOOST_AUTO_TEST_CASE(ast_compile_base_class_test)
{
    Scope scope;
    {
        // keep scope construction local; test should use scope only
        Class Vec("Vec");
        Class Color("Color");
        Color.base_name = "Vec";
        scope.push_back(Vec);
        scope.push_back(Color);
    }

    scope.compile();

    // check that references are not set
    Class & Vec = get<Class>(scope[0]);
    Class & Color = get<Class>(scope[1]);
    BOOST_CHECK_EQUAL(&Color.get_base_class().get(), &Vec);
}

BOOST_AUTO_TEST_CASE(ast_compile_complex_test)
{
    Scope scope;
    // keep scope construction local; test should use scope only
    {
        // class Int
        // class Float
        Class Int("Int");
        Class Float("Float");
        Class Vec("Vec");
        Attr x("Float", "x");
        Attr y("Int", "y");
        Attr z("Int", "z");
        // class Vec:
        //     Float x
        //     Int y
        //     Int z
        Vec.scope = Scope();
        Vec.scope.get().push_back(Float); // pushes a *copy*!!!
        Vec.scope.get().push_back(x); // pushes a *copy*!!!
        Vec.scope.get().push_back(y); // pushes a *copy*!!!
        Vec.scope.get().push_back(z); // pushes a *copy*!!!
        // if true:
        //     class Bool
        //     Vec pos
        //     Bool is_local
        IfElifsElse ifelifselse;
        Class Bool("Bool");
        Attr pos("Vec", "pos");
        Attr is_local("Bool", "is_local");
        ifelifselse.ifs_.resize(1);
        ifelifselse.ifs_[0].expr = true;
        ifelifselse.ifs_[0].scope.push_back(Bool);
        ifelifselse.ifs_[0].scope.push_back(pos);
        ifelifselse.ifs_[0].scope.push_back(is_local);
        // else:
        //     class Color(Vec)
        //     Color col
        Class Color("Color");
        Color.base_name = "Vec";
        Attr col("Color", "col");
        ifelifselse.else_ = Scope();
        ifelifselse.else_.get().push_back(Color);
        ifelifselse.else_.get().push_back(col);
        scope.push_back(Int); // pushes a *copy*!!!
        // Float is defined as a nested class inside Vec; see above
        // Bool is defined as a nested class inside the if; see above
        scope.push_back(Vec); // pushes a *copy*!!!
        scope.push_back(ifelifselse); // pushes a *copy*!!!
    }

    // check that classes and indices are not set
    Class & Int = get<Class>(scope[0]);
    Class & Vec = get<Class>(scope[1]);
    Class & Float = get<Class>(Vec.scope.get()[0]);
    Attr & x = get<Attr>(Vec.scope.get()[1]);
    Attr & y = get<Attr>(Vec.scope.get()[2]);
    Attr & z = get<Attr>(Vec.scope.get()[3]);
    IfElifsElse & ifelifselse = get<IfElifsElse>(scope[2]);
    Class & Bool = get<Class>(ifelifselse.ifs_[0].scope[0]);
    Attr & pos = get<Attr>(ifelifselse.ifs_[0].scope[1]);
    Attr & is_local = get<Attr>(ifelifselse.ifs_[0].scope[2]);
    Class & Color = get<Class>(ifelifselse.else_.get()[0]);
    Attr & col = get<Attr>(ifelifselse.else_.get()[1]);
    BOOST_CHECK_THROW(x.get_class(), std::runtime_error);
    BOOST_CHECK_THROW(y.get_class(), std::runtime_error);
    BOOST_CHECK_THROW(z.get_class(), std::runtime_error);
    BOOST_CHECK_THROW(pos.get_class(), std::runtime_error);
    BOOST_CHECK_THROW(is_local.get_class(), std::runtime_error);
    BOOST_CHECK_THROW(col.get_class(), std::runtime_error);
    BOOST_CHECK_THROW(x.get_index(), std::runtime_error);
    BOOST_CHECK_THROW(y.get_index(), std::runtime_error);
    BOOST_CHECK_THROW(z.get_index(), std::runtime_error);
    BOOST_CHECK_THROW(pos.get_index(), std::runtime_error);
    BOOST_CHECK_THROW(is_local.get_index(), std::runtime_error);
    BOOST_CHECK_THROW(col.get_index(), std::runtime_error);

    // compile the scope
    scope.compile();

    // check local classes
    BOOST_CHECK_EQUAL(&scope.get_local_class("Int"), &Int);
    BOOST_CHECK_EQUAL(&scope.get_local_class("Vec"), &Vec);
    BOOST_CHECK_EQUAL(&Vec.scope.get().get_local_class("Float"), &Float);
    BOOST_CHECK_EQUAL(&ifelifselse.ifs_[0].scope.get_local_class("Bool"), &Bool);
    BOOST_CHECK_EQUAL(&ifelifselse.else_.get().get_local_class("Color"), &Color);

    // check parent scopes
    BOOST_CHECK_THROW(scope.get_parent_scope(), std::runtime_error);
    BOOST_CHECK_EQUAL(&Vec.scope.get().get_parent_scope(), &scope);
    BOOST_CHECK_EQUAL(&ifelifselse.ifs_[0].scope.get_parent_scope(), &scope);
    BOOST_CHECK_EQUAL(&ifelifselse.else_.get().get_parent_scope(), &scope);

    // check class of each attribute
    BOOST_CHECK_EQUAL(&x.get_class(), &Float);
    BOOST_CHECK_EQUAL(&y.get_class(), &Int);
    BOOST_CHECK_EQUAL(&z.get_class(), &Int);
    BOOST_CHECK_EQUAL(&pos.get_class(), &Vec);
    BOOST_CHECK_EQUAL(&is_local.get_class(), &Bool);
    BOOST_CHECK_EQUAL(&col.get_class(), &Color);
    // check index of each attribute
    BOOST_CHECK_EQUAL(x.get_index(), 0);
    BOOST_CHECK_EQUAL(y.get_index(), 1);
    BOOST_CHECK_EQUAL(z.get_index(), 2);
    BOOST_CHECK_EQUAL(pos.get_index(), 0);
    BOOST_CHECK_EQUAL(is_local.get_index(), 1);
    BOOST_CHECK_EQUAL(col.get_index(), 2);

    // check base class
    BOOST_CHECK_EQUAL(&Color.get_base_class().get(), &Vec);
}

BOOST_AUTO_TEST_SUITE_END()
