
/**
 * \file parser.hpp
 * \brief Main class for parsing.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(PARSER_HPP)
#define PARSER_HPP

#include "ast/ast_adapted.hpp"
#include "ast/ast_variables.hpp"
#include "config.hpp"
#include "inter_repr.hpp"

#include <iostream>
#include <string>
#include <vector>

/**
 * \brief Parsing control.
 */
class Parser
{
private:

   /**
    * \brief Information obtained during post-parsing.
    */
   Inter_repr data;

   /**
    * \brief User rules as input.
    */
   std::string myStr;

   /**
    * \brief Print parsed text.
    */
   void print(void);

   /**
    * \brief Starting of Spirit X3.
    */
   void parse(void);

   /**
    * \brief Semantic check of each stage.
    */
   void semantic_check(void);

   /**
    * \brief Fill Inter_repr data structure.
    */
   void add_aliases(void);

public:

   /**
    * \param[in] str User rules as input.
    */
   Parser(std::string str): myStr(str) {};

   /**
    * \brief Start parsing. Call this function after class constructor.
    */
   void run(void);

   /**
    * \return information obtained during parsing.
    */
   Inter_repr get_data(void) {
      return data;
   }

   /**
    * \return number of LibTrap output interfaces
    */
   int get_number_of_output_interfaces(void);
};

#endif
