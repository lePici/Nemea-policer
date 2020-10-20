/**
 * \file selector_stage.cpp
 * \brief Selector stage instantiate.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "selector_stage.hpp"
#include "selector_stage_def.hpp"
#include "../config.hpp"

namespace client { namespace parser
{
   BOOST_SPIRIT_INSTANTIATE(selector_stage_type, iterator_type, context_type);
}}

