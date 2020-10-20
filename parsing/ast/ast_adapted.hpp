/**
 * \file ast_adapted.hpp
 * \brief Boost fusion adaptions over structures in ast.hpp.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AST_ADAPTED_HPP)
#define BOOST_SPIRIT_X3_AST_ADAPTED_HPP

#include "ast.hpp"
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(client::ast::main_branches, first, rest)
BOOST_FUSION_ADAPT_STRUCT(client::ast::main_branch, name, mbm)
BOOST_FUSION_ADAPT_STRUCT(client::ast::filter_stage, body, succ)
BOOST_FUSION_ADAPT_STRUCT(client::ast::grouper_stage, body, succ)
BOOST_FUSION_ADAPT_STRUCT(client::ast::window_stage, body, succ)
BOOST_FUSION_ADAPT_STRUCT(client::ast::aggregator_stage, body, succ)
BOOST_FUSION_ADAPT_STRUCT(client::ast::group_filter_stage, body, succ)
BOOST_FUSION_ADAPT_STRUCT(client::ast::selector_stage, body)

BOOST_FUSION_ADAPT_STRUCT(client::ast::grouper_body, type)
BOOST_FUSION_ADAPT_STRUCT(client::ast::window_body, type, range, slide)
BOOST_FUSION_ADAPT_STRUCT(client::ast::aggregator_body, first, rest)
BOOST_FUSION_ADAPT_STRUCT(client::ast::selector_body, first, rest)

BOOST_FUSION_ADAPT_STRUCT(client::ast::grouper_keys, first, rest)
BOOST_FUSION_ADAPT_STRUCT(client::ast::aggr_assignment, variable_name, func_type)
BOOST_FUSION_ADAPT_STRUCT(client::ast::aggr_func_with_param_s, func_name, param)
BOOST_FUSION_ADAPT_STRUCT(client::ast::sel_assignment, output, sel_expression_)
BOOST_FUSION_ADAPT_STRUCT(client::ast::sel_expression, first, rest)
BOOST_FUSION_ADAPT_STRUCT(client::ast::sel_operation, operator_, operand_)
BOOST_FUSION_ADAPT_STRUCT(client::ast::sel_unary, operator_, operand_)

BOOST_FUSION_ADAPT_STRUCT(client::ast::branch_after_fi, name, succ)
BOOST_FUSION_ADAPT_STRUCT(client::ast::branch_after_gr, name, succ)
BOOST_FUSION_ADAPT_STRUCT(client::ast::branch_after_wi, name, succ)
BOOST_FUSION_ADAPT_STRUCT(client::ast::branch_after_ag, name, succ)
BOOST_FUSION_ADAPT_STRUCT(client::ast::branch_after_gf, name, succ)
BOOST_FUSION_ADAPT_STRUCT(client::ast::branch_after_fi_s, first, rest)
BOOST_FUSION_ADAPT_STRUCT(client::ast::branch_after_gr_s, first, rest)
BOOST_FUSION_ADAPT_STRUCT(client::ast::branch_after_wi_s, first, rest)
BOOST_FUSION_ADAPT_STRUCT(client::ast::branch_after_ag_s, first, rest)
BOOST_FUSION_ADAPT_STRUCT(client::ast::branch_after_gf_s, first, rest)

#endif
