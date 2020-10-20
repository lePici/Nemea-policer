
/**
 * \file ast_alias4variables.cpp
 * \brief Defintion of Alias4variables class.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "ast_alias4variables.hpp"
#include <iostream>
#include <string>
#include "../../backend/unirec_template.hpp"

using namespace client::ast;

void Alias4variables::operator() (aggregator_stage const &as) {
   aggIsPresent = true;
   unpack_structure(as.body);
   lastAggr = get_vars(aggrVars, "aggregator");
   boost::apply_visitor((*this), as.succ);
   lastAggr = NULL;
   aggIsPresent = false;
}

void Alias4variables::operator() (aggr_assignment const &as) {
   lastAggrVar.name = as.variable_name;
   boost::apply_visitor((*this), as.func_type);
   add_var(aggrVars, "aggregator", lastAggrVar);
}

void Alias4variables::operator() (aggr_func_with_param_s const &as) {
   if (as.func_name == ap_countDistinct) {
      lastAggrVar.alias = put_together_function_alias(as.func_name, as.param);
      define_new_unirec_field(lastAggrVar.alias);
   } else {
      lastAggrVar.alias = string_unirecEnum(as.param);
   }
}

void Alias4variables::operator() (aggr_func_without_param const &as) {
   if (as == ad_count) {
      lastAggrVar.alias = "COUNT";
   } else {
      lastAggrVar.alias = string_aggrWithoutParamEnum(as);
   }
}

void Alias4variables::operator() (group_filter_stage const &gfs) {
   varsT *gfVec = get_vars(group_filterVars, "group-filter");
   varsT *agVec = lastAggr == NULL ? NULL : lastAggr;

   if (gfVec != NULL && agVec != NULL) {
      for (auto & gf_item:*gfVec) {
         for (auto const &ag_item:*agVec) {
            if (gf_item.name == ag_item.name) {
               gf_item.alias = ag_item.alias;
            }
         }
      }
   }
   boost::apply_visitor((*this), gfs.succ);
}

void Alias4variables::operator() (selector_stage const &ss) {
   unpack_structure(ss.body);
   varsT *seVec = get_vars(selVars, "selector");
   varsT *agVec = lastAggr == NULL ? NULL : lastAggr;

   if (seVec != NULL && agVec != NULL) {
      for (auto & se_item:*seVec) {
         for (auto const &ag_item:*agVec) {
            if (se_item.name == ag_item.name) {
               se_item.alias = ag_item.alias;
            }
         }
      }
   }
}

void Alias4variables::operator() (sel_assignment const &sa) {
   selDive = true;
   std::string const *pointer = boost::get < std::string > (&sa.output);

   if (pointer != NULL) {
      /* "description = expr" || "agg_variable" */

      if (boost::optional < sel_expression > opt = sa.sel_expression_) {
         /* "description = expr" */
         (*this) (*opt);
      }
      /* "agg_variable" */
      else {
         /* COUNT as unirec field is special case */
         if (aggIsPresent && (*pointer == "COUNT"));
         else {
            varNameT var;

            var.name = *pointer;
            add_var(selVars, "selector", var);
         }
      }
   } else {
      /* "unirec" */
      ;
   }
   selDive = false;
}

void Alias4variables::operator() (std::string const &str) {
   if (selDive) {
      varNameT var;

      var.name = str;
      add_var(selVars, "selector", var);
   }
}

std::string client::ast::put_together_function_alias(aggr_func_with_param func_name, unirec param)
{
   std::string output = string_aggrWithParamEnum(func_name);
   output.append("_");
   output.append(string_unirecEnum(param));
   return output;
}
