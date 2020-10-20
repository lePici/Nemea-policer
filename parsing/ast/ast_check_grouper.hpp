
/**
 * \file ast_check_grouper.hpp
 * \brief Semantic checks in grouper stage.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AST_CHECK_GROUPER_HPP)
#define BOOST_SPIRIT_X3_AST_CHECK_GROUPER_HPP

#include "ast.hpp"
#include "ast_check_branch_names.hpp"
#include <string>
#include <vector>


namespace client { namespace ast
{

   /**
    * \brief Semantic checks in Grouper stage.
    */
   class Check_grouper: public Check_branch_names
   {
      /**
       * \brief Aggregation keys.
       */
      std::vector<std::string> keys;

      /**
       * \brief Storing aggregation parameters.
       * \detail needed because the aggregation function and the aggregation key
       *    cannot have the same parameter.
       */
      std::vector<std::string> agg_params;

      /**
       * \brief Flag to note if we are in a Grouper stage.
       */
      bool groDiv = false;

      /**
       * \brief Flag to note if we are in a Aggregator stage.
       */
      bool aggDiv = false;

      /**
       * \brief Check for valid input keyword as aggregation key.
       */
      void check_keys1(void);

      /**
       * \brief Check if aggregation key is not in collision with aggregation function.
       */
      void check_keys2(void);
   
      public:

      using Check_branch_names::operator();
      void operator()(unirec const&);
      void operator()(aggregator_stage const& as);
      void operator()(grouper_stage const& gs);
};
}}

#endif
