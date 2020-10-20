/**
 * \file grouper_stage_def.hpp
 * \brief Grammar rules for grouper stage.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_GROUPER_STAGE_DEF_HPP)
#define BOOST_SPIRIT_X3_GROUPER_STAGE_DEF_HPP

#include "../common.hpp"
#include "../ast/ast_adapted.hpp"
#include "../error_handler.hpp"
#include "grouper_stage.hpp"

#include "selector_stage.hpp"
#include "group_filter_stage.hpp"
#include "aggregator_stage.hpp"
#include "window_stage.hpp"

namespace client { namespace parser
{
   extern x3::symbols<ast::unirec> input_keyword;
   x3::symbols<ast::grouper_case> grouper_case_keyword;

   void add_grouper_keywords()
   {
      static bool once = false;
      if (once) {
          return;
      }
      once = true;

      grouper_case_keyword.add
          ("_bidirectional_", ast::g_bidirectional)
          ;
   }

   using x3::lit;

   struct grouper_body_class;
   struct grouper_keys_class;
   struct gr_stage_succ_class;

   struct grouper_successor_class;
   struct branch_after_gr_s_class;
   struct branch_after_gr_class;
   struct branch_after_gr_succ_class;

   grouper_stage_type const grouper_stage = "grouper_stage";
   x3::rule<grouper_body_class, ast::grouper_body> const grouper_body = "grouper_body";
   x3::rule<grouper_keys_class, ast::grouper_keys> const grouper_keys = "grouper_keys";
   x3::rule<gr_stage_succ_class, ast::gr_stage_succ> const gr_stage_succ = "gr_stage_succ";

   x3::rule<grouper_successor_class, ast::grouper_successor> const grouper_successor = "grouper_successor";
   x3::rule<branch_after_gr_s_class, ast::branch_after_gr_s> const branch_after_gr_s = "branch_after_gr_s";
   x3::rule<branch_after_gr_class, ast::branch_after_gr> const branch_after_gr = "branch_after_gr";
   x3::rule<branch_after_gr_succ_class, ast::branch_after_gr_succ> const branch_after_gr_succ = "branch_after_gr_succ";

   namespace {
      auto const& window_stage = client::window_stage();
      auto const& aggregator_stage = client::aggregator_stage();
      auto const& group_filter_stage = client::group_filter_stage();
      auto const& selector_stage = client::selector_stage();
   }

   /* grouper stage important rules definitions - begin
    * -------*/
   auto const grouper_stage_def = 
      lit("grouper") > ':' > grouper_body > ';' > grouper_successor;

   auto const grouper_body_def = 
      grouper_case_keyword | grouper_keys;

   auto const grouper_keys_def =
      input_keyword > *(',' > input_keyword);

   auto const gr_stage_succ_def = 
      window_stage | aggregator_stage | group_filter_stage | selector_stage;
   /* -------
    * end */

   auto const grouper_successor_def = 
      gr_stage_succ | branch_after_gr_s;
   auto const branch_after_gr_s_def =
      branch_after_gr > +branch_after_gr;
   auto const branch_after_gr_def = 
      lit("branch") > identifier > ('{' > branch_after_gr_succ > '}');
   auto const branch_after_gr_succ_def =
      branch_after_gr | gr_stage_succ;

   BOOST_SPIRIT_DEFINE(
        grouper_stage
      , grouper_body
      , grouper_keys
      , gr_stage_succ
      , grouper_successor
      , branch_after_gr_s
      , branch_after_gr
      , branch_after_gr_succ
   );

   struct grouper_stage_class : error_handler_base {};
}}

namespace client
{
   parser::grouper_stage_type const& grouper_stage()
   {
      parser::add_grouper_keywords();
      return parser::grouper_stage;
   }
}

#endif
