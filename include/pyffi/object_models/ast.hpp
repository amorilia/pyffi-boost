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

#ifndef PYFFI_OM_AST_HPP_INCLUDED
#define PYFFI_OM_AST_HPP_INCLUDED

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant.hpp>

namespace pyffi
{

namespace object_models
{

//! An expression (for now, simply defined as a primitive boolean type).
typedef bool Expr;

class Class;
class Attr;
class IfElifsElse;

//! A declaration: a \ref Class "class", \ref Attr "attribute", or \ref IfElifsElse "if/elif/.../else".
typedef boost::variant<boost::recursive_wrapper<Class>, boost::recursive_wrapper<Attr>, boost::recursive_wrapper<IfElifsElse> > Declaration;

//! A scope is a vector of declarations.
typedef std::vector<Declaration> Scope;

//! A class declaration is a named scope, along with a base class.
class Class
{
public:
    //! Default constructor.
    Class()
        : name(), base_name(), scope() {};
    //! Constructor.
    Class(std::string const & name)
        : name(name), base_name(), scope() {};
    std::string name;                       //!< Name of this class.
    boost::optional<std::string> base_name; //!< The base class name.
    boost::optional<Scope> scope;           //!< Declarations of this class.
};

//! An attribute declaration has a class (its type), and a name.
class Attr
{
public:
    //! Default constructor.
    Attr()
        : class_name(), name() {};
    //! Constructor.
    Attr(std::string const & class_name, std::string const & name)
        : class_name(class_name), name(name) {};

    std::string class_name; //!< Name of the class of this attribute.
    std::string name;       //!< Name of this attribute.
};

//! A simple if declaration: an expression and a scope.
class If
{
public:
    If() : expr(), scope() {}; //<! Constructor.
    Expr expr;   //!< Condition.
    Scope scope; //!< Declarations.
};

//! An if/elif/else declaration.
class IfElifsElse
{
public:
    IfElifsElse() : if_(), elifs_(), else_() {}; //<! Constructor.
    If if_;                 //!< The if part.
    std::vector<If> elifs_; //!< The elif parts.
    Scope else_;            //!< The else part.
};

//! Convert format description to abstract syntax tree.
bool parse(std::istream & in, Scope & scope);

//! Convert abstract syntax tree to format description.
bool generate(std::ostream & out, Scope const & scope);

} // namespace object_models

} // namespace pyffi

// upgrade structs to fusion sequences

BOOST_FUSION_ADAPT_STRUCT(
    pyffi::object_models::Class,
    (std::string, name)
    (boost::optional<std::string>, base_name)
    (boost::optional<pyffi::object_models::Scope>, scope)
)

BOOST_FUSION_ADAPT_STRUCT(
    pyffi::object_models::Attr,
    (std::string, class_name)
    (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    pyffi::object_models::If,
    (pyffi::object_models::Expr, expr)
    (pyffi::object_models::Scope, scope)
)

BOOST_FUSION_ADAPT_STRUCT(
    pyffi::object_models::IfElifsElse,
    (pyffi::object_models::If, if_)
    (std::vector<pyffi::object_models::If>, elifs_)
    (pyffi::object_models::Scope, else_)
)

#endif
