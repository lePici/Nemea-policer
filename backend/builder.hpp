
/**
 * \file builder.hpp
 * \brief Allocation and deallocation of stages.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "interface.hpp"
#include "../parsing/ast/ast_variables.hpp"
#include "../parsing/inter_repr.hpp"
#include "program_arguments.hpp"

#include <iostream>
#include <string>
#include <vector>

class Builder_stage_base;

using builderVec = std::vector<Builder_stage_base*>;
using pipelineVec = std::vector<Stage_intf*>;

/**
 * \brief Class for manage Stage component. Every builder has his own Stage.
 * \details The builder manages the allocation and deallocation of his component, 
 *    calls component initialization and provides successors for the component in pipeline.
 *    Builders are interwined like in pipeline.
 */
class Builder_stage_base
{
protected:
   Stage_intf *my_stage = NULL; ///< Managed stage.
   std::string options;         ///< Parameters for managed stage.
   builderVec next_builders;    ///< Next immediate builders like in processing pipeline.

   /**
    * \brief Class for manage Stage component. Every builder has his own Stage.
    * \param[in] *ms managed stage.
    * \param[in] opt parameters for managed stage.
    * \param[in] nb vector of next immediate builders.
    */
   Builder_stage_base(Stage_intf *ms, std::string opt, builderVec nb): my_stage(ms), options(opt),
      next_builders(nb) {} 

public:

   /**
    * \brief Create Stage successors, initialize next builders and initialize own managed Stage.
    * \details One call causes the initialization of other builders as the avalanche effect.
    * \return 0 on success, otherwise a negative error value.
    */
   int init(void);

   /**
    * \return managed stage (my_stage).
    */
   Stage_intf* get_my_stage(void);

   virtual ~Builder_stage_base(void);
};

/**
 * \brief Class template for create binding with Builder and some Stage.
 * \details Example:
 * \code
 *    new Builder_stage<Selector> (options, builderVec());
 * \endcode
 */
template <class T> class Builder_stage: public Builder_stage_base
{
public:

   Builder_stage(std::string opt, builderVec nb): Builder_stage_base(new T, opt, nb) {}
};


namespace client { namespace ast {

/**
 * \brief Class that create and connect builders.
 *    This will create processing pipeline.
 * \details The main task is to create a pipeline. To do this, class uses an abstract syntactic tree.
 *    During that, it finds corresponding parameters for each stage.
 *    It also detects how stages are linked (successors).
 *    The remaining responsibility is left to the Builder_stage_base class.
 */
class Builder: public Variables
{
   Inter_repr *inter_repr;    ///< Information obtained during parsing.
   Program_arguments *config; ///< Arguments from command line.
   using builderStackT = std::vector<builderVec*>;
   builderStackT b_stack;     ///< For storage successors.
   builderVec root;           ///< For storing main branches.
   std::string gro_opt;       ///< Current option for Grouper stage.
   bool groDive = false;      ///< If Grouper stage is present in current branch.
   std::string win_opt;       ///< Current option for Window stage.
   std::string agg_opt;       ///< Current option for Aggregator stage.
   std::string sel_opt;       ///< Current option for Selector stage.
   bool selDive = false;      ///< If Selector stage is present in current branch.

   /**
    * \brief Auxiliary function for proper nesting to the branch.
    * \param[in] &mb main_brach structure from AST.
    */
   void unpack_main_branch(main_branch const &mb);

   /**
    * \return vector of main branches.
    */
   builderVec get_build_root(void);

   /**
    * \brief Rename Aggregator variables to unirec keywords.
    * \param[in] stage_body etc. Selector body string.
    * \param[in] *aliases structure of variable-keyword mapping for the Stage.
    * \param[in] body_id ID of the Stage.
    * \return new string with edited names.
    */
   std::string apply_aliases(std::string stage_body, varsT *aliases, int body_id);

public:

   /**
    * \param[in] ir Information obtained during parsing.
    * \param[in] c Arguments from command line.
    */
   Builder(Inter_repr *ir, Program_arguments *c): inter_repr(ir), config(c) {};

   /**
    * \brief Create processing pipeline and initialize each stage.
    * \return 0 on success, otherwise a negative error value.
    */
   int build(void);

   /**
    * \brief This function call after build(void) method.
    * \return main branches ready to processing records.
    */
   pipelineVec get_pipelineVec(void);

   ~Builder(void);

   using Check_branch_names::operator();
   // using Variables::operator();

   void operator() (main_branch const &mb) override;
   void operator() (filter_stage const &fs) override;
   void operator() (grouper_stage const &gs) override;
   void operator() (window_stage const &ws) override;
   void operator() (aggregator_stage const &as) override;
   void operator() (aggr_func_with_param_s const &agf) override;
   void operator() (aggr_func_without_param const &agf) override;
   void operator() (group_filter_stage const &gfs) override;
   void operator() (selector_stage const &ss) override;
   void operator() (sel_assignment const &sa) override;
   void operator() (std::string const &str) override;
   void operator() (unirec const &un) override;
};

}};
