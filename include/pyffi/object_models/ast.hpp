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

#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>
#include <boost/variant.hpp>
#include <vector>

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
typedef boost::make_recursive_variant<Class, Attr, IfElifsElse>::type Declaration;

//! A scope is a vector of declarations.
class Scope : public std::vector<Declaration>
{
public:
    //! Convert format description to abstract syntax tree.
    bool parse(std::istream & in);

    //! Convert abstract syntax tree to format description.
    bool generate(std::ostream & out) const;

    //! Compile all local class map.
    void compile_local_class_maps();

    //! Compile everything.
    void compile();

    //! Type of local_class_map.
    typedef boost::unordered_map<std::string, Class const * const> LocalClassMap;

    //! Map local class names to classes.
    LocalClassMap local_class_map;
};

//! Default init implementation.
template<class ValueType>
boost::any _init()
{
    return boost::any(ValueType());
};

//! Default read implementation.
template<class ValueType>
void _read(boost::any & value, std::istream & is)
{
    is.read((char *)boost::any_cast<ValueType>(&value), sizeof(ValueType));
};

//! Default write implementation.
template<class ValueType>
void _write(boost::any const & value, std::ostream & os)
{
    os.write((char *)boost::any_cast<ValueType>(&value), sizeof(ValueType));
};

//! A class declaration is a named scope, along with a base class.
class Class
{
public:
    //! Default constructor.
    Class()
        : name(), base_name(), scope(), init(), read(), write() {};
    //! Constructor.
    Class(std::string const & name)
        : name(name), base_name(), scope() {};

    // information about the class which is stored in the format description
    std::string name;                       //!< Name of this class.
    boost::optional<std::string> base_name; //!< The base class name.
    boost::optional<Scope> scope;           //!< Declarations of this class.

    // methods to work with instances of the class
    boost::function<boost::any()> init; //!< Constructor.
    boost::function<void(boost::any &, std::istream &)> read; //!< Read from stream.
    boost::function<void(boost::any const &, std::ostream &)> write; //!< Write to stream.

    //! Set default implementation for given type.
    template <class ValueType>
    void set_type() {
        init = &_init<ValueType>;
        read = &_read<ValueType>;
        write = &_write<ValueType>;
    };
};

//! An attribute declaration has a class (its type), and a name.
class Attr
{
public:
    //! Default constructor.
    Attr()
        : class_name(), name(), class_() {};
    //! Constructor.
    Attr(std::string const & class_name, std::string const & name)
        : class_name(class_name), name(name), class_() {};

    std::string class_name; //!< Name of the class of this attribute.
    std::string name;       //!< Name of this attribute.
    boost::optional<Class const &> class_; //!< Reference to the actual class.
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
    IfElifsElse() : ifs_(), else_() {}; //<! Constructor.
    std::vector<If> ifs_;         //!< The if and elif parts.
    boost::optional<Scope> else_; //!< The else part.
};

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
    (std::vector<pyffi::object_models::If>, ifs_)
    (boost::optional<pyffi::object_models::Scope>, else_)
)

#endif
