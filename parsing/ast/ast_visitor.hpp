
/**
 * \file ast_visitor.hpp
 * \brief Crawling through abstract syntax tree.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#if !defined(BOOST_SPIRIT_X3_AST_VISITOR_HPP)
#define BOOST_SPIRIT_X3_AST_VISITOR_HPP

#include "ast.hpp"
#include <iostream>

namespace client { namespace ast
{

   class Visitor_T;
   class Printer_T;
   
   /**
    * \brief Interface for browsing through Abstract syntax tree.
    */
   class Ast_interface
   {
      public:

      virtual void operator()(main_branches const& mb) = 0;
      virtual void operator()(filter_stage const& fs) = 0;
      virtual void operator()(grouper_stage const& gs) = 0;
      virtual void operator()(window_stage const& ws) = 0;
      virtual void operator()(aggregator_stage const& as) = 0;
      virtual void operator()(group_filter_stage const& gfs) = 0;
      virtual void operator()(selector_stage const& ss) = 0;
   
      virtual void operator()(main_branch_vec const& mbv) = 0;
      virtual void operator()(main_branch const& mb) = 0;
      virtual void operator()(stage const& stg) = 0;
   
      virtual void operator()(fi_stage_succ const& fss) = 0;
   
      virtual void operator()(grouper_keys const&) = 0;
      virtual void operator()(grouper_case const&) = 0;
      virtual void operator()(gr_stage_succ const& gss) = 0;
   
      virtual void operator()(wi_stage_succ const& wss) = 0;
   
      virtual void operator()(aggr_assignment const& as) = 0;
      virtual void operator()(aggr_func_with_param_s const&) = 0;
      virtual void operator()(aggr_func_without_param const&) = 0;
      virtual void operator()(ag_stage_succ const& ass) = 0;
   
      virtual void operator()(sel_assignment const& sa) = 0;
      virtual void operator()(sel_expression const&) = 0;
      virtual void operator()(sel_operation const&) = 0;
      virtual void operator()(nil const&) = 0;
      virtual void operator()(double const&) = 0;
      virtual void operator()(sel_variable const&) = 0;
      virtual void operator()(sel_unary const&) = 0;
      virtual void operator()(std::string const&) = 0;
      virtual void operator()(unirec const&) = 0;
   
      virtual void operator()(branch_after_fi_s const& param) = 0;
      virtual void operator()(branch_after_gr_s const& param) = 0;
      virtual void operator()(branch_after_wi_s const& param) = 0;
      virtual void operator()(branch_after_ag_s const& param) = 0;
      virtual void operator()(branch_after_gf_s const& param) = 0;
   
      virtual void operator()(branch_after_fi const& param) = 0;
      virtual void operator()(branch_after_gr const& param) = 0;
      virtual void operator()(branch_after_wi const& param) = 0;
      virtual void operator()(branch_after_ag const& param) = 0;
      virtual void operator()(branch_after_gf const& param) = 0;
   
      virtual void operator()(boost::recursive_wrapper<main_branch_vec> const& param) = 0; 
      virtual void operator()(boost::recursive_wrapper<branch_after_fi> const& param) = 0;
      virtual void operator()(boost::recursive_wrapper<branch_after_gr> const& param) = 0;
      virtual void operator()(boost::recursive_wrapper<branch_after_wi> const& param) = 0;
      virtual void operator()(boost::recursive_wrapper<branch_after_ag> const& param) = 0;
      virtual void operator()(boost::recursive_wrapper<branch_after_gf> const& param) = 0;
   };
   
   template<typename T>
   class Choose_Visitor_template: public Ast_interface {};
   
   /**
    * \brief Default template for classic usage of Visitor.
    */
   template<>
   class Choose_Visitor_template<class Visitor_T>: public Ast_interface
   {
      public:
   
      /**
       * \brief Browsing structure of format "fist"-"rest" (etc. main_branches in ast.hpp).
       */
      template <class Unpack>
      void unpack_structure(Unpack st)
      {
         (*this)(st.first);
         for (auto const& item: st.rest) {
            (*this)(item);
         }
      }
   
      /**
       * \brief Continue browsing.
       */
      template <class Branch>
      void branch_body_after_stage(Branch st)
      {
         boost::apply_visitor((*this), st.succ);
      }
   };
   

   /**
    * \brief Visitor template for Printer class.
    */
   template<>
   class Choose_Visitor_template<class Printer_T>: public Ast_interface
   {
      public:
   
      int const tabsize = 4;
   
      /**
       * \brief Tabulator using.
       */
      void tab(int update=0)
      {
         using namespace std;
   
         static int indent = 0;
         indent += update;
         for (int i = 0; i < indent; ++i) {
             cout << ' ';
         }
      }
   
      /**
       * \brief Printing branch.
       */
      template <class Branch>
      void branch_body_after_stage(Branch st)
      {
         using namespace std;
   
         cout << "branch ";
         cout << st.name << " ";
         cout << "{" << endl;
         tab(tabsize);
         boost::apply_visitor((*this), st.succ);
         tab(tabsize * -1);
         cout << "}" << endl;
      }
   
      /**
       * \brief Maintaining alignment while printing items.
       */
      template <class Unpack>
      void unpack_structure(Unpack st)
      {
         (*this)(st.first);
         for (auto const& item: st.rest) {
            tab(0);
            (*this)(item);
         }
      }
   
      /**
       * \brief Separate objects when printing.
       */
      template <class Unpack>
      void unpack_structure(Unpack st, std::string delimiter)
      {
         using namespace std;
   
         (*this)(st.first);
         for (auto const& item: st.rest) {
            cout << delimiter;
            (*this)(item);
         }
      }
   };
   
   /**
    * \brief All these functions allow to navigate through the AST.
    * \detail if you want to do some operations on the AST,
    *    inherit these methods and override some of them.
    */
   template<typename T>
   struct Visitor : public Choose_Visitor_template<T>
   {
      virtual void operator()(main_branches const& mb)
      {
         (*this)(mb.first); (*this)(mb.rest);
      }

      virtual void operator()(filter_stage const& fs)
      {
         boost::apply_visitor((*this), fs.succ);
      }

      virtual void operator()(grouper_stage const& gs)
      {
         boost::apply_visitor((*this), gs.body.type);
         boost::apply_visitor((*this), gs.succ);
      }

      virtual void operator()(window_stage const& ws)
      {
         boost::apply_visitor((*this), ws.succ);
      }

      virtual void operator()(aggregator_stage const& as)
      {
         Choose_Visitor_template<T>::unpack_structure(as.body);
         boost::apply_visitor((*this), as.succ);
      }

      virtual void operator()(group_filter_stage const& gfs)
      {
         boost::apply_visitor((*this), gfs.succ);
      }

      virtual void operator()(selector_stage const& ss)
      {
         Choose_Visitor_template<T>::unpack_structure(ss.body);
      }
   
      virtual void operator()(main_branch_vec const& mbv)
      {
         for (auto const& item: mbv) {
            (*this)(item);
         }
      }
      
      virtual void operator()(main_branch const& mb)
      {
         boost::apply_visitor((*this), mb.mbm);
      }

      virtual void operator()(stage const& stg)
      {
         boost::apply_visitor((*this), stg);
      }
   
      virtual void operator()(fi_stage_succ const& fss)
      {
         boost::apply_visitor((*this), fss);
      }
   
      virtual void operator()(grouper_keys const& gk)
      {
         Choose_Visitor_template<T>::unpack_structure(gk);
      }

      virtual void operator()(grouper_case const&){};

      virtual void operator()(gr_stage_succ const& gss)
      {
         boost::apply_visitor((*this), gss);
      }
   
      virtual void operator()(wi_stage_succ const& wss)
      {
         boost::apply_visitor((*this), wss);
      }
   
      virtual void operator()(aggr_assignment const& as)
      {
         boost::apply_visitor((*this), as.func_type);
      }

      virtual void operator()(aggr_func_with_param_s const& agf)
      {
         (*this)(agf.param);
      }

      virtual void operator()(aggr_func_without_param const&){};

      virtual void operator()(ag_stage_succ const& ass)
      {
         boost::apply_visitor((*this), ass);
      }
   
      virtual void operator()(sel_assignment const& sa)
      {
         boost::apply_visitor((*this), sa.output);
      }

      virtual void operator()(sel_expression const& se)
      {
         boost::apply_visitor((*this), se.first);
         for(auto const& item: se.rest)
            (*this)(item);
      }

      virtual void operator()(sel_operation const& so)
      {
         boost::apply_visitor((*this), so.operand_);
      }

      virtual void operator()(sel_variable const& sv)
      {
         boost::apply_visitor((*this), sv);
      }

      virtual void operator()(nil const&){};
      virtual void operator()(double const&){};
      virtual void operator()(sel_unary const&){};
      virtual void operator()(std::string const&){};
      virtual void operator()(unirec const&){};
   
      virtual void operator()(branch_after_fi_s const& param)
          {Choose_Visitor_template<T>::unpack_structure(param);};
      virtual void operator()(branch_after_gr_s const& param)
          {Choose_Visitor_template<T>::unpack_structure(param);};
      virtual void operator()(branch_after_wi_s const& param)
          {Choose_Visitor_template<T>::unpack_structure(param);};
      virtual void operator()(branch_after_ag_s const& param)
          {Choose_Visitor_template<T>::unpack_structure(param);};
      virtual void operator()(branch_after_gf_s const& param)
          {Choose_Visitor_template<T>::unpack_structure(param);};
   
      virtual void operator()(branch_after_fi const& param)
          {Choose_Visitor_template<T>::branch_body_after_stage(param);};
      virtual void operator()(branch_after_gr const& param)
          {Choose_Visitor_template<T>::branch_body_after_stage(param);};
      virtual void operator()(branch_after_wi const& param)
          {Choose_Visitor_template<T>::branch_body_after_stage(param);};
      virtual void operator()(branch_after_ag const& param)
          {Choose_Visitor_template<T>::branch_body_after_stage(param);};
      virtual void operator()(branch_after_gf const& param)
          {Choose_Visitor_template<T>::branch_body_after_stage(param);};
   
      virtual void operator()(boost::recursive_wrapper<main_branch_vec> const& param)
          {(*this)(param);};
      virtual void operator()(boost::recursive_wrapper<branch_after_fi> const& param)
          {(*this)(param);};
      virtual void operator()(boost::recursive_wrapper<branch_after_gr> const& param)
          {(*this)(param);};
      virtual void operator()(boost::recursive_wrapper<branch_after_wi> const& param)
          {(*this)(param);};
      virtual void operator()(boost::recursive_wrapper<branch_after_ag> const& param)
          {(*this)(param);};
      virtual void operator()(boost::recursive_wrapper<branch_after_gf> const& param)
          {(*this)(param);};
   
      virtual ~Visitor(){};
   };
}}

#endif
