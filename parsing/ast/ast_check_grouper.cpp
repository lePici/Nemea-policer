/**
 * \file ast_check_grouper.cpp
 * \brief Definition of Check_grouper class.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "ast_check_grouper.hpp"
#include "../exception.hpp"
#include <string>
#include <algorithm>

using namespace client::ast;

void Check_grouper::check_keys1(void)
{
   for(auto const& item: keys) {
      if(item == "COUNT"
         || item == "TIME_FIRST"
         || item == "TIME_LAST") {
         using namespace ProgramError;
         check_grouper_tag1 tag;
         std::string errorMsg = get_branchPath().c_str(); 
         errorMsg.append("/grouper");
         throw Semantic(tag, errorMsg.c_str(), item.c_str());
      }
   }
}

void Check_grouper::check_keys2(void)
{
   for(auto const& item: agg_params) {
      if(std::find(keys.begin(), keys.end(), item) != keys.end()) {
         using namespace ProgramError;
         check_grouper_tag2 tag;
         throw Semantic(tag, get_branchPath().c_str(), item.c_str());
      }
   }
}

void Check_grouper::operator()(grouper_stage const& gs)
{
   groDiv = true;
   boost::apply_visitor((*this), gs.body.type);
   groDiv = false;

   check_keys1();

   boost::apply_visitor((*this), gs.succ);
   keys.clear();
}

void Check_grouper::operator()(aggregator_stage const& as)
{
   using namespace std;
   aggDiv = true;
   unpack_structure(as.body);
   aggDiv = false;

   check_keys2();

   boost::apply_visitor((*this), as.succ);
   agg_params.clear();
}

void Check_grouper::operator()(unirec const& unir)
{
   if(groDiv) {
      groDiv = false;
      keys.push_back(string_unirecEnum(unir));
   }
   if(aggDiv) {
      aggDiv = false;
      agg_params.push_back(string_unirecEnum(unir));
   }
}
