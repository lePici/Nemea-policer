
/**
 * \file ast_check_selector.cpp
 * \brief Defintion of Check_selector class.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "ast_check_selector.hpp"
#include "../exception.hpp"

using namespace client::ast;

void Check_selector::operator() (grouper_stage const &gs)
{
   groDiv = true;
   boost::apply_visitor((*this), gs.body.type);
   groDiv = false;
   boost::apply_visitor((*this), gs.succ);
   gro_input_keywords.clear();
}

void Check_selector::operator() (aggregator_stage const &as)
{
   aggDiv = true;
   unpack_structure(as.body);
   aggDiv = false;
   boost::apply_visitor((*this), as.succ);
   agg_input_keywords.clear();
}

void Check_selector::operator() (selector_stage const &ss)
{
   selDiv = true;
   unpack_structure(ss.body);
   selDiv = false;
}

void Check_selector::operator() (unirec const &unir)
{
   if (groDiv) {
      gro_input_keywords.insert(string_unirecEnum(unir));
   } else if (aggDiv) {
      agg_input_keywords.insert(string_unirecEnum(unir));
   } else if (selDiv) {
      if (gro_input_keywords.empty() && agg_input_keywords.empty()) {
         return;
      }
      if ((unir == f_timeFirst) || (unir == f_timeLast)) {
         return;
      }
      std::string thisUnirec = string_unirecEnum(unir);
      std::set<std::string> names;
      names.insert(gro_input_keywords.begin(), gro_input_keywords.end());
      names.insert(agg_input_keywords.begin(), agg_input_keywords.end());
      const bool is_in = names.find(thisUnirec) != names.end();

      if (!is_in) {
         using namespace ProgramError;

         std::string errorMsg = get_branchPath().c_str();
         errorMsg.append("/selector");
         check_selector_tag tag;
         throw Semantic(tag, errorMsg.c_str(), thisUnirec.c_str());
      }
   }
}
