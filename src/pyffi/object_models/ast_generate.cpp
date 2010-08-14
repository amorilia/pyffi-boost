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
#include <boost/spirit/include/support_ostream_iterator.hpp>

#include "pyffi/object_models/ast.hpp"

namespace pyffi
{

namespace object_models
{

namespace karma = boost::spirit::karma;

template <typename Iterator>
class scope_karma_grammar : public karma::grammar<Iterator, Scope()>
{
public:
    karma::rule<Iterator, Scope()> start;
    karma::rule<Iterator, Declaration(int)> declaration;
    karma::rule<Iterator, Class(int)> class_;
    karma::rule<Iterator, Attr(int)> attr;
    karma::rule<Iterator, Scope(int)> scope;
    karma::rule<Iterator, void(int)> indent;

    scope_karma_grammar() : scope_karma_grammar::base_type(start) {
        indent = karma::left_align(karma::_r1)[karma::eps];
        start = scope(0) << karma::eol;
        declaration = class_(karma::_r1) | attr(karma::_r1);
        scope = declaration(karma::_r1) % karma::eol;
        class_ =
            indent(karma::_r1)
            << "class "
            << karma::string // Class.name
            << -('(' << karma::string << ')') // Class.base_name
            << -(':' << karma::eol << scope(karma::_r1 + 4)); // Class.scope
        attr =
            indent(karma::_r1)
            << karma::string // Attr.class_name
            << ' '
            << karma::string; // Attr.name
    }
};

bool generate(std::ostream & out, Scope const & scope)
{
    // wrap ostream into iterator
    boost::spirit::ostream_iterator sink(out);

    // create parser
    scope_karma_grammar<boost::spirit::ostream_iterator> parser;

    // use iterator to parse class
    return karma::generate(sink, parser, scope);
}

}

}
