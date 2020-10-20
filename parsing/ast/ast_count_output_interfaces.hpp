/**
 * \file ast_count_output_interfaces.hpp
 * \brief Counting of selector stages.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AST_COUNT_OUTPUT_INTERFACES_HPP)
#define BOOST_SPIRIT_X3_AST_COUNT_OUTPUT_INTERFACES_HPP

#include "ast_visitor.hpp"

namespace client { namespace ast
{

   class Count_output_interfaces: public Visitor<class Visitor_T>
   {
      /**
       * \brief In other words number of Selector stages.
       */
      int number_of_output_interfaces = 0;

      public:

      using Visitor::operator();

      void operator()(selector_stage const& ss) override;

      /**
       * \return number of Selector stages.
       */
      int get_number_of_output_interfaces();
   };
}}
#endif
