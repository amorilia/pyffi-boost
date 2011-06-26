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

#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/support_ostream_iterator.hpp>

#include "pyffi/object_models/scope.hpp"

namespace pyffi
{

namespace object_models
{

namespace engine = boost::spirit::karma;

template <typename Iterator>
class scope_grammar : public engine::grammar<Iterator, Scope()>
{
public:
    engine::rule<Iterator, Scope()> start;
    engine::rule<Iterator, Declaration(int)> declaration;
    engine::rule<Iterator, Class(int)> class_;
    engine::rule<Iterator, Attr(int)> attr;
    engine::rule<Iterator, Expr()> expr;
    engine::rule<Iterator, If(int)> if_;
    engine::rule<Iterator, If(int)> elif_;
    engine::rule<Iterator, Scope(int)> else_;
    engine::rule<Iterator, std::vector<If>(int)> if_elifs;
    engine::rule<Iterator, IfElifsElse(int)> if_elifs_else;
    engine::rule<Iterator, Doc(int)> doc;
    engine::rule<Iterator, Scope(int)> scope;
    engine::rule<Iterator, void(int)> indent;
    engine::rule<Iterator, std::string()> class_name;
    engine::rule<Iterator, std::string()> attr_name;
    engine::rule<Iterator, std::string()> doc_line;
    engine::rule<Iterator, void()> eol;

    scope_grammar() : scope_grammar::base_type(start) {
        indent = engine::repeat(engine::_r1)[' '];
        start = scope(0) << eol;
        declaration = class_(engine::_r1) | attr(engine::_r1) | if_elifs_else(engine::_r1) | doc(engine::_r1);
        scope = declaration(engine::_r1) % eol;
        class_ =
            indent(engine::_r1)
            << "class "
            << class_name // Class.name
            << -('(' << class_name << ')') // Class.base_name
            << -(':' << eol << scope(engine::_r1 + 4)); // Class.scope
        attr =
            indent(engine::_r1)
            << class_name // Attr.class_name
            << ' '
            << attr_name // Attr.name
            << -(':' << eol << doc(engine::_r1 + 4)); // Attr.doc
        expr = engine::bool_; // for now
        if_ =
            indent(engine::_r1)
            << "if "
            << expr // If.expr
            << ':' << eol << scope(engine::_r1 + 4); // If.scope
        elif_ =
            indent(engine::_r1)
            << "elif "
            << expr // If.expr
            << ':' << eol << scope(engine::_r1 + 4); // If.scope
        else_ =
            indent(engine::_r1)
            << "else"
            << ':' << eol << scope(engine::_r1 + 4); // Scope
        if_elifs =
            if_(engine::_r1) // IfElifsElse.ifs_[0]
            << *(eol << elif_(engine::_r1)); // IfElifsElse.ifs_[1:]
        if_elifs_else =
            if_elifs(engine::_r1) // IfElifsElse.ifs_
            << -(eol << else_(engine::_r1)); // IfElifsElse.else_
        doc =
            indent(engine::_r1)
            << "\"\"\""
            << (doc_line % (eol << indent(engine::_r1)))
            << -(engine::eps(boost::phoenix::size(engine::_val) > 1) << (eol << indent(engine::_r1)))
            << "\"\"\"" << eol; // extra eol here for prettier formatting
        class_name = engine::string; // must be CamelCase for parser
        attr_name = engine::string; // must be lower_case_with_underscores for parser
        doc_line = engine::string; // must not contain newlines or """ for parser
        eol = engine::eol;

#ifdef DEBUG
        indent.name("indent");
        start.name("start");
        declaration.name("declaration");
        scope.name("scope");
        class_.name("class");
        attr.name("attr");
        expr.name("expr");
        if_.name("if");
        elif_.name("elif");
        else_.name("else");
        if_elifs.name("if-elifs");
        if_elifs_else.name("if-elifs-else");
        doc.name("doc");
        class_name.name("class-name");
        attr_name.name("attr-name");
        doc_line.name("doc-line");
        eol.name("eol");

        engine::debug(indent);
        engine::debug(start);
        engine::debug(declaration);
        engine::debug(scope);
        engine::debug(class_);
        engine::debug(attr);
        engine::debug(expr);
        engine::debug(if_);
        engine::debug(elif_);
        engine::debug(else_);
        engine::debug(if_elifs);
        engine::debug(if_elifs_else);
        engine::debug(doc);
        engine::debug(class_name);
        engine::debug(attr_name);
        engine::debug(doc_line);
        engine::debug(eol);
#endif // DEBUG
    }
};

bool Scope::generate(std::ostream & out) const
{
    // wrap ostream into iterator
    boost::spirit::ostream_iterator sink(out);

    // create parser
    scope_grammar<boost::spirit::ostream_iterator> parser;

    // use iterator to generate stream
    return engine::generate(sink, parser, *this);
}

} // namespace object_models

} // namespace pyffi
