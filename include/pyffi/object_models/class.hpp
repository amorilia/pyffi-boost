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

#ifndef PYFFI_OM_CLASS_HPP_INCLUDED
#define PYFFI_OM_CLASS_HPP_INCLUDED

#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/optional.hpp>

#include "pyffi/object_models/attr_map.hpp"
#include "pyffi/object_models/scope.hpp"

namespace pyffi
{

namespace object_models
{

// forward declarations

//! Default init implementation for classes.
/*!
  \param class_ The \ref Class "class" to create an instance from.
  \return A vector of \ref Instance "instances" for each \ref Attr
          "attribute" in the class scope.
*/
boost::any class_init(Class const & class_);

//! Default read implementation for classes.
/*!
  \param class_ The class of the instance.
  \param value The internal representation of the instance.
  \param is The input stream.
*/
void class_read(Class const & class_, boost::any & value, std::istream & is);

//! Default write implementation for classes.
/*!
  \param class_ The class of the instance.
  \param value The internal representation of the instance.
  \param os The output stream.
*/
void class_write(Class const & class_, boost::any const & value, std::ostream & os);

//! Default attribute implementation for classes.
/*!
  \param class_ The class of the instance.
  \param value The internal representation of the instance.
  \param name The attribute name.
*/
Instance & class_attr(Class const & class_, boost::any & value, std::string const & name);

//! Default const attribute implementation for classes.
/*!
  \param class_ The class of the instance.
  \param value The internal representation of the instance.
  \param name The attribute name.
*/
Instance const & class_const_attr(Class const & class_, boost::any const & value, std::string const & name);

//! Init implementation for primitive types.
/*!
  \tparam ValueType The primitive type that is used to represent this class.
  \param class_ The \ref Class "class" to create an instance from.
  \return A ValueType instance, created by calling the default constructor.
*/
template<class ValueType>
boost::any type_init(Class const & class_)
{
    return boost::any(ValueType());
};

//! Read implementation for primitive types.
/*!
  \tparam ValueType The primitive type that is used to represent this class.
  \param class_ The class of the instance.
  \param value The internal representation of the instance.
  \param is The input stream.
*/
template<class ValueType>
void type_read(Class const & class_, boost::any & value, std::istream & is)
{
    is.read((char *)boost::any_cast<ValueType>(&value), sizeof(ValueType));
};

//! Write implementation for primitive types.
/*!
  \tparam ValueType The primitive type that is used to represent this class.
  \param class_ The class of the instance.
  \param value The internal representation of the instance.
  \param os The output stream.
*/
template<class ValueType>
void type_write(Class const & class_, boost::any const & value, std::ostream & os)
{
    os.write((char *)boost::any_cast<ValueType>(&value), sizeof(ValueType));
};

//! Attribute implementation for primitive types.
/*!
  Always throws a runtime error.

  \tparam ValueType The primitive type that is used to represent this class.
  \param class_ The class of the instance.
  \param value The internal representation of the instance.
  \param name The attribute name.
*/
template<class ValueType>
Instance & type_attr(Class const & class_, boost::any & value, std::string const & name)
{
    throw std::runtime_error("class has no attributes");
};

//! Const attribute implementation for primitive types.
/*!
  Always throws a runtime error.

  \tparam ValueType The primitive type that is used to represent this class.
  \param class_ The class of the instance.
  \param value The internal representation of the instance.
  \param name The attribute name.
*/
template<class ValueType>
Instance const & type_const_attr(Class const & class_, boost::any const & value, std::string const & name)
{
    throw std::runtime_error("class has no attributes");
};

//! A class declaration is a named scope, along with a base class.
class Class
{
public:
    //! Default constructor.
    Class()
        : name(), base_name(), scope(),
          init(&class_init), read(&class_read), write(&class_write),
          attr(&class_attr), const_attr(&class_const_attr),
          base_class() {};
    //! Constructor.
    Class(std::string const & name)
        : name(name), base_name(), scope(),
          init(&class_init), read(&class_read), write(&class_write),
          attr(&class_attr), const_attr(&class_const_attr),
          base_class() {};

    // information about the class which is stored in the format description
    std::string name;                       //!< Name of this class.
    boost::optional<std::string> base_name; //!< The base class name.
    boost::optional<Scope> scope;           //!< Declarations of this class.

    //! Constructor method.
    boost::function<boost::any(Class const &)> init;
    //! Read from stream method.
    boost::function<void(Class const &, boost::any &, std::istream &)> read;
    //! Write to stream method.
    boost::function<void(Class const &, boost::any const &, std::ostream &)> write;
    //! Get attribute.
    boost::function<Instance &(Class const &, boost::any &, std::string const &)> attr;
    //! Get const attribute.
    boost::function<Instance const &(Class const &, boost::any const &, std::string const &)> const_attr;

    //! Set default implementation for given type.
    template <class ValueType>
    void set_type() {
        init = &type_init<ValueType>;
        read = &type_read<ValueType>;
        write = &type_write<ValueType>;
        attr = &type_attr<ValueType>;
        const_attr = &type_const_attr<ValueType>;
    };

    //! Get a reference to the actual class.
    boost::optional<Class const &> get_base_class() const;

    //! Get attribute (Attr, not Instance).
    Attr const & get_attr(std::string const & name) const;

    //! Equality operator.
    bool operator==(Class const & other) const {
        return
            (name == other.name) &&
            (base_name == other.base_name) &&
            (scope == other.scope);
    };

    //! Inequality operator.
    bool operator!=(Class const & other) const {
        return !(*this == other);
    };

private:

    Class const *base_class; //!< Pointer to the base class.
    AttrMap attr_map;        //!< Maps attribute names to attributes.

    friend class declaration_compile_a_bc_visitor; // sets base_class
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

#endif
