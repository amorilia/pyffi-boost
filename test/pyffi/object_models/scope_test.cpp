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

/**

\file

Regression tests for pyffi::object_models::Scope.

*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "pyffi/object_models/scope.hpp"

using boost::get;
using namespace pyffi;
using namespace pyffi::object_models;

BOOST_AUTO_TEST_SUITE(scope_test_suite)

//! Check that scopes can be created.
BOOST_AUTO_TEST_CASE(scope_create_test)
{
	BOOST_CHECK_NO_THROW(Scope::create());
}

//! Check that classes can be created.
BOOST_AUTO_TEST_CASE(class_create_test)
{
	PClass cls;
	BOOST_CHECK_NO_THROW(cls = Class::create("TestClass"));
	BOOST_CHECK_EQUAL(cls->name, "TestClass");
}

//! Check that attributes can be created.
BOOST_AUTO_TEST_CASE(attr_create_test)
{
	PAttr attr;
	BOOST_CHECK_NO_THROW(attr = Attr::create("TestClass", "test"));
	BOOST_CHECK_EQUAL(attr->class_name, "TestClass");
	BOOST_CHECK_EQUAL(attr->name, "test");
}

//! Check that scope class declaration syntax.
BOOST_AUTO_TEST_CASE(scope_create_class_test)
{
	PScope scope;
	// define some classes
	BOOST_CHECK_NO_THROW(
	    scope =
	        Scope::create()
	        ->class_("TestClass1")
	        ->class_("TestClass2")
	);
	// ... do some stuff ...
	// declare another class
	BOOST_CHECK_NO_THROW(scope->class_("TestClass3"));
	// check scope declarations
	BOOST_CHECK_EQUAL(scope->declarations.size(), 3);
	PClass cls;
	BOOST_CHECK_NO_THROW(cls = get<PClass>(scope->declarations[0]))
	BOOST_CHECK_EQUAL(cls->name, "TestClass1");
	BOOST_CHECK_NO_THROW(cls = get<PClass>(scope->declarations[1]))
	BOOST_CHECK_EQUAL(cls->name, "TestClass2");
	BOOST_CHECK_NO_THROW(cls = get<PClass>(scope->declarations[2]))
	BOOST_CHECK_EQUAL(cls->name, "TestClass3");
}

//! Check that scope class declaration syntax.
BOOST_AUTO_TEST_CASE(scope_create_attr_test)
{
	PScope scope;
	// define some classes
	BOOST_CHECK_NO_THROW(
	    scope =
	        Scope::create()
	        ->class_("Float")
	        ->attr("Float", "x")
	        ->attr("Float", "y")
	        ->attr("Float", "z")
	        ->class_("Int")
	        ->attr("Float", "time")
	        ->attr("Int", "num_vertices")
	);
	// check scope declarations
	BOOST_CHECK_EQUAL(scope->declarations.size(), 7);
	PClass cls;
	PAttr attr;
	BOOST_CHECK_NO_THROW(cls = get<PClass>(scope->declarations[0]))
	BOOST_CHECK_EQUAL(cls->name, "Float");
	BOOST_CHECK_NO_THROW(attr = get<PAttr>(scope->declarations[1]));
	BOOST_CHECK_EQUAL(attr->class_name, "Float");
	BOOST_CHECK_EQUAL(attr->name, "x");
	BOOST_CHECK_NO_THROW(attr = get<PAttr>(scope->declarations[2]));
	BOOST_CHECK_EQUAL(attr->class_name, "Float");
	BOOST_CHECK_EQUAL(attr->name, "y");
	BOOST_CHECK_NO_THROW(attr = get<PAttr>(scope->declarations[3]));
	BOOST_CHECK_EQUAL(attr->class_name, "Float");
	BOOST_CHECK_EQUAL(attr->name, "z");
	BOOST_CHECK_NO_THROW(cls = get<PClass>(scope->declarations[4]))
	BOOST_CHECK_EQUAL(cls->name, "Int");
	BOOST_CHECK_NO_THROW(attr = get<PAttr>(scope->declarations[5]));
	BOOST_CHECK_EQUAL(attr->class_name, "Float");
	BOOST_CHECK_EQUAL(attr->name, "time");
	BOOST_CHECK_NO_THROW(attr = get<PAttr>(scope->declarations[6]));
	BOOST_CHECK_EQUAL(attr->class_name, "Int");
	BOOST_CHECK_EQUAL(attr->name, "num_vertices");
}

//! Check declaration of scopes within classes.
BOOST_AUTO_TEST_CASE(class_scope_test)
{
	PScope scope;
	// define a scope within a class
	BOOST_CHECK_NO_THROW(
	    scope =
	        Scope::create()
	        ->class_("Int")
	        ->class_("TestClass")->scope(
	            Scope::create()
	            ->attr("Int", "a")
	            ->attr("Int", "b")
	        )
	);
	PClass cls;
	BOOST_CHECK_EQUAL(scope->declarations.size(), 2);
	BOOST_CHECK_NO_THROW(cls = get<PClass>(scope->declarations[0]));
	BOOST_CHECK_EQUAL(cls->scope, PScope());
	BOOST_CHECK_NO_THROW(cls = get<PClass>(scope->declarations[1]));
	BOOST_CHECK_EQUAL(scope, cls->parent.lock());
	PScope cls_scope = cls->scope;
	BOOST_CHECK_EQUAL(
	    cls, boost::apply_visitor(
	        Scope::get_declaration_visitor<Class>(), cls_scope->parent));
	BOOST_CHECK_EQUAL(cls_scope->declarations.size(), 2);
	PAttr attr;
	BOOST_CHECK_NO_THROW(attr = get<PAttr>(cls_scope->declarations[0]));
	BOOST_CHECK_EQUAL(attr->class_name, "Int");
	BOOST_CHECK_EQUAL(attr->name, "a");
	BOOST_CHECK_NO_THROW(attr = get<PAttr>(cls_scope->declarations[1]));
	BOOST_CHECK_EQUAL(attr->class_name, "Int");
	BOOST_CHECK_EQUAL(attr->name, "b");
}

//! Check that attributes cannot have scopes.
BOOST_AUTO_TEST_CASE(attr_scope_test)
{
	BOOST_CHECK_THROW(
	    Scope::create()
	    ->class_("Int")
	    ->attr("Int", "a")->scope(
	        Scope::create()
	        ->class_("Float")
	    ),
	    syntax_error
	);
}

//! Check scope visitation.
BOOST_AUTO_TEST_CASE(visitor_test)
{
	PScope scope =
	    Scope::create() // create a scope
	    ->class_("Test"); // declare a class
	Scope::Declaration decl = scope->declarations[0];
	PAttr attr = boost::apply_visitor(
	                 Scope::get_declaration_visitor<Attr>(), decl);
	BOOST_CHECK(!attr); // first declaration is not an attribute
	PClass cls = boost::apply_visitor(
	                 Scope::get_declaration_visitor<Class>(), decl);
	BOOST_CHECK(cls); // first declaration is a class
	BOOST_CHECK_EQUAL(cls->name, "Test");
	BOOST_CHECK_EQUAL(
	    scope,
	    boost::apply_visitor(
	        Scope::get_parent_scope_visitor(), decl));
}

//! Check if we can get the classes by name.
BOOST_AUTO_TEST_CASE(get_class_test)
{
	PScope scope;
	// define various classes
	BOOST_CHECK_NO_THROW(
	    scope =
	        Scope::create()
	        ->class_("Int")
	        ->class_("Test")->scope(
	            Scope::create()
	            ->class_("Float")
	            ->class_("Char")
	        )
	);
	PClass cls_int, cls_test, cls_float, cls_char;
	BOOST_CHECK_NO_THROW(cls_int = scope->get_class("Int"));
	BOOST_CHECK_NO_THROW(cls_test = scope->get_class("Test"));
	// Float is not in scope!!
	BOOST_CHECK_NO_THROW(cls_float = scope->get_class("Float"));
	// Char is not in scope!!
	BOOST_CHECK_NO_THROW(cls_char = scope->get_class("Char"));
	BOOST_CHECK(cls_int);
	BOOST_CHECK_EQUAL(cls_int->name, "Int");
	BOOST_CHECK(cls_test);
	BOOST_CHECK_EQUAL(cls_test->name, "Test");
	BOOST_CHECK_EQUAL(cls_float, PClass());
	BOOST_CHECK_EQUAL(cls_char, PClass());
	// double check nested scope, and parentship
	PScope test_scope = cls_test->scope;
	BOOST_CHECK(test_scope);
	BOOST_CHECK_EQUAL(scope, cls_test->parent.lock());
	// inspect scope within test class
	BOOST_CHECK_EQUAL(cls_int, test_scope->get_class("Int"));
	BOOST_CHECK_EQUAL(cls_test, test_scope->get_class("Test"));
	BOOST_CHECK_NO_THROW(cls_float = test_scope->get_class("Float"));
	BOOST_CHECK_NO_THROW(cls_char = test_scope->get_class("Char"));
	BOOST_CHECK(cls_float);
	BOOST_CHECK_EQUAL(cls_float->name, "Float");
	BOOST_CHECK(cls_char);
	BOOST_CHECK_EQUAL(cls_char->name, "Char");
	// destroy the parent scope and check scope changes
	scope.reset(); // nulls the pointer, effectively deleting this scope
	BOOST_CHECK_EQUAL(PClass(), test_scope->get_class("Int"));
	BOOST_CHECK_EQUAL(PClass(), test_scope->get_class("Test"));
	// test_scope is staying alive, so Float and Char should still exist
	BOOST_CHECK_EQUAL(cls_float, test_scope->get_class("Float"));
	BOOST_CHECK_EQUAL(cls_char, test_scope->get_class("Char"));
}

//! Test base class syntax.
BOOST_AUTO_TEST_CASE(scope_base_class_test)
{
	PScope scope;
	// define various classes
	BOOST_CHECK_NO_THROW(
	    scope =
	        Scope::create()
	        ->class_("Int")
	        ->class_("Test")->base_class("Int")
	        ->class_("X")
	);
	PClass cls_int = scope->get_class("Int");
	PClass cls_test = scope->get_class("Test");
	BOOST_CHECK_EQUAL(cls_test->base_class.lock(), cls_int);
}

//! Check if we can get attributes by name.
BOOST_AUTO_TEST_CASE(get_attr_test)
{
	// define various classes
	PScope scope =
	    Scope::create()
	    ->class_("Int")
	    ->class_("A")->scope(
	        Scope::create()
	        ->attr("Int", "a")
	        ->attr("Int", "b")
	    )
	    ->class_("B")->base_class("A")->scope(
	        Scope::create()
	        ->attr("Int", "c")
	        ->attr("Int", "d")
	    );
	PClass base_cls = scope->get_class("A");
	PClass derived_cls = scope->get_class("B");
	PAttr attr_a, attr_b, attr_c, attr_d;
	// check base class attributes
	BOOST_CHECK_NO_THROW(attr_a = base_cls->get_attr("a"));
	BOOST_CHECK_NO_THROW(attr_b = base_cls->get_attr("b"));
	BOOST_CHECK_NO_THROW(attr_c = base_cls->get_attr("c"));
	BOOST_CHECK_NO_THROW(attr_d = base_cls->get_attr("d"));
	BOOST_CHECK(attr_a);
	BOOST_CHECK(attr_b);
	BOOST_CHECK_EQUAL(attr_c, PAttr());
	BOOST_CHECK_EQUAL(attr_d, PAttr());
	BOOST_CHECK_EQUAL(attr_a->name, "a");
	BOOST_CHECK_EQUAL(attr_a->class_name, "Int");
	BOOST_CHECK_EQUAL(attr_b->name, "b");
	BOOST_CHECK_EQUAL(attr_b->class_name, "Int");
	// check derived class attributes
	BOOST_CHECK_EQUAL(attr_a, derived_cls->get_attr("a"));
	BOOST_CHECK_EQUAL(attr_b, derived_cls->get_attr("b"));
	BOOST_CHECK_NO_THROW(attr_c = derived_cls->get_attr("c"));
	BOOST_CHECK_NO_THROW(attr_d = derived_cls->get_attr("d"));
	BOOST_CHECK(attr_c);
	BOOST_CHECK(attr_d);
	BOOST_CHECK_EQUAL(attr_c->name, "c");
	BOOST_CHECK_EQUAL(attr_c->class_name, "Int");
	BOOST_CHECK_EQUAL(attr_d->name, "d");
	BOOST_CHECK_EQUAL(attr_d->class_name, "Int");
}

BOOST_AUTO_TEST_SUITE_END()
