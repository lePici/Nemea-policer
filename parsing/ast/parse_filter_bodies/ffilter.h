/*

 Copyright (c) 2015-2017, Tomas Podermanski, Lukas Hutak, Imrich Stoffa, Adam Piecek

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
 * \file ffilter.h
 * \brief netflow fiter implementation - C interface
 */

#ifndef _FLOW_FILTER2_H_
#define _FLOW_FILTER2_H_//

#define FF_MAX_STRING  1024
#define NUMBER_OF_STRINGS 500

typedef enum {
	FF_OK = 0x1,				/** No error occuried */
	FF_ERR_NOMEM = -0x1,
	FF_ERR_UNKN  = -0x2,
	FF_ERR_UNSUP  = -0x3,
	FF_ERR_OTHER  = -0xE,
	FF_ERR_OTHER_MSG  = -0xF, 	/** Closer description of fault can be received from ff2_error */
} ff2_error_t;

/**
 * \brief Filter object instance
 */
typedef struct ff2_s {
    int items;
    char* strs[NUMBER_OF_STRINGS];
    char error_str[FF_MAX_STRING];	/**< Last error set */
    ff2_error_t retCode;
} ff2_t;
/**
 * \brief Create filter structure and compile filter expression
 * Filter object is created then expr is compiled to internal representation.
 * \param[out] ff2_filter  Address of pointer to filter object
 * \param[in]  expr       Filter expression
 * \return FF_OK on success
 */
ff2_error_t ff2_init(ff2_t **filter, const char *expr);

/**
 * \brief Release memory allocated for filter object and destroy it
 * \param[out] filter Compiled filter object
 * \return FF_OK on success
 */
ff2_error_t ff2_free(ff2_t *filter);

/**
 * \brief Set error string to filter object
 * \param[in] filter Compiled filter object
 * \param[in] format Format string as used in printf
 * \param[in] ...
 */
void ff2_set_error(ff2_t *filter, const char *format, ...);

/**
 * \brief Retrive last error set form filter object
 * \param[in]  filter Compiled filter object
 * \param[out] buf    Place where to copy error string
 * \param[in]  buflen Length of buffer available for error string
 * \return Pointer to copied error string
 */
const char* ff2_error(ff2_t *filter, const char *buf, int buflen);

const char* ff2_t_get_data(ff2_t* d, int index);
int ff2_t_size(ff2_t* d);
int ff2_t_isError(ff2_t* d);

#endif /* _LNF_FILTER_H */
