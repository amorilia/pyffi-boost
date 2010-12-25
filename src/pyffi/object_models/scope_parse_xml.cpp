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

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream> // DEBUG

#include "pyffi/object_models/scope.hpp"

namespace pyffi
{

namespace object_models
{

using namespace boost::property_tree;

bool Scope::parse_xml(std::istream & in)
{
    // disable skipping of whitespace
    in.unsetf(std::ios::skipws);

    // read xml into property tree
    ptree pt;
    xml_parser::read_xml(in, pt);

    BOOST_FOREACH(ptree::value_type & decl, pt.get_child("niftoolsxml")) {
        if (decl.first == "basic") {
            // set class name
            Class class_(decl.second.get<std::string>("<xmlattr>.name"));
            push_back(class_);
        } else if (decl.first == "compound" || decl.first == "niobject") {
            // set class name
            Class class_(decl.second.get<std::string>("<xmlattr>.name"));
            // set base class name
            class_.base_name = decl.second.get_optional<std::string>("<xmlattr>.inherit");
            Scope scope;
            BOOST_FOREACH(ptree::value_type & add, decl.second) {
                if (add.first == "add") {
                    Attr attr(
                        add.second.get<std::string>("<xmlattr>.type"),
                        add.second.get<std::string>("<xmlattr>.name"));
                    scope.push_back(attr);
                };
            };
            if (!scope.empty()) {
                class_.scope = scope;
            };
            push_back(class_);
        };
    };

    // DEBUG
    generate(std::cout);

    // successful parse
    return true;
}

} // namespace object_models

} // namespace pyffi
