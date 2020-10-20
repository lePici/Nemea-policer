/**
 * \file ast_count_output_interfaces.cpp
 * \brief Defintion of Count_output_interfaces class.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "ast_count_output_interfaces.hpp"
using namespace client::ast;


void Count_output_interfaces::operator()(selector_stage const& /*ss*/)
{
   number_of_output_interfaces++;
}

int Count_output_interfaces::get_number_of_output_interfaces()
{
   return number_of_output_interfaces;
}
