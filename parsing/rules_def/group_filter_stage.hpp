/**
 * \file group_filter_stage.hpp
 * \brief Interface for group-filter grammar rules.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_GROUP_FILTER_STAGE_HPP)
#define BOOST_SPIRIT_X3_GROUP_FILTER_STAGE_HPP

#include <boost/spirit/home/x3.hpp>
#include "../ast/ast.hpp"

namespace client
{
   namespace x3 = boost::spirit::x3;
   namespace parser
   {
      struct group_filter_stage_class;
      typedef x3::rule<group_filter_stage_class, ast::group_filter_stage> group_filter_stage_type;
      typedef group_filter_stage_type::id group_filter_stage_id;
      BOOST_SPIRIT_DECLARE(group_filter_stage_type);
   }

   parser::group_filter_stage_type const& group_filter_stage();
}

#endif
