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

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <cctype> // std::toupper, std::isalpha (singe character)

#include "pyffi/object_models/scope.hpp"

namespace pyffi
{

namespace object_models
{

namespace algorithm = boost::algorithm;

//! A visitor for fixing names.
class declaration_fix_visitor
    : public boost::static_visitor<void>
{
private:
    void fix_class_name(std::string & name) const {
        // remove all non-alphanumeric characters
        algorithm::find_format_all(
            name,
            algorithm::token_finder(
                !algorithm::is_alnum(),
                algorithm::token_compress_on),
            algorithm::empty_formatter(name));
        // must be non-empty
        if (name.empty()) {
            throw std::runtime_error(
                "cannot fix class name without alphanumeric characters");
        };
        // must start with an upper case letter
        if (!std::isalpha(name[0])) {
            throw std::runtime_error(
                "cannot fix class name '" + name +
                "' that does not start with an alphabetic character");
        };
        name[0] = std::toupper(name[0]);
    };
    void fix_attr_name(std::string & name) const {
        // trim whitespace
        algorithm::trim_if(name, algorithm::is_space());
        // convert all subsequences of non-alphanumeric characters to "_"
        algorithm::find_format_all(
            name,
            algorithm::token_finder(
                !algorithm::is_alnum(),
                algorithm::token_compress_on),
            algorithm::const_formatter("_"));
        // must be lower case
        algorithm::to_lower(name);
        // must be non-empty
        if (name.empty()) {
            throw std::runtime_error(
                "cannot fix attribute name without valid characters");
        };
    };
public:
    //! A class.
    void operator()(Class & class_) const {
        fix_class_name(class_.name);
        if (class_.base_name) {
            fix_class_name(class_.base_name.get());
        };
        if (class_.scope) {
            // fix the nested scope
            class_.scope.get().fix();
        };
    };

    //! An attribute.
    void operator()(Attr & attr) const {
        fix_attr_name(attr.name);
        fix_class_name(attr.class_name);
    };

    //! An if/elif/.../else structure.
    void operator()(IfElifsElse & ifelifselse) const {
        BOOST_FOREACH(If & if_, ifelifselse.ifs_) {
            // fix this if's scope
            if_.scope.fix();
        };
        if (ifelifselse.else_) {
            // fix the else's scope
            ifelifselse.else_.get().fix();
        };
    };
};

void Scope::fix()
{
    BOOST_FOREACH(Declaration & decl, *this) {
        // fix all declarations
        boost::apply_visitor(
            declaration_fix_visitor(), decl);
    };
}

} // namespace object_models

} // namespace pyffi
