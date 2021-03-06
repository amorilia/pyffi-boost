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

#ifndef PYFFI_OM_ATTR_HPP_INCLUDED
#define PYFFI_OM_ATTR_HPP_INCLUDED

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/optional.hpp>
#include <string>

#include "pyffi/object_models/doc.hpp"

namespace pyffi
{

namespace object_models
{

// forward declarations
class AttrMap;
class Class;

//! An attribute declaration has a class (its type), and a name.
class Attr
{
public:
    //! Default constructor.
    Attr()
        : class_name(), name(), class_(), index(), doc() {};
    //! Constructor.
    Attr(std::string const & class_name, std::string const & name)
        : class_name(class_name), name(name), class_(), doc() {};

    std::string class_name; //!< Name of the class of this attribute.
    std::string name;       //!< Name of this attribute.
    boost::optional<Doc> doc; //<! Documentation.

    //! Get a reference to the actual class.
    Class const & get_class() const;

    //! Get the index.
    std::size_t get_index() const;

    //! Equality operator.
    bool operator==(Attr const & other) const {
        return
            (class_name == other.class_name) &&
            (name == other.name) &&
            (doc == other.doc);
    };

    //! Inequality operator.
    bool operator!=(Attr const & other) const {
        return !(*this == other);
    };

private:
    Class const *class_; //!< Pointer to the actual class.
    boost::optional<std::size_t> index; //!< Index in the attribute map.

    friend class AttrMap; // sets index
    friend class declaration_compile_a_bc_visitor; // sets class_
};

} // namespace object_models

} // namespace pyffi

// upgrade struct to fusion sequence

BOOST_FUSION_ADAPT_STRUCT(
    pyffi::object_models::Attr,
    (std::string, class_name)
    (std::string, name)
    (boost::optional<pyffi::object_models::Doc>, doc)
)

#endif
