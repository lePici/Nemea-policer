
/**
 * \file program_arguments.hpp
 * \brief Handling of command line arguments
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(PROGRAM_ARGUMENTS_H)
#define PROGRAM_ARGUMENTS_H

#include <string>

/**
 * Return this constant if everything is ok.
 */
#define ARGS_OK 100

/**
 * \brief Class for checking command line arguments.
 */
class Program_arguments
{
   int argc = 0;
   char **argv = NULL;
   int n_outputs_in_argument = 0; ///< Number of output Libtrap interfaces.
   std::string srcIn_filename;    ///< Filename with user's rules.
   bool srcIn_flag = false;       ///< If -f option is present.

   /**
    * \brief Check private class members srcIn_flag and srcIn_filename.
    * \details Variable srcIn_flag must be True. 
    *    Variable srcIn_filename must contain name of existing file.
    * \return True on success, otherwise False.
    */
   bool check_arguments(void);

public:

   /**
    * \param[in] argc Number of strings.
    * \param[in] argv Array of arguments also contains Libtrap parameters.
    */
   Program_arguments(int argc, char **argv): argc(argc), argv(argv) {};

   /**
    * \brief Start processing arguments. Call this method after class constructor.
    * \return ARGS_OK on success, otherwise negative number.
    */
   int processing_arguments(void);

   /**
    * \brief Check if number of output interfaces on command line match with number of Selectors in user's rules file.
    * \param[in] n_outputs Number of Selector stages.
    * \warning The user must manually ensure that the number of Selector stages corresponds to the number of output Libtrap interfaces.
    * \return 0 on success, otherwise negative number.
    */
   int check_number_of_output_interfaces(int n_outputs);

   /**
    * \brief Get number of Libtrap output interfaces.
    * \return Number from 0 to 32.
    */
   int get_number_of_output_interfaces(void);

   /**
    * \return Name of file contains user's rules.
    */
   std::string get_srcIn_filename(void)
   {
      return srcIn_filename;
   }

   ~Program_arguments(void);
};

#endif /* program_arguments_h */
