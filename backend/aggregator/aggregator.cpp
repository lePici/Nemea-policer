#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <unistd.h>

#include <getopt.h>
#include <string.h>
#include <unirec/unirec.h>
#include "../fields.h"
#include <iostream>

#include <unordered_map>

#include "output.hpp"
#include "configuration.hpp"
#include "aggregator.hpp"

//#define DEBUG
#ifdef DEBUG
#define DBG(x) fprintf x;
#else
#define DBG(x)
#endif

/** Value (2^21) for default hash map space reservation before rehash needed.*/
#define MAP_RESERVE 2097152
#define MAX_ARGS 128
#define MAX_STRING 1024

/**
 * Statically defined fields COUNT, TIME_FIRST, TIME_LAST always used by module
 */

int Agg::stop = 0;


/* ================================================================= */
/* ================== Develop/helper functions ===================== */
/* ================================================================= */
/**
 * Print names globally defined fields to std output.
 */
void print_all_defined_ur_fields()
{
   int16_t size = ur_field_specs.ur_last_id;
   for (int i = 0; i < size; i++) {
      const char* name = ur_get_name(i);
      printf("%d. name = %s\n", i, name);
   }
   printf("All data from global unirec structure printed.\n\n");
}
/* ----------------------------------------------------------------- */
/**
 * Print names of fields in given UniRec template.
 * @param [in] tmplt template to print values from.
 */
void print_template_fields(ur_template_t* tmplt)
{
   int16_t size = tmplt->count;
   for (int i = 0; i < size; i++) {
      const char* name = ur_get_name(tmplt->ids[i]);
      printf("%d. name = %s\n", i, name);
   }
   printf("All data from given template printed.\n\n");
}

/* ----------------------------------------------------------------- */
/**
 * Function to free memory allocated by module.
 * @param [in] in_tmplt input UniRec template to free.
 * @param [in] out_tmplt output UniRec template to free.
 * @param [in] storage Container with stored data to be freed.
 */
void Agg::clean_memory(){
   std::unordered_map<Key, void*>::iterator it;
   for ( it = storage.begin(); it != storage.end(); it++) {
      ur_free_record(it->second);
   }
   storage.clear();

   if (outputTemp.out_tmplt){
      ur_free_template(outputTemp.out_tmplt);
      outputTemp.out_tmplt = NULL;
   }
   ur_finalize();
}

void Agg::clean_memory_with_ptrs(){
   std::unordered_map<Key, void*>::iterator it;
   if (outputTemp.used_fields_like_ptrs > 0) {
      for ( it = storage.begin(); it != storage.end(); it++) {
         for (int i = 0; i < outputTemp.used_fields_like_ptrs; i++){
            outputTemp.dealloc_ptr_fields[i]((void*)(*((uint64_t*) ur_get_ptr_by_id(outputTemp.out_tmplt, it->second, outputTemp.fields_like_ptr[i]))));
         }
      }
   }
   else{
      for ( it = storage.begin(); it != storage.end(); it++) {
         ur_free_record(it->second);
      }
   }
   storage.clear();

   if (outputTemp.out_tmplt){
      ur_free_template(outputTemp.out_tmplt);
      outputTemp.out_tmplt = NULL;
   }
   ur_finalize();
}

/* ----------------------------------------------------------------- */
/**
 * Wrapper function for creating UniRec record of given template and size.
 * @param [in] tmplt UniRec template to generate record from.
 * @param [in] length Size of variable part of message.
 * @return Pointer to allocated block of memory which represents created record.
 */
void * create_record(ur_template_t *tmplt, int length)
{
   return ur_create_record(tmplt, length);
}
/* ----------------------------------------------------------------- */
/**
 * Function to update the record values with specified rules from user input.
 * Always increase count (aggregated records counter). Use minimal value of TIME_FIRST field
 * and maximal value of TIME_LAST.
 * @param [in] in_tmplt UniRec template of received record.
 * @param [in] src_rec pointer to received record.
 * @param [in] out_tmplt UniRec template of stored/updated record.
 * @param [in, out] dst_rec pointer to stored/updated record.
 */
