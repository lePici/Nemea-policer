
/**
 * \file ast_alias4variables.hpp
 * \brief Find aliases for variables.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AST_ALIAS4VARIABLES_HPP)
#define BOOST_SPIRIT_X3_AST_ALIAS4VARIABLES_HPP

#include "ast_variables.hpp"

namespace client { namespace ast
{

   /**
    * \brief Creation of aliases and their corresponding variables.
    */
   class Alias4variables: public Variables
   {

   private:
      filMapT filterVars;        ///< User variables from each filter bodie.
      grfiMapT group_filterVars; ///< User variables from each group-filter bodie.
      aggrMapT aggrVars;         ///< User variables from each aggregator.
      selMapT selVars;           ///< User variables from each selector.
      bool aggIsPresent = true;  ///< Set true if Aggregator stage is present in current branch.
      bool selDive = false;      ///< Set true while browsing through Selector body.
      varsT *lastAggr;           ///< Pointer to last Aggregator variables.
      varNameT lastAggrVar;      ///< Last processed Aggregator variable.

   public:

      /**
       * \param[in] fb User variables from each Filter bodie.
       * \param[in] gfb User variables from each Group-filter bodie.
       */
      Alias4variables(filMapT fb, grfiMapT gfb) {
         filterVars = fb;
         group_filterVars = gfb;
         lastAggr = NULL;
      };

      filMapT& get_filterVars(void) {
        return filterVars;
      }

      aggrMapT& get_aggrVars(void) {
        return aggrVars;
      }

      grfiMapT& get_group_filterVars(void) {
        return group_filterVars;
      }

      selMapT& get_selVars(void) {
        return selVars;
      }

      using Variables::operator();

      // void operator()(filter_stage const& fs) override;
      void operator() (aggregator_stage const &as) override;
      void operator() (aggr_assignment const &as) override;
      void operator() (aggr_func_with_param_s const &) override;
      void operator() (aggr_func_without_param const &) override;
      void operator() (group_filter_stage const &gfs) override;
      void operator() (selector_stage const &ss) override;
      void operator() (sel_assignment const &sa) override;
      void operator() (std::string const &) override;
      };

      /**
       * \brief Concatenation of aggregation function name and Unirec name with "_" delimiter.
       * \param[in] func_name name of aggregation function.
       * \param[in] param Unirec name.
       * \return string consisting of [(string) func_name]_[(string) param].
       */
      std::string put_together_function_alias(aggr_func_with_param func_name, unirec param);

}}

#endif
