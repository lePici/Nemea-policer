
/**
 * \file selector.cpp
 * \brief Definition of Selector class.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "selector.hpp"
#include "../string_functions.hpp"

#include <algorithm>
#include <errno.h>     
#include <inttypes.h>  
#include <iostream>
#include <string.h>    

#include <libtrap/trap.h>

#define SEND_EOF 1

int Selector::define_new_alias_field(char const *alias_name, char const *field_name)
{
   ur_field_type_t type = ur_get_type(ur_get_id_by_name(field_name));

   if (ur_define_field(alias_name, type) == UR_E_TYPE_MISMATCH) {
      fprintf(stderr, "Error: In Selector stage -> please change %s to other name\n", alias_name);
      return -1;
   }
   return 0;
}

int Selector::fill_dict(std::vector<std::string> const &input, std::string delimiter)
{
   using namespace std;

   for (auto const &item: input) {
      size_t found = item.find(delimiter);

      /* look like: SRC_IP */
      if (found == string::npos) {
         const char *field_name = item.c_str();

         dict.push_back((sel_dict_item) {
                        NULL, 0, strdup(field_name), ur_get_id_by_name(field_name)});
      }
      /* look like: attacker=SRC_IP */
      else {
         string alias_name = item.substr(0, found);
         string field_name = item.substr(found + 1);

         if (define_new_alias_field(alias_name.c_str(), field_name.c_str()) != 0) {
            return -1;
         }
         dict.push_back((sel_dict_item) {
                        strdup(alias_name.c_str()),
                        ur_get_id_by_name(alias_name.c_str()),
                        strdup(field_name.c_str()),
                        ur_get_id_by_name(field_name.c_str())
                        }
         );
      }
   }
   return 0;
}

int Selector::set_ifc_num(std::string input)
{
   ifc_num = std::stoi(input);
   return 0;
}

int Selector::set_dict(std::string input)
{
   using namespace std;

   input.erase(remove_if
               (
                  input.begin(),
                  input.end(),
                  [](unsigned char x) { return isspace(x); }
               ),
               input.end());
   return fill_dict(divide_str(input, ","), "=");
}

void Selector::print_dict(dictT dict)
{
   using namespace std;

   for (auto const &item: dict) {
      if (item.alias_name != NULL) {
         printf("alias_name: %s\n", item.alias_name);
         printf("alias_id: %d\n", item.alias_id);
      }
      printf("name_name: %s\n", item.field_name);
      printf("name_id: %d\n", item.field_id);
   }
}

int Selector::create_output_template(void)
{
   std::string fields;
   for (auto & item: dict) {
      if (item.alias_name != NULL) {
         fields.append(item.alias_name);
         fields.append(",");
      } else {
         fields.append(item.field_name);
         fields.append(",");
      }
   }
   fields.pop_back();
   out_tmplt = ur_create_output_template(ifc_num, fields.c_str(), NULL);
   if (out_tmplt == NULL) {
      fprintf(stderr, "ur_create_output_template error\n");
      return -1;
   }
   return 0;
}

int Selector::init(char const *options, std::vector<Stage_intf*> /* succ */ )
{
   using namespace std;

   errno = 0;
   vector<string> param = divide_str(options, ":");
   if (param.size() != 2) {
      cerr << "Internal Error: selector_init - param.size()" << endl;
      return -1;
   }
   int ret = 0;

   set_ifc_num(param[0]);
   if (set_dict(param[1]) != 0) {
      cerr << "Internal Error: selector_init - sel_dict" << endl;
      return -1;
   }
   // print_dict(dict);
   ret |= create_output_template();
   static_size_of_out_tmplt = ur_rec_fixlen_size(out_tmplt);
   out_rec = ur_create_record(out_tmplt, 0);
   if ((errno | ret) != 0) {
      cerr << "Internal Error: selector_init" << endl;
   }
   return errno | ret;
}

int Selector::eval(void const *in_rec, ur_template_t const *in_tmplt)
{

   // TODO: create two loops without inner if statement
   for (auto const &item: dict) {
      if (item.alias_name == NULL) {
         void *src = ur_get_ptr_by_id(in_tmplt, in_rec, item.field_id);
         void *dst = ur_get_ptr_by_id(out_tmplt, out_rec, item.field_id);

         memcpy(dst, src, ur_get_size(item.field_id));
      } else {
         void *src = ur_get_ptr_by_id(in_tmplt, in_rec, item.field_id);
         void *dst = ur_get_ptr_by_id(out_tmplt, out_rec, item.alias_id);

         memcpy(dst, src, ur_get_size(item.alias_id));
      }
   }

   int ret = trap_send(ifc_num, out_rec, static_size_of_out_tmplt);

   TRAP_DEFAULT_SEND_ERROR_HANDLING(ret, 0, fprintf(stderr, "Error: trap sending failed\n"));
   return 0;
}

