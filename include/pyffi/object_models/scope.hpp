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

//! A scope which contains declarations.
/*!
 * Declarations can be classes, attributes, enums, and if/elif/else
 * statements. Scopes can be constructed using method chaining: all
 * methods return a shared pointer to this.
 */
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
		declarations.push_back(cls);
		cls->parent = shared_from_this();
		return shared_from_this();
	};

	//! Attach a base class to the last declaration.
	PScope base_class(std::string const & class_name) {
		if (PClass cls = // assignment!
		        boost::apply_visitor(
		            get_class_visitor(), declarations.back())) {
			if (PClass base_class = get_class(class_name)) {
				cls->base_class = base_class;
			} else {
				throw name_error("base class " + class_name + " not found");
			}
		} else {
			throw syntax_error("only classes can have a base class");
		};
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
		    set_scope_visitor(scope_), declarations.back());
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

		//! The base class.
		boost::weak_ptr<Class> base_class;

		//! The parent of this class, always a scope.
		boost::weak_ptr<Scope> parent;

	private:
		//! Private constructor to prevent it from being used.
		Class(std::string const & name) : name(name) {};
	};

	//! Shortcut.
	typedef Class::PClass PClass;

	//! Get class with given name from scope.
	PClass get_class(std::string const & class_name) const {
		// look for class in scope declarations
		BOOST_FOREACH(Declaration declaration, declarations) {
			if (PClass result = // assignment!
			        boost::apply_visitor(
			            get_class_visitor(),
			            declaration)) {
				if (result->name == class_name) {
					return result;
				}
			}
		}
		// get the scope of the parent (class, if, elif, else, etc.)
		if (PScope parent_scope = // assignment!
		        boost::apply_visitor(
		            get_parent_scope_visitor(),
		            parent)) {
			// search for the class in this scope
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

	//! Get attribute with given name from scope.
	PAttr get_attr(std::string const & attr_name) const {
		// look for attribute in scope declarations
		BOOST_FOREACH(Declaration declaration, declarations) {
			if (PAttr result = // assignment!
			        boost::apply_visitor(
			            get_attr_visitor(attr_name),
			            declaration)) {
				return result;
			}
		}
		// look for attribute in base class
		// parent_scope->cls->scope
		if (PClass cls = // assignment!
		        boost::apply_visitor(
		            get_class_visitor(),
		            parent)) {
			if (PClass base_class =
			        cls->base_class.lock()) {
				if (PScope base_scope = base_class->scope) {
					return base_scope->get_attr(attr_name);
				}
			}
		}
		// all failed
		return PAttr();
	};

	//! The type of a declaration (class, or attribute).
	typedef boost::variant<PClass, PAttr> Declaration;

	//! Declarations in this scope.
	std::vector<Declaration> declarations;

	//! The type of a parent of this scope (class, if, elif, or else).
	typedef boost::variant<boost::weak_ptr<Class> > Parent;

	//! Parent of this scope.
	Parent parent;

	//! A visitor for attaching a scope to a declaration.
	class set_scope_visitor : public boost::static_visitor<>
	{
	public:
		//! Constructor.
		set_scope_visitor(PScope scope) : scope(scope) {};

		//! Attach scope to class.
		void operator()(PClass cls) const {
			cls->scope = scope;
			scope->parent = cls;
		};

		//! No scope for attributes, so throw an exception.
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
		get_class_visitor() {};

		//! Return class.
		PClass operator()(PClass cls) const {
			return cls;
		};

		//! Attribute returns nothing.
		PClass operator()(PAttr attr) const {
			return PClass();
		};

		//! Return class.
		PClass operator()(boost::weak_ptr<Class> cls) const {
			return cls.lock();
		};
	};

	//! A visitor for finding an attribute.
	class get_attr_visitor : public boost::static_visitor<PAttr>
	{
	public:
		//! Constructor.
		get_attr_visitor(std::string const & attr_name)
			: attr_name(attr_name) {};

		//! A class never matches.
		PAttr operator()(PClass cls) const {
			return PAttr();
		};

		//! Return attribute if name matches.
		PAttr operator()(PAttr attr) const {
			if (attr->name == attr_name) {
				return attr;
			} else {
				return PAttr();
			}
		};

		//! The name of the attribute to get.
		std::string attr_name;
	};

	//! A visitor for finding the parent scope.
	class get_parent_scope_visitor : public boost::static_visitor<PScope>
	{
	public:
		//! Constructor.
		get_parent_scope_visitor() {};

		//! Return parent scope of a class.
		PScope operator()(PClass cls) const {
			return cls->parent.lock();
		};

		//! Return parent scope of a class.
		PScope operator()(boost::weak_ptr<Class> cls) const {
			if (PClass cls_ = cls.lock()) {
				return cls_->parent.lock();
			} else {
				return PScope();
			}
		};
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
