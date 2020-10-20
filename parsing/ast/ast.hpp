/**
 * \file ast.hpp
 * \brief Data structure of abstract syntax tree.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AST_HPP)
#define BOOST_SPIRIT_X3_AST_HPP

#include <boost/optional.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/fusion/include/io.hpp>

#include <string>
#include <vector>

/*
 * Our Abstract syntax tree consists of these structures.
 */

namespace client { namespace ast
{
   namespace fusion = boost::fusion;
   namespace x3 = boost::spirit::x3;

   typedef std::string aux_string;
   typedef std::vector<int> aux_int;

   /**
    * \brief Unirec keywords.
    */
   enum unirec {   
      f_dstIP,        ///< Destination IP address.
      f_srcIP,        ///< Source IP address.
      f_bytes,        ///< Total number of bytes transferred by the flow.
      f_linkBitField, ///< Bit field where each bit marks whether a flow was captured on corresponding link.
      f_timeFirst,    ///< Timestamp of the first packet of the flow.
      f_timeLast,     ///< Timestamp of the last packet of the flow.
      f_packets,      ///< Number of packets of the flow.
      f_dstPort,      ///< Destination port of Transport layer.
      f_srcPort,      ///< Source port of Transport layer.
      f_dirBitField,  ///< Bit field used for detemining incomming/outgoing flow.
      f_protocol,     ///< Transport protocol identification (e.g. 6 for TCP, 17 for UDP)
      f_tcpFlags,     ///< TCP flags of all packets from the flow - flag bits are added bitwise.
      f_tos,          ///< Type of service field from IP header.
      f_ttl           ///< Time-To-Live value from IP header.
   };
   typedef std::vector<unirec> unirec_vec;

   /**
    * \brief Special mode for Grouper stage.
    */
   enum grouper_case {
      g_bidirectional ///< TODO: not implemented.
   };

   /**
    * \brief Window type in Window stage.
    */
   enum window_type {
      /**
       * Every received record is compared with the stored one of the equal aggregation key (if exists).
       * If the TIME_FIRST field value of received record is greater than
       * TIME_FIRST field value + TIMEOUT of stored record, the stored record is sent out from module
       * and replaced with the new one. Otherwise the record fields values are updated due to specified rules.
       */
      w_active,

      /**
       * Every stored record is periodically checked every TIMEOUT second.
       * In case that TIME_LAST field value is TIMEOUT seconds old (untouched) or older,
       * the record is sent out and removed from storage. No action is taken otherwise.
       */
      w_passive,

      /**
       * Each stored records are periodically sent out every TIMEOUT second.
       */
      w_global,

      /**
       * This type presents combination of Active and Passive timeout types.
       * Both can have different TIMEOUT length value and both values are used separately for given type.
       */
      w_mixed
   };

   /**
    * \brief Aggregation function with parameter.
    */
   enum aggr_func_with_param {   
      ap_sum,           ///< Makes total sum of field values.
      ap_average,       ///< Makes average of field values.
      ap_minimal,       ///< Keep minimal value of field across all received records.
      ap_maximal,       ///< Keep maximal value of field across all received records.
      ap_first,         ///< Keep the first obtained value of field.
      ap_last,          ///< Update the field with every new received record.
      ap_or,            ///< Makes bitwise OR of field with every new received record.
      ap_and,           ///< Makes bitwise AND of field with every new received record.
      ap_countDistinct, ///< Count the number of unique items.
      ap_rate           ///< Number of items per second.
      //ap_mean,
      //ap_stddev,
   };

   /**
    * \brief Aggregation function that does not contain a parameter.
    */
   enum aggr_func_without_param {   
      ad_count ///< Counts the number of items.
   };

   /**
    * \brief Symbol as an aggregate function parameter.
    */
   enum aggr_func_param_symbol {
      afp_star ///< Has no effect. It's just for decoration.
   };

   typedef unsigned int seconds;

   /**
    * \brief String contains whole filter body.
    */
   typedef std::string filter_body;

