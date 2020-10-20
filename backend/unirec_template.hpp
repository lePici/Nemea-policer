
/**
 * \file unirec_template.hpp
 * \brief Managing of unirec template.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(UNIREC_TEMPLATE_H)
#define UNIREC_TEMPLATE_H

#include "../parsing/ast/ast_visitor.hpp"
#include "../parsing/inter_repr.hpp"

#include <set>
#include <string>

#include <unirec/unirec.h>

namespace client { namespace ast {

   class Unirec_input_template_fields: public Visitor <class Visitor_T> {
      Inter_repr *inter_repr;       ///< Information obtained during parsing.
      std::set <std::string> names; ///< Set of Unirec names.
  
      /**
       * Constructor's auxiliary function. Go through AST and store Unirec names.
       */
      void prepare_fields(void);

public:
      /**
       * \param[in] ir information obtained during parsing.
       */
      Unirec_input_template_fields(Inter_repr *ir);

      /**
       * \return String of unirec names separated by a comma.
       */
      std::string get_input_template_fields(void);

      /**
       * Inherited operator for browsing AST.
       */
      using Visitor::operator();

      /**
       * Override operator () for storing unirec names.
       */
      void operator() (unirec const &) override;
   };
}}

/**
 * \brief This function exists only for the purpose of COUNT_DISTINCT.
 * \todo Replace with something better.
 * \param[in] name of new unirec field.
 * \return ID of created or existing field or negative value as error.
 */
int define_new_unirec_field(std::string name);

#endif /* unirec_template_h */
