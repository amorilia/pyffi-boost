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

#include <iostream>
#include <fstream>

#include "pyffi/object_models/scope.hpp"

using boost::get;
using namespace pyffi;
using namespace pyffi::object_models;

struct XmlParser {
    XmlParser(std::string const & filename, bool result = true) {
        BOOST_TEST_MESSAGE(
            "Parsing \"" + std::string(TEST_PATH) + filename + "\"");
        std::ifstream is((std::string(TEST_PATH) + filename).c_str());
        if (result) {
            BOOST_CHECK_EQUAL(scope.parse_xml(is), true);
        } else {
            BOOST_CHECK_THROW(scope.parse_xml(is), std::runtime_error);
        };
    };
    Scope scope;
};

BOOST_AUTO_TEST_SUITE(scope_parse_xml_test_suite)

BOOST_AUTO_TEST_CASE(scope_parse_xml_basic_test)
{
    XmlParser xml("/data/xml/test_basic.xml");
}

BOOST_AUTO_TEST_CASE(scope_parse_xml_enum_test)
{
    XmlParser xml("/data/xml/test_enum.xml");
}

BOOST_AUTO_TEST_CASE(scope_parse_xml_full_test)
{
    XmlParser xml("/data/xml/test_full.xml");
}

BOOST_AUTO_TEST_CASE(scope_parse_xml_header_test)
{
    XmlParser xml("/data/xml/test_header.xml");
}

BOOST_AUTO_TEST_CASE(scope_parse_xml_invalid_test)
{
    XmlParser xml("/data/xml/test_invalid.xml", false);
}

BOOST_AUTO_TEST_CASE(scope_parse_xml_minimal_test)
{
    XmlParser xml("/data/xml/test_minimal.xml");
}

BOOST_AUTO_TEST_CASE(scope_parse_xml_version_test)
{
    XmlParser xml("/data/xml/test_version.xml");
}

BOOST_AUTO_TEST_SUITE_END()
