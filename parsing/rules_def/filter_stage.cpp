/**
 * \file filter_stage.cpp
 * \brief Filter stage instantiate.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "filter_stage.hpp"
#include "filter_stage_def.hpp"
#include "../config.hpp"

namespace client { namespace parser
{
   BOOST_SPIRIT_INSTANTIATE(filter_stage_type, iterator_type, context_type);
}}

