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

//! A visitor for updating a local class map with the given
//! declaration.
class declaration_update_local_class_map_visitor
    : public boost::static_visitor<void>
{
public:
    //! Constructor.
    declaration_update_local_class_map_visitor(Scope::LocalClassMap & local_class_map)
        : local_class_map(local_class_map) {};

    //! A class: update the map.
    void operator()(Class const & class_) const {
        std::pair<Scope::LocalClassMap::iterator, bool> ret =
            local_class_map.insert(
                std::make_pair(class_.name, &class_));
        if (!ret.second) {
            // insert failed
            throw std::runtime_error(
                "duplicate definition of class '"
                + class_.name + "'.");
        };
    };

    //! An attribute: ignore.
    void operator()(Attr const & attr) const {};

    //! An if/elif/.../else structure: ignore.
    void operator()(IfElifsElse const & ifelifselse) const {};

    Scope::LocalClassMap & local_class_map;
};

//! A visitor for compiling the local class map of nested scopes of
//! a declaration.
class declaration_compile_nested_local_class_maps_visitor
    : public boost::static_visitor<void>
{
public:
    //! Constructor.
    declaration_compile_nested_local_class_maps_visitor() {};

    //! A class: update local map in nested scope.
    void operator()(Class & class_) const {
        // compile the local class map of the class scope
        if (class_.scope) {
            class_.scope.get().compile_local_class_maps();
        };
    };

    //! An attribute has no nested scopes, so ignore.
    void operator()(Attr & attr) const {};

    //! An if/elif/.../else structure: update local maps in nested scopes.
    void operator()(IfElifsElse & ifelifselse) const {
        BOOST_FOREACH(If & if_, ifelifselse.ifs_) {
            // compile the local class map of this if's scope
            if_.scope.compile_local_class_maps();
        };
        if (ifelifselse.else_) {
            // compile the local class map of else's scope
            ifelifselse.else_.get().compile_local_class_maps();
        };
    };
};

void Scope::compile_local_class_maps()
{
    BOOST_FOREACH(Declaration & decl, *this) {
        // update our local class map
        boost::apply_visitor(
            declaration_update_local_class_map_visitor(local_class_map), decl);
        // compile local class maps of nested scopes
        boost::apply_visitor(
            declaration_compile_nested_local_class_maps_visitor(), decl);
    };
}

void Scope::compile()
{
    // set up all local maps from class names to class references
    compile_local_class_maps();

    // set up all parent scope references

    // set up all class references in attributes
};

} // namespace object_models

} // namespace pyffi
