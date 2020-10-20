/**
 * \file csv_unirec.cpp
 * \brief Unirec to csv convertion. The code is not used.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include <unirec/unirec.h>
#include "fields.h"
//#include <iterator>
//#include <fstream>
#include "csv_unirec.hpp"
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

std::string const& CSV_unirec::operator[](std::size_t index) const{
    return m_data[index];
}

const char* CSV_unirec::operator[](std::string head_item) const{
    auto it = std::find(header.begin(), header.end(), head_item);
    if(it != header.end()){
        return m_data[it - header.begin()].c_str();
    }
    else{
        return "";
    }
}

std::size_t CSV_unirec::size() const{
    return m_data.size();
}

std::string CSV_unirec::string() const{
    std::string output;
    for(auto const& item: m_data){
        output.append(item);
        output.append(",");
    }
    output.pop_back();
    return output;
}

void CSV_unirec::set_header(){
    std::string withoutTypes = (*this).string();
    char* charPtr = ur_ifc_data_fmt_to_field_names(withoutTypes.c_str());
    std::string input = charPtr;
    std::stringstream ss(input);
    std::string item;
    while (std::getline(ss, item, ','))
    {
        header.push_back(item);
    }
    free(charPtr);
}

void CSV_unirec::readNextRow(std::istream& str){
    std::string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    std::string cell;

    m_data.clear();
    while(std::getline(lineStream, cell, ','))
    {
        m_data.push_back(cell);
    }
    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty())
    {
        // If there was a trailing comma then add an empty element.
        m_data.push_back("");
    }
}
