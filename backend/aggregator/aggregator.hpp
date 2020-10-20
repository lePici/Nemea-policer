#include "../interface.hpp"
#include "configuration.hpp"
#include "key.h"
#include <vector>
#include <time.h>
#include <unordered_map>
#include <thread>
#include <mutex>


/* ----------------------------------------------------------------- */
#ifdef  AGGREGATOR_KEYWORD_H
namespace std {
   /**
    * std::hash() specialization to use with class Key.
    * Specialization needed by use class Key as key in std::unordered_map
    */
    template<>
    struct hash<Key>
    {
        size_t operator()(const Key &k) const
        {
           return SuperFastHash(k.get_data(), k.get_size());
        }
    };
}
#endif
/* ----------------------------------------------------------------- */

class Agg : public Stage_intf{

    public:

    static int stop;
    int init(char const* options, const std::vector<Stage_intf*> succ);
    int eval(void const* rec, ur_template_t const* in_tmplt);

    private:
    Config config;
    OutputTemplate outputTemp;
    KeyTemplate keyTemp;
    std::unordered_map<Key , void*> storage;
    time_t time_last_from_record;             // Passive timeout time info set due to records time

    std::thread timeout_thread;
    std::mutex storage_mutex;                 // For storage modifying sections
    std::mutex time_last_from_record_mutex;   // For modifying Passive timeout time info

    void clean_memory();
    void clean_memory_with_ptrs();
    void process_agg_functions(ur_template_t const* in_tmplt, void const* src_rec, void *dst_rec);
    void init_record_data(ur_template_t const* in_tmplt, void const* src_rec, void *dst_rec);
    void init_ptr_field(ur_template_t const* in_tmplt, void const* src_rec, void* dst_rec);
    void prepare_to_send(void *stored_rec);
    bool send_record_out(void *out_rec);
    void check_timeouts();
    void flush_storage();
    
    public:
    Agg() : timeout_thread(){};
    ~Agg();
};


