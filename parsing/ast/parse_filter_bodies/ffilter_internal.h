/*

 Copyright (c) 2015-2017, Imrich Stoffa, Tomas Podermanski, Adam Piecek

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

#ifndef _FFILTER_INTERNAL2_H
#define _FFILTER_INTERNAL2_H

#include "ffilter.h"

/* scanner instance */
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

YY_BUFFER_STATE ff2_yy_scan_string(const char *str, yyscan_t yyscanner);
int ff2_yyparse(yyscan_t yyscanner, ff2_t *filter);
//int lnf_filter_yylex(YYSTYPE *yylval, void *scanner);

// error function for scanner
void yyerror(yyscan_t yyscanner, ff2_t *filter, char *);
void ff2_new_leaf(yyscan_t scanner, ff2_t *filter, char *fieldstr, char *valstr);

// lex bison prototypes
int ff2g_get_column(yyscan_t yyscanner);
void ff2g_set_column(int , yyscan_t);
int ff2g_lex_init(yyscan_t *yyscanner);
YY_BUFFER_STATE ff2g__scan_string(const char *, yyscan_t yyscanner);
int ff2g_parse(yyscan_t yyscanner, ff2_t*);
int ff2g_lex_destroy(yyscan_t yyscanner);

#endif

