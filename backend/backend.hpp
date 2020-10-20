
/**
 * \file backend.hpp
 * \brief Backend controller.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BACKEND_H)
#define BACKEND_H

#include "builder.hpp"
#include "interface.hpp"
#include "../parsing/inter_repr.hpp"
#include "program_arguments.hpp"
#include "unirec_template.hpp"

#include <csignal>
#include <vector>


/**
 * \brief Process run control.
 */
class Backend {

   Inter_repr *inter_repr;                              ///< Information obtained during parsing.
   client::ast::Unirec_input_template_fields templater; ///< Input Unirec template.
   Program_arguments *config;                           ///< Arguments from command line.
   client::ast::Builder *builder = NULL;                ///< Processing pipeline builder.
   pipelineVec pipelines;                               ///< Processing pipelines

   // function is not used
   //int processing_csv_input_file();

public:

   static sig_atomic_t stopFlag; ///< Interrupt the entire processing.

   /**
    * \param[in] ir information obtained during parsing.
    * \param[in] pa arguments from command line.
    */
   Backend(Inter_repr *ir, Program_arguments *pa): inter_repr(ir), templater(ir), config(pa) {};

   /**
    * \brief Prepare processing. Call this method after class constructor.
    * \return 0 on success.
    */
   int build_backend(void);

   /**
    * \brief Prepare processing. Call this method after build_backend() method.
    * \return 0 on success.
    */
   int start_processing(void);

   ~Backend(void);
};

#endif /* backend_h */
