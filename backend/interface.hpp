
/**
 * \file interface.hpp
 * \brief Interface of stages in pipeline.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(INTERFACE_H)
#define INTERFACE_H

#include <vector>

#include <unirec/unirec.h>

/**
 * Interface for component (stage) in pipeline.
 */
struct Stage_intf {

protected:

   /**
    * Vector of next immediate components in pipeline.
    */
   std::vector<Stage_intf*> pipeline_successors;

   /**
    * \brief Send record to next immediate components in pipeline.
    * \param[in] *out_rec record ready to send.
    * \param[in] *out_tmplt unirec template of out_rec.
    */
   void send(void const *out_rec, ur_template_t const *out_tmplt)
   {
      for (auto const &successor: pipeline_successors)
         successor->eval(out_rec, out_tmplt);
   }

public:

   /**
    * \brief Initiate function for component. Call this function before records processing starts.
    * \param[in] *options settings for component.
    * \param[in] *succ vector of next immediate components in pipeline.
    * \return 0 on success, otherwise a negative error value.
    */
   virtual int init(char const *options, const std::vector<Stage_intf*> succ) = 0;

   /**
    * \brief Start processing the record.
    * \param[in] *rec record for processing.
    * \param[in] *in_tmplt unirec template of input record.
    * \return 0 on success, otherwise a negative error value.
    */
   virtual int eval(void const *rec, ur_template_t const *in_tmplt) = 0;

   virtual ~Stage_intf(void) {};
};

#endif /* interface_h */
