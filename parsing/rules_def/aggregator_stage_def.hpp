
/**
 * \file aggregator_stage_def.hpp
 * \brief Grammar rules for aggregator stage.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AGGREGATOR_STAGE_DEF_HPP)
#define BOOST_SPIRIT_X3_AGGREGATOR_STAGE_DEF_HPP

#include "../common.hpp"
#include "../ast/ast_adapted.hpp"
#include "../error_handler.hpp"
#include "aggregator_stage.hpp"

#include "selector_stage.hpp"
#include "group_filter_stage.hpp"

namespace client { namespace parser {

   extern x3::symbols<ast::unirec> input_keyword;
   x3::symbols<ast::aggr_func_with_param> aggr_func_with_param_keyword;
   x3::symbols<ast::aggr_func_without_param> aggr_func_without_param_keyword;

   void add_aggregator_keywords()
   {
      static bool once = false;
      if (once) {
          return;
      }
      once = true;

      aggr_func_with_param_keyword.add("SUM", ast::ap_sum)
      ("AVG", ast::ap_average)
      ("MIN", ast::ap_minimal)
      ("MAX", ast::ap_maximal)
      ("FIRST", ast::ap_first)
      ("LAST", ast::ap_last)
      ("OR", ast::ap_or)
      ("AND", ast::ap_and)
      ("COUNT_DISTINCT", ast::ap_countDistinct)
      ("RATE", ast::ap_rate);

      aggr_func_without_param_keyword.add("COUNT", ast::ad_count);
   }
   using x3::lit;

   struct aggregator_body_class;
   struct aggr_assignment_class;
   struct aggr_func_type_class;
   struct aggr_func_with_param_s_class;
   struct aggr_func_without_param_class;
   struct ag_stage_succ_class;

   struct aggregator_successor_class;
   struct branch_after_ag_s_class;
   struct branch_after_ag_class;
   struct branch_after_ag_succ_class;

   aggregator_stage_type const aggregator_stage = "aggregator_stage";
   x3::rule<aggregator_body_class, ast::aggregator_body> const aggregator_body = "aggregator_body";
   x3::rule<aggr_assignment_class, ast::aggr_assignment> const aggr_assignment = "aggr_assignment";
   x3::rule<aggr_func_type_class, ast::aggr_func_type> const aggr_func_type = "aggr_func_type";
   x3::rule<aggr_func_with_param_s_class, ast::aggr_func_with_param_s> const aggr_func_with_param_s = "aggr_func_with_param_s";
   x3::rule<aggr_func_without_param_class, ast::aggr_func_without_param> const aggr_func_without_param = "aggr_func_without_param";
   x3::rule<ag_stage_succ_class, ast::ag_stage_succ> const ag_stage_succ = "ag_stage_succ";

   x3::rule<aggregator_successor_class, ast::aggregator_successor> const aggregator_successor = "aggregator_successor";
   x3::rule<branch_after_ag_s_class, ast::branch_after_ag_s> const branch_after_ag_s = "branch_after_ag_s";
   x3::rule<branch_after_ag_class, ast::branch_after_ag> const branch_after_ag = "branch_after_ag";
   x3::rule<branch_after_ag_succ_class, ast::branch_after_ag_succ> const branch_after_ag_succ = "branch_after_ag_succ";

   namespace {
     auto const &group_filter_stage = client::group_filter_stage();
     auto const &selector_stage = client::selector_stage();
   }

   /* aggregator stage important rules definitions - begin
    * ------- */
   auto const aggregator_stage_def =
      lit("aggregator") > ':' > aggregator_body > ';' > aggregator_successor;

   auto const aggregator_body_def =
      aggr_assignment > *(',' > aggr_assignment);

   auto const aggr_assignment_def =
      identifier > '=' > aggr_func_type;

   auto const aggr_func_type_def =
      aggr_func_with_param_s | aggr_func_without_param;

   auto const aggr_func_with_param_s_def =
      aggr_func_with_param_keyword > ('(' > input_keyword > ')');

   auto const aggr_func_without_param_def =
      aggr_func_without_param_keyword > ('(' > -lit('*') > ')');

   auto const ag_stage_succ_def =
       group_filter_stage | selector_stage;

   /* -------
    * end */

   auto const aggregator_successor_def =
       ag_stage_succ | branch_after_ag_s;
   auto const branch_after_ag_s_def =
       branch_after_ag > +branch_after_ag;
   auto const branch_after_ag_def =
      lit("branch") > identifier > ('{' > branch_after_ag_succ > '}');
   auto const branch_after_ag_succ_def =
       branch_after_ag | ag_stage_succ;

   BOOST_SPIRIT_DEFINE(
        aggregator_stage
      , aggregator_body
      , aggr_assignment
      , aggr_func_type
      , aggr_func_with_param_s
      , aggr_func_without_param
      , ag_stage_succ
      , aggregator_successor
      , branch_after_ag_s
      , branch_after_ag
      , branch_after_ag_succ
   );

   struct aggregator_stage_class:error_handler_base {};
}}

namespace client {
   parser::aggregator_stage_type const &aggregator_stage()
   {
      parser::add_aggregator_keywords();
      return parser::aggregator_stage;
   }
}
#endif
