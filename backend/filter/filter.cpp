
/**
 * \file filter.cpp
 * \brief Definition of Filter class.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "filter.hpp"
#include "../fields.h"
#include "../interface.hpp"

extern "C" {
#include "ffilter.h"
}

#include <iostream>
#include <stdio.h>

#include <unirec/unirec.h>

ff3_error_t data_func(struct ff3_s *filter, void const *rec, ff3_extern_id_t id, char **data, size_t * /* size */ );
ff3_error_t rval_map_func(struct ff3_s *, const char *, ff3_type_t, ff3_extern_id_t, char *, size_t *);
ff3_error_t lookup_func(struct ff3_s * /* filter */ , const char *fieldstr, ff3_lvalue_t * lvalue);


/**
 * Data Callback signature
 * \brief Select requested data from record.
 * Callback copies data associated with external identification, as set by lookup callback, from evaluated record
 * to buffer and marks length of these data. Structure of record must be known to data function.
 * \param ff3_s Filter object
 * \param[in]     record General data pointer to record
 * \param[in]     id     Indentfication of data field in recrod
 * \param[in/out] buf    in - Pointer to buffer, out - Pointer to buffer with retrieved data
 * \param[in/out] vsize  in - Size of passed buffer, out - size of valid data in buffer
 */
ff3_error_t data_func(struct ff3_s *filter, void const *rec, ff3_extern_id_t id, char **data,
                      size_t * /* size */ )
{
   if (ur_get_type(id.index) == UR_TYPE_IP) {

      const ip_addr_t *addr =
         (const ip_addr_t*) (ur_get_ptr_by_id(static_cast<ur_template_t const*>(filter->in_tmplt), rec, id.index));

      if (ip_is4(addr)) {
         filter->ui64[0] = (*addr).ui64[0];
         filter->ui64[1] = (*addr).ui64[1];
         filter->ui64[1] <<= 32;
         *data = (char *) (&filter->ui64);
         return FF_OK;
      }
   }

   *data =
      (char*) (ur_get_ptr_by_id(static_cast<ur_template_t const*>(filter->in_tmplt), rec, id.index));

   return FF_OK;
}

/**
 * Rval_map Callback signature
 * \brief Translate constant values unresolved by filter convertors.
 * Callback is used to transform literal value to given ff3_type_t when internal conversion function fails.
 * \param ff3_s Filter object
 * \param[in]  valstr String representation of value
 * \param[in]  type   Required ffilter internal type
 * \param[in]  id     External identification of field (form transforming exceptions like flags)
 * \param[out] buf    Buffer to copy data
 * \param[out] size   Length of valid data in buffer
 */
ff3_error_t rval_map_func(struct ff3_s *, const char *, ff3_type_t, ff3_extern_id_t, char *, size_t *)
{
   return FF_OK;
}

/**
 * Lookup callback signature.
 * \brief Lookup the field name found in filter expresson and identify its type one of and associated data elements
 * Callback fills in information about field into ff3_lvalue_t sturcture. Required information are external
 * identification of field as understood by data function, data type of filed as one of ff3_type_t enum
 * \param[in]  filter   Filter object
 * \param[in]  fieldstr Name of element to identify
 * \param[out] lvalue   Identification representing field
 * \return FF_OK on success
 */
ff3_error_t lookup_func(struct ff3_s * /* filter */ , const char *fieldstr, ff3_lvalue_t * lvalue)
{

   if (fieldstr != NULL) {

      lvalue->options = FF_OPTS_NONE;

      if (!strcmp(fieldstr, "DST_IP")) {
         lvalue->type = FF_TYPE_ADDR;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "SRC_IP")) {
         lvalue->type = FF_TYPE_ADDR;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "BYTES")) {
         lvalue->type = FF_TYPE_UINT64;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "LINK_BIT_FIELD")) {
         lvalue->type = FF_TYPE_UINT64;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "TIME_FIRST")) {
         lvalue->type = FF_TYPE_TIMESTAMP;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "TIME_LAST")) {
         lvalue->type = FF_TYPE_TIMESTAMP;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "PACKETS")) {
         lvalue->type = FF_TYPE_UINT32;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "DST_PORT")) {
         lvalue->type = FF_TYPE_UINT16;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "SRC_PORT")) {
         lvalue->type = FF_TYPE_UINT16;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "DIR_BIT_FIELD")) {
         lvalue->type = FF_TYPE_UINT8;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "PROTOCOL")) {
         lvalue->type = FF_TYPE_UINT8;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "TCP_FLAGS")) {
         lvalue->type = FF_TYPE_UINT8;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "TOS")) {
         lvalue->type = FF_TYPE_UINT8;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "TTL")) {
         lvalue->type = FF_TYPE_UINT8;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (!strcmp(fieldstr, "COUNT")) {
         lvalue->type = FF_TYPE_UINT32;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else if (strstr(fieldstr, "COUNT_DISTINCT") != NULL) {
         lvalue->type = FF_TYPE_UINT64;
         lvalue->id[0].index = ur_get_id_by_name(fieldstr);
      } else {
         return FF_ERR_UNKN;
      }
      return FF_OK;
   }
   return FF_ERR_OTHER;
}

int Filter::init(char const *options, const std::vector<Stage_intf*>succ)
{
   pipeline_successors = succ;
   ff3_options_init(&callbacks);
   callbacks->ff3_data_func = data_func;
   callbacks->ff3_lookup_func = lookup_func;
   callbacks->ff3_rval_map_func = rval_map_func;
   if (ff3_init(&filter, options, callbacks) == FF_OK) {
      return 0;
   } else {
      char msg[300];
      ff3_error(filter, msg, 300);
      std::cerr << "filter init: " << msg << std::endl;
      return -1;
   }
}

int Filter::eval(void const *rec, ur_template_t const *in_tmplt)
{
   filter->in_tmplt = (void const *) in_tmplt;
   int res = ff3_eval(filter, rec);

   if (res != 0) {
      send(rec, in_tmplt);
   }
   return 0;
}

Filter::~Filter()
{
   ff3_options_free(callbacks);
   ff3_free(filter);
}
