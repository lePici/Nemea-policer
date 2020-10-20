
/**
 * \file error_handler.hpp
 * \brief Syntax error message due parsing.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_ERROR_HANDLER_HPP)
#define BOOST_SPIRIT_X3_ERROR_HANDLER_HPP

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

namespace client {
   namespace parser {

      namespace x3 = boost::spirit::x3;

      /**
       * \brief Our error handler.
       */
      template <typename Iterator> using error_handler = x3::error_handler<Iterator>;

      /**
       * \brief Tag used to get our error handler from the context.
       */
      using error_handler_tag = x3::error_handler_tag;

      struct error_handler_base {
 
      /**
       * \brief Exception can be handled by calling a handler with the context at which the parsing failed
       *    can be reported.
       * \details on_error is the counterpart of on_success. While on_success handlers
       *    are callback hooks to client code that are executed by the parser after a successful parse,
       *    on_error handlers are callback hooks to client code that are executed by the parser
       *    when an expectation_failure is thrown via the expect operator or directive.
       *    on_error handlers have access to the iterators, the context and the exception that was thrown. 
       */
         template <typename Iterator, typename Exception, typename Context>
         x3::error_handler_result on_error(
            // Iterator& first, Iterator const& last
            Iterator, Iterator, Exception const &x,
            Context const &context)
         {
            static bool once = false;
            if (once) {
               return x3::error_handler_result::fail;
            }
            once = true;
            std::string message = "Error! Expecting: " + x.which() + " here:";
            auto & error_handler = x3::get < error_handler_tag > (context).get();
            error_handler(x.where(), message);
            return x3::error_handler_result::accept;
         }
      };
   }
}

#endif
