/**
 * \file ast_check_seq_stages.hpp
 * \brief Semantic check if stage sequences is valid.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AST_CHECK_SEQ_STAGES_HPP)
#define BOOST_SPIRIT_X3_AST_CHECK_SEQ_STAGES_HPP

#include "ast.hpp"
#include "ast_visitor.hpp"
#include <vector>
#include <string>


namespace client { namespace ast
{
    
   class Check_seq_stages : public Visitor<class Visitor_T>
   {
      private:
   
      /**
       * \brief Abbreviations for the stages.
       */
      enum stage_type {
         fi, ///< Filter stage.
         gr, ///< Grouper stage.
         wi, ///< Window stage.
         ag, ///< Aggregator stage.
         gf, ///< Group-filter stage.
         se  ///< Selector stage.
      };
   
      /**
       * \brief Enumeration of allowed stage sequences.
       */
      const std::vector<std::vector<stage_type>> allowedComb{
         {fi,gr,wi,ag,gf,se},
         {   gr,wi,ag,gf,se},
         {fi,   wi,ag,gf,se},
         {      wi,ag,gf,se},
         {fi,gr,   ag,gf,se},
         {   gr,   ag,gf,se},
         {fi,      ag,gf,se},
         {         ag,gf,se},
         {fi,gr,wi,   gf,se},
         {   gr,wi,   gf,se},
         {fi,gr,      gf,se},
         {   gr,      gf,se},
         {fi,gr,wi,ag,   se},
         {   gr,wi,ag,   se},
         {fi,   wi,ag,   se},
         {      wi,ag,   se},
         {fi,gr,   ag,   se},
         {   gr,   ag,   se},
         {fi,      ag,   se},
         {         ag,   se},
         {fi,gr,wi,      se},
         {   gr,wi,      se},
         {fi,gr,         se},
         {   gr,         se},
         {fi,            se},
         {               se}
      };
   
      /**
       * \brief To store a sequence of stages.
       */
      std::vector<stage_type> stack;

      /**
       * \brief The name of the current main branch.
       */
      std::string actualMainBranch;
   
      /**
       * \brief Convert sequence of stages to one string.
       * \param[in] vec squence of stages.
       * \return string in format: stage->stage...
       */
      std::string stage_types2string(std::vector<stage_type> const& vec);

      /**
       * \brief Compare sequence of stages in user code with allowed sequences.
       * \param[in] foundComb sequence of stages in user code.
       */
      void check(std::vector<stage_type> const& foundComb);
   
      public:
   
      using Visitor::operator();
   
      void operator()(main_branch const& mb) override;
      void operator()(filter_stage const& fs) override;
      void operator()(grouper_stage const& gs) override;
      void operator()(window_stage const& ws) override;
      void operator()(aggregator_stage const& as) override;
      void operator()(group_filter_stage const& gfs) override;
      void operator()(selector_stage const& ss) override;
   };
}}

#endif
