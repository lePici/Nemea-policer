/**
 * \file filter_stage_def.hpp
 * \brief Grammar rules for filter stage.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_FILTER_STAGE_DEF_HPP)
#define BOOST_SPIRIT_X3_FILTER_STAGE_DEF_HPP

#include "../common.hpp"
#include "../ast/ast_adapted.hpp"
#include "../error_handler.hpp"
#include "filter_stage.hpp"

#include "selector_stage.hpp"
#include "aggregator_stage.hpp"
#include "window_stage.hpp"
#include "grouper_stage.hpp"

#include <iostream>

namespace client { namespace parser
{
   using x3::lit;
   using ascii::char_;
   using ascii::print;
   using ascii::space;
   using x3::no_skip;

   struct filter_body_class;
   struct fi_stage_succ_class;

   struct filter_successor_class;
   struct branch_after_fi_s_class;
   struct branch_after_fi_class;
   struct branch_after_fi_succ_class;

   filter_stage_type const filter_stage = "filter_stage";
   x3::rule<filter_body_class, ast::filter_body> const filter_body = "filter_body";
   x3::rule<fi_stage_succ_class, ast::fi_stage_succ> const fi_stage_succ = "fi_stage_succ";

   x3::rule<filter_successor_class, ast::filter_successor> const filter_successor = "filter_successor";
   x3::rule<branch_after_fi_s_class, ast::branch_after_fi_s> const branch_after_fi_s = "branch_after_fi_s";
   x3::rule<branch_after_fi_class, ast::branch_after_fi> const branch_after_fi = "branch_after_fi";
   x3::rule<branch_after_fi_succ_class, ast::branch_after_fi_succ> const branch_after_fi_succ = "branch_after_fi_succ";

   namespace {
      auto const& grouper_stage = client::grouper_stage();
      auto const& window_stage = client::window_stage();
      auto const& aggregator_stage = client::aggregator_stage();
      auto const& selector_stage = client::selector_stage();
   }

   /* filter stage important rules definitions - begin
    * ------- */
   auto const filter_stage_def = 
      lit("filter") > ':' > filter_body > ';' > filter_successor;

   auto const filter_body_def = 
      +((!(lit(';'))) > no_skip[char_]);

   auto const fi_stage_succ_def = 
      grouper_stage | window_stage | aggregator_stage | selector_stage;
   /* -------
    * end */

   auto const filter_successor_def = 
      fi_stage_succ | branch_after_fi_s;
   auto const branch_after_fi_s_def =
      branch_after_fi > +branch_after_fi;
   auto const branch_after_fi_def = 
      lit("branch") > identifier > ('{' > branch_after_fi_succ > '}');
   auto const branch_after_fi_succ_def =
      branch_after_fi | fi_stage_succ;

   BOOST_SPIRIT_DEFINE(
        filter_stage
      , filter_body
      , fi_stage_succ
      , filter_successor
      , branch_after_fi_s
      , branch_after_fi
      , branch_after_fi_succ
   );

   struct filter_stage_class : error_handler_base {};
}}

namespace client
{
   parser::filter_stage_type const& filter_stage()
   {
      return parser::filter_stage;
   }
}

#endif