void Agg::process_agg_functions(ur_template_t const* in_tmplt, void const* src_rec, void* dst_rec)
{
   // Always increase count when processing functions
   ur_set(outputTemp.out_tmplt, dst_rec, F_COUNT, ur_get(outputTemp.out_tmplt, dst_rec, F_COUNT) + 1);

   // Modify time attributes TIME_FIRST:min
   uint64_t stored = ur_get(outputTemp.out_tmplt, dst_rec, F_TIME_FIRST);
   uint64_t record = ur_get(in_tmplt, src_rec, F_TIME_FIRST);
   if (record < stored)
      ur_set(outputTemp.out_tmplt, dst_rec, F_TIME_FIRST, ur_get(in_tmplt, src_rec, F_TIME_FIRST));

   // Modify time attributes TIME_LAST:max
   stored = ur_get(outputTemp.out_tmplt, dst_rec, F_TIME_LAST);
   record = ur_get(in_tmplt, src_rec, F_TIME_LAST);
   if (record > stored)
      ur_set(outputTemp.out_tmplt, dst_rec, F_TIME_LAST, ur_get(in_tmplt, src_rec, F_TIME_LAST));

   void *ptr_dst;
   void *ptr_src;
   // Process all registered fields with their agg function
   for (int i = 0; i < outputTemp.used_fields; i++) {
      if (ur_is_fixlen(i)) {
         ptr_dst = ur_get_ptr_by_id(outputTemp.out_tmplt, dst_rec, outputTemp.indexes_to_record[i]);
         ptr_src = ur_get_ptr_by_id(in_tmplt, src_rec, outputTemp.indexes_to_record[i]);
         outputTemp.process[i](ptr_src, ptr_dst, outputTemp.out_tmplt);
      }
      else {
         var_params params = {dst_rec, i, ur_get_var_len(in_tmplt, src_rec, outputTemp.indexes_to_record[i])};
         ptr_src = ur_get_ptr_by_id(in_tmplt, src_rec, outputTemp.indexes_to_record[i]);
         outputTemp.process[i](ptr_src, (void*)&params, outputTemp.out_tmplt);
      }
   }

   // Process all ptr registered fields with their agg function
   for (int i = 0; i < outputTemp.used_fields_like_ptrs; i++) {
      ptr_dst = ur_get_ptr_by_id(outputTemp.out_tmplt, dst_rec, outputTemp.fields_like_ptr[i]);
      ptr_src = ur_get_ptr_by_id(in_tmplt, src_rec, outputTemp.ptr_target[i]);
      outputTemp.process_ptr[i](ptr_src, (void*)(*((uint64_t*)ptr_dst)), NULL);
   }
}

void Agg::init_ptr_field(ur_template_t const* in_tmplt, void const* src_rec, void* dst_rec){
    for(int i = 0; i < outputTemp.used_fields_like_ptrs; i++){
        //TODO: allocation exception
        void* newVec   = outputTemp.alloc_ptr_fields[i](ur_get_ptr_by_id(in_tmplt, src_rec, outputTemp.ptr_target[i]));
        void* emptyPtr = (void*) ur_get_ptr_by_id(outputTemp.out_tmplt, dst_rec, outputTemp.fields_like_ptr[i]);
        memcpy(emptyPtr, &newVec, sizeof(void*));
    }
}

/* ----------------------------------------------------------------- */
/**
 * Set default (initial) values to from received to stored/output record.
 * Default values are considered to be copies of data from received record.
 * @param [in] in_tmplt UniRec template of received record.
 * @param [in] src_rec pointer to received record.
 * @param [in] out_tmplt UniRec template of initialized (output) record.
 * @param [in,out] dst_rec pointer to initialized (output) record.
 */
void Agg::init_record_data(ur_template_t const* in_tmplt, void const* src_rec, void* dst_rec)
{
   ur_clear_varlen(outputTemp.out_tmplt, dst_rec);
   // Copy all fields which are part of output template
   ur_copy_fields(outputTemp.out_tmplt, dst_rec, in_tmplt, src_rec);
   // Set initial value of module field(s)
   ur_set(outputTemp.out_tmplt, dst_rec, F_COUNT, 1);

   init_ptr_field(in_tmplt, src_rec, dst_rec);
}

/* ----------------------------------------------------------------- */
/**
 * Function to make all necessary post processing of output record before it is send.
 * There should be added all data modifications uncompleted during record live cycle.
 * @param [in,out] stored_rec pointer to stored record, which has to be processed.
 */
