
/**
 * \file program_arguments.cpp
 * \brief Handling of command line arguments
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "program_arguments.hpp"

#include <iostream>
#include <fstream>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include <libtrap/trap.h>

/* Delete when compile with cmake. */
#define TRAP_GETOPT(argc, argv, optstr, longopts) getopt_long(argc, argv, optstr, longopts, NULL)

#define EXPECTED_N_TRAP_INPUTS 1

#define MODULE_BASIC_INFO(BASIC) \
  BASIC("policer","policer description",EXPECTED_N_TRAP_INPUTS,-1)

#define MODULE_PARAMS(PARAM) \
  PARAM('f', "source_code", "Input file with source code", required_argument, "string")

/**
 * \param[in] argc from command line.
 * \param[in] argv from command line.
 * \return Count all trap interfaces.
 */
unsigned int count_trap_interfaces(int argc, char *argv[]);

/**
 * \brief Check if the file exists.
 * \param[in] fileName the name of the file you are looking for.
 * \return True on success, otherwise False.
 */
bool is_file_exist(std::string fileName);

int Program_arguments::processing_arguments(void)
{
   trap_module_info_t *module_info = NULL;

   INIT_MODULE_INFO_STRUCT(MODULE_BASIC_INFO, MODULE_PARAMS)

   n_outputs_in_argument = count_trap_interfaces(argc, argv) - EXPECTED_N_TRAP_INPUTS;
   module_info->num_ifc_out = n_outputs_in_argument;
   module_info->num_ifc_in = EXPECTED_N_TRAP_INPUTS;

   TRAP_DEFAULT_INITIALIZATION(argc, argv, *module_info);

   if (n_outputs_in_argument <= 0) {
      std::cerr << "Error: at least one output interface must be set" << std::endl;
      return -1;
   }
   if (n_outputs_in_argument > 32) {
      std::cerr << "Error: More than 32 output interfaces is not allowed by TRAP library." << std::endl;
      std::cerr << "       You must reduce the number of selectors" << std::endl;
      return -2;
   }

   signed char opt;

   while ((opt = TRAP_GETOPT(argc, argv, module_getopt_string, long_options)) != -1) {
      switch (opt) {
      case 'f':
         srcIn_flag = true;
         srcIn_filename = optarg;
         break;
      default:
         std::cerr << "Error: Invalid arguments." << std::endl;
         return -3;
      }
   }
   /* Important for reuse getopt function more times. */
   optind = 1;

   FREE_MODULE_INFO_STRUCT(MODULE_BASIC_INFO, MODULE_PARAMS)

   return check_arguments()? ARGS_OK : -1;
}


bool Program_arguments::check_arguments(void)
{
   using namespace std;

   if (srcIn_flag) {
      return is_file_exist(srcIn_filename);
   } else {
      std::cerr << "Error: parameter -f is required" << std::endl;
      return false;
   }
}

int Program_arguments::check_number_of_output_interfaces(int n_outputs)
{
   if (n_outputs == n_outputs_in_argument) {
      return 0;
   } else {
      std::cerr << "Error: the number of selectors (" << n_outputs << ") ";
      std::cerr << "does not match the number of trap output interfaces (" << n_outputs_in_argument <<
         ")" << std::endl;
      return -1;
   }
}

int Program_arguments::get_number_of_output_interfaces(void)
{
   return n_outputs_in_argument;
}


Program_arguments::~Program_arguments(void)
{
   trap_send_flush(0);
   trap_finalize();
}

unsigned int count_trap_interfaces(int argc, char *argv[])
{
   char *interfaces = NULL;

   for (int i = 1; i < argc; i++) {
      /* Find argument for param -i. */
      if (!strcmp(argv[i], "-i") && i + 1 < argc) {
         interfaces = argv[i + 1];
      }
   }
   unsigned int ifc_cnt = 1;

   if (interfaces != NULL) {
      while (*interfaces) {
         /* Count number of specified interfaces. */
         if (*(interfaces++) == ',') {
            ifc_cnt++;
         }
      }
      return ifc_cnt;
   }

   return ifc_cnt;
}

bool is_file_exist(std::string fileName)
{
   std::ifstream infile(fileName);
   return infile.good();
}

