
/**
 * \file ast_variables.hpp
 * \brief Creating map of variable and alias for all stages.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AST_VARIABLES_HPP)
#define BOOST_SPIRIT_X3_AST_VARIABLES_HPP
#include "ast_check_branch_names.hpp"
#include <map>
#include <vector>
#include <string>

namespace client { namespace ast
{

   /**
    * \brief Management of user variables.
    */
   struct Variables: public Check_branch_names
   {
      typedef struct varNameT_s {
         std::string name;
         std::string alias;
      } varNameT;
   
      using pathT = branchPathT;
      using varsT = std::vector<varNameT>;
      using varsMapT = std::map<pathT, varsT>;
      using filMapT = varsMapT;
      using grfiMapT = varsMapT;
      using aggrMapT = varsMapT;
      using selMapT = varsMapT;
   
      /**
       * \brief Get user variables from current path. 
       * \param[in] map of type filMapT, grfiMapt, aggrMapT or selMapT.
       * \param[in] act_stage name of current stage from which to take variables.
       * \return vector of name-alias tuples.
       */
      varsT* get_vars(varsMapT& map, std::string const& act_stage);

      /**
       * \brief Insert new name-alias vector to map.
       * \param[in, out] map destination.
       * \param[in] key path to stage.
       * \param[in] vars vector of name-alias tuple.
       */
      void add_vars(varsMapT& map, pathT const& key, varsT const& vars);

      /**
       * \brief Insert new name-alias tuple to vector which is located in map.
       * \param[in, out] map destination.
       * \param[in] act_stage name of current stage.
       * \param[in] var name-alias tuple.
       */
      void add_var(varsMapT& map, std::string const& act_stage, varNameT const& var);

      /**
       * \brief Get path to current stage.
       * \param[in] act_stage name of current stage.
       * \return path to requested stage.
       */
      branchPathT get_branchPath(std::string const& act_stage);

      /**
       * \brief Printing map to stdout.
       * \param[in] map of paths and variables.
       * \return path to requested stage.
       */
      void print(varsMapT const& map);
   
      using Check_branch_names::operator();
   
      virtual ~Variables(){};
   };
}}
#endif
