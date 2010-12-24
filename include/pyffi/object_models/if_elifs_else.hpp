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

#ifndef PYFFI_OM_IFELIFSELSE_HPP_INCLUDED
#define PYFFI_OM_IFELIFSELSE_HPP_INCLUDED

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/optional.hpp>
#include <vector>

// optional<Scope> needs full class definition of Scope
#include "pyffi/object_models/scope.hpp"

namespace pyffi
{

namespace object_models
{

//! An expression (for now, simply defined as a primitive boolean type).
typedef bool Expr;

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
    IfElifsElse() : ifs_(), else_() {}; //<! Constructor.
    std::vector<If> ifs_;         //!< The if and elif parts.
    boost::optional<Scope> else_; //!< The else part.
};

} // namespace object_models

} // namespace pyffi

// upgrade structs to fusion sequences

BOOST_FUSION_ADAPT_STRUCT(
    pyffi::object_models::If,
    (pyffi::object_models::Expr, expr)
    (pyffi::object_models::Scope, scope)
)

BOOST_FUSION_ADAPT_STRUCT(
    pyffi::object_models::IfElifsElse,
    (std::vector<pyffi::object_models::If>, ifs_)
    (boost::optional<pyffi::object_models::Scope>, else_)
)

#endif