   /**
    * \brief String contains whole group-filter body.
    */
   typedef std::string group_filter_body;

   /**
    * \brief Collection of aggregation keys.
    */
   struct grouper_keys {
      unirec first;    ///< First aggregation key.
      unirec_vec rest; ///< Remaining aggregation keys.
   };

   /**
    * \brief Aggregation keys or some special type.
    */
   struct grouper_type : x3::variant<
           grouper_keys,
           grouper_case
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Body of Grouper stage.
    */
   struct grouper_body {
      grouper_type type; ///< Setting option for Grouper stage.
   };

   /**
    * \brief Body of Window stage.
    */
   struct window_body {
      window_type type;               ///< Type of window.
      seconds range;                  ///< Size of window in seconds.
      boost::optional<seconds> slide; ///< How much the window move.
   };

   /**
    * \brief Aggregation function with parameter.
    */
   struct aggr_func_with_param_s {
      aggr_func_with_param func_name; ///< Name of function.
      unirec param;                   ///< Parameter as Unirec keyword.
   };

   /**
    * \brief Aggregation function with or without parameter.
    */
   struct aggr_func_type : x3::variant<
           aggr_func_with_param_s,
           aggr_func_without_param
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Assignment output of aggregation function to variable.
    */
   struct aggr_assignment {
      std::string variable_name; ///< Variable name.
      aggr_func_type func_type;  ///< Type of aggregation function.
   };

   /**
    * \brief Items in aggregation body.
    */
   typedef std::vector<aggr_assignment> aggr_assignment_vec;

   /**
    * \brief Body of Aggregator stage.
    */
   struct aggregator_body {
      aggr_assignment first;    ///< First item.
      aggr_assignment_vec rest; ///< Remaining items.
   };

   /**
    * \brief Empty token.
    */
   struct nil {};
   struct sel_unary;
   struct sel_expression;

   /**
    * \brief Variable in Selector is Unirec keyword or user variable as unknown string.
    */
   struct sel_variable : x3::variant<
           unirec,
           std::string
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Empty token or digit or variable or expression.
    */
   struct sel_operand : x3::variant<
           nil,
           double,
           sel_variable,
           x3::forward_ast<sel_unary>,
           x3::forward_ast<sel_expression>
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };
   
   /**
    * \brief Arithmetic operations in selector expression.
    */
   enum sel_optoken {
      op_plus,     ///< +
      op_minus,    ///< -
      op_times,    ///< *
      op_divide,   ///< /
      op_positive, ///< +(digit)
      op_negative, ///< -(digit)
   };

   /**
    * \brief Unary operand before variable/digit/expression.
    */
   struct sel_unary {
      sel_optoken operator_;
      sel_operand operand_;
   };

   /**
    * \brief Arithmetic operation.
    */
   //struct sel_operation : x3::position_tagged
   struct sel_operation {
      sel_optoken operator_;
      sel_operand operand_;
   };
   typedef std::vector<sel_operation> sel_operation_vec;

   /**
    * \brief Expression in Selector stage.
    */
   //struct sel_expression : x3::position_tagged
   struct sel_expression {
      sel_operand first;
      sel_operation_vec rest;
   };

   /**
    * \brief Output forms are User variable as string or Unirec keyword.
    */
   struct sel_output : x3::variant<
           std::string,
           unirec
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Selector assignment look like "[keyword] = [expression]"
    */
   struct sel_assignment {
      sel_output output;
      boost::optional<sel_expression> sel_expression_;
   };
   typedef std::vector<sel_assignment> sel_assignment_vec;

   /**
    * \brief Body of Selector stage.
    */
   struct selector_body {
      sel_assignment first;
      sel_assignment_vec rest;
   };

   struct branch_after_fi;
   struct branch_after_gr;
   struct branch_after_wi;
   struct branch_after_ag;
   struct branch_after_gf;
   struct grouper_stage;
   struct window_stage;
   struct aggregator_stage;
   struct group_filter_stage;
   struct selector_stage;
   //struct branch_after_fi_succ;
   
