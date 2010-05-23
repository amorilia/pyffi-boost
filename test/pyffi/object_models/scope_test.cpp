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

BOOST_AUTO_TEST_SUITE(scope_test_suite)

// Check that scopes can be created.
BOOST_AUTO_TEST_CASE(scope_create_test)
{
	BOOST_CHECK_NO_THROW(Scope::create());
}

// Check that classes can be created.
BOOST_AUTO_TEST_CASE(class_create_test)
{
	PClass cls;
	BOOST_CHECK_NO_THROW(cls = Class::create("TestClass"));
	BOOST_CHECK_EQUAL(cls->name, "TestClass");
}

// Check that attributes can be created.
BOOST_AUTO_TEST_CASE(attribute_create_test)
{
	PAttribute attr;
	BOOST_CHECK_NO_THROW(attr = Attribute::create("TestClass", "test"));
	BOOST_CHECK_EQUAL(attr->class_name, "TestClass");
	BOOST_CHECK_EQUAL(attr->name, "test");
}

// Check that scope class declaration syntax.
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

// Check that scope class declaration syntax.
BOOST_AUTO_TEST_CASE(scope_create_attribute_test)
{
	PScope scope;
	// define some classes
	BOOST_CHECK_NO_THROW(
	    scope =
	        Scope::create()
	        ->class_("Float")
	        ->attribute("Float", "x")
	        ->attribute("Float", "y")
	        ->attribute("Float", "z")
	        ->class_("Int")
	        ->attribute("Float", "time")
	        ->attribute("Int", "num_vertices")
	);
	// check scope declarations
	BOOST_CHECK_EQUAL(scope->declarations.size(), 7);
	PClass cls;
	PAttribute attr;
	BOOST_CHECK_NO_THROW(cls = get<PClass>(scope->declarations[0]))
	BOOST_CHECK_EQUAL(cls->name, "Float");
	BOOST_CHECK_NO_THROW(attr = get<PAttribute>(scope->declarations[1]));
	BOOST_CHECK_EQUAL(attr->class_name, "Float");
	BOOST_CHECK_EQUAL(attr->name, "x");
	BOOST_CHECK_NO_THROW(attr = get<PAttribute>(scope->declarations[2]));
	BOOST_CHECK_EQUAL(attr->class_name, "Float");
	BOOST_CHECK_EQUAL(attr->name, "y");
	BOOST_CHECK_NO_THROW(attr = get<PAttribute>(scope->declarations[3]));
	BOOST_CHECK_EQUAL(attr->class_name, "Float");
	BOOST_CHECK_EQUAL(attr->name, "z");
	BOOST_CHECK_NO_THROW(cls = get<PClass>(scope->declarations[4]))
	BOOST_CHECK_EQUAL(cls->name, "Int");
	BOOST_CHECK_NO_THROW(attr = get<PAttribute>(scope->declarations[5]));
	BOOST_CHECK_EQUAL(attr->class_name, "Float");
	BOOST_CHECK_EQUAL(attr->name, "time");
	BOOST_CHECK_NO_THROW(attr = get<PAttribute>(scope->declarations[6]));
	BOOST_CHECK_EQUAL(attr->class_name, "Int");
	BOOST_CHECK_EQUAL(attr->name, "num_vertices");
}

BOOST_AUTO_TEST_SUITE_END()
