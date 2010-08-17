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

#ifndef PYFFI_OM_INSTANCE_HPP_INCLUDED
#define PYFFI_OM_INSTANCE_HPP_INCLUDED

#include "pyffi/object_models/ast.hpp"

namespace pyffi
{

namespace object_models
{

class Instance
{
public:
    //! Instantiate a given class.
    Instance(Class const & class_)
        : class_(class_), value(class_.init(class_)) {};
    //! Copy constructor.
    Instance(Instance const & instance)
        : class_(instance.class_), value(instance.value) {};
    //! Constructor to set the type and the value.
    template<typename ValueType> Instance(const ValueType & value)
        : value(value) {};
    //! Get reference to value stored in the instance.
    template<typename ValueType> ValueType & get() {
        try {
            return boost::any_cast<ValueType &>(value);
        } catch (const boost::bad_any_cast &) {
            throw std::runtime_error(
                "Type mismatch on value get (required "
                + std::string(value.type().name()) + " but got "
                + std::string(typeid(ValueType).name()) + ").");
        };
    };
    //! Get const reference to value stored in the instance.
    template<typename ValueType> const ValueType & get() const {
        try {
            return boost::any_cast<const ValueType &>(value);
        } catch (const boost::bad_any_cast &) {
            throw std::runtime_error(
                "Type mismatch on value get (required "
                + std::string(value.type().name()) + " but got "
                + std::string(typeid(ValueType).name()) + ").");
        };
    };
    //! Override assignment operator so type cannot be changed.
    Instance & operator=(const Instance & instance) {
        if (&class_ != &instance.class_)
            throw std::runtime_error(
                "Type mismatch on instance assignment (required "
                + class_.name + " but got " + instance.class_.name + ").");
        value = instance.value;
        return *this;
    };
    //! Override assignment operator so type cannot be changed.
    template<typename ValueType> Instance & operator=(const ValueType & value_) {
        if (value.type() != typeid(value_)) {
            throw std::runtime_error(
                "Type mismatch on value assignment (required "
                + std::string(value.type().name()) + " but got "
                + std::string(typeid(value_).name()) + ").");
        };
        value = value_;
        return *this;
    };
    //! Read from stream.
    void read(std::istream & is) {
        class_.read(class_, value, is);
    };
    //! Write to stream.
    void write(std::ostream & os) const {
        class_.write(class_, value, os);
    };

    Class const & class_; //!< Reference to the class of this instance.
private:
    boost::any value; //!< The value (actual data) of this instance.
};

}

}

#endif

