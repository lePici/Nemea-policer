
/**
 * \file common.hpp
 * \brief Common rules and functions to Spirit X3 parsing.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_COMMON_HPP)
#define BOOST_SPIRIT_X3_COMMON_HPP

#include <boost/spirit/home/x3.hpp>
#include <string>
#include <vector>

namespace client {
   namespace parser {

      namespace x3 = boost::spirit::x3;
      using x3::raw;
      using x3::lexeme;
      using x3::alpha;
      using x3::alnum;
      struct identifier_class;
      typedef x3::rule <identifier_class, std::string> identifier_type;
      identifier_type const identifier = "identifier";

      /**
       * \brief Identifier of aggregator variable or name of branch.
       */
      auto const identifier_def = raw[lexeme[(alpha | '_') >> *(alnum | '_')]];

      BOOST_SPIRIT_DEFINE(identifier);

      /**
       * \brief Definition of Unirec keywords.
       */
      void define_input_keywords(std::vector<std::string> keywords);
}}
#endif
