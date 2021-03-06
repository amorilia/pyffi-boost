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

#include "pyffi/object_models/scope.hpp"
#include "pyffi/object_models/instance.hpp"

namespace pyffi
{

namespace object_models
{

Class const & Scope::get_local_class(std::string const & class_name) const
{
    LocalClassMap::const_iterator it = local_class_map.find(class_name);
    if (it != local_class_map.end()) {
        return *(it->second);
    } else {
        throw std::runtime_error("class '" + class_name + "' not found");
    };
};

Scope const & Scope::get_parent_scope() const
{
    if (parent_scope) {
        return *parent_scope;
    } else {
        throw std::runtime_error("scope has no parent");
    };
}

Class const & Scope::get_class(std::string const & class_name) const
{
    LocalClassMap::const_iterator it = local_class_map.find(class_name);
    if (it != local_class_map.end()) {
        return *(it->second);
    } else {
        if (parent_scope) {
            return parent_scope->get_class(class_name);
        } else {
            throw std::runtime_error("class '" + class_name + "' not found");
        };
    };
};

//! A visitor for initializing all attributes of a scope.
class declaration_init_visitor
    : public boost::static_visitor<void>
{
public:
    //! Constructor.
    declaration_init_visitor(std::vector<Instance> & instances)
        : instances(instances) {};

    //! A class.
    void operator()(Class const & class_) const {};

    //! An attribute.
    void operator()(Attr const & attr) const {
        // instantiate
        instances.push_back(Instance(attr.get_class()));
    };

    //! An if/elif/.../else structure.
    void operator()(IfElifsElse const & ifelifselse) const {
        BOOST_FOREACH(If const & if_, ifelifselse.ifs_) {
            // instantiate attributes of this if's scope
            if_.scope.init(instances);
        };
        if (ifelifselse.else_) {
            // instantiate attributes of the else's scope
            ifelifselse.else_.get().init(instances);
        };
    };

    //! Documentation.
    void operator()(Doc const & doc) const {};

    std::vector<Instance> & instances;
};

void Scope::init(std::vector<Instance> & instances) const
{
    BOOST_FOREACH(Declaration const & decl, *this) {
        boost::apply_visitor(declaration_init_visitor(instances), decl);
    };
};

//! A visitor for comparing declarations.
class declaration_equality_visitor
    : public boost::static_visitor<bool>
{
public:
    template <typename T, typename U>
    bool operator()(const T &, const U &) const {
        return false;
    }

    template <typename T>
    bool operator()(const T & lhs, const T & rhs) const {
        return lhs == rhs;
    }
};

bool Scope::operator==(Scope const & other) const
{
    if (size() != other.size()) {
        return false;
    };
    for (int i = 0; i < size(); i++) {
        if (!boost::apply_visitor(declaration_equality_visitor(), (*this)[i], other[i])) {
            return false;
        };
    };
    return true;
};

} // namespace object_models

} // namespace pyffi
