/**
 * \file group_filter_stage.cpp
 * \brief Group-filter stage instantiate.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "group_filter_stage.hpp"
#include "group_filter_stage_def.hpp"
#include "../config.hpp"

namespace client { namespace parser
{
   BOOST_SPIRIT_INSTANTIATE(group_filter_stage_type, iterator_type, context_type);
}}

