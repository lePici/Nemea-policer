
/**
 * \file ast_printer.cpp
 * \brief defintion of Printer class
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "ast_printer.hpp"

using namespace client::ast;
using namespace std;

void Printer::operator() (main_branch const &mb)
{
   cout << "branch ";
   cout << mb.name << " ";
   cout << "{" << endl;
   tab(tabsize);
   boost::apply_visitor((*this), mb.mbm);
   tab(tabsize * -1);
   cout << "}" << endl;
}

void Printer::operator() (filter_stage const &fs)
{
   cout << "filter: " << fs.body << ";" << endl;
   tab();
   boost::apply_visitor((*this), fs.succ);
}

void Printer::operator() (grouper_stage const &gs)
{
   cout << "grouper: ";
   boost::apply_visitor((*this), gs.body.type);
   boost::apply_visitor((*this), gs.succ);
}

void Printer::operator() (grouper_keys const &gk)
{
   cout << string_unirecEnum(gk.first);
   for (auto const &item:gk.rest) {
      cout << ", ";
      cout << string_unirecEnum(item);
   }
   cout << ";" << endl;
   tab();
}

void Printer::operator() (grouper_case const &)
{
   cout << "grouper_case;" << endl;
   tab();
}

void Printer::operator() (window_stage const &ws)
{
   cout << "window: ";
   window_body wb = ws.body;

   cout << "type = ";
   cout << string_windowTypeEnum(wb.type);
   cout << ", ";

   cout << "range = " << wb.range << " seconds";
   if (wb.slide) {
      cout << ", " << "slide = " << *wb.slide << " seconds";
   }
   cout << ";" << endl;
   tab();

   boost::apply_visitor((*this), ws.succ);
}

void Printer::operator() (aggregator_stage const &as)
{
   cout << "aggregator: ";
   cout << endl;
   unpack_structure(as.body, ",\n");
   cout << ";" << endl;
   tab();
   boost::apply_visitor((*this), as.succ);
}

void Printer::operator() (aggr_assignment const &as)
{
   tab();
   tab();
   cout << as.variable_name << " = ";
   boost::apply_visitor((*this), as.func_type);
}

void Printer::operator() (aggr_func_with_param_s const &af)
{
   cout << string_aggrWithParamEnum(af.func_name);
   cout << "(";
   cout << string_unirecEnum(af.param);
   cout << ")";
}

void Printer::operator() (aggr_func_without_param const &af)
{
   cout << string_aggrWithoutParamEnum(af);
   cout << "(*)";
}

void Printer::operator() (group_filter_stage const &gfs)
{
   cout << "group-filter: " << gfs.body << ";" << endl;
   tab();
   boost::apply_visitor((*this), gfs.succ);
}

void Printer::operator() (selector_stage const &ss)
{
   cout << "selector: ";
   unpack_structure(ss.body, ", ");
   cout << ";" << endl;
}

void Printer::operator() (sel_assignment const &sa)
{
   boost::apply_visitor((*this), sa.output);
   if (sa.sel_expression_) {
      cout << " = ";
      // cout << "arit_expr";
      (*this) (*sa.sel_expression_);
   }
}

void Printer::operator() (sel_expression const &se)
{
   cout << '(';
   boost::apply_visitor((*this), se.first);
   for (auto const &item:se.rest) {
      (*this) (item);
   }
   cout << ')';
}

void Printer::operator() (sel_operation const &so)
{
   cout << string_sel_optokenEnum(so.operator_);
   boost::apply_visitor((*this), so.operand_);
}

void Printer::operator() (nil const &)
{
   return;
}

void Printer::operator() (double const &integer)
{
   cout << integer;
}

void Printer::operator() (sel_variable const &sv)
{
   boost::apply_visitor((*this), sv);
}

void Printer::operator() (sel_unary const &su)
{
   (*this) (su.operator_);
   boost::apply_visitor((*this), su.operand_);
}

void Printer::operator() (string const &str)
{
   cout << str;
}

void Printer::operator() (unirec const &unir)
{
   cout << string_unirecEnum(unir);
}
