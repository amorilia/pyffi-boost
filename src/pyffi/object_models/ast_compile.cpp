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

#include "pyffi/object_models/ast.hpp"

namespace pyffi
{

namespace object_models
{

//! A visitor for compiling the declaration of a given scope.
class declaration_compile_visitor
    : public boost::static_visitor<void>
{
public:
    //! Constructor.
    declaration_compile_visitor(Scope & scope)
        : scope(scope) {};

    //! A class.
    void operator()(Class & class_) const {
        // update the local class map
        std::pair<Scope::LocalClassMap::iterator, bool> ret =
            scope.local_class_map.insert(
                std::make_pair(class_.name, &class_));
        if (!ret.second) {
            // insert failed
            throw std::runtime_error(
                "duplicate definition of class '" + class_.name + "'.");
        };
        // set the class's parent scope
        class_.scope.get().parent_scope = &scope;
        // compile the nested scope
        if (class_.scope) {
            class_.scope.get().compile();
        };
    };

    //! An attribute.
    void operator()(Attr & attr) const {};

    //! An if/elif/.../else structure.
    void operator()(IfElifsElse & ifelifselse) const {
        BOOST_FOREACH(If & if_, ifelifselse.ifs_) {
            // set the if's parent scope
            if_.scope.parent_scope = &scope;
            // compile this if's scope
            if_.scope.compile();
        };
        if (ifelifselse.else_) {
            // set the if's parent scope
            ifelifselse.else_.get().parent_scope = &scope;
            // compile the else's scope
            ifelifselse.else_.get().compile();
        };
    };

    Scope & scope;
};

void Scope::compile()
{
    BOOST_FOREACH(Declaration & decl, *this) {
        // compile all declarations
        boost::apply_visitor(
            declaration_compile_visitor(*this), decl);
    };
}

} // namespace object_models

} // namespace pyffi
