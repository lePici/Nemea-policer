/**
 * \file selector_stage_def.hpp
 * \brief Grammar rules for selector stage.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_SELECTOR_STAGE_DEF_HPP)
#define BOOST_SPIRIT_X3_SELECTOR_STAGE_DEF_HPP

#include "../common.hpp"
#include "../ast/ast_adapted.hpp"
#include "../error_handler.hpp"
#include "selector_stage.hpp"

namespace client { namespace parser
{

   extern x3::symbols<ast::unirec> input_keyword;
   x3::symbols<ast::sel_optoken> sel_additive_op;
   x3::symbols<ast::sel_optoken> sel_multiplicative_op;
   x3::symbols<ast::sel_optoken> sel_unary_op;

   void add_selector_keywords()
   {
      static bool once = false;
      if (once) {
          return;
      }
      once = true;

      sel_additive_op.add
         ("+", ast::op_plus)
         ("-", ast::op_minus)
         ;

      sel_multiplicative_op.add
         ("*", ast::op_times)
         ("/", ast::op_divide)
         ;

      sel_unary_op.add
         ("+", ast::op_positive)
         ("-", ast::op_negative)
         ;
   }

   using x3::lit;
   using ascii::char_;
   using x3::double_;
   using x3::no_skip;

   struct selector_body_class;
   struct sel_assignment_class;
   struct sel_expression_class;
   struct sel_output_class;
   struct sel_additive_expr_class;
   struct sel_multiplicative_expr_class;
   struct sel_unary_expr_class;
   struct sel_primary_expr_class;
   struct sel_variable_class;

   selector_stage_type const selector_stage = "selector_stage";
   x3::rule<selector_body_class, ast::selector_body> const selector_body = "selector_body";

   x3::rule<sel_assignment_class, ast::sel_assignment> const sel_assignment = "sel_assignment";
   x3::rule<sel_expression_class, ast::sel_expression> const sel_expression = "sel_expression";
   x3::rule<sel_output_class, ast::sel_output> const sel_output = "sel_output";
   x3::rule<sel_additive_expr_class, ast::sel_expression> const sel_additive_expr = "sel_additive_expr";
   x3::rule<sel_multiplicative_expr_class, ast::sel_expression> const sel_multiplicative_expr = "sel_multiplicative_expr";
   x3::rule<sel_unary_expr_class, ast::sel_operand> const sel_unary_expr = "sel_unary_expr";
   x3::rule<sel_primary_expr_class, ast::sel_operand> const sel_primary_expr = "sel_primary_expr";
   x3::rule<sel_variable_class, ast::sel_variable> const sel_variable = "sel_variable";

   auto const selector_stage_def = 
      lit("selector") > ':' > selector_body > ';';
   auto const selector_body_def = 
      sel_assignment > *(',' > sel_assignment);
   auto const sel_assignment_def = 
      sel_output > -('=' > sel_expression);
   auto const sel_output_def = 
      input_keyword | ('"' > +((!(lit('"'))) > no_skip[char_]) > '"') | identifier;

   auto const sel_expression_def = 
      sel_additive_expr;

   auto const sel_additive_expr_def =
          sel_multiplicative_expr
      > *(sel_additive_op > sel_multiplicative_expr)
      ;

   auto const sel_multiplicative_expr_def =
          sel_unary_expr
      > *(sel_multiplicative_op > sel_unary_expr)
      ;

   auto const sel_unary_expr_def =
          sel_primary_expr
      |   (sel_unary_op > sel_primary_expr)
      ;

   auto const sel_primary_expr_def =
          double_
      |   sel_variable
      |   ('(' > (sel_expression) > ')')
      ;

   auto const sel_variable_def =
      input_keyword | identifier;

   BOOST_SPIRIT_DEFINE(
         selector_stage
      ,  selector_body
      ,  sel_assignment
      ,  sel_output
      ,  sel_expression
      ,  sel_additive_expr
      ,  sel_multiplicative_expr
      ,  sel_unary_expr
      ,  sel_primary_expr
      ,  sel_variable
   );

   struct selector_stage_class : error_handler_base {};
}}

namespace client
{
   parser::selector_stage_type const& selector_stage()
   {
      parser::add_selector_keywords();
      return parser::selector_stage;
   }
}

#endif
