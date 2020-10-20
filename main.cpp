
/**
 * \file main.cpp
 * \brief Main source code of the module.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "backend/backend.hpp"
#include "parsing/ast/ast_adapted.hpp"
#include "parsing/exception.hpp"
#include "parsing/parser.hpp"

#include <boost/spirit/home/x3.hpp>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
   /* Handling command line arguments. */
   Program_arguments config(argc, argv);
   int ret;

   if ((ret = config.processing_arguments()) != ARGS_OK) {
      return ret;
   }

   /* Open the file containing the user rules. */
   std::ifstream in(config.get_srcIn_filename(), std::ios_base::in);

   std::string storage;         /* We will read the contents here. */
   in.unsetf(std::ios::skipws); /* No white space skipping! */
   std::copy(std::istream_iterator < char >(in),
             std::istream_iterator < char >(), std::back_inserter(storage));

   Parser parser(storage);

   try {
      /* Parse user rules. */
      parser.run();
   }
   catch(ProgramError::Error & e) {
      std::cerr << e.msg_body << std::endl;
      return e.retCode;
   }

   /* Check the number of output interfaces. */
   if ((ret = config.check_number_of_output_interfaces(parser.get_number_of_output_interfaces()))) {
      return ret;
   }

   /* Take the information obtained during parsing. */
   Inter_repr data = parser.get_data();

   // data.print(data.filterVars);
   // data.print(data.group_filterVars);
   // data.print(data.aggrVars);
   // data.print(data.selVars);

   if (check_inter_repr(data)) {
      return -1;
   }

   Backend backend(&data, &config);

   /* Prepare backend. */
   if (backend.build_backend() != 0) {
      return -1;
   }

   /* Start Policer processing. */
   if (backend.start_processing() != 0) {
      return -1;
   }

   return 0;
}
