/*

 Copyright (c) 2015-2017, Imrich Stoffa, Tomas Podermanski

 This file is part of libnf.net project.

 Libnf is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Libnf is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with libnf.  If not, see <http://www.gnu.org/licenses/>.

*/

/**
 * \file ffilter_internal.h
 * \brief netflow fiter private interface for helper functions and abstract syntax tree constructors
 */

#ifndef _FFILTER_INTERNAL3_H
#define _FFILTER_INTERNAL3_H

#include "ffilter.h"

#ifndef HAVE_HTONLL
#ifdef WORDS_BIGENDIAN
#   define ntohll(n)    (n)
#   define htonll(n)    (n)
#else
#   define ntohll(n)    ((((uint64_t)ntohl(n)) << 32) | ntohl(((uint64_t)(n)) >> 32))
#   define htonll(n)    ((((uint64_t)htonl(n)) << 32) | htonl(((uint64_t)(n)) >> 32))
#endif
#define HAVE_HTONLL 1
#endif

/* scanner instance */
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

YY_BUFFER_STATE ff3_yy_scan_string(const char *str, yyscan_t yyscanner);
int ff3_yyparse(yyscan_t yyscanner, ff3_t *filter);
//int lnf_filter_yylex(YYSTYPE *yylval, void *scanner);

// error function for scanner
void yyerror(yyscan_t yyscanner, ff3_t *filter, char *);

// conversion from string to numeric/bit value
unsigned get_unit(char *unit);
int64_t ff3_strtoll(char *num, char**endptr, int *err);
uint64_t ff3_strtoull(char *num, char**endptr, int *err);

int str_to_uint(ff3_t *filter, char *str, ff3_type_t type, char **res, size_t *vsize);
int str_to_int(ff3_t *filter, char *str, ff3_type_t type, char **res, size_t *vsize);

int str_to_uint64(ff3_t *filter, char *str, char **res, size_t *vsize);
int str_to_int64(ff3_t *filter, char *str, char **res, size_t *vsize);
int str_to_real(ff3_t *filter, char *str, char **res, size_t *vsize);
int str_to_mac(ff3_t *filter, char *str, char **res, size_t *vsize);
int str_to_addr(ff3_t *filter, char *str, char **res, size_t *vsize);
int str_to_timestamp(ff3_t *filter, char *str, char **res, size_t *vsize);
int int_to_netmask(int numbits, ff3_ip_t *mask);
char* unwrap_ip(char *ip_str, int numbits);

ff3_error_t ff3_type_cast(yyscan_t *scanner, ff3_t *filter, char *valstr, ff3_node_t* node);

// add new node into parse tree
ff3_node_t* ff3_duplicate_node(ff3_node_t* original);
ff3_node_t* ff3_new_mval(yyscan_t scanner, ff3_t *filter, char *valstr, ff3_oper_t oper,  ff3_node_t* nextptr);
ff3_node_t* ff3_new_leaf(yyscan_t scanner, ff3_t *filter, char *fieldstr, ff3_oper_t oper, char *valstr);
ff3_node_t* ff3_new_node(yyscan_t scanner, ff3_t *filter, ff3_node_t* left, ff3_oper_t oper, ff3_node_t* right);
ff3_node_t* ff3_branch_node(ff3_node_t *node, ff3_oper_t oper, ff3_lvalue_t* lvalue);
int ff3_oper_eval(char* buf, size_t size, ff3_node_t *node);

// evaluate filter
int ff3_eval_node(ff3_t *filter, ff3_node_t *node, void const* rec);

// release memory allocated by nodes
void ff3_free_node(ff3_node_t* node);

// lex bison prototypes
int ff3g_get_column(yyscan_t yyscanner);
void ff3g_set_column(int , yyscan_t);
int ff3g_lex_init(yyscan_t *yyscanner);
YY_BUFFER_STATE ff3g__scan_string(const char *, yyscan_t yyscanner);
int ff3g_parse(yyscan_t yyscanner, ff3_t*);
int ff3g_lex_destroy(yyscan_t yyscanner);

#endif

