
/**
 * \file parser.cpp
 * \brief Defintion of Parser class.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "rules_def/main_branches.hpp"
#include "rules_def/selector_stage.hpp"
#include "rules_def/group_filter_stage.hpp"
#include "rules_def/aggregator_stage.hpp"
#include "rules_def/window_stage.hpp"
#include "rules_def/grouper_stage.hpp"
#include "rules_def/filter_stage.hpp"

#include "ast/ast_printer.hpp"
#include "ast/ast_filter_bodies.hpp"
#include "ast/ast_check_seq_stages.hpp"
#include "ast/ast_check_branch_names.hpp"
#include "ast/ast_check_grouper.hpp"
#include "ast/ast_check_selector.hpp"
#include "ast/ast_alias4variables.hpp"
#include "ast/ast_count_output_interfaces.hpp"
#include "exception.hpp"
#include "common.hpp"
#include "config.hpp"
#include "parser.hpp"

#include <vector>
#include <string>

namespace For_handler
{
   using namespace client;

   /* it is typically called the parsing_start_at(ast::main_branches) function */

   parser::main_branches_type parsing_start_at(ast::main_branches) {
      return main_branches();
   }

   /* remains is useful during development*/

   parser::filter_stage_type parsing_start_at(ast::filter_stage) {
      return filter_stage();
   }

   parser::grouper_stage_type parsing_start_at(ast::grouper_stage) {
      return grouper_stage();
   }

   parser::window_stage_type parsing_start_at(ast::window_stage) {
      return window_stage();
   }

   parser::aggregator_stage_type parsing_start_at(ast::aggregator_stage) {
      return aggregator_stage();
   }
   
   parser::group_filter_stage_type parsing_start_at(ast::group_filter_stage) {
      return group_filter_stage();
   }

   parser::selector_stage_type parsing_start_at(ast::selector_stage) {
      return selector_stage();
   }
}

void Parser::parse(void)
{
   typedef std::string::const_iterator iterator_type;

   // iterator_type iter = storage.begin();
   // iterator_type end = storage.end();
   iterator_type iter = myStr.begin();
   iterator_type end = myStr.end();

   using boost::spirit::x3::ascii::space;
   using boost::spirit::x3::with;
   using boost::spirit::x3::error_handler_tag;
   using error_handler_type = boost::spirit::x3::error_handler<iterator_type>;

   /* Our error handler */
   error_handler_type error_handler(iter, end, std::cerr);

   std::vector<std::string> config_keyword_set {"unirec_default"};
   client::parser::define_input_keywords(config_keyword_set);

   auto const parser =
      /* We pass our error handler to the parser so we can access
       * it later in our on_error and on_sucess handlers.
       */
      with<error_handler_tag> (std::ref(error_handler))
         [For_handler::parsing_start_at(data.ast)];

   bool r = phrase_parse(iter, end, parser, space, data.ast);

   if (r && iter == end);       /* Parsing succeeded */
   else {
      std::string::const_iterator some = iter + 30;
      std::string context(iter, (some > end) ? end : some);
      std::cout << "-------------------------\n";
      std::cout << "Parsing failed\n";
      std::cout << "-------------------------\n";
      using namespace ProgramError;
      parser_tag tag;
      throw Syntactic(tag);
   }

   /* Additionaly parsing filter body and group-filter body */
   client::ast::Parse_filter_bodies filters;
   filters(data.ast);
   data.filterVars = filters.get_filterVars();
   data.group_filterVars = filters.get_group_filterVars();
}

void Parser::print(void)
{
   client::ast::Printer printer;
   printer(data.ast);
}

void Parser::semantic_check(void)
{
   using namespace client::ast;

   Check_seq_stages checkStages;
   Check_branch_names checkBranch;
   Check_grouper checkGrouper;
   Check_selector checkSelector;
   Check_filter_bodies checkVars(data.filterVars, data.group_filterVars);

   checkStages(data.ast);
   checkBranch(data.ast);
   checkGrouper(data.ast);
   checkSelector(data.ast);
   checkVars(data.ast);
}

void Parser::add_aliases(void)
{
   using namespace client::ast;
   Alias4variables alv(data.filterVars, data.group_filterVars);

   alv(data.ast);
   data.filterVars = alv.get_filterVars();
   data.group_filterVars = alv.get_group_filterVars();
   data.aggrVars = alv.get_aggrVars();
   data.selVars = alv.get_selVars();
}

void Parser::run(void)
{
   parse();
#ifdef DEVEL
   print();
#endif
   semantic_check();
   add_aliases();
}

int Parser::get_number_of_output_interfaces(void)
{
   using namespace client::ast;
   Count_output_interfaces coi;

   coi(data.ast);
   return coi.get_number_of_output_interfaces();
}
