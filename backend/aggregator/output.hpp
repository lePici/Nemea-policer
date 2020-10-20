/**
 * \file output.hpp
 * \brief Output template representation.
 * \author Michal Slabihoudek <slabimic@fit.cvut.cz>
 * \date 2020
 */
/*
 * Copyright (C) 2018 CESNET
 *
 * LICENSE TERMS
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Company nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * ALTERNATIVELY, provided that this notice is retained in full, this
 * product may be distributed under the terms of the GNU General Public
 * License (GPL) version 2 or later, in which case the provisions
 * of the GPL apply INSTEAD OF those given above.
 *
 * This software is provided ``as is'', and any express or implied
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose are disclaimed.
 * In no event shall the company or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even
 * if advised of the possibility of such damage.
 *
 */

#ifndef AGGREGATOR_OUTPUT_H
#define AGGREGATOR_OUTPUT_H


#include "key.h"
#include "agg_functions.hpp"

/**
 * Aggregation function pointer type definition.
 * Define pointer do aggregation function as a data type.
 */
typedef void (*agg_func)(const void *src, void *dst, void*);
/**
 * Postprocessing average function pointer type definition.
 * Define pointer to make_avg function template.
 */
typedef void (*final_avg)(void *record, uint32_t count);

typedef void* (*alloc_func)(void* init_data);
typedef void  (*dealloc_func)(void* container);
typedef void  (*final_make_func)(void *src, void *dst, dealloc_func df);

/**
 * Class to represent template for output records and its fields processing.
 */
class OutputTemplate {
public:
   ur_template_t *out_tmplt = NULL;                   /*!< Output UniRec template pointer. */
   int indexes_to_record[MAX_KEY_FIELDS];      /*!< Field index from global unirec structure. */
   int used_fields = 0;                            /*!< Count of stored and set fields in template. */
   agg_func process[MAX_KEY_FIELDS];           /*!< Pointer to aggregation function of field data type. */
   bool prepare_to_send = false;                       /*!< Flag is record postprocessing required by assigned aggregation function. */
   bool avg_postprocessing = false;
   final_avg avg_fields[MAX_KEY_FIELDS];       /*!< Pointer to postprocessing function for average function of field data type. */
   final_avg rate_fields[MAX_KEY_FIELDS];

   int             fields_like_ptr    [MAX_KEY_FIELDS]; 
   int             ptr_target         [MAX_KEY_FIELDS]; 
   alloc_func      alloc_ptr_fields   [MAX_KEY_FIELDS];
   agg_func        process_ptr        [MAX_KEY_FIELDS]; 
   final_make_func make_fields        [MAX_KEY_FIELDS];
   dealloc_func    dealloc_ptr_fields [MAX_KEY_FIELDS];
   int used_fields_like_ptrs = 0; 

   /**
    * Assign field with all required parameters to template.
    * @param [in] record_id index of field from global unirec structure.
    * @param [in] foo pointer to aggregation function of field with given record_id.
    * @param [in] avg_flag whether the field has avg function assigned.
    * @param [in] foo2 pointer to postprocessing average function or NULL instead.
    */
   void add_field(int record_id, agg_func foo, bool avg, bool rate, final_avg foo2);

   void add_ptr_field(int record_id, int target_id, alloc_func foo, agg_func foo2, final_make_func foo3, dealloc_func foo4);

   /**
    * Reset all fields to default (empty) state.
    */
   void reset();
};

#endif //AGGREGATOR_OUTPUT_H
