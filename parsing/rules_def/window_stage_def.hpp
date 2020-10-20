/**
 * \file window_stage_def.hpp
 * \brief Grammar rules for window stage.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_WINDOW_STAGE_DEF_HPP)
#define BOOST_SPIRIT_X3_WINDOW_STAGE_DEF_HPP

#include "../common.hpp"
#include "../ast/ast_adapted.hpp"
#include "../error_handler.hpp"
#include "window_stage.hpp"

#include "selector_stage.hpp"
#include "group_filter_stage.hpp"
#include "aggregator_stage.hpp"

namespace client { namespace parser
{
   extern x3::symbols<ast::unirec> input_keyword;
   x3::symbols<ast::window_type> window_type_keyword;

   void add_window_keywords()
   {
      static bool once = false;
      if (once) {
         return;
      }
      once = true;

      window_type_keyword.add
         ("active", ast::w_active)
         ("passive", ast::w_passive)
         ("global", ast::w_global)
         ("mixed", ast::w_mixed)
         ;
   }

   using x3::lit;
   using ascii::char_;
   using x3::uint_;

   struct window_body_class;
   struct window_type_class;
   struct window_range_class;
   struct window_slide_class;
   struct window_seconds_class;
   struct wi_stage_succ_class;

   struct window_successor_class;
   struct branch_after_wi_s_class;
   struct branch_after_wi_class;
   struct branch_after_wi_succ_class;

   window_stage_type const window_stage = "window_stage";
   x3::rule<window_body_class, ast::window_body> const window_body = "window_body";
   x3::rule<window_type_class, ast::window_type> const window_type = "window_type";
   x3::rule<window_range_class, ast::seconds> const window_range = "RANGE";
   x3::rule<window_slide_class, ast::seconds> const window_slide = "SLIDE";
   x3::rule<window_seconds_class, ast::seconds> const window_seconds = "window_seconds";
   x3::rule<wi_stage_succ_class, ast::wi_stage_succ> const wi_stage_succ = "wi_stage_succ";

   x3::rule<window_successor_class, ast::window_successor> const window_successor = "window_successor";
   x3::rule<branch_after_wi_s_class, ast::branch_after_wi_s> const branch_after_wi_s = "branch_after_wi_s";
   x3::rule<branch_after_wi_class, ast::branch_after_wi> const branch_after_wi = "branch_after_wi";
   x3::rule<branch_after_wi_succ_class, ast::branch_after_wi_succ> const branch_after_wi_succ = "branch_after_wi_succ";

   namespace {
      auto const& aggregator_stage = client::aggregator_stage();
      auto const& group_filter_stage = client::group_filter_stage();
      auto const& selector_stage = client::selector_stage();
   }

   auto const minutes2seconds = [](auto& ctx){
      _val(ctx) = _attr(ctx) * 60;
   };
   auto const hours2seconds = [](auto& ctx){
      _val(ctx) = _attr(ctx) * 3600;
   };
   auto const justAssignment = [](auto& ctx){
      _val(ctx) = _attr(ctx);
   };

   /* not used */
   auto const check_time_field = [](auto& ctx){
       /* WARNING: this is for UNIREC only right now */
       if((_attr(ctx) != ast::unirec::f_timeFirst) && (_attr(ctx) != ast::unirec::f_timeLast)){
           _pass(ctx) = false;
       }
       else{
           _val(ctx) = _attr(ctx);
       }
   };

   /* window stage important rules definitions - begin
    * ------- */
   auto const window_stage_def = 
      lit("window") > ':' > window_body > ';' > window_successor;

   auto const window_body_def = 
      window_type > ',' > window_range > -(',' > window_slide);

   auto const window_type_def =
      (lit("type") | lit("TYPE")) > '=' > window_type_keyword; 

   auto const window_range_def =
      (lit("range") | lit("RANGE")) > '=' > window_seconds;

   auto const window_slide_def =
      (lit("slide") | lit("SLIDE")) > '=' > window_seconds;

   auto const window_seconds_def =
      (uint_[justAssignment] >> (lit("seconds") | lit("second"))) | 
      (uint_[minutes2seconds] >> (lit("minutes") | lit("minute"))) | 
      (uint_[hours2seconds] >> (lit("hours") | lit("hour")));

   auto const wi_stage_succ_def = 
      aggregator_stage | group_filter_stage | selector_stage;

   /* -------
    * end */
   auto const window_successor_def = 
      wi_stage_succ | branch_after_wi_s;
   auto const branch_after_wi_s_def =
      branch_after_wi > +branch_after_wi;
   auto const branch_after_wi_def = 
      lit("branch") > identifier > ('{' > branch_after_wi_succ > '}');
   auto const branch_after_wi_succ_def =
      branch_after_wi | wi_stage_succ;

   BOOST_SPIRIT_DEFINE(
        window_stage
      , window_body
      , window_type
      , window_range
      , window_slide
      , window_seconds
      , wi_stage_succ
      , window_successor
      , branch_after_wi_s
      , branch_after_wi
      , branch_after_wi_succ
   );

   struct window_stage_class : error_handler_base {};
}}

namespace client
{
   parser::window_stage_type const& window_stage()
   {
      parser::add_window_keywords();
      return parser::window_stage;
   }
}

#endif
