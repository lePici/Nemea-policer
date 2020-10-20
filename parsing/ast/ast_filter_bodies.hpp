/**
 * \file ast_filter_bodies.hpp
 * \brief Syntactic and semantic checks in filter stage.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AST_FILTER_BODIES_HPP)
#define BOOST_SPIRIT_X3_AST_FILTER_BODIES_HPP
#include "ast.hpp"
#include "ast_variables.hpp"
#include <string>
#include <vector>
#include <map>

namespace client { namespace ast
{
   
   /**
    * \brief Parsing of filter/group-filter bodies.
    */
   class Parse_filter_bodies: public Variables
   {
      private:

      /**
       * \brief User variables from each Filter bodie. (It will be empty yet.)
       */
      filMapT filterVars;

      /**
       * \brief User variables from each Group-filter bodie.
       */
      grfiMapT group_filterVars;
   
      /**
       * \brief Parse filter body and get user variables.
       * \return found variables.
       */
      varsT parse_filter_body(filter_body, branchPathT);
   
      public:
   
      /**
       * \return filter variables.
       */
      filMapT get_filterVars(void)
      {
         return filterVars;
      }

      /**
       * \return group-filter variables.
       */
      grfiMapT get_group_filterVars(void)
      {
         return group_filterVars;
      }
   
      using Variables::operator();
   
      void operator()(group_filter_stage const& gfs) override;
   };
   
   /**
    * \brief Semantic checks in Filter/Group-filter stage.
    */
   class Check_filter_bodies: public Variables
   {
      private:
   
      /**
       * \brief User variables from each Filter bodie. (It will be empty yet.)
       */
      filMapT filterVars;

      /**
       * \brief User variables from each Group-filter bodie.
       */
      grfiMapT group_filterVars;
   
      /**
       * \brief User variables from corresponding Aggregator stage.
       */
      std::vector<std::string> aggrVars;
   
      public:
   
      /**
       * \param[in] User variables from each Filter bodie.
       * \param[in] User variables from each Group-filter bodie.
       */
      Check_filter_bodies(filMapT fb, grfiMapT gfb)
      {
         filterVars = fb;
         group_filterVars = gfb;
      };
   
      using Variables::operator();
   
      void operator()(filter_stage const& fs) override;
      void operator()(aggregator_stage const& as) override;
      void operator()(aggr_assignment const& as) override;
      void operator()(group_filter_stage const& gfs) override;
   };
}}

#endif
