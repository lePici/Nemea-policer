
/**
 * \file ast_variables.cpp
 * \brief Defintion of Variables class.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "ast_variables.hpp"
#include <iostream>

using namespace client::ast;

Variables::varsT* Variables::get_vars(varsMapT &map, std::string const &act_stage)
{
   branchPathT key;

   key = Check_branch_names::get_branchPath();
   key.append("/");
   key.append(act_stage);
   auto iter = map.find(key);

   if (iter == map.end()) {
      return NULL;
   } else {
      return &(iter->second);
   }
}

Variables::pathT Variables::get_branchPath(std::string const &act_stage)
{
   branchPathT path;

   path = Check_branch_names::get_branchPath();
   path.append("/");
   path.append(act_stage);
   return path;
}

void Variables::add_vars(varsMapT &map, pathT const &key, varsT const &vars)
{
   std::pair<Variables::pathT, Variables::varsT> pair(key, vars);
   map.insert(pair);
}

void Variables::add_var(varsMapT & map, std::string const &act_stage, varNameT const &var)
{
   pathT key = get_branchPath(act_stage);
   auto it = map.find(key);

   if (it != map.end()) {
      it->second.push_back(var);
   } else {
      varsT vars;

      vars.push_back(var);
      std::pair<Variables::pathT, Variables::varsT> pair(key, vars);
      map.insert(pair);
   }
}

void Variables::print(varsMapT const &map)
{
   using namespace std;

   for (auto const &mapItem:map) {
      cout << mapItem.first << ':' << endl << "\t";
      for (auto const &vecItem:mapItem.second) {
         cout << vecItem.name << " -- " << vecItem.alias;
         cout << endl << "\t";
      }
      cout << endl;
   }
}
