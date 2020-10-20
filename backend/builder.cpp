
/**
 * \file builder.cpp
 * \brief definition of Builder and Builder_stage_base classes
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "aggregator/aggregator.hpp"
#include "builder.hpp"
#include "filter/filter.hpp"
#include "selector/selector.hpp"
#include "string_functions.hpp"

#include <iostream>
#include <limits>
#include <stdio.h>

#define SELECTOR_BODY 1
#define GROUP_FILTER_BODY 2

int Builder_stage_base::init(void)
{
   std::vector<Stage_intf*> stage_successors {};

   for (auto const &item: next_builders) {
      stage_successors.push_back(item->get_my_stage());
   }
   int retVal = 0;

   for (auto const &next_builder: next_builders) {
      retVal |= next_builder->init();
   }
   retVal |= my_stage->init(options.c_str(), stage_successors);
   return retVal;
}

Stage_intf* Builder_stage_base::get_my_stage(void)
{
   return my_stage;
}

Builder_stage_base::~Builder_stage_base(void)
{
   delete my_stage;

   for (auto const &next_builder_ptr: next_builders) {
      delete next_builder_ptr;
   }
}

using namespace client::ast;

std::string option_windowTypeEnum(window_type item);
std::string option_aggrWithParamEnum(aggr_func_with_param item);
std::string get_max_window(void);

std::string Builder::apply_aliases(std::string stage_body, varsT *aliases, int body_id)
{
   if (aliases == NULL) {
      return stage_body;
   }
   for (auto const &item: *aliases) {
      if (!item.alias.empty()) {
         if (body_id == SELECTOR_BODY) {
            stage_body = replace_aliases_in_selector_body(stage_body, item.name, item.alias);
         }
         else if (body_id == GROUP_FILTER_BODY) {
            stage_body = replace_aliases_in_group_filter_body(stage_body, item.name, item.alias);
         }
      }
   }
   return stage_body;
}

int Builder::build(void)
{
   (*this) (inter_repr->ast);
   int retVal = 0;

   for (auto const &item: root) {
      retVal |= item->init();
   }
   return retVal;
}

builderVec Builder::get_build_root(void)
{
   return root;
}

pipelineVec Builder::get_pipelineVec(void)
{
   pipelineVec ret;

for (auto const &item: root) {
      ret.push_back(item->get_my_stage());
   }
   return ret;
}

void Builder::unpack_main_branch(main_branch const &mb)
{
   builderVec *my_vec = new builderVec;

   b_stack.push_back(my_vec);
   boost::apply_visitor((*this), mb.mbm);
   root.insert(root.end(), my_vec->begin(), my_vec->end());
   delete my_vec;
}

void Builder::operator() (main_branch const &mb) {
   if (get_depth() == 1)
      unpack_main_branch(mb);
   else
      boost::apply_visitor((*this), mb.mbm);
}

void Builder::operator() (filter_stage const &fs) {
   builderVec *my_vec = new builderVec;

   b_stack.push_back(my_vec);
   boost::apply_visitor((*this), fs.succ);
   b_stack.pop_back();

   std::string options = fs.body;
   Builder_stage<Filter> *my_builder = new Builder_stage<Filter> (options, *my_vec);
   b_stack.back()->push_back(my_builder);
   delete my_vec;
}

void Builder::operator() (grouper_stage const &gs) {
   groDive = true;
   boost::apply_visitor((*this), gs.body.type);
   groDive = false;
   boost::apply_visitor((*this), gs.succ);
   gro_opt = "";
}

void Builder::operator() (window_stage const &ws) {
   win_opt.append(" -t ");
   win_opt.append(option_windowTypeEnum(ws.body.type));
   win_opt.append(":");
   win_opt.append(std::to_string(ws.body.range));
   win_opt.append(" ");
   boost::apply_visitor((*this), ws.succ);
   win_opt = "";
}

void Builder::operator() (aggregator_stage const &as) {
   unpack_structure(as.body);
   builderVec *my_vec = new builderVec;

   b_stack.push_back(my_vec);
   boost::apply_visitor((*this), as.succ);
   b_stack.pop_back();

   std::string options = "param ";
   options.append(gro_opt);
   if (win_opt.empty())
      options.append(get_max_window());
   else
      options.append(win_opt);
   options.append(agg_opt);
   Builder_stage<Agg> *my_builder = new Builder_stage<Agg> (options, *my_vec);
   b_stack.back()->push_back(my_builder);
   delete my_vec;

   agg_opt = "";
}

void Builder::operator() (aggr_func_with_param_s const &agf) {
   agg_opt.append(option_aggrWithParamEnum(agf.func_name));
   agg_opt.append(string_unirecEnum(agf.param));
}

void Builder::operator() (aggr_func_without_param const & /* agf */ ) {
}

