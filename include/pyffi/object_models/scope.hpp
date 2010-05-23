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
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>
#include <vector>

#include "pyffi/exceptions.hpp"

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
		declarations.push_back(Class::create(name));
		return shared_from_this();
	};

	//! Create an attribute within the current scope.
	PScope attr(std::string const & class_name,
	            std::string const & name) {
		declarations.push_back(Attr::create(class_name, name));
		return shared_from_this();
	};

	//! Attach a nested scope to the last declaration.
	PScope scope(PScope scope_) {
		// attach scope to last declaration
		boost::apply_visitor(
		    scope_visitor(scope_), declarations.back());
		// register scope parent
		scope_->parent = shared_from_this();
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

		//! The declarations within this class.
		PScope scope;

	private:
		//! Private constructor to prevent it from being used.
		Class(std::string const & name) : name(name) {};
	};

	//! Shortcut.
	typedef Class::PClass PClass;

	//! Get class with given name from scope.
	PClass get_class(std::string const & class_name) {
		// look for class in scope declarations
		BOOST_FOREACH(Declaration declaration, declarations) {
			if (PClass result = // assignment!
			        boost::apply_visitor(
			            get_class_visitor(class_name),
			            declaration)) {
				return result;
			}
		}
		// look for class in parent scope
		if (PScope parent_scope = parent.lock()) {
			return parent_scope->get_class(class_name);
		}
		// all failed
		return PClass();
	};

	//! A named typed attribute.
	class Attr
	{
	public:
		//! Shared pointer to attribute.
		typedef boost::shared_ptr<Attr> PAttr;

		//! Constructor.
		static PAttr create(std::string const & class_name,
		                    std::string const & name) {
			return PAttr(new Attr(class_name, name));
		};

		//! Name of the class of this attribute.
		std::string class_name;

		//! Name of this attribute.
		std::string name;

	private:
		//! Private constructor to prevent it from being used.
		Attr(std::string const & class_name,
		     std::string const & name)
			: class_name(class_name), name(name) {};
	};

	//! Shortcut.
	typedef Attr::PAttr PAttr;

	//! The type of a declaration (class, or attribute).
	typedef boost::variant<PClass, PAttr> Declaration;

	//! Declarations in this scope.
	std::vector<Declaration> declarations;

	//! Parent scope.
	boost::weak_ptr<Scope> parent;

	//! A visitor for attaching a scope to a declaration.
	class scope_visitor : public boost::static_visitor<>
	{
	public:
		//! Constructor.
		scope_visitor(PScope scope) : scope(scope) {};

		//! Attach scope to class.
		void operator()(PClass cls) const {
			cls->scope = scope;
		};

		//! For attributes, we cannot have a scope, so throw
		//! an exception.
		void operator()(PAttr attr) const {
			throw syntax_error("attributes cannot have a scope");
		};

		//! The scope to attach.
		PScope scope;
	};

	//! A visitor for finding a class.
	class get_class_visitor : public boost::static_visitor<PClass>
	{
	public:
		//! Constructor.
		get_class_visitor(std::string const & class_name)
			: class_name(class_name) {};

		//! Return class if name matches.
		PClass operator()(PClass cls) const {
			if (cls->name == class_name) {
				return cls;
			} else {
				return PClass();
			}
		};

		//! Attribute never matches.
		PClass operator()(PAttr attr) const {
			return PClass();
		};

		//! The name of the class to get.
		std::string class_name;
	};

private:
	//! Private constructor to prevent it from being used.
	Scope() : declarations(), parent() {};
};

//! Shortcut.
typedef Scope::PScope PScope;

//! Shortcut.
typedef Scope::Class Class;

//! Shortcut.
typedef Scope::PClass PClass;

//! Shortcut.
typedef Scope::Attr Attr;

//! Shortcut.
typedef Scope::PAttr PAttr;

} // namespace object_models

} // namespace pyffi

#endif
