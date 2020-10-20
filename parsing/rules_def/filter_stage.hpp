/**
 * \file filter_stage.hpp
 * \brief Interface for filter grammar rules.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_FILTER_STAGE_HPP)
#define BOOST_SPIRIT_X3_FILTER_STAGE_HPP

#include <boost/spirit/home/x3.hpp>
#include "../ast/ast.hpp"

namespace client
{
   namespace x3 = boost::spirit::x3;
   namespace parser
   {
      struct filter_stage_class;
      typedef x3::rule<filter_stage_class, ast::filter_stage> filter_stage_type;
      typedef filter_stage_type::id filter_stage_id;
      BOOST_SPIRIT_DECLARE(filter_stage_type);
   }

   parser::filter_stage_type const& filter_stage();
}

#endif
