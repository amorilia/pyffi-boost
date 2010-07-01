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

#include "pyffi/object_models/ast.hpp"

namespace pyffi
{

namespace object_models
{

namespace qi = boost::spirit::qi;

template <typename Iterator, typename Skipper = qi::grammar<Iterator> >
struct scope_qi_grammar : qi::grammar<Iterator, Skipper, Scope()> {
    qi::rule<Iterator, Skipper, Scope()> start;
    qi::rule<Iterator, Skipper, Class(int)> class_;
    qi::rule<Iterator, Skipper, Attr(int)> attr;
    qi::rule<Iterator, Skipper, Scope(int)> scope;
    qi::rule<Iterator, Skipper, void(int)> indent;

    scope_qi_grammar()
        : scope_qi_grammar::base_type(start) {

    /*
    indent = qi::repeat(qi::_r1)[' '];

    start = scope(0);

    scope = *(class_(qi::_r1) | attr(qi::_r1));

    class_ =
        indent(qi::_r1)
        >> qi::lit("class")
        >> qi::string // Class.name
        >> -('(' >> qi::string >> ')') // Class.base_name
        >> -(':' >> qi::eol >> scope(qi::_r1 + 4)) // Class.scope
        >> qi::eol;

    attr =
        indent(qi::_r1)
        >> qi::string // Attr.class_name
        >> qi::string // Attr.name
        >> qi::eol;
    */
}
};

bool parse(std::istream & in, Scope & scope)
{
	// wrap istream into iterator
	boost::spirit::istream_iterator first(in);
	boost::spirit::istream_iterator last;

	// create parser
	scope_qi_grammar<boost::spirit::istream_iterator, qi::ascii::space_type> parser;

	// use iterator to parse stream
	bool r = qi::phrase_parse(first, last, parser, qi::ascii::space, scope);

	// fail if we did not get a full match
	if (!r || first != last)
		throw std::runtime_error("Syntax error while parsing.");
}

}

}
