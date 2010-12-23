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

#ifndef PYFFI_OM_ATTRMAP_HPP_INCLUDED
#define PYFFI_OM_ATTRMAP_HPP_INCLUDED

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/iterator/iterator_adaptor.hpp>

// needs full definition of Attr so we can hash the map by name
#include "pyffi/object_models/attr.hpp"

namespace pyffi
{

namespace object_models
{

//! An attribute map which remembers insertion order (like Python's
//! OrderedDict). Use by the Class implementation.
class AttrMap
{
private:
    //! The map type.
    typedef boost::multi_index_container
    <Attr const *,
    boost::multi_index::indexed_by<
    // hashed by name
    boost::multi_index::hashed_unique<
    boost::multi_index::member<Attr const, std::string const, &Attr::name> >,
    // ordered by insertion (which is the same as ordered by index)
    boost::multi_index::sequenced<> > > Map;

    //! The map data.
    Map map;

public:
    //! Constant iterator.
    class const_iterator
        : public boost::iterator_adaptor<
        const_iterator,
        Map::nth_index<1>::type::const_iterator, // base_type
        Attr const // value_type
        >
    {
    public:
        explicit const_iterator(const_iterator::base_type const & iter)
            : const_iterator::iterator_adaptor_(iter) {}
    private:
        friend class boost::iterator_core_access;
        Attr const & dereference() const {
            return **base();
        }
    };

    //! Default constructor.
    AttrMap() : map() {};

    //! Insert an attribute in the map, and sets the attribute's
    //! index.  If an attribute with the same name already exists,
    //! then the map remains unchanged.
    void push_back(Attr & attr);

    //! Get the attribute of the given name.
    Attr const & operator[](std::string const & name) const;

    //! Iterator (by insertion order) begin.
    const_iterator begin() const;

    //! Iterator (by insertion order) end.
    const_iterator end() const;

    //! Size.
    std::size_t size() const;
};

} // namespace object_models

} // namespace pyffi

#endif
