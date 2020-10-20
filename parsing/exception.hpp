
/**
 * \file exception.hpp
 * \brief Throwing exception for syntactic or semantic error.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_EXCEPTION_HPP)
#define BOOST_SPIRIT_X3_EXCEPTION_HPP

#include <exception>

namespace ProgramError {

   /**
    * \brief Error codes.
    */
   enum ErrorT {
      IO = 1,           ///< Input or output error.
      Syntax = 10,      ///< Unspecified syntax error.
      BranchNames = 20, ///< Non-unique branch name.
      StagesComb,       ///< Bad sequence of stages. 
      Filter,           ///< Error in Filter body
      FilterVar,        ///< Unknown keyword in filter body.
      GrouperKey1,      ///< Invalid input keyword as aggregate key.
      GrouperKey2,      ///< Aggregate key and aggregate function simultaneously.
      Selector,         ///< Desired item cannot be selected.
      Unknown = 99      ///< Unkown error.
   };

   /**
    * \brief Unknown error.
    */
   struct Error: public std::exception
   {
      static int const msg_len = 500;
      char msg_body[msg_len] = {'\0'};
      ErrorT retCode = Unknown;
   };

   struct parser_tag {};
   struct filter_bodies_tag {};

   /**
    * \brief List of Syntactic errors.
    */
   struct Syntactic: public Error
   {
      char msg_prefix[18] = "Syntactic error! ";

      /**
       * \brief Parsing failed on some rule.
       */
      Syntactic(parser_tag)
      {
         retCode = Syntax;
      }

      /**
       * \brief Syntactic error in filter body.
       */
      Syntactic(filter_bodies_tag, const char *branchPath, const char *name);
   };

   struct check_branch_names_tag {};
   struct check_seq_stages_tag {};
   struct check_grouper_tag1 {};
   struct check_grouper_tag2 {};
   struct check_filter_varname_tag {};
   struct check_selector_tag {};

   /**
    * \brief List of Semantic errors.
    */
   struct Semantic: public Error
   {
      char msg_prefix[17] = "Semantic error! ";

      /**
       * \brief Non-unique branch name.
       */
      Semantic(check_branch_names_tag, const char *branchPath, const char *name);

      /**
       * \brief Bad sequence of stages.
       */
      Semantic(check_seq_stages_tag, const char *mainBranch, const char *stagesSeq);

      /**
       * \brief Invalid input keyword as aggregation key.
       */
      Semantic(check_grouper_tag1, const char *branchPath, const char *name);

      /**
       * \brief Aggregate key and aggregate function simultaneously.
       */
      Semantic(check_grouper_tag2, const char *branchPath, const char *name);

      /**
       * \brief Unknown keyword in filter body.
       */
      Semantic(check_filter_varname_tag, const char *mainBranch, const char *name);

      /**
       * \brief Desired item cannot be selected.
       */
      Semantic(check_selector_tag, const char *branchPath, const char *name);
   };
}

#endif
