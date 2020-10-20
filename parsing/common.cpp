
/**
 * \file common.cpp
 * \brief Common keywords of language.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "ast/ast.hpp"
#include "common.hpp"

namespace client {
   namespace parser {

      x3::symbols<ast::unirec> input_keyword;

      void define_input_keywords(std::vector<std::string> keywords)
      {
         static bool once = false;
         if (once) {
            return;
         }
         once = true;

         if (std::find(keywords.begin(), keywords.end(), "unirec_default") != keywords.end()) {
            input_keyword.add("SRC_IP", ast::f_srcIP)
               ("DST_IP", ast::f_dstIP)
               ("BYTES", ast::f_bytes)
               ("LINK_BIT_FIELD", ast::f_linkBitField)
               ("TIME_FIRST", ast::f_timeFirst)
               ("TIME_LAST", ast::f_timeLast)
               ("PACKETS", ast::f_packets)
               ("SRC_PORT", ast::f_srcPort)
               ("DST_PORT", ast::f_dstPort)
               ("DIR_BIT_FIELD", ast::f_dirBitField)
               ("PROTOCOL", ast::f_protocol)
               ("TCP_FLAGS", ast::f_tcpFlags)
               ("TOS", ast::f_tos)
               ("TTL", ast::f_ttl);
         }
      }
   }
}
