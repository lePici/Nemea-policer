
/**
 * \file unirec_template.cpp
 * \brief Definition of Unirec_input_template_fields class.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "unirec_template.hpp"
#include "fields.h"
#include <set>
#include <iostream>
#include <string>

UR_FIELDS(ipaddr DST_IP,
          ipaddr SRC_IP,
          uint64 BYTES,
          uint64 LINK_BIT_FIELD,
          time TIME_FIRST,
          time TIME_LAST,
          uint32 PACKETS,
          uint16 DST_PORT,
          uint16 SRC_PORT,
          uint8 DIR_BIT_FIELD,
          uint8 PROTOCOL,
          uint8 TCP_FLAGS,
          uint8 TOS,
          uint8 TTL,
          uint32 NUMBER,
          uint32 KEY,
          uint32 COUNT,
          uint64 COUNT_DISTINCT,)

int define_new_unirec_field(std::string name)
{
   if (name.find("COUNT_DISTINCT") == 0) {
      ur_field_type_t type = UR_TYPE_UINT64;
      return ur_define_field(name.c_str(), type);
   }
   return -20;
}

using namespace client::ast;

Unirec_input_template_fields::Unirec_input_template_fields(Inter_repr *ir): inter_repr(ir)
{
   prepare_fields();
}

void Unirec_input_template_fields::prepare_fields()
{
   (*this) (inter_repr->ast);
}

std::string Unirec_input_template_fields::get_input_template_fields()
{
   for (auto const &ir_item:inter_repr->filterVars) {
      auto list = ir_item.second;

      for (auto const &l_item:list) {
         names.insert(l_item.name);
      }
   }
   names.insert("TIME_FIRST");
   names.insert("TIME_LAST");

   std::string output;
   for (auto const &item:names) {
      output.append(item);
      output.append(",");
   }
   output.pop_back();

   return output;
}

void Unirec_input_template_fields::operator() (unirec const &unir) {
   names.insert(string_unirecEnum(unir));
}
