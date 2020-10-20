/**
 * \file output.cpp
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

#include "output.hpp"

/*
*  class definitions
*/
/* ================================================================= */
/* ============= OutputTemplate class initializations ================= */
/* ================================================================= */

/* ----------------------------------------------------------------- */
void OutputTemplate::add_field(int record_id, agg_func foo, bool avg, bool rate, final_avg foo2)
{
   indexes_to_record[used_fields] = record_id;
   process[used_fields] = foo;
   avg_fields[used_fields] = NULL;
   rate_fields[used_fields] = NULL;
   // If avg used for the first time set prepare_to_send flag
   if (avg || rate) {
      prepare_to_send = true;
      avg_postprocessing = true;
   }

   if (avg)
      avg_fields[used_fields] = foo2;
   else if (rate)
      rate_fields[used_fields] = foo2;

   used_fields++;
}

void OutputTemplate::add_ptr_field(int record_id, int target_id, alloc_func foo, agg_func foo2, final_make_func foo3, dealloc_func foo4)
{
   fields_like_ptr    [used_fields_like_ptrs] = record_id;
   ptr_target         [used_fields_like_ptrs] = target_id;
   alloc_ptr_fields   [used_fields_like_ptrs] = foo;
   process_ptr        [used_fields_like_ptrs] = foo2;
   make_fields        [used_fields_like_ptrs] = foo3;
   dealloc_ptr_fields [used_fields_like_ptrs] = foo4;

   prepare_to_send = true;
   used_fields_like_ptrs++;
}

/* ----------------------------------------------------------------- */
void OutputTemplate::reset()
{
   prepare_to_send = false;
   avg_postprocessing = false;
   used_fields = 0;
   used_fields_like_ptrs = 0;
   ur_free_template(out_tmplt);
}
/* ----------------------------------------------------------------- */
