/**
 * \file aggregator_stage.cpp
 * \brief Aggregator stage instantiate.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "aggregator_stage.hpp"
#include "aggregator_stage_def.hpp"
#include "../config.hpp"

namespace client { namespace parser
{
   BOOST_SPIRIT_INSTANTIATE(aggregator_stage_type, iterator_type, context_type);
}}

