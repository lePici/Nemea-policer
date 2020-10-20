/*

 Copyright (c) 2015-2017, Imrich Stoffa, Adam Piecek

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
 * \file ffilter.c
 * \brief netflow fiter implementation
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ffilter_internal.h"
#include "ffilter_gram.h"
#include "ffilter.h"

///* set error to error buffer */
///* set error string */
void ff2_set_error(ff2_t *filter, const char *format, ...)
{
va_list args;

	va_start(args, format);
	vsnprintf(filter->error_str, FF_MAX_STRING - 1, format, args);
	va_end(args);
}

/* get error string */
const char* ff2_error(ff2_t *filter, const char *buf, int buflen)
{
	strncpy((char *)buf, filter->error_str, buflen - 1);
	return buf;
}

void ff2_t_create(ff2_t* d)
{
    d->items = 0;
    d->retCode = FF_OK;
}

void ff2_t_append(ff2_t* d, const char* st)
{
    if(d->items +1 > NUMBER_OF_STRINGS){
        d->retCode = FF_ERR_NOMEM;
	ff2_set_error(d, "the NUMBER_OF_STRINGS constant has been exceeded");
        return;
    }
    d->strs[d->items] = (char*) malloc(strlen(st) + 1);
    strcpy(d->strs[d->items], st);
    d->items = d->items +1;
    return;
}

const char* ff2_t_get_data(ff2_t* d, int index){
    return d->strs[index];
}

int ff2_t_size(ff2_t* d){
    return d->items;
}

void ff2_t_destroy(ff2_t* d){
    for(int i = 0; i < d->items; i++)
        free(d->strs[i]);
    d->items = 0;
    d->retCode = FF_OK;
}

int ff2_t_isError(ff2_t* d){
    return d->retCode;
}


/* Add leaf entry into expr tree */
void ff2_new_leaf(yyscan_t scanner, ff2_t *filter, char *fieldstr, char *valstr)
{
    ff2_t_append(filter, fieldstr);
    return;
}

ff2_error_t ff2_init(ff2_t **pfilter, const char *expr){
	yyscan_t scanner;
	//YY_BUFFER_STATE buf;
	int parse_ret;
	ff2_t *filter;

	filter = (ff2_t*)malloc(sizeof(ff2_t));
	*pfilter = NULL;

	if (filter == NULL) {
            return FF_ERR_NOMEM;
	}

	ff2_set_error(filter, "No Error.");
        ff2_t_create(filter);

	ff2g_lex_init(&scanner);
    // Buff is unused for now
	//buf = ff2__scan_string(expr, scanner);
    ff2g__scan_string(expr, scanner);
	parse_ret = ff2g_parse(scanner, filter);


	ff2g_lex_destroy(scanner);

	/* error in parsing */
	if (parse_ret != 0) {
		*pfilter = filter;
		return FF_ERR_OTHER_MSG;
	}

	*pfilter = filter;

	return FF_OK;
}
///* release all resources allocated by filter */
ff2_error_t ff2_free(ff2_t *filter) {
    ff2_t_destroy(filter);
    free(filter);
    return FF_OK;
}
