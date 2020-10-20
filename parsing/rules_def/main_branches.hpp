/**
 * \file main_branches.hpp
 * \brief Interface for main branches grammar rules.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_MAIN_BRANCHES_HPP)
#define BOOST_SPIRIT_X3_MAIN_BRANCHES_HPP

#include <boost/spirit/home/x3.hpp>
#include "../ast/ast.hpp"
#include "../common.hpp"
#include <vector>
#include <string>

namespace client
{
   namespace x3 = boost::spirit::x3;
   namespace parser
   {
      struct main_branches_class;
      typedef x3::rule<main_branches_class, ast::main_branches> main_branches_type;
      typedef main_branches_type::id main_branches_id;
      BOOST_SPIRIT_DECLARE(main_branches_type);
   }

   parser::main_branches_type const& main_branches();
}

#endif
