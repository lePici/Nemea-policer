/**
 * \file grouper_stage.cpp
 * \brief Grouper stage instantiate.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "grouper_stage.hpp"
#include "grouper_stage_def.hpp"
#include "../config.hpp"

namespace client { namespace parser
{
   BOOST_SPIRIT_INSTANTIATE(grouper_stage_type, iterator_type, context_type);
}}