void Agg::prepare_to_send(void *stored_rec)
{
   // Proces activities needed to be done before sending the record

   if (outputTemp.avg_postprocessing) {
      for (int i = 0; i < outputTemp.used_fields; i++) {
         if (outputTemp.avg_fields[i]) {
            // Count Average function
            outputTemp.avg_fields[i](ur_get_ptr_by_id(outputTemp.out_tmplt, stored_rec, outputTemp.indexes_to_record[i]), ur_get(outputTemp.out_tmplt, stored_rec, F_COUNT));
         }
         else {
            // Count Rate function (because there is no other option yet)
            ur_time_t first = ur_get(outputTemp.out_tmplt, stored_rec, F_TIME_FIRST);
            ur_time_t last  = ur_get(outputTemp.out_tmplt, stored_rec, F_TIME_LAST);
            outputTemp.rate_fields[i](ur_get_ptr_by_id(outputTemp.out_tmplt, stored_rec, outputTemp.indexes_to_record[i]), ur_timediff(last,first) / 1000);
         }
      }
   }

   /*
    * Add every new post processing of agg function here
    */

   for(int i = 0; i < outputTemp.used_fields_like_ptrs; i++) {
      outputTemp.make_fields[i](
         (void*)(*((uint64_t*)ur_get_ptr_by_id(outputTemp.out_tmplt, stored_rec, outputTemp.fields_like_ptr[i]))),
                              ur_get_ptr_by_id(outputTemp.out_tmplt, stored_rec, outputTemp.fields_like_ptr[i]),
                              outputTemp.dealloc_ptr_fields[i]
       );
   }

}
/* ----------------------------------------------------------------- */
/**
 * Makes all necessary steps before record can be send to output interface.
 * @param [in] out_tmplt UniRec template of stored/output record.
 * @param [in] out_rec pointer to record which is going to be send
 * @return True if record successfully sent, false if record was not send.
 */
bool Agg::send_record_out(void *out_rec)
{

   DBG((stderr, "Count of message to send is: %d\n", ur_get(outputTemp.out_tmplt, out_rec, F_COUNT)));

   if(outputTemp.prepare_to_send) {
      prepare_to_send(out_rec);
   }

   //Warning - for threading -> copy
   send(out_rec, outputTemp.out_tmplt);
   return true;

   //fprintf(stderr, "Cannot send record due to error or time_out\n");
   //return false;
}

/* ----------------------------------------------------------------- */
/**
 * Tries to send out all stored records, free their memory and clear the storage.
 */
void Agg::flush_storage()
{
   // Send all stored data
  
   std::unordered_map<Key, void*>::iterator it;
   for ( it = storage.begin(); it != storage.end(); it++) {
      send_record_out(it->second);
      ur_free_record(it->second);
   }
   storage.clear();
}

/* ----------------------------------------------------------------- */
/**
 * Passive and global timeout control function.
 * Specially designed to run with another thread.
 * @param [in] input pointer to structure which pass all needed parameters to run by another thread.
 * @return nothing valuable, always NULL.
 */
void Agg::check_timeouts()
{
   int timeout_type = config.get_timeout_type();

   if (timeout_type == TIMEOUT_GLOBAL) {
      int timeout = config.get_timeout(TIMEOUT_GLOBAL);
      while (!Agg::stop) {
         time_t start = time(NULL);

         // Lock the storage -- CRITICAL SECTION START
         storage_mutex.lock();
         flush_storage();
         // Unlock the storage -- CRITICAL SECTION END
         storage_mutex.unlock();
         time_t end = time(NULL);

         int elapsed = difftime(end, start);
         int sec_to_sleep = (timeout - elapsed);
         if (sec_to_sleep > 0){
            sleep(sec_to_sleep);
         }
      }
      return;
   }

   if ((timeout_type == TIMEOUT_PASSIVE) || (timeout_type == TIMEOUT_ACTIVE_PASSIVE)) {
      int timeout = config.get_timeout(TIMEOUT_PASSIVE);

      time_last_from_record_mutex.lock();
      time_last_from_record += timeout;
      time_last_from_record_mutex.unlock();

      while (!Agg::stop) {
         time_t start = time(NULL);

         /* Can happen that record accesed for timeout check is being processed by main thread, need to use lock
          * Only accessing and modifying different elements is thread safe in stl map container */
         // Lock the storage -- CRITICAL SECTION START
         storage_mutex.lock();

         for (std::unordered_map<Key, void*>::iterator it = storage.begin(); it != storage.end(); ) {
            if (ur_time_get_sec(ur_get(outputTemp.out_tmplt, it->second, F_TIME_LAST)) < time_last_from_record - timeout) {
               // Send record out
               send_record_out(it->second);
               ur_free_record(it->second);
               it = storage.erase(it);
            }
            else {
               ++it;
            }
         }
         // Unlock the storage -- CRITICAL SECTION END
         storage_mutex.unlock();

         time_t end = time(NULL);
         int elapsed = difftime(end, start);
         int sec_to_sleep = (timeout - elapsed);

         time_last_from_record_mutex.lock();
         // Update the last record time with elapsed time interval
         time_last_from_record += (elapsed + sec_to_sleep);
         time_last_from_record_mutex.unlock();

         // Assume regularly timeout period
         if (sec_to_sleep > 0) {
            sleep(sec_to_sleep);
         }

      }
      return;
   }

   // Timeout is ACTIVE only, no thread checks needed, close the thread.
   return;
}
/* ----------------------------------------------------------------- */


