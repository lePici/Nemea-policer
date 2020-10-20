/**
 * \file main_branches.cpp
 * \brief Main branches instantiate.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "main_branches_def.hpp"
#include "../config.hpp"

namespace client { namespace parser
{
   BOOST_SPIRIT_INSTANTIATE(main_branches_type, iterator_type, context_type);
}}

