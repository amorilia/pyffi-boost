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

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <stdexcept>

#include "pyffi/object_models/scope.hpp"

namespace pyffi
{

namespace object_models
{

namespace engine = boost::spirit::qi;

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
    engine::rule<Iterator, Scope(int)> scope;
    engine::rule<Iterator, void(int)> indent;
    engine::rule<Iterator, std::string()> class_name;
    engine::rule<Iterator, std::string()> attr_name;

    scope_grammar() : scope_grammar::base_type(start) {
        indent %= engine::repeat(engine::_r1)[' '];
        start %= scope(0) >> engine::eol;
        declaration %= class_(engine::_r1) | attr(engine::_r1) | if_elifs_else(engine::_r1);
        scope %= declaration(engine::_r1) % engine::eol;
        class_ %=
            indent(engine::_r1)
            >> "class "
            >> class_name // Class.name
            >> -('(' >> class_name >> ')') // Class.base_name
            >> -(':' >> engine::eol >> scope(engine::_r1 + 4)); // Class.scope
        attr %=
            indent(engine::_r1)
            >> class_name // Attr.class_name
            >> ' '
            >> attr_name; // Attr.name
        expr = engine::bool_; // for now
        if_ %=
            indent(engine::_r1)
            >> "if "
            >> expr // If.expr
            >> ':' >> engine::eol >> scope(engine::_r1 + 4); // If.scope
        elif_ %=
            indent(engine::_r1)
            >> "elif "
            >> expr // If.expr
            >> ':' >> engine::eol >> scope(engine::_r1 + 4); // If.scope
        else_ %=
            indent(engine::_r1)
            >> "else"
            >> ':' >> engine::eol >> scope(engine::_r1 + 4); // Scope
        if_elifs %=
            if_(engine::_r1) // IfElifsElse.ifs_[0]
            >> *(engine::eol >> elif_(engine::_r1)); // IfElifsElse.ifs_[1:]
        if_elifs_else %=
            if_elifs(engine::_r1) // IfElifsElse.ifs_
            >> -(engine::eol >> else_(engine::_r1)); // IfElifsElse.else_
        class_name %= engine::upper >> *(engine::lower | engine::upper | engine::digit);
        attr_name %= engine::lower >> *(engine::lower | engine::digit | engine::char_('_'));

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
        class_name.name("class-name");
        attr_name.name("attr-name");

        /*
        engine::debug(indent);
        //engine::debug(start);
        //engine::debug(declaration);
        //engine::debug(scope);
        //engine::debug(class_);
        engine::debug(attr);
        engine::debug(expr);
        //engine::debug(if_);
        //engine::debug(elif_);
        //engine::debug(else_);
        //engine::debug(if_elifs);
        //engine::debug(if_elifs_else);
        engine::debug(class_name);
        engine::debug(attr_name);
        */
    }
};

bool Scope::parse(std::istream & in)
{
    // disable skipping of whitespace
    in.unsetf(std::ios::skipws);

    // wrap istream into iterator
    boost::spirit::istream_iterator first(in);
    boost::spirit::istream_iterator last;

    // create parser
    scope_grammar<boost::spirit::istream_iterator> parser;

    // use iterator to parse stream
    bool r = engine::parse(first, last, parser, *this);

    // fail if we did not get a full match
    if (!r || first != last) {
        std::string rest(first, last);
        throw std::runtime_error(
            "Syntax error while parsing\nStopped at:\n" + rest);
    }
    return r;
}

} // namespace object_models

} // namespace pyffi
