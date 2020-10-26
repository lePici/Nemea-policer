
/**
 * \file ast_check_branch_names.cpp
 * \brief Defintion of Check_branch_names class.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "ast_check_branch_names.hpp"
#include "../exception.hpp"
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

using namespace client::ast;

std::string Check_branch_names::make_path(stackT vec)
{
   std::string retString;
   if (vec.empty()) {
      return "/";
   }
   for (auto const &item:vec) {
      retString.append("/");
      retString.append(item);
   }
   return retString;
}

std::string Check_branch_names::get_branchPath()
{
   return make_path(stack);
}

size_t Check_branch_names::get_depth()
{
   return stack.size();
}

void Check_branch_names::check_and_push(stackT & vec, std::string const &elem)
{
   if (find(vec.begin(), vec.end(), elem) != vec.end()) {
      using namespace ProgramError;
      check_branch_names_tag tag;
      throw Semantic(tag, make_path(stack).c_str(), elem.c_str());
   }
   vec.push_back(elem);
}

template <typename First, typename Rest>
void Check_branch_names::check_branches(First &one, Rest &more)
{
   std::vector<std::string> names;
   names.push_back(one.name);

   stack.push_back(one.name);
   (*this) (one);
   stack.pop_back();

   for (auto const &item: more) {
      check_and_push(names, item.name);

      stack.push_back(item.name);
      (*this) (item);
      stack.pop_back();
   }
}

void Check_branch_names::operator() (main_branch_vec const &param) {
   for (auto const &item:param) {
      stack.push_back(item.name);
      (*this) (item);
      stack.pop_back();
   }
}

void Check_branch_names::operator() (main_branches const &param) {
   check_branches(param.first, param.rest);
}

void Check_branch_names::operator() (branch_after_fi_s const &param) {
   check_branches(param.first, param.rest);
}

void Check_branch_names::operator() (branch_after_gr_s const &param) {
   check_branches(param.first, param.rest);
}

void Check_branch_names::operator() (branch_after_wi_s const &param) {
   check_branches(param.first, param.rest);
}

void Check_branch_names::operator() (branch_after_ag_s const &param) {
   check_branches(param.first, param.rest);
}

void Check_branch_names::operator() (branch_after_gf_s const &param) {
   check_branches(param.first, param.rest);
}

template void Check_branch_names::check_branches
   <main_branch, main_branch_vec> (main_branch &f, main_branch_vec &r);
template void Check_branch_names::check_branches
   <branch_after_fi, branch_after_fi_vec> (branch_after_fi &f, branch_after_fi_vec &r);
template void Check_branch_names::check_branches
   <branch_after_gr, branch_after_gr_vec> (branch_after_gr &f, branch_after_gr_vec &r);
template void Check_branch_names::check_branches
   <branch_after_wi, branch_after_wi_vec> (branch_after_wi &f, branch_after_wi_vec &r);
template void Check_branch_names::check_branches
   <branch_after_ag, branch_after_ag_vec> (branch_after_ag &f, branch_after_ag_vec &r);
template void Check_branch_names::check_branches
   <branch_after_gf, branch_after_gf_vec> (branch_after_gf &f, branch_after_gf_vec &r);
