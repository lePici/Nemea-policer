/**
 * \file ast_printer.hpp
 * \brief Print of parsed text.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "ast.hpp"
#include "ast_visitor.hpp"
#include <iostream>
#include <string>

namespace client { namespace ast
{
    
   /**
    * \brief Print Abstract syntax tree.
    */
   struct Printer: public Visitor<class Printer_T>
   {
   
      public:
   
      using Visitor::operator();
   
      void operator()(filter_stage const& fs) override;
      void operator()(grouper_stage const& gs) override;
      void operator()(window_stage const& ws) override;
      void operator()(aggregator_stage const& as) override;
      void operator()(group_filter_stage const& gfs) override;
      void operator()(selector_stage const& ss) override;
   
      void operator()(main_branch const& mb) override;
   
      void operator()(grouper_keys const& gk) override;
      void operator()(grouper_case const& gc) override;
   
      void operator()(aggr_assignment const& as) override;
      void operator()(aggr_func_with_param_s const& af) override;
      void operator()(aggr_func_without_param const& af) override;
   
      void operator()(sel_assignment const& sa) override;
      void operator()(sel_expression const& se) override;
      void operator()(sel_operation const& so) override;
      void operator()(nil const& ni) override;
      void operator()(double const& integer) override;
      void operator()(sel_variable const& sv) override;
      void operator()(sel_unary const& su) override;
      void operator()(std::string const& str) override;
      void operator()(unirec const& unir) override;
   
      void operator()(boost::recursive_wrapper<main_branch_vec> const& param) override
         {tab(0);(*this)(param);};
   };
}}