void Builder::operator() (group_filter_stage const &gfs) {
   builderVec *my_vec = new builderVec;

   b_stack.push_back(my_vec);
   boost::apply_visitor((*this), gfs.succ);
   b_stack.pop_back();

   std::string options =
      apply_aliases(gfs.body, get_vars(inter_repr->group_filterVars, "group-filter"),
                    GROUP_FILTER_BODY);
   Builder_stage<Filter> *my_builder = new Builder_stage<Filter> (options, *my_vec);
   b_stack.back()->push_back(my_builder);
   delete my_vec;
}

void Builder::operator() (selector_stage const &ss) {
   selDive = true;
   unpack_structure(ss.body);
   selDive = false;

   std::string options;
   static int interface_counter = 0;

   options.append(std::to_string(interface_counter));
   options.append(":");
   interface_counter++;

   options.append(apply_aliases(sel_opt, get_vars(inter_repr->selVars, "selector"), SELECTOR_BODY));
   Builder_stage<Selector> *my_builder = new Builder_stage<Selector> (options, builderVec());
   b_stack.back()->push_back(my_builder);
   sel_opt = "";
}

void Builder::operator() (sel_assignment const &sa) {
   if (!sel_opt.empty())
      sel_opt.append(",");
   std::string const *strPtr = boost::get<std::string> (&sa.output);

   if (strPtr != NULL) {
      sel_opt.append(*strPtr);
      if (boost::optional<sel_expression> opt = sa.sel_expression_) {
         sel_opt.append("=");
         (*this) (*opt);
      }
   } else {
      unirec const *unirPtr = boost::get<unirec> (&sa.output);

      if (unirPtr != NULL) {
         sel_opt.append(string_unirecEnum(*unirPtr));
      }
   }
}

void Builder::operator() (std::string const &str) {
   if (selDive) {
      sel_opt.append(str);
   }
}

void Builder::operator() (unirec const &un) {
   if (selDive) {
      sel_opt.append(string_unirecEnum(un));
   }
   if (groDive) {
      gro_opt.append(" -k ");
      gro_opt.append(string_unirecEnum(un));
      gro_opt.append(" ");
   }
}

Builder::~Builder()
{
for (auto const &item:root) {
      delete item;
   }
}

/**
* \brief Mapping window_type to corresponding Aggregator parameter.
*/
std::string option_windowTypeEnum(window_type item)
{
   using namespace client::ast;

   switch (item) {
   case w_active:
      return "A";
   case w_passive:
      return "P";
   case w_global:
      return "G";
   case w_mixed:
      return "M";
   default:
      return "unknown";
   }
}

/**
* \brief Mapping aggr_func_with_param to corresponding Aggregator parameter.
*/
std::string option_aggrWithParamEnum(aggr_func_with_param item)
{
   using namespace client::ast;

   switch (item) {
   case ap_sum:
      return " -s ";
   case ap_average:
      return " -a ";
   case ap_minimal:
      return " -m ";
   case ap_maximal:
      return " -M ";
   case ap_first:
      return " -f ";
   case ap_last:
      return " -l ";
   case ap_or:
      return " -o ";
   case ap_and:
      return " -n ";
   case ap_countDistinct:
      return " -c ";
   case ap_rate:
      return " -r ";
   default:
      return "unknown";
   }
}

/**
 * \return maximal size for Aggregator's window.
 */
std::string get_max_window(void)
{
   std::string output;
   output.append(" -t A:");
   output.append(std::to_string(std::numeric_limits<int>::max()));
   output.append(" ");
   return output;
}
