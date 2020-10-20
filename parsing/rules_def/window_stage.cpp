/**
 * \file window_stage.cpp
 * \brief Window stage instantiate.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "window_stage.hpp"
#include "window_stage_def.hpp"
#include "../config.hpp"

namespace client { namespace parser
{
   BOOST_SPIRIT_INSTANTIATE(window_stage_type, iterator_type, context_type);
}}
