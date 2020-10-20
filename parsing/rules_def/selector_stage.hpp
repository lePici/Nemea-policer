/**
 * \file selector_stage.hpp
 * \brief Interface for selector grammar rules.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_SELECTOR_STAGE_HPP)
#define BOOST_SPIRIT_X3_SELECTOR_STAGE_HPP

#include <boost/spirit/home/x3.hpp>
#include "../ast/ast.hpp"

namespace client
{
   namespace x3 = boost::spirit::x3;
   namespace ascii = boost::spirit::x3::ascii;

   namespace parser
   {
      struct selector_stage_class;
      typedef x3::rule<selector_stage_class, ast::selector_stage> selector_stage_type;
      typedef selector_stage_type::id selector_stage_id;
      BOOST_SPIRIT_DECLARE(selector_stage_type);
   }

   parser::selector_stage_type const& selector_stage();
}

#endif
