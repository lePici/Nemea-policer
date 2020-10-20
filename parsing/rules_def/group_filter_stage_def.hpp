/**
 * \file group_filter_stage_def.hpp
 * \brief Grammar rules for group-filter stage.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_GROUP_FILTER_STAGE_DEF_HPP)
#define BOOST_SPIRIT_X3_GROUP_FILTER_STAGE_DEF_HPP

#include "../common.hpp"
#include "../ast/ast_adapted.hpp"
#include "../error_handler.hpp"
#include "group_filter_stage.hpp"

#include "selector_stage.hpp"

namespace client { namespace parser
{
   using x3::lit;
   using ascii::char_;
   using x3::no_skip;

   struct group_filter_body_class;
   struct group_filter_successor_class;

   struct branch_after_gf_s_class;
   struct branch_after_gf_class;
   struct branch_after_gf_succ_class;

   group_filter_stage_type const group_filter_stage = "group_filter_stage";
   x3::rule<group_filter_body_class, ast::group_filter_body> const group_filter_body = "group_filter_body";
   x3::rule<group_filter_successor_class, ast::group_filter_successor> const group_filter_successor = "group_filter_successor";

   x3::rule<branch_after_gf_s_class, ast::branch_after_gf_s> const branch_after_gf_s = "branch_after_gf_s";
   x3::rule<branch_after_gf_class, ast::branch_after_gf> const branch_after_gf = "branch_after_gf";
   x3::rule<branch_after_gf_succ_class, ast::branch_after_gf_succ> const branch_after_gf_succ = "branch_after_gf_succ";

   namespace {
      auto const& selector_stage = client::selector_stage();
   }

   /* group filter stage important rules definitions - begin
    * ------- */
   auto const group_filter_stage_def = 
      lit("group-filter") > ':' > group_filter_body > ';' > group_filter_successor;

   auto const group_filter_body_def = 
      +((!(lit(';'))) > no_skip[char_]);

   auto const group_filter_successor_def = 
      selector_stage | branch_after_gf_s;

   /* -------
    * end */

   auto const branch_after_gf_s_def =
      branch_after_gf > +branch_after_gf;
   auto const branch_after_gf_def = 
      lit("branch") > identifier > ('{' > branch_after_gf_succ > '}');
   auto const branch_after_gf_succ_def =
      branch_after_gf | selector_stage;

   BOOST_SPIRIT_DEFINE(
        group_filter_stage
      , group_filter_body
      , group_filter_successor
      , branch_after_gf_s
      , branch_after_gf
      , branch_after_gf_succ
   );

   struct group_filter_stage_class : error_handler_base {};
}}

namespace client
{
   parser::group_filter_stage_type const& group_filter_stage()
   {
      return parser::group_filter_stage;
   }
}

#endif