   /**
    * \brief After the Filter can be Grouper, Window, Aggregator or Selector.
    */
   struct fi_stage_succ : x3::variant<
           x3::forward_ast<grouper_stage>,
           x3::forward_ast<window_stage>,
           x3::forward_ast<aggregator_stage>,
           x3::forward_ast<selector_stage>
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief After the Grouper can be Window, Aggregator, Group-filter or Selector.
    */
   struct gr_stage_succ : x3::variant<
           x3::forward_ast<window_stage>,
           x3::forward_ast<aggregator_stage>,
           x3::forward_ast<group_filter_stage>,
           x3::forward_ast<selector_stage>
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief After the Window can be Aggregator, Group-filter or Selector.
    */
   struct wi_stage_succ : x3::variant<
           x3::forward_ast<aggregator_stage>,
           x3::forward_ast<group_filter_stage>,
           x3::forward_ast<selector_stage>
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief After the Aggregator can be Group-filter or Selector.
    */
   struct ag_stage_succ : x3::variant<
           x3::forward_ast<group_filter_stage>,
           x3::forward_ast<selector_stage>
           //x3::forward_ast<publisher_stage>,
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Branch after Filter contains Filter's successors or another branch.
    */
   struct branch_after_fi_succ : x3::variant<
           fi_stage_succ,
           boost::recursive_wrapper<branch_after_fi>
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Branch after Grouper contains Grouper's successors or another branch.
    */
   struct branch_after_gr_succ : x3::variant<
           gr_stage_succ,
           boost::recursive_wrapper<branch_after_gr>
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Branch after Window contains Window's successors or another branch.
    */
   struct branch_after_wi_succ : x3::variant<
           wi_stage_succ,
           boost::recursive_wrapper<branch_after_wi>
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Branch after Aggregator contains Aggregator's successors or another branch.
    */
   struct branch_after_ag_succ : x3::variant<
           ag_stage_succ,
           boost::recursive_wrapper<branch_after_ag>
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Branch after Group-filter contains Group-filter's successors or another branch.
    */
   struct branch_after_gf_succ : x3::variant<
           x3::forward_ast<selector_stage>,
           boost::recursive_wrapper<branch_after_gf>
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Branch after Filter has name and contains Filter's successors.
    */
   struct branch_after_fi {
      std::string name;
      branch_after_fi_succ succ;
   };

   typedef std::vector<branch_after_fi> branch_after_fi_vec;

   /**
    * \brief Branch after Grouper has name and contains Grouper's successors.
    */
   struct branch_after_gr {
      std::string name;
      branch_after_gr_succ succ;
   };

   typedef std::vector<branch_after_gr> branch_after_gr_vec;

   /**
    * \brief Branch after Window has name and contains Window's successors.
    */
   struct branch_after_wi {
      std::string name;
      branch_after_wi_succ succ;
   };

   typedef std::vector<branch_after_wi> branch_after_wi_vec;

   /**
    * \brief Branch after Aggregator has name and contains Aggregator's successors.
    */
   struct branch_after_ag {
      std::string name;
      branch_after_ag_succ succ;
   };

   typedef std::vector<branch_after_ag> branch_after_ag_vec;

   /**
    * \brief Branch after Group-filter has name and contains Group-filter's successors.
    */
   struct branch_after_gf {
      std::string name;
      branch_after_gf_succ succ;
   };

   typedef std::vector<branch_after_gf> branch_after_gf_vec;

   /**
    * \brief Consecutive nested branches behind the Filter.
    */
   struct branch_after_fi_s {
      branch_after_fi first;
      branch_after_fi_vec rest;
   };

   /**
    * \brief Consecutive nested branches behind the Grouper.
    */
   struct branch_after_gr_s {
      branch_after_gr first;
      branch_after_gr_vec rest;
   };

