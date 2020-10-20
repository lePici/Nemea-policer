
/**
 * \file ast_check_branch_names.hpp
 * \brief Semantic checks over branch names.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AST_CHECK_BRANCH_NAMES_HPP)
#define BOOST_SPIRIT_X3_AST_CHECK_BRANCH_NAMES_HPP

#include "ast.hpp"
#include "ast_visitor.hpp"
#include <vector>
#include <string>


namespace client { namespace ast
{
    
   /**
    * \brief Check if branch name is unique.
    */
   class Check_branch_names : public Visitor<class Visitor_T>
   {
      protected:
   
      /**
       * \brief Path to stage.
       * \detail The path consists of the names of branches that are separated by a slash.
       */
      using branchPathT = std::string;

      /**
       * \brief Creates a path from the current location in the AST.
       * \detail class member stactT stack is used.
       * \return path consists of the names of branches that are separated by a slash.
       */
      branchPathT get_branchPath(void);

      /**
       * \brief Calculates the number of nested branches from current location in the AST.
       * \detail class member stactT stack is used.
       */
      size_t get_depth(void);
   
      private:
   
      using stackT = std::vector<branchPathT>;
   
      /**
       * \brief Storing names of branches depending on the current position in the AST.
       */
      stackT stack;

      /**
       * \brief Push branch name to stack and implicitly check duplicate names.
       * \param[in, out] vec vector of branch names.
       * \param[in] elem item to push.
       */
      void check_and_push(stackT& vec, std::string const& elem);

      /**
       * \brief Wrapper for get_BranchPath(void).
       * \param[in] stackT stack of branch names.
       * \return path consists of the names of branches that are separated by a slash.
       */
      branchPathT make_path(stackT);
   
      /**
       * \brief Push to branch names to stack procedure.
       */
      template <typename First, typename Rest>
      void check_branches(First& f, Rest& r);
   
      public:
   
      using Visitor::operator();
   
      void operator()(main_branches const& param) override;
      void operator()(main_branch_vec const& param) override;
      void operator()(branch_after_fi_s const& param) override;
      void operator()(branch_after_gr_s const& param) override;
      void operator()(branch_after_wi_s const& param) override;
      void operator()(branch_after_ag_s const& param) override;
      void operator()(branch_after_gf_s const& param) override;
   };
}}

#endif
