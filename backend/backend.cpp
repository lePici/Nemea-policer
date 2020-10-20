
/**
 * \file backend.cpp
 * \brief Definition of backend controller.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "aggregator/aggregator.hpp"
#include "backend.hpp"
#include "csv_unirec.hpp"
#include "fields.h"
#include "filter/filter.hpp"
#include "interface.hpp"
#include "selector/selector.hpp"

#include <csignal>
#include <fstream>
#include <stdio.h>
#include <vector>

#include <unirec/unirec.h>

#ifdef MEASURE
#include <ctime>
#endif

void my_signal_handler(int signal);

sig_atomic_t Backend::stopFlag = 0;

/**
 * Function to handle SIGTERM and SIGINT signals used to stop the module.
 * @param [in] signal caught signal value.
 */
void my_signal_handler(int signal)
{
   if (signal == SIGTERM || signal == SIGINT) {
      fprintf(stderr, "Signal caught, exiting module\n");
      Backend::stopFlag = 1;
   }
}

int Backend::build_backend(void)
{
   int retVal = 0;

   builder = new client::ast::Builder(inter_repr, config);
   retVal |= builder->build();
   pipelines = builder->get_pipelineVec();

   signal(SIGTERM, my_signal_handler);
   signal(SIGINT, my_signal_handler);
   return retVal;
}


int Backend::start_processing(void)
{

   std::string fields = templater.get_input_template_fields();
   ur_template_t *tmplt = ur_create_input_template(0, fields.c_str(), NULL);

   if (tmplt == NULL) {
      fprintf(stderr, "ur_create_template error\n");
      return 1;
   }

   /* Set signal handling for termination. */
   signal(SIGTERM, my_signal_handler);
   signal(SIGINT, my_signal_handler);
   int ret = 0;

#ifdef MEASURE
   clock_t begin = clock();
#endif

   while (Backend::stopFlag == 0) {
      const void *in_rec;
      uint16_t in_rec_size;

      /*
       * Receive data from input interface 0.
       * Block if data are not available immediately (unless a timeout is set using trap_ifcctl).
       */
      ret = TRAP_RECEIVE(0, in_rec, in_rec_size, tmplt);

      /* Handle possible errors. */
      TRAP_DEFAULT_RECV_ERROR_HANDLING(ret, continue, break);

      /* Check size of received data. */
      if (in_rec_size < ur_rec_fixlen_size(tmplt)) {
         if (in_rec_size <= 1) {
            /* End of data (used for testing purposes). */
            break; 
         } else {
            fprintf(stderr,
                    "Error: data with wrong size received (expected size: >= %hu, received size: %hu)\n",
                    ur_rec_fixlen_size(tmplt), in_rec_size);
            ret = -1;
            break;
         }
      }

      for (auto const &pipeline: pipelines) {
         pipeline->eval(in_rec, tmplt);
      }
      if (Backend::stopFlag) {
         break;
      }
   }

#ifdef MEASURE
   clock_t end = clock();
   printf("%.2f", double (end - begin) / CLOCKS_PER_SEC);
#endif

   ur_free_template(tmplt);
   return ret;
}

Backend::~Backend(void)
{
   delete builder;
}

//int Backend::processing_csv_input_file()
//{
//
//   std::string fields = templater.get_input_template_fields();
//   ur_template_t *tmplt = ur_create_template(fields.c_str(), NULL);
//
//   if (tmplt == NULL) {
//      fprintf(stderr, "ur_create_template error\n");
//      return 1;
//   }
//   std::string dataFile = "someCsvFile";
//   std::ifstream file(dataFile);
//   CSV_unirec csv;
//   std::vector < void *>recVec;
//
//   file >> csv;
//   csv.set_header();
//   ur_field_id_t id;
//
//   while (file >> csv) {
//      void *record = ur_create_record(tmplt, 0);
//
//      id = UR_ITER_BEGIN;
//      while ((id = ur_iter_fields(tmplt, id)) != UR_ITER_END) {
//         ur_set_from_string(tmplt, record, id, csv[ur_get_name(id)]);
//      }
//      recVec.push_back(record);
//   }
//
//   for (auto const &record:recVec) {
//      for (auto const &pipeline:pipelines) {
//         pipeline->eval(record, tmplt);
//      }
//      if (Backend::stopFlag)
//         break;
//   }
//
//   for (auto const &item:recVec)
//      ur_free_record(item);
//
//   ur_free_template(tmplt);
//   return 0;
//}
