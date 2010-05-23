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

#ifndef PYFFI_OM_SCOPE_HPP_INCLUDED
#define PYFFI_OM_SCOPE_HPP_INCLUDED

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace pyffi
{

namespace object_models
{

//! A scope which contains declarations (classes, attributes, enums,
//! and if/elif/else statements). Scopes can be constructed using
//! method chaining: all methods return a shared pointer to this.
class Scope : public boost::enable_shared_from_this<Scope>
{
public:
	//! Shared pointer to scope.
	typedef boost::shared_ptr<Scope> PScope;

	//! Constructor.
	static PScope create() {
		return PScope(new Scope);
	};

	//! Create a class within the current scope.
	PScope class_(std::string const & name) {
		PClass cls = Class::create(name);
		return shared_from_this();
	};

	//! A named class containing further declarations.
	class Class
	{
	public:
		//! Shared pointer to class.
		typedef boost::shared_ptr<Class> PClass;

		//! Constructor.
		static PClass create(std::string const & name) {
			return PClass(new Class(name));
		};

		//! Name of this class.
		std::string name;

	private:
		//! Private constructor to prevent it from being used.
		Class(std::string const & name) : name(name) {};
	};

	//! Shortcut.
	typedef Class::PClass PClass;

private:
	//! Private constructor to prevent it from being used.
	Scope() {};
};

//! Shortcut.
typedef Scope::PScope PScope;

//! Shortcut.
typedef Scope::Class Class;

//! Shortcut.
typedef Scope::PClass PClass;

} // namespace object_models

} // namespace pyffi

#endif
