
/**
 * \file inter_repr.hpp
 * \brief Inter represantion of parsed text - parser output.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(INTER_REPR_HPP)
#define INTER_REPR_HPP

#include "ast/ast_adapted.hpp"
#include "ast/ast_variables.hpp"

using astT = client::ast::main_branches;

/**
 * \brief Contains abstract syntax tree and mapping of user variables to unirec equivalent.
 * \details MapT contains the path to the stage as a key. The item contains tuple name-alias
 *    where "name" is user variable name and "alias" is unirec keyword.
 *    filMapT filterVars is useless right now due to the absence of the Publisher stage.
 */
struct Inter_repr: public client::ast::Variables
{
   astT ast;                  ///< Whole abstract syntax tree.
   filMapT filterVars;        ///< User variables from each Filter bodie. (It will be empty yet.)
   grfiMapT group_filterVars; ///< User variables from each Group-filter bodie.
   aggrMapT aggrVars;         ///< User variables from each Aggregator.
   selMapT selVars;           ///< User variables from each Selector.
};

/**
 * \brief Check if user variable was initialized by aggregate function.
 * \param[in] &data Inter_repr structure from parsing.
 * \return 0 on success, otherwise a negative error value.
 */
int check_inter_repr(Inter_repr const &data);

#endif
