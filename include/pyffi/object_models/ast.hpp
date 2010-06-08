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

/**

\file
The abstract syntax tree model.

\sa \ref pyffi_om_ast
\sa ast_test.cpp

*/

#ifndef PYFFI_OM_AST_HPP_INCLUDED
#define PYFFI_OM_AST_HPP_INCLUDED

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
		            get_declaration_visitor<Class>(),
		            declarations.back())) {
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
		PAttr attr = Attr::create(class_name, name);
		declarations.push_back(attr);
		attr->parent = shared_from_this();
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

			//! The parent of this attribute, always a scope.
			boost::weak_ptr<Scope> parent;

		private:
			//! Private constructor to prevent it from being used.
			Attr(std::string const & class_name,
			     std::string const & name)
				: class_name(class_name), name(name) {};
		};

		//! Shortcut.
		typedef Attr::PAttr PAttr;

		//! Get attribute with given name from class.
		PAttr get_attr(std::string const & attr_name) const {
			// look for attribute in scope declarations
			BOOST_FOREACH(Declaration declaration, scope->declarations) {
				if (PAttr result = // assignment!
				        boost::apply_visitor(
				            get_declaration_visitor<Attr>(),
				            declaration)) {
					if (result->name == attr_name) {
						return result;
					}
				}
			}
			// look for attribute in base class
			if (PClass base_class_ = base_class.lock()) {
				return base_class_->get_attr(attr_name);
			}
			// all failed
			return PAttr();
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
			            get_declaration_visitor<Class>(),
			            declaration)) {
				if (result->name == class_name) {
					return result;
				}
			}
		}
		// get the scope of the parent (class, if, elif, else,
		// etc.)  of this scope
		if (PScope parent_scope = // assignment!
		        boost::apply_visitor(
		            get_parent_scope_visitor(), parent)) {
			// search for the class in this scope
			return parent_scope->get_class(class_name);
		}
		// all failed
		return PClass();
	};

	//! Shortcut.
	typedef Class::Attr Attr;

	//! Shortcut.
	typedef Class::PAttr PAttr;

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

	//! A visitor for getting a shared pointer.
	template <typename T>
	class get_declaration_visitor
		: public boost::static_visitor<boost::shared_ptr<T> >
	{
	public:
		//! Constructor.
		get_declaration_visitor() {};

		//! Return shared pointer.
		boost::shared_ptr<T> operator()(const boost::shared_ptr<T> & t) const {
			return t;
		};

		//! Lock weak pointer and return shared pointer.
		boost::shared_ptr<T> operator()(const boost::weak_ptr<T> & t) const {
			return t.lock();
		};

		//! Return nothing.
		template <typename S>
		boost::shared_ptr<T> operator()(const boost::shared_ptr<S> & s) const {
			return boost::shared_ptr<T>();
		};

		//! Return nothing.
		template <typename S>
		boost::shared_ptr<T> operator()(const boost::weak_ptr<S> & s) const {
			return boost::shared_ptr<T>();
		};
	};

	//! A visitor for finding the parent Scope of a Scope::Declaration.
	class get_parent_scope_visitor : public boost::static_visitor<PScope>
	{
	public:
		//! Constructor.
		get_parent_scope_visitor() {};

		//! Return parent scope.
		template <typename T>
		PScope operator()(const boost::shared_ptr<T> & t) const {
			return t->parent.lock();
		};

		//! Lock weak pointer and return parent scope.
		template <typename T>
		PScope operator()(const boost::weak_ptr<T> & t) const {
			if (boost::shared_ptr<T> t_ = t.lock()) {
				return t_->parent.lock();
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
