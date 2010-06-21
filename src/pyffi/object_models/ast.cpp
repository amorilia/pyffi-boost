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

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/support_ostream_iterator.hpp>

#include "pyffi/object_models/ast.hpp"

// upgrade structs to fusion sequences

BOOST_FUSION_ADAPT_STRUCT(
    pyffi::object_models::Class,
    (std::string, name)
    (boost::optional<std::string>, base_name)
    (boost::optional<pyffi::object_models::Scope>, scope)
)

BOOST_FUSION_ADAPT_STRUCT(
    pyffi::object_models::Attr,
    (std::string, class_name)
    (std::string, name)
)

namespace pyffi
{

namespace object_models
{

template <typename Iterator>
bool parse(Iterator first, Iterator last, Scope & scope)
{
}

bool parse(std::istream & in, Scope & scope)
{
}


namespace karma = boost::spirit::karma;

template <typename OutputIterator>
struct scope_karma_grammar : karma::grammar<OutputIterator, Scope()> {
    karma::rule<OutputIterator, Class()> class_;
    karma::rule<OutputIterator, Attr()> attr;
    karma::rule<OutputIterator, Scope()> scope;
    karma::rule<OutputIterator, void(int)> indent;

    scope_karma_grammar()
        : scope_karma_grammar::base_type(scope) {

    indent = karma::left_align(karma::_r1)[karma::eps];

    scope = *(class_ | attr);

    class_ =
        indent(0)
        << "class "
        << karma::string // Class.name
        << -('(' << karma::string << ')') // Class.base_name
        << karma::eol;

    attr =
        indent(0)
        << karma::string // Attr.class_name
        << ' ' << karma::string // Attr.name
        << karma::eol;
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
