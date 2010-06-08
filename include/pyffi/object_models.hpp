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

#ifndef PYFFI_OM_HPP_INCLUDED
#define PYFFI_OM_HPP_INCLUDED

#include "pyffi/object_models/ast.hpp"

namespace pyffi
{

/**

\brief Object models.

\section pyffi_om_ast Abstract Syntax Tree

All classes for abstract syntax trees are in ast.hpp, and the
regression tests reside in ast_test.cpp.

\subsection pyffi_om_ast_struct Structure

The Scope class stores a list of Scope::declarations, and is the top
level object of the abstract syntax tree of a format specification. A
Scope::Declaration can be a Scope::Class, a Scope::Class::Attr, a Scope::If,
a Scope::Elif, or a Scope::Else.

The Scope::Class, Scope::If, Scope::Elif, and Scope::Else, again
contain a Scope (see Scope::Class::scope, Scope::If::scope,
Scope::Elif::scope, and Scope::Else::scope), which in turn contains
further Scope::declarations.

Each Scope also contains a weak pointer (Scope::parent) to the
declaration in which it is defined, and each possible
Scope::Declaration has a weak pointer to the scope in which it is
defined (Scope::Class::parent, Scope::If::parent, Scope::Elif::parent,
and Scope::Else::parent).

\subsection pyffi_om_ast_construction Construction

\todo Add a description of tree construction.

\dontinclude test/pyffi/object_models/ast_test.cpp
\skip BOOST_AUTO_TEST_CASE(get_attr_test)
\skip scope =
\until );

\subsection pyffi_om_ast_nav Navigation

To aid navigation through the syntax tree, the following visitors are
available.

To get a shared pointer to a Scope::Declaration, use \ref
Scope::get_declaration_visitor\<Scope::Class\>, \ref
Scope::get_declaration_visitor\<Scope::Class::Attr\>, and so on: the
result will be null if the declaration's type does not match.

Similarly, to get a shared pointer to the parent Scope of a
Scope::Declaration, use Scope::get_parent_scope_visitor.

\dontinclude test/pyffi/object_models/ast_test.cpp
\skip BOOST_AUTO_TEST_CASE(visitor_test)
\skip scope =
\until get_parent_scope_visitor

*/

namespace object_models
{

} // namespace object_models

} // namespace pyffi

#endif
