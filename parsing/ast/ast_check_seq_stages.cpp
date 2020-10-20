
/**
 * \file ast_check_seq_stages.cpp
 * \brief Definition of Check_seq_stages class.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "ast_check_seq_stages.hpp"
#include "../exception.hpp"
#include <algorithm>
#include <iostream>
#include <string>

using namespace client::ast;

std::string Check_seq_stages::stage_types2string(std::vector<stage_type> const &vec)
{
   std::string output;
   for (auto const &item:vec) {
      switch (item) {
      case fi:
         output.append("filter->");
         break;
      case gr:
         output.append("grouper->");
         break;
      case wi:
         output.append("window->");
         break;
      case ag:
         output.append("aggregator->");
         break;
      case gf:
         output.append("group-filter->");
         break;
      case se:
         output.append("selector ");
         break;
      default:
         output.append("unknown ");
      }
   }
   return output;
}

void Check_seq_stages::check(std::vector<stage_type> const &foundComb)
{
   if (find(allowedComb.begin(), allowedComb.end(), foundComb) != allowedComb.end())
      /* combination found */ ;
   else {
      using namespace ProgramError;
      check_seq_stages_tag tag;
      throw Semantic(tag, actualMainBranch.c_str(), stage_types2string(foundComb).c_str());
   }
}

void Check_seq_stages::operator() (main_branch const &mb)
{
   actualMainBranch = mb.name;
   boost::apply_visitor((*this), mb.mbm);
}

void Check_seq_stages::operator() (filter_stage const &fs)
{
   stack.push_back(fi);
   boost::apply_visitor((*this), fs.succ);
   stack.pop_back();
}

void Check_seq_stages::operator() (grouper_stage const &gs)
{
   stack.push_back(gr);
   boost::apply_visitor((*this), gs.succ);
   stack.pop_back();
}

void Check_seq_stages::operator() (window_stage const &ws)
{
   stack.push_back(wi);
   boost::apply_visitor((*this), ws.succ);
   stack.pop_back();
}

void Check_seq_stages::operator() (aggregator_stage const &as)
{
   stack.push_back(ag);
   boost::apply_visitor((*this), as.succ);
   stack.pop_back();
}

void Check_seq_stages::operator() (group_filter_stage const &gfs)
{
   stack.push_back(gf);
   boost::apply_visitor((*this), gfs.succ);
   stack.pop_back();
}

void Check_seq_stages::operator() (selector_stage const & /* ss */ )
{
   stack.push_back(se);
   check(stack);
   stack.pop_back();
}
