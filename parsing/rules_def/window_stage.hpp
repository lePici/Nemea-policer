/**
 * \file window_stage.hpp
 * \brief Interface for window grammar rules.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_WINDOW_STAGE_HPP)
#define BOOST_SPIRIT_X3_WINDOW_STAGE_HPP

#include <boost/spirit/home/x3.hpp>
#include "../ast/ast.hpp"

namespace client
{
   namespace x3 = boost::spirit::x3;
   namespace ascii = boost::spirit::x3::ascii;

   namespace parser
   {
      struct window_stage_class;
      typedef x3::rule<window_stage_class, ast::window_stage> window_stage_type;
      typedef window_stage_type::id window_stage_id;
      BOOST_SPIRIT_DECLARE(window_stage_type);
   }

   parser::window_stage_type const& window_stage();
}

#endif