   /**
    * \brief Consecutive nested branches behind the Window.
    */
   struct branch_after_wi_s {
      branch_after_wi first;
      branch_after_wi_vec rest;
   };

   /**
    * \brief Consecutive nested branches behind the Aggregator.
    */
   struct branch_after_ag_s {
      branch_after_ag first;
      branch_after_ag_vec rest;
   };

   /**
    * \brief Consecutive nested branches behind the Group-filter.
    */
   struct branch_after_gf_s {
      branch_after_gf first;
      branch_after_gf_vec rest;
   };

   /**
    * \brief Filter successor is next stage or branch.
    */
   struct filter_successor : x3::variant<
           fi_stage_succ,
           branch_after_fi_s
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Grouper successor is next stage or branch.
    */
   struct grouper_successor : x3::variant<
           gr_stage_succ,
           branch_after_gr_s
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Window successor is next stage or branch.
    */
   struct window_successor : x3::variant<
           wi_stage_succ,
           branch_after_wi_s
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Aggregator successor is next stage or branch.
    */
   struct aggregator_successor : x3::variant<
           ag_stage_succ,
           branch_after_ag_s
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Group-filter successor is next stage or branch.
    */
   struct group_filter_successor : x3::variant<
           x3::forward_ast<selector_stage>,
           branch_after_gf_s
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Filter stage structure.
    */
   struct filter_stage : x3::position_tagged {
      filter_body body;
      filter_successor succ;
   };

   /**
    * \brief Grouper stage structure.
    */
   struct grouper_stage : x3::position_tagged {
      grouper_body body;
      grouper_successor succ;
   };

   /**
    * \brief Window stage structure.
    */
   struct window_stage : x3::position_tagged {
      window_body body;
      window_successor succ;
   };

   /**
    * \brief Aggregator stage structure.
    */
   struct aggregator_stage : x3::position_tagged {
      aggregator_body body;
      aggregator_successor succ;
   };

   /**
    * \brief Group-filter stage structure.
    */
   struct group_filter_stage : x3::position_tagged {
      group_filter_body body;
      group_filter_successor succ;
   };

   /**
    * \brief Selector stage structure.
    */
   struct selector_stage : x3::position_tagged {
      selector_body body;
   };

   /**
    * \brief Stage can be Filter, Grouper, Window, Aggregator or Selector
    */
   struct stage : x3::variant<
           filter_stage,
           grouper_stage,
           window_stage,
           aggregator_stage,
           selector_stage
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   struct main_branch;
   struct main_branches;
   typedef std::vector<main_branch> main_branch_vec;
   
   /**
    * \brief First item in main branch body is stage or nested main branch.
    */
   struct main_branch_body : x3::variant<
           stage,
           boost::recursive_wrapper<main_branch_vec>
       >
   {
      using base_type::base_type;
      using base_type::operator=;
   };

   /**
    * \brief Main branch structure.
    */
   struct main_branch {
      std::string name;
      main_branch_body mbm;
   };

   /**
    * \brief All main branches.
    */
   struct main_branches : x3::position_tagged {
      main_branch first;
      main_branch_vec rest;
   };

   /**
    * \brief Convert Unirec Enum item to string.
    */
   std::string string_unirecEnum(unirec item);

   /**
    * \brief Check if string is Unirec keyword.
    */
   bool string_isUnirec(std::string unirec);

   /**
    * \brief Convert Window type Enum to string.
    */
   std::string string_windowTypeEnum(window_type item);

   /**
    * \brief Convert Aggregation without parameter Enum to string.
    */
   std::string string_aggrWithoutParamEnum(aggr_func_without_param item);

   /**
    * \brief Convert Aggregation with parameter Enum to string.
    */
   std::string string_aggrWithParamEnum(aggr_func_with_param item);

   /**
    * \brief Convert Selector operation Enum to string.
    */
   std::string string_sel_optokenEnum(sel_optoken item);

   using boost::fusion::operator<<;
}}

#endif
