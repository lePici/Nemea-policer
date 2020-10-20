/**
 * \file aggregator_stage.hpp
 * \brief Interface for aggregator grammar rules.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AGGREGATOR_STAGE_HPP)
#define BOOST_SPIRIT_X3_AGGREGATOR_STAGE_HPP

#include <boost/spirit/home/x3.hpp>
#include "../ast/ast.hpp"

namespace client
{
   namespace x3 = boost::spirit::x3;
   namespace parser
   {
      struct aggregator_stage_class;
      typedef x3::rule<aggregator_stage_class, ast::aggregator_stage> aggregator_stage_type;
      typedef aggregator_stage_type::id aggregator_stage_id;
      BOOST_SPIRIT_DECLARE(aggregator_stage_type);
   }

   parser::aggregator_stage_type const& aggregator_stage();
}

#endif
