/**
 * \file grouper_stage.hpp
 * \brief Interface for grouper grammar rules.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_GROUPER_STAGE_HPP)
#define BOOST_SPIRIT_X3_GROUPER_STAGE_HPP

#include <boost/spirit/home/x3.hpp>
#include "../ast/ast.hpp"

namespace client
{
   namespace x3 = boost::spirit::x3;
   namespace parser
   {
      struct grouper_stage_class;
      typedef x3::rule<grouper_stage_class, ast::grouper_stage> grouper_stage_type;
      typedef grouper_stage_type::id grouper_stage_id;
      BOOST_SPIRIT_DECLARE(grouper_stage_type);
   }

   parser::grouper_stage_type const& grouper_stage();
}

#endif
