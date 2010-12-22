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

#include "pyffi/object_models/ast.hpp"
#include "pyffi/object_models/instance.hpp"

namespace pyffi
{

namespace object_models
{

//! A visitor for initializing all attributes of a class.
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

    std::vector<Instance> & instances;
};

void Scope::init(std::vector<Instance> & instances) const
{
    BOOST_FOREACH(Declaration const & decl, *this) {
        boost::apply_visitor(declaration_init_visitor(instances), decl);
    };
};

boost::any class_init(Class const & class_)
{
    // TODO use the class_.attr_map here
    std::vector<Instance> instances;
    // instantiate base class attributes
    boost::optional<Class const &> base_class = class_.get_base_class();
    if (base_class) {
        instances =
            boost::any_cast<std::vector<Instance> >(
                base_class.get().init(base_class.get()));
    };
    // instantiate class attributes
    if (class_.scope) {
        class_.scope.get().init(instances);
    };
    return boost::any(instances);
};

boost::any class_read(Class const & class_, boost::any & value, std::istream & is)
{
    std::vector<Instance> result;
    BOOST_FOREACH(Declaration const & decl, class_.scope.get()) {
        //boost::apply_visitor(declaration_read_visitor(result), decl);
    };
    return result;
};

boost::any class_write(Class const & class_, boost::any const & value, std::ostream & os)
{
    std::vector<Instance> result;
    BOOST_FOREACH(Declaration const & decl, class_.scope.get()) {
        //boost::apply_visitor(declaration_write_visitor(result), decl);
    };
    return result;
};

Instance & class_attr(Class const & class_, boost::any & value, std::string const & name)
{
    std::vector<Instance> & instances
    = boost::any_cast<std::vector<Instance> &>(value);
    Attr::Map::nth_index<1>::type::iterator it
    = class_.attr_map.get<1>().find(name);
    if (it == class_.attr_map.get<1>().end()) {
        throw std::runtime_error("attribute '" + name + "'not found");
    }
    return instances[(*it)->get_index()];
}

Instance const & class_const_attr(Class const &, boost::any const &, std::string const &)
{
    throw std::runtime_error("not yet implemented");
}

boost::optional<Class const &> Class::get_base_class() const
{
    if (base_class) {
        return boost::optional<Class const &>(*base_class);
    } else {
        return boost::optional<Class const &>();
    };
};

} // namespace object_models

} // namespace pyffi
