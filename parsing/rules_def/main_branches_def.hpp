/**
 * \file main_branches_def.hpp
 * \brief Grammar rules for main branches.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_MAIN_BRANCHES_DEF_HPP)
#define BOOST_SPIRIT_X3_MAIN_BRANCHES_DEF_HPP

#include "../common.hpp"
#include "../ast/ast_adapted.hpp"
#include "../error_handler.hpp"
#include "main_branches.hpp"

#include "selector_stage.hpp"
#include "aggregator_stage.hpp"
#include "window_stage.hpp"
#include "grouper_stage.hpp"
#include "filter_stage.hpp"

namespace client { namespace parser
{
   using x3::lit;

   struct main_branch_class;
   struct main_branch_body_class;
   struct stage_class;

   main_branches_type const main_branches = "main_branches";
   x3::rule<main_branch_class, ast::main_branch> const main_branch = "main_branch";
   x3::rule<main_branch_body_class, ast::main_branch_body> const main_branch_body = "main_branch_body";
   x3::rule<stage_class, ast::stage> const stage = "stage";

   /* Import the rules */
   namespace {
      auto const& filter_stage = client::filter_stage();
      auto const& grouper_stage = client::grouper_stage();
      auto const& window_stage = client::window_stage();
      auto const& aggregator_stage = client::aggregator_stage();
      auto const& selector_stage = client::selector_stage();
   }
   
   
   auto const main_branches_def = 
      main_branch > *main_branch;

   auto const main_branch_def =
      lit("branch") > identifier > ('{' > main_branch_body > '}');

   auto const main_branch_body_def =
      stage | +main_branch;

   auto const stage_def = 
      filter_stage | grouper_stage | window_stage | aggregator_stage | selector_stage;

   BOOST_SPIRIT_DEFINE(
        main_branches
      , main_branch
      , main_branch_body
      , stage
   );

   struct main_branches_class : error_handler_base {};
}}

namespace client
{
   parser::main_branches_type const& main_branches()
   {
      return parser::main_branches;
   }
}

#endif

