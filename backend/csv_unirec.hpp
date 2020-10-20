/**
 * \file csv_unirec.hpp
 * \brief Unirec to csv convertion. The code is not used.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(CSV_UNIREC_HPP)
#define CSV_UNIREC_HPP

#include <unirec/unirec.h>
#include "fields.h"
#include <vector>
#include <string>
#include <sstream>

class CSV_unirec
{

    std::vector<std::string>    m_data;
    std::vector<std::string>    header;

    public:
    std::string const& operator[](std::size_t index) const;
    const char* operator[](std::string head_item) const;

    std::size_t size() const;
    std::string string() const;
    void set_header();
    void readNextRow(std::istream& str);
};

inline std::istream& operator>>(std::istream& str, CSV_unirec& data){
    data.readNextRow(str);
    return str;
}

#endif