void get_argc_argv(char* const input, int& argc, char*(&argv)[MAX_ARGS]){
    argc = 0;
    char *p2 = strtok(input, " ");
    while (p2 && argc < MAX_ARGS-1)
    {
        argv[argc++] = p2;
        p2 = strtok(0, " ");
    }
    argv[argc] = 0;
}


int Agg::init(char const* options, const std::vector<Stage_intf*> succ)
{
   int argc;
   char* argv[MAX_ARGS];
   char text[MAX_STRING];
   strcpy(text, options);
   get_argc_argv(text, argc, argv);

   signed char opt;
   //WARNING: getopt is not thread-safe
   /*
    * Parse program arguments defined by MODULE_PARAMS macro with getopt() function (getopt_long() if available)
    * This macro is defined in config.h file generated by configure script
    */
   while ((opt = getopt(argc, argv, "k:t:s:a:m:M:f:l:o:n:c:r:")) != -1) {
      switch (opt) {
      case 'k':
         config.add_member(KEY, optarg);
         break;
      case 't':
         config.set_timeout(optarg);
         break;
      case 's':
         config.add_member(SUM, optarg);
         break;
      case 'a':
         config.add_member(AVG, optarg);
         break;
      case 'm':
         config.add_member(MIN, optarg);
         break;
      case 'M':
         config.add_member(MAX, optarg);
         break;
      case 'f':
         config.add_member(FIRST, optarg);
         break;
      case 'l':
         config.add_member(LAST, optarg);
         break;
      case 'o':
         config.add_member(BIT_OR, optarg);
         break;
      case 'n':
         config.add_member(BIT_AND, optarg);
         break;
      case 'c':
         config.add_ptr_member("COUNT_DISTINCT", optarg);
         break;
      case 'r':
         config.add_member(RATE, optarg);
         break;
      default:
         fprintf(stderr, "Invalid argument %c, skipped...\n", opt);
      }
   }
   // important for reuse getopt function more times
   optind = 1;

   int id;
   for (int i = 0; i < config.get_used_fields(); i++) {
       id = ur_get_id_by_name(config.get_name(i));

       if (id == UR_E_INVALID_NAME) {
           fprintf(stderr, "Requested field %s not in input records, cannot continue.\n", config.get_name(i));
           clean_memory();
           return 1;
       }

       if (ur_is_varlen(id) && !config.is_variable())
           config.set_variable(true);

       if (config.is_key(i)) {
           keyTemp.add_field(id, ur_get_size(id));
       }
       else if (config.is_ptr(i)){
           int assocId = config.get_associated_id(i);
           outputTemp.add_ptr_field(id,
                                    assocId,
                                    config.get_alloc_ptr(i, ur_get_type(assocId)),
                                    config.get_function_ptr(i, ur_get_type(assocId)),
                                    config.get_final_make_ptr(i, ur_get_type(assocId)),
                                    config.get_dealloc_ptr(i, ur_get_type(assocId))
                                   );
       }
       else {
           outputTemp.add_field(id, config.get_function_ptr(i, ur_get_type(id)), config.is_func(i, AVG), config.is_func(i, RATE), config.get_avg_ptr(i, ur_get_type(id)));
       }
   }
   char *tmplt_def = config.return_template_def();
   outputTemp.out_tmplt = ur_create_template(tmplt_def, NULL);
   delete [] tmplt_def;

   if (outputTemp.out_tmplt == NULL){
       fprintf(stderr, "Error: Output template could not be created.\n");
       clean_memory();
       return -1;
   }

   pipeline_successors = succ;
   storage.reserve(MAP_RESERVE);        // Reserve enough space for records without need of rehash()


#ifdef DEBUG
   // DEBUG: print configuration
   config.print();
#endif

   time_last_from_record = time(NULL);
   timeout_thread = std::thread(&Agg::check_timeouts, this);

   //print_template_fields(outputTemp.out_tmplt);
   //print_all_defined_ur_fields();

   return 0;
}


