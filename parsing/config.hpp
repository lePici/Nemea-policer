
/**
 * \file config.hpp
 * \brief Configure context of the parser.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3CONFIG_HPP)
#define BOOST_SPIRIT_X3_CONFIG_HPP

#include <boost/spirit/home/x3.hpp>
#include "error_handler.hpp"

namespace client {
   namespace parser {

      typedef std::string::const_iterator iterator_type;

      /**
       * \brief Phrase parse context for parser.
       */
      typedef x3::phrase_parse_context<x3::ascii::space_type>::type phrase_context_type;

      /**
       * \brief Our error handling.
       */
      typedef error_handler<iterator_type> error_handler_type;

      typedef x3::context<
              error_handler_tag
            , std::reference_wrapper<error_handler_type>
            , phrase_context_type>
            context_type;
}}
#endif
