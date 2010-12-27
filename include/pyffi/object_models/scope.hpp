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

#ifndef PYFFI_OM_SCOPE_HPP_INCLUDED
#define PYFFI_OM_SCOPE_HPP_INCLUDED

#include <boost/any.hpp>
#include <boost/unordered_map.hpp> // for LocalClassMap
#include <boost/variant.hpp>
#include <vector>

namespace pyffi
{

namespace object_models
{

// forward declarations
class Attr; // full declaration included later
class AttrMap;
class Class; // full declaration included later
class IfElifsElse; // full declaration included later
class Instance;
class Doc; // full declaration included later

//! A declaration: a \ref Class "class", \ref Attr "attribute", \ref IfElifsElse "if/elif/.../else", or \ref Doc "documentation".
typedef boost::make_recursive_variant<Class, Attr, IfElifsElse, Doc>::type Declaration;

//! A scope is a vector of \ref Declaration "declarations".
class Scope : public std::vector<Declaration>
{
public:
    //! Constructor.
    Scope() : std::vector<Declaration>(), local_class_map(), parent_scope() {};

    //! Convert format description to abstract syntax tree.
    bool parse(std::istream & in);

    //! Convert xml format description to abstract syntax tree.
    bool parse_xml(std::istream & in);

    //! Fix all names (CamelCase for classes
    //! lower_case_with_underscores for attributes) recursively, to
    //! ensure that they can be parsed.
    void fix();

    //! Convert abstract syntax tree to format description.
    bool generate(std::ostream & out) const;

    //! Compile everything (only to be called on a top-level scope).
    void compile();

    //! Get locally defined class by name.
    Class const & get_local_class(std::string const & class_name) const;

    //! Get parent scope.
    Scope const & get_parent_scope() const;

    //! Get class by name (also inspecting parent scopes).
    Class const & get_class(std::string const & class_name) const;

    //! Instantiate and append all attributes.
    void init(std::vector<Instance> & instances) const;

    //! Read all attributes.
    void read(std::vector<Instance> & instances, std::istream & is) const;

    //! Write all attributes.
    void write(std::vector<Instance> & instances, std::ostream & os) const;

private:
    //! Type of local_class_map.
    typedef boost::unordered_map<std::string, Class const *> LocalClassMap;

    //! Map local class names to classes.
    LocalClassMap local_class_map;

    //! The parent scope in the syntax tree hierarchy.
    Scope const *parent_scope;

    //! Compile the local class maps (lcm) and parent scopes (ps).
    void compile_lcm_ps();

    //! Compile the class of every attribute (a) and every base class (bc).
    void compile_a_bc(AttrMap & attr_map);

    // The next three methods are helper functions for class_init,
    // class_read, and class_write. Therefore their implementation
    // resides in ast_class.cpp.

    friend class declaration_compile_lcm_ps_visitor; // part of implementation of compile_lcm_ps
    friend class declaration_compile_a_bc_visitor; // part of implementation of compile_a_bc
};

} // namespace object_models

} // namespace pyffi

#include "pyffi/object_models/attr.hpp"
#include "pyffi/object_models/class.hpp"
#include "pyffi/object_models/doc.hpp"
#include "pyffi/object_models/if_elifs_else.hpp"

#endif
