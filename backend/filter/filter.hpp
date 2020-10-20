
/**
 * \file filter.hpp
 * \brief Filter stage.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "../interface.hpp"

extern "C" {
#include "ffilter.h"
}

/**
 * Implementation of Filter in pipeline.
 */
class Filter: public Stage_intf
{
   ff3_t *filter = NULL;     ///< Pointer to netflow filter implementation from ffilter.h
   ff3_options_t *callbacks; ///< Callbacks function for ffilter.

public:

   /**
    * \brief Initiate function for Filter. Call this function before records processing starts.
    * \param[in] *options settings for Filter class.
    *    In options is expected whole filter body (filter expression).
    * \param[in] *succ vector of next immediate components in pipeline.
    * \return 0 on success, otherwise a negative error value.
    */
   int init(char const *options, const std::vector<Stage_intf*> succ);

   /**
    * \brief Start processing the record.
    * \param[in] *rec record for processing.
    * \param[in] *in_tmplt unirec template of input record.
    * \return 0 on success, otherwise a negative error value.
    */
   int eval(void const *rec, ur_template_t const *in_tmplt);

   ~Filter();
};
