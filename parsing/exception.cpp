
/**
 * \file exception.cpp
 * \brief Syntax or semantic error message.
 * \author Adam Piecek <adam.piecek@gnj.cz>
 * \date 2020
 */

#include "exception.hpp"
#include <string.h>

using namespace ProgramError;

Syntactic::Syntactic(filter_bodies_tag, const char *branchPath, const char *name)
{
   strcat(msg_body, msg_prefix);
   char bodyPart1[] = "In the branch path \'";

   strcat(msg_body, bodyPart1);
   strcat(msg_body, branchPath);
   char bodyPart2[] = "\' is \'";

   strcat(msg_body, bodyPart2);
   strcat(msg_body, name);
   msg_body[strlen(msg_body)] = '\'';

   retCode = Filter;
}

Semantic::Semantic(check_branch_names_tag, const char *branchPath, const char *name)
{
   strcat(msg_body, msg_prefix);
   char bodyPart1[] = "In the branch path \'";

   strcat(msg_body, bodyPart1);
   strcat(msg_body, branchPath);
   char bodyPart2[] = "\' is the duplicate branch name \'";

   strcat(msg_body, bodyPart2);
   strcat(msg_body, name);
   msg_body[strlen(msg_body)] = '\'';

   retCode = BranchNames;
}

Semantic::Semantic(check_seq_stages_tag, const char *mainBranch, const char *stagesSeq)
{
   strcat(msg_body, msg_prefix);
   char bodyPart1[] = "Main branch \'";

   strcat(msg_body, bodyPart1);
   strcat(msg_body, mainBranch);
   char bodyPart2[] = "\' contains a bad sequence of stages:\n\t\'";

   strcat(msg_body, bodyPart2);
   strcat(msg_body, stagesSeq);
   char bodyPart3[] = "\' is not allowed";

   strcat(msg_body, bodyPart3);

   retCode = StagesComb;
}

Semantic::Semantic(check_grouper_tag1, const char *branchPath, const char *name)
{
   strcat(msg_body, msg_prefix);
   char bodyPart1[] = "In the branch path \'";

   strcat(msg_body, bodyPart1);
   strcat(msg_body, branchPath);
   char bodyPart2[] = "\' it is not supported to have a grouper key \'";

   strcat(msg_body, bodyPart2);
   strcat(msg_body, name);
   msg_body[strlen(msg_body)] = '\'';

   retCode = GrouperKey1;
}

Semantic::Semantic(check_grouper_tag2, const char *branchPath, const char *name)
{
   strcat(msg_body, msg_prefix);
   char bodyPart1[] = "In the branch path \'";

   strcat(msg_body, bodyPart1);
   strcat(msg_body, branchPath);
   char bodyPart2[] = "\' it is not supported to have an aggregate for the grouper key \'";

   strcat(msg_body, bodyPart2);
   strcat(msg_body, name);
   char bodyPart3[] = "\' in the same main branch";

   strcat(msg_body, bodyPart3);

   retCode = GrouperKey2;
}

Semantic::Semantic(check_filter_varname_tag, const char *branchPath, const char *name)
{
   strcat(msg_body, msg_prefix);
   char bodyPart1[] = "In the branch path \'";

   strcat(msg_body, bodyPart1);
   strcat(msg_body, branchPath);
   char bodyPart2[] = "\' is a variable with name \'";

   strcat(msg_body, bodyPart2);
   strcat(msg_body, name);
   char bodyPart3[] =
      "\' that does not defined in appropriate aggregator stage and at the same time it is not unirec";
   strcat(msg_body, bodyPart3);

   retCode = FilterVar;
}

Semantic::Semantic(check_selector_tag, const char *branchPath, const char *name)
{
   strcat(msg_body, msg_prefix);
   char bodyPart1[] = "In the branch path \'";

   strcat(msg_body, bodyPart1);
   strcat(msg_body, branchPath);
   char bodyPart2[] = "\' is parameter with name \'";

   strcat(msg_body, bodyPart2);
   strcat(msg_body, name);
   char bodyPart3[] =
      "\'\n                but is not possible to provide a meaningful result due to an influence of the grouper or aggregator stage";
   strcat(msg_body, bodyPart3);

   retCode = Selector;
}