/* ================================================================= */
/* ========================= M A I N =============================== */
/* ================================================================= */
int Agg::eval(void const* in_rec, ur_template_t const* in_tmplt)
{
   /* **** Main processing loop **** */

   // Warning: should be static thread_local?
   static bool once = true;
   if(once){
      // Lock the time variable -- CRITICAL SECTION START
      time_last_from_record_mutex.lock();
      time_last_from_record = ur_time_get_sec(ur_get(in_tmplt, in_rec, F_TIME_LAST));
      // Unlock the time variable -- CRITICAL SECTION END
      time_last_from_record_mutex.unlock();
      once = false;
   }

   // Read data from input, process them and write to output
   if (!Agg::stop) {

      /* Start message processing */
      time_t record_first = ur_time_get_sec(ur_get(in_tmplt, in_rec, F_TIME_FIRST));

      // Generate key
      Key rec_key(keyTemp.key_size);

      for (uint i = 0; i < keyTemp.used_fields; i++) {
         rec_key.add_field(ur_get_ptr_by_id(in_tmplt, in_rec, keyTemp.indexes_to_record[i]),
                           ur_get_size(keyTemp.indexes_to_record[i]));
      }

      void *init_ptr = NULL;
      std::pair<std::unordered_map<Key, void*>::iterator, bool> inserted;
      // Lock the storage -- CRITICAL SECTION START
      storage_mutex.lock();
      inserted = storage.insert(std::make_pair(rec_key, init_ptr));

      if (inserted.second == false) {
         // Element already exists
         bool new_time_window = false;
         void *stored_rec = inserted.first->second;
         // Main thread checks time window only when active timeout set
         if ( (config.get_timeout_type() == TIMEOUT_ACTIVE) || (config.get_timeout_type() == TIMEOUT_ACTIVE_PASSIVE)) {
            // Check time window for active timeout
            time_t stored_first = ur_time_get_sec(ur_get(outputTemp.out_tmplt, stored_rec, F_TIME_FIRST));
            // Record is not in current time window
            if (stored_first + config.get_timeout(TIMEOUT_ACTIVE) < record_first ) {
               new_time_window = true;
            }
         }
         if (new_time_window) {
            if(!send_record_out(stored_rec)) {
               return 0;
            }

            init_record_data(in_tmplt, in_rec, stored_rec);
         }
         else {
            process_agg_functions(in_tmplt, in_rec, stored_rec);
         }
      }
      else {
         // New element
         // If there should be place for variable length field in record reserve it
         int var_length = config.is_variable() == false ? 0 : 2048;
         void * out_rec = create_record(outputTemp.out_tmplt, var_length);
         if (!out_rec) {
            clean_memory_with_ptrs();
            fprintf(stderr, "Error: Memory allocation problem (output record).\n");
            return -1;
         }
         init_record_data(in_tmplt, in_rec, out_rec);
         inserted.first->second = out_rec;
      }
      // Unlock the storage -- CRITICAL SECTION END
      storage_mutex.unlock();
   }
   return 0;
}

Agg::~Agg(){
   DBG((stderr, "Module canceled, waiting for running threads.\n"));
   Agg::stop = 1;
   if(timeout_thread.joinable())
      timeout_thread.join();
   DBG((stderr, "Other threads ended, cleaning storage and exiting.\n"));
   // All other threads not running now, no need to use mutexes there

   flush_storage();
   sleep(1);

   /* **** Cleanup **** */
   // Free unirec templates and stored records
   clean_memory();
}

