
/**
 * \file inter_repr.cpp
 * \brief Semantic check of Internal representation structure.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "inter_repr.hpp"
#include <string>


int check_items(const client::ast::Variables::varsMapT vm);

/**
 * \brief Check if alias is not empty.
 * \param[in] &vm map of path and (name, alias) tuple.
 * \return 0 on success, otherwise a negative error value.
 */
int check_items(const client::ast::Variables::varsMapT vm)
{
   using namespace std;

   for (auto const &mapItem:vm) {
      std::string path = mapItem.first;
      for (auto const &vecItem:mapItem.second) {
         std::string varName = vecItem.name;
         std::string alias = vecItem.alias;
         if (alias.empty() && (!client::ast::string_isUnirec(varName))) {
            cerr << "Error: in branch path \'" << path << "\' variable with name \'";
            cerr << varName << "\' has not been initialized" << endl;
            return -1;
         }

      }
   }
   return 0;
}

int check_inter_repr(Inter_repr const &data)
{
   int ret = 0;

   ret |= check_items(data.filterVars);
   ret |= check_items(data.group_filterVars);
   ret |= check_items(data.aggrVars);
   ret |= check_items(data.selVars);
   return ret;
}
