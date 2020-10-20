
/**
 * \file ast_filter_bodies.cpp
 * \brief Definition of Parse_filter_bodies and Check_filter_bodies classes.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "ast_filter_bodies.hpp"
#include "../exception.hpp"
extern "C" {
#include "parse_filter_bodies/ffilter.h"
}
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace client::ast;

Variables::varsT Parse_filter_bodies::parse_filter_body(filter_body str, branchPathT path)
{
   varsT vars;
   ff2_t *filter = NULL;

   if (ff2_init(&filter, str.c_str()) == FF_OK) {
      for (int i = 0; i < ff2_t_size(filter); i++) {
         varNameT vn {ff2_t_get_data(filter, i), ""};
         vars.push_back(vn);
      }
      ff2_free(filter);
   } else {
      char msg[300];

      ff2_error(filter, msg, 300);
      ff2_free(filter);

      using namespace ProgramError;
      filter_bodies_tag tag;
      throw Syntactic(tag, path.c_str(), &msg[0]);
   }
   return vars;
}

void Parse_filter_bodies::operator() (group_filter_stage const &gfs)
{
   branchPathT key = get_branchPath("group-filter");
   varsT vars = parse_filter_body(gfs.body, key);

   add_vars(group_filterVars, key, vars);
}

void Check_filter_bodies::operator() (filter_stage const &fs)
{
   /* just check if no error */
   ff2_t *filter = NULL;

   if (ff2_init(&filter, fs.body.c_str()) != FF_OK) {
      branchPathT path = get_branchPath("filter");
      char msg[300];

      ff2_error(filter, msg, 300);
      ff2_free(filter);
      using namespace ProgramError;
      filter_bodies_tag tag;
      throw Syntactic(tag, path.c_str(), &msg[0]);
   }
   ff2_free(filter);
   boost::apply_visitor((*this), fs.succ);
}


void Check_filter_bodies::operator() (aggregator_stage const &as)
{
   unpack_structure(as.body);
   boost::apply_visitor((*this), as.succ);
   aggrVars.clear();
}

void Check_filter_bodies::operator() (aggr_assignment const &as)
{
   aggrVars.push_back(as.variable_name);
}

void Check_filter_bodies::operator() (group_filter_stage const & /* gfs */ )
{
   branchPathT key = get_branchPath("group-filter");
   varsT *vars = get_vars(group_filterVars, "group-filter");

   if (vars != NULL) {
      for (auto const &item:*vars) {
         /* group-filter input_keyword is not aggregator var nor unirec */
         if ((std::find(aggrVars.begin(), aggrVars.end(), item.name) == aggrVars.end())
             && (!string_isUnirec(item.name))) {
            std::cout << string_isUnirec(item.name) << std::endl;
            using namespace ProgramError;
            check_filter_varname_tag tag;
            throw Semantic(tag, key.c_str(), item.name.c_str());
         }
      }
   }
}
