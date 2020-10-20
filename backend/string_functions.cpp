
/**
 * \file string_functions.cpp
 * \brief Helping functions for strings.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "string_functions.hpp"

#include <algorithm>
#include <string>
#include <vector>

std::vector<std::string> divide_str(std::string input, const std::string delimiter)
{
   using namespace std;
   size_t pos = 0;
   string token;

   vector<string> output;
   while ((pos = input.find(delimiter)) != string::npos) {
      token = input.substr(0, pos);
      output.push_back(token);
      input.erase(0, pos + delimiter.length());
   }
   output.push_back(input);
   return output;
}

std::string findAndReplaceAll(std::string input, const std::string searched,
                              const std::string replaceStr)
{
   /* Get the first occurrence. */
   size_t pos = input.find(searched);

   /* Repeat till end is reached. */
   while (pos != std::string::npos) {
      /* Replace this occurrence of substring. */
      input.replace(pos, searched.size(), replaceStr);
      /* Get the next occurrence from the current position. */
      pos = input.find(searched, pos + replaceStr.size());
   }
   return input;
}

std::string vecOfstr2str(const std::vector<std::string> vec, const std::string delimiter)
{
   std::string output;
   for (auto const &item: vec) {
      output.append(delimiter);
      output.append(item);
   }
   output.erase(0, 1);
   return output;
}

std::string replace_aliases_in_selector_body(std::string input, const std::string searched,
                                             const std::string replaceStr)
{
   if (input.empty()) {
      return std::string();
   }
   /* Erase white spacing. */
   input.erase(remove_if (input.begin(), input.end(),::isspace), input.end());

   std::vector<std::string> inputVec = divide_str(input, ",");
   for (auto & item: inputVec) {
      size_t equalPos = item.find("=");

      if (equalPos != std::string::npos) {
         std::string rightSide = item.substr(equalPos + 1, item.size() - equalPos);
         rightSide = findAndReplaceAll(rightSide, searched, replaceStr);
         item.replace(item.begin() + equalPos + 1, item.end(), rightSide);
      } else {
         if (item.find(searched) != std::string::npos)
            item = replaceStr;
      }
   }
   return vecOfstr2str(inputVec, ",");
}

std::string replace_aliases_in_group_filter_body(std::string input, const std::string searched,
                                                 const std::string replaceStr)
{
   return findAndReplaceAll(input, searched, replaceStr);
}
