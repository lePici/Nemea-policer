
/**
 * \file selector.hpp
 * \brief Selector stage.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "../interface.hpp"

#include <vector>
#include <string>

#include <unirec/unirec.h>

class Selector:public Stage_intf {

   /**
    * \brief Item in dictionary of output names.
    * \details etc. for string attacker=SRC_IP is "attacker" alias_name
    *    and "SRC_IP" is field_name
    */
   typedef struct {
      char *alias_name; ///< User output name.
      int alias_id;     ///< User output ID for name.
      char *field_name; ///< Unirec name.
      int field_id;     ///< Unirec ID.
   } sel_dict_item;

   using dictT = std::vector<sel_dict_item>;
   dictT dict;                       ///< Dictionary of output names.
   ur_template_t *out_tmplt = NULL;  ///< Output unirec template.
   int ifc_num = -1;                 ///< Index of output trap interface. Every Selector has his own unique ifc_num.
   const char delimiter = ',';       ///< Delimiter in csv output.
   int static_size_of_out_tmplt = 0; ///< Total size of UniRec record except variable-length fields.
   void *out_rec = NULL;             ///< Output record.

   /**
    * \brief Set output trap interface from number in string datatype.
    * \details Just convert string to number.
    * \param[in] input source containing number.
    * \return 0
    */
   int set_ifc_num(std::string input);

   /**
    * \brief Define new Unirec field. 
    * \param[in] *alias_name name of the new Unirec field.
    * \param[in] *field_name is used to derive the type of the new Unirec field.
    * \return 0 on success, otherwise a negative error value.
    */
   int define_new_alias_field(char const *alias_name, char const *field_name);

   /**
    * \brief Parse Selector body and add tokens to dictionary.
    * \details This function remove blank spaces and call fill_dict(input, delimiter) function.
    * \param[in] input string of Selector body.
    * \return 0 on success, otherwise a negative error value.
    */
   int set_dict(std::string input);

   /**
    * \brief Add tokens to dictionary.
    * \param[in] &input Selector body without white spaces.
    * \param[in] delimiter of expression (=).
    * \return 0 on success, otherwise a negative error value.
    */
   int fill_dict(std::vector<std::string> const &input, std::string delimiter);

   /**
    * \brief Print content of dictionary to stdout. For debugging.
    * \param[in] dict dictionary of names.
    */
   void print_dict(dictT dict);

   /**
    * \brief Creating of Unirec output template.
    * \return 0 on success, otherwise a negative error value.
    */
   int create_output_template(void);

   /**
    * \brief Print record to stdout in csv format. Function is not used.
    */
   int print_in_csv(void const *rec, ur_template_t const *in_tmplt);

public:

   /**
    * \brief Initiate function for Selector. Call this function before records processing starts.
    * \param[in] *options settings for Selector. Expected "ifc_num:selector_body".
    *    Item selector_body look like etc. "attacker = SRC_IP, SRC_PORT".
    * \param[in] *succ vector of next immediate components in pipeline.
    * \return 0 on success, otherwise a negative error value.
    */
   int init(char const *options, std::vector<Stage_intf*> /* succ */ );

   /**
    * \brief Start processing the record.
    * \param[in] *rec record for processing.
    * \param[in] *in_tmplt unirec template of input record.
    * \return 0 on success, otherwise a negative error value.
    */
   int eval(void const *rec, ur_template_t const *in_tmplt);

   ~Selector();
};
