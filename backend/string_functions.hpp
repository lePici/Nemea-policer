
/**
 * \file string_functions.hpp
 * \brief Helping functions for strings.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(STRING_FUNCTIONS_H)
#define STRING_FUNCTIONS_H

#include <string>
#include <vector>

/**
 * \brief Divide input string to vector by delimiter.
 * \param[in] input source of string.
 * \param[in] delimiter to be divided.
 * \return Vector of chunks.
 */
std::vector<std::string> divide_str(std::string input, const std::string delimiter);

/**
 * \brief Replace all occurrences of the search string.
 * \param[in] input source of string.
 * \param[in] searched wanted string.
 * \param[in] replaceStr the search string will be replaced with replaceStr.
 * \return String after substitutions.
 */
std::string findAndReplaceAll(std::string input, const std::string searched,
                              const std::string replaceStr);

/**
 * \brief Converting vector of strings to one string.
 * \param[in] vec source.
 * \param[in] delimiter in united string.
 * \return United string.
 */
std::string vecOfstr2str(const std::vector<std::string> vec, const std::string delimiter);

/**
 * \brief Replace searched string in selector body. Replacing aggregator variable to unirec keyword.
 * \details Function divide input string to tokens by comma delimiter.
 *    Token may have two forms - (1. partA), or (2. partB = partC).
 *    PartA and partC will be edited if contains searched string.
 * \see Example:
 * \code
 *    replace_vares_in_selector_body("var", "var", "PACKETS")
 *       output -> "PACKETS"
 *    replace_vares_in_selector_body("userKeyword = var + var", "var", "PACKETS")
 *       output -> "userKeyword=PACKETS+PACKETS"
 * \endcode
 * \param[in] input source of string.
 * \param[in] searched wanted string.
 * \param[in] replaceStr the search string will be replaced with replaceStr.
 * \return New Selector stage body without aggregator variables.
 */
std::string replace_aliases_in_selector_body(std::string input, const std::string searched,
                                             const std::string replaceStr);

/**
 * \brief Replace searched string in group-filter body. Replacing aggregator variable to unirec keyword.
 * \param[in] input source of string.
 * \param[in] searched wanted string.
 * \param[in] replaceStr the search string will be replaced with replaceStr.
 * \return New Group-filter stage body without aggregator variables.
 */
std::string replace_aliases_in_group_filter_body(std::string input, const std::string searched,
                                                 const std::string replaceStr);

#endif /* string_functions_h */
