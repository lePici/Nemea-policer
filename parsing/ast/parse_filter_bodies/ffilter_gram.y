/*

 Copyright (c) 2013-2017, Tomas Podermanski, Imrich Štoffa, Adam Piecek

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

%defines
%pure-parser
%lex-param      { yyscan_t scanner }
%lex-param      { ff2_t *filter }
%parse-param    { yyscan_t scanner }
%parse-param    { ff2_t *filter }
%name-prefix "ff2g_"

%{
	#include <stdio.h>
    #include <string.h>

	#include "ffilter.h"
	#include "ffilter_internal.h"
	#include "ffilter_gram.h"

	#define YY_EXTRA_TYPE ff2_t

	void yyerror(yyscan_t scanner, ff2_t *filter, char *msg)
	{
	    ff2_set_error(filter, msg);
	}

%}

%union {
	unsigned int	t_uint;
	double		t_double;
	char 		string[FF_MAX_STRING];
	void		*node;
};

%token AND OR NOT
%token ANY EXIST
%token EQ LT GT ISSET
%token IN
%token <string> IDENT STRING QUOTED DIR DIR_2 PAIR_AND PAIR_OR
%token <string> BAD_TOKEN

%type <t_uint> cmp
%type <string> field value string consistent_val
%type <node> expr filter
%type <node> list

%left	OR
%left	AND
%left	NOT

%%

filter:
	expr                { }
	|                   { }
	;

field:
	IDENT               { strncpy($$, $1, FF_MAX_STRING - 1); }
	| DIR IDENT         { snprintf($$, FF_MAX_STRING - 1, "%s%s", $1, $2); }
	| DIR_2 IDENT       { snprintf($$, FF_MAX_STRING - 1, "%s%s", $1, $2); }
	| DIR_2 DIR IDENT   { snprintf($$, FF_MAX_STRING - 1, "%s%s%s", $1,$2,$3); }
	| PAIR_OR IDENT     { snprintf($$, FF_MAX_STRING - 1, "%c%s", '|', $2); }
	| PAIR_AND IDENT    { snprintf($$, FF_MAX_STRING - 1, "%c%s", '&', $2); }
	;

string:
	IDENT               { strncpy($$, $1, FF_MAX_STRING - 1); /*TRY not to copy, only pass pointer*/ }
	| STRING            { strncpy($$, $1, FF_MAX_STRING - 1); }

value:
	consistent_val      { strncpy($$, $1, FF_MAX_STRING - 1); }
	| string string     { snprintf($$, FF_MAX_STRING - 1, "%s %s", $1, $2); }

consistent_val:
	string              { strncpy($$, $1, FF_MAX_STRING - 1); }
	| QUOTED            { $1[strlen($1)-1] = 0; snprintf($$, FF_MAX_STRING - 1, "%s", &$1[1]); /*Dequote*/}
	;

expr:
	ANY                 { }
	| NOT expr          { }
	| expr AND expr     { }
	| expr OR expr      { }
	| '(' expr ')'      { }
	| EXIST field       { ff2_new_leaf(scanner, filter, $2, ""); }
	| field cmp value   { ff2_new_leaf(scanner, filter, $1, $3); }
	| field IN list     { ff2_new_leaf(scanner, filter, $1, ""); }
	| IDENT             { ff2_new_leaf(scanner, filter, $1, ""); }
	;

list:
	consistent_val ',' list {}
	| consistent_val list    {}
	| consistent_val ']'    {}
	;

cmp:
	ISSET       {}
	| EQ        {}
	| LT        {}
	| GT        {}
	|           {}
	;

%%

