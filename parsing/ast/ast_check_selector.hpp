
/**
 * \file ast_check_selector.hpp
 * \brief Semantic checks in selector stage.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AST_CHECK_SELECTOR_HPP)
#define BOOST_SPIRIT_X3_AST_CHECK_SELECTOR_HPP

#include "ast_check_branch_names.hpp"
#include <set>
#include <string>

namespace client { namespace ast
{

   /**
    * \brief Semantic checks in Selector stage.
    */
   class Check_selector: public Check_branch_names
   {
      /**
       * \brief Flag to note if we are in a Grouper stage.
       */
      bool groDiv = false;

      /**
       * \brief Flag to note if we are in a Aggregator stage.
       */
      bool aggDiv = false;

      /**
       * \brief Flag to note if we are in a Selector stage.
       */
      bool selDiv = false;

      /**
       * \brief Storing keywords in last Grouper stage.
       */
      std::set<std::string> gro_input_keywords;

      /**
       * \brief Storing keywords in last Aggregator stage.
       */
      std::set<std::string> agg_input_keywords;
   
      public:
      using Check_branch_names::operator();
   
      void operator()(grouper_stage const& gs) override;
      void operator()(aggregator_stage const& as) override;
      void operator()(selector_stage const& ss) override;
      void operator()(unirec const& un) override;
   };

}}
#endif
