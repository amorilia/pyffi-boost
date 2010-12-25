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


Instance::Instance(Class const & class_)
    : class_(class_), value(class_.init(class_)) {};

Instance::Instance(Instance const & instance)
    : class_(instance.class_), value(instance.value) {};

Instance & Instance::operator=(const Instance & instance)
{
    if (&class_ != &instance.class_)
        throw std::runtime_error(
            "Type mismatch on instance assignment (required "
            + class_.name + " but got " + instance.class_.name + ").");
    value = instance.value;
    return *this;
};

void Instance::read(std::istream & is)
{
    class_.read(class_, value, is);
};

void Instance::write(std::ostream & os) const
{
    class_.write(class_, value, os);
};

Instance & Instance::attr(std::string const & name)
{
    return class_.attr(class_, value, name);
}

Instance const & Instance::attr(std::string const & name) const
{
    return class_.const_attr(class_, value, name);
}

} // namespace object_models

} // namespace pyffi