Selector::~Selector()
{
   if (SEND_EOF == 1) {
      int ret = trap_send(ifc_num, out_rec, 1);

      if (ret != TRAP_E_OK) {
         std::cerr << "ERROR while sending message of module ending: trap_last_error_msg" << std::endl;
         std::cerr << "Receiving side probably did not notice shutdown of module" << std::endl;
      }
   }
   for (auto const &item:dict) {
      free(item.alias_name);
      free(item.field_name);
   }
   ur_finalize();
   ur_free_template(out_tmplt);
   ur_free_record(out_rec);
}

int Selector::print_in_csv(void const *rec, ur_template_t const *in_tmplt)
{
   FILE *file = stdout;
   static bool once = true;

   if (once) {
      char delim = '\0';

      for (auto const &item:dict) {
         if (item.alias_name != NULL) {
            fprintf(file, "%c%s", delim, item.alias_name);
         } else {
            fprintf(file, "%c%s", delim, item.field_name);
         }
         delim = ',';
      }
      fprintf(file, "\n");
      fflush(file);
      once = false;
   }

   int delim = 0;

   for (auto const &item:dict) {
      if (delim != 0) {
         fprintf(file, "%c", delimiter);
      }
      delim = 1;
      int unir_id = ur_get_id_by_name(item.field_name);

      if (ur_is_present(in_tmplt, unir_id)) {
         /* Get pointer to the field (valid for static fields only). */
         void *ptr = ur_get_ptr_by_id(in_tmplt, rec, unir_id);

         /* Static field - check what type is it and use appropriate format. */
         switch (ur_get_type(unir_id)) {
         case UR_TYPE_UINT8:
            fprintf(file, "%u", *(uint8_t *) ptr);
            break;
         case UR_TYPE_UINT16:
            fprintf(file, "%u", *(uint16_t *) ptr);
            break;
         case UR_TYPE_UINT32:
            fprintf(file, "%u", *(uint32_t *) ptr);
            break;
         case UR_TYPE_UINT64:
            fprintf(file, "%" PRIu64, *(uint64_t *) ptr);
            break;
         case UR_TYPE_INT8:
            fprintf(file, "%i", *(int8_t *) ptr);
            break;
         case UR_TYPE_INT16:
            fprintf(file, "%i", *(int16_t *) ptr);
            break;
         case UR_TYPE_INT32:
            fprintf(file, "%i", *(int32_t *) ptr);
            break;
         case UR_TYPE_INT64:
            fprintf(file, "%" PRIi64, *(int64_t *) ptr);
            break;
         case UR_TYPE_CHAR:
            fprintf(file, "%c", *(char *) ptr);
            break;
         case UR_TYPE_FLOAT:
            fprintf(file, "%f", *(float *) ptr);
            break;
         case UR_TYPE_DOUBLE:
            fprintf(file, "%f", *(double *) ptr);
            break;
         case UR_TYPE_IP:
            {
               /* IP address - convert to human-readable format and print. */
               char str[46];

               ip_to_str((ip_addr_t *) ptr, str);
               fprintf(file, "%s", str);
            }
            break;
         case UR_TYPE_MAC:
            {
               /* MAC address - convert to human-readable format and print. */
               char str[MAC_STR_LEN];

               mac_to_str((mac_addr_t *) ptr, str);
               fprintf(file, "%s", str);
            }
            break;
         case UR_TYPE_TIME:
            {
               /* Timestamp - convert to human-readable format and print. */
               time_t sec = ur_time_get_sec(*(ur_time_t *) ptr);
               int msec = ur_time_get_msec(*(ur_time_t *) ptr);
               char str[32];

               strftime(str, 31, "%FT%T", gmtime(&sec));
               fprintf(file, "%s.%03i", str, msec);
            }
            break;
         case UR_TYPE_STRING:
            {
               /* Printable string - print it as it is. */
               int size = ur_get_var_len(in_tmplt, rec, unir_id);
               char *data = (char *) ur_get_ptr_by_id(in_tmplt, rec, unir_id);

               putc('"', file);
               while (size--) {
                  switch (*data) {
                  case '\n':   /* Replace newline with space. */
                     putc(' ', file);
                     break;
                  case '"':    /* Double quotes in string. */
                     putc('"', file);
                     putc('"', file);
                     break;
                  default:     /* Check if character is printable. */
                     if (isprint(*data)) {
                        putc(*data, file);
                     }
                  }
                  data++;
               }
               putc('"', file);
            }
            break;
         case UR_TYPE_BYTES:
            {
               /* Generic string of bytes - print each byte as two hex digits. */
               int size = ur_get_var_len(in_tmplt, rec, unir_id);
               unsigned char *data = (unsigned char *) ur_get_ptr_by_id(in_tmplt, rec, unir_id);

               while (size--) {
                  fprintf(file, "%02x", *data++);
               }
            }
            break;
         default:
            {
               /* Unknown type - print the value in hex. */
               int size = ur_get_len(in_tmplt, rec, unir_id);

               fprintf(file, "0x");
               for (int i = 0; i < size; i++) {
                  fprintf(file, "%02x", ((unsigned char *) ptr)[i]);
               }
            }
            break;
         }                      /* case (field type) */
      }                         /* if present */
   }                            /* loop over fields */
   fprintf(file, "\n");
   fflush(file);
   return 0;
}
