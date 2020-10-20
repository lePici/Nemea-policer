
/**
 * \file ast.cpp
 * \brief Functions over ast.hpp.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "ast.hpp"

std::string client::ast::string_unirecEnum(unirec item)
{
   using namespace client::ast;

   switch (item) {
   case f_dstIP:
      return "DST_IP";
   case f_srcIP:
      return "SRC_IP";
   case f_bytes:
      return "BYTES";
   case f_linkBitField:
      return "LINK_BIT_FIELD";
   case f_timeFirst:
      return "TIME_FIRST";
   case f_timeLast:
      return "TIME_LAST";
   case f_packets:
      return "PACKETS";
   case f_dstPort:
      return "DST_PORT";
   case f_srcPort:
      return "SRC_PORT";
   case f_dirBitField:
      return "DIR_BIT_FIELD";
   case f_protocol:
      return "PROTOCOL";
   case f_tcpFlags:
      return "TCP_FLAGS";
   case f_tos:
      return "TOS";
   case f_ttl:
      return "TTL";
   default:
      return "unknown";
   }
}

bool client::ast::string_isUnirec(std::string item)
{
   if (item == "DST_IP")
      return true;
   if (item == "SRC_IP")
      return true;
   if (item == "LINK_BIT_FIELD")
      return true;
   if (item == "TIME_FIRST")
      return true;
   if (item == "TIME_LAST")
      return true;
   if (item == "PACKETS")
      return true;
   if (item == "DST_PORT")
      return true;
   if (item == "SRC_PORT")
      return true;
   if (item == "DIR_BIT_FIELD")
      return true;
   if (item == "PROTOCOL")
      return true;
   if (item == "TCP_FLAGS")
      return true;
   if (item == "TOS")
      return true;
   if (item == "TTL")
      return true;
   return false;
}

std::string client::ast::string_windowTypeEnum(window_type item)
{
   using namespace client::ast;

   switch (item) {
   case w_active:
      return "active";
   case w_passive:
      return "passive";
   case w_global:
      return "global";
   case w_mixed:
      return "mixed";
   default:
      return "unknown";
   }
}

std::string client::ast::string_aggrWithoutParamEnum(aggr_func_without_param item)
{
   using namespace client::ast;

   switch (item) {
   case ad_count:
      return "COUNT";
   default:
      return "unknown";
   }
}

std::string client::ast::string_aggrWithParamEnum(aggr_func_with_param item)
{
   switch (item) {
   case ap_sum:
      return "SUM";
   case ap_average:
      return "AVG";
   case ap_minimal:
      return "MIN";
   case ap_maximal:
      return "MAX";
   case ap_first:
      return "FIRST";
   case ap_last:
      return "LAST";
   case ap_or:
      return "OR";
   case ap_and:
      return "AND";
   case ap_countDistinct:
      return "COUNT_DISTINCT";
   case ap_rate:
      return "RATE";
   default:
      return "unknown";
   }
}

std::string client::ast::string_sel_optokenEnum(sel_optoken item)
{
   switch (item) {
   case op_plus:
      return "+";
   case op_minus:
      return "-";
   case op_times:
      return "*";
   case op_divide:
      return "/";
   case op_positive:
      return "+";
   case op_negative:
      return "-";
   default:
      return "unknown";
   }
}
