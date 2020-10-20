CPP=g++
CC=gcc

# -MMD -> not wish to include system headers
# -MP  -> this suppresses the errors that would otherwise occur when a header file is deleted or renamed
WnoFLAGS=-Wno-unused-parameter -Wno-unused-const-variable -Wno-format-truncation -Wno-implicit-function-declaration

ifeq ($(MODE),perf)
    CPPFLAGS=-std=c++14 -Wall -Wextra -DMEASURE -O3
    CCFLAGS=-Wall -Wextra -O3
else ifeq ($(MODE),devel)
    CPPFLAGS=-std=c++14 -Wall -Wextra -DDEVEL -MMD -MP -g -O0
    CCFLAGS=-Wall -Wextra -MMD -MP -g
else
    CPPFLAGS=-std=c++14 -Wall -Wextra -O3
    CCFLAGS=-Wall -Wextra -O3
endif

CPPFLAGS +=-isystem$(CURDIR)/boost

LIBS=-lnemea-common -lunirec -ltrap -pthread

# generate fields.h and fields.c
ur_processor=ur_processor.sh

# replace .cpp to .o by directory name parameter
define generate_OBJ
	$(patsubst %.cpp, %.o, $(wildcard $(1)/*.cpp))
endef

# clean *.o and *.d files by *.o files
define clean_f
	rm -f $(addsuffix /*.o, $(shell echo "$(1)"  | tr " " "\n" | xargs dirname | sort | uniq | tr "\n" " "))
	rm -f $(addsuffix /*.d, $(shell echo "$(1)"  | tr " " "\n" | xargs dirname | sort | uniq | tr "\n" " "))
endef

root_DIR=.
root_OBJ=$(call generate_OBJ,$(root_DIR))

parsing_DIR=$(root_DIR)/parsing
parsing_OBJ=$(call generate_OBJ,$(parsing_DIR))

rules_DIR=$(parsing_DIR)/rules_def
rules_OBJ_FILES=selector_stage.o group_filter_stage.o aggregator_stage.o window_stage.o grouper_stage.o filter_stage.o main_branches.o
rules_OBJ=$(addprefix $(rules_DIR)/, $(rules_OBJ_FILES))

ast_DIR=$(parsing_DIR)/ast
ast_OBJ=$(call generate_OBJ,$(ast_DIR))

pars_filter_DIR=$(ast_DIR)/parse_filter_bodies
pars_filter_OBJ_FILES=pars_gram.o pars_lex.o pars_ffilter.o
pars_filter_OBJ=$(addprefix $(pars_filter_DIR)/, $(pars_filter_OBJ_FILES))
pars_filter_REM_FILES=ffilter_gram.h ffilter_gram.c ffilter_lex.h ffilter_lex.c
pars_filter_REM=$(addprefix $(pars_filter_DIR)/, $(pars_filter_REM_FILES))

backend_DIR=$(root_DIR)/backend
backend_OBJ=$(call generate_OBJ,$(backend_DIR))

fields_DIR=$(backend_DIR)
fields_OBJ=$(fields_DIR)/fields.o
fields_REM_FILES=fields.h fields.c
fields_REM=$(addprefix $(fields_DIR)/, $(fields_REM_FILES))

filter_DIR=$(backend_DIR)/filter

ffilter_gram_DIR=$(filter_DIR)
ffilter_gram_OBJ_FILES=pars_gram.o pars_lex.o
ffilter_gram_OBJ=$(addprefix $(ffilter_gram_DIR)/, $(ffilter_gram_OBJ_FILES))
ffilter_gram_REM_FILES=ffilter_gram.h ffilter_gram.c ffilter_lex.h ffilter_lex.c
ffilter_gram_REM=$(addprefix $(ffilter_gram_DIR)/, $(ffilter_gram_REM_FILES))

aggregator_DIR=$(backend_DIR)/aggregator
aggregator_OBJ=$(call generate_OBJ,$(aggregator_DIR))

selector_DIR=$(backend_DIR)/selector
selector_OBJ=$(call generate_OBJ,$(selector_DIR))

OBJ=$(root_OBJ) \
    $(parsing_OBJ) \
    $(rules_OBJ) \
    $(ast_OBJ) \
    $(pars_filter_OBJ) \
    $(fields_OBJ) \
    $(backend_OBJ) \
    $(ffilter_gram_OBJ) \
    $(filter_DIR)/fcore.o \
    $(filter_DIR)/ffilter.o \
    $(filter_DIR)/filter.o \
    $(aggregator_OBJ) \
    $(selector_OBJ)

REM=$(pars_filter_REM) \
    $(fields_REM) \
    $(ffilter_gram_REM) \

EXE=policer

$(EXE): $(OBJ)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJ) $(LIBS)

$(root_OBJ): $(root_DIR)/%.o : $(root_DIR)/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(parsing_OBJ): $(parsing_DIR)/%.o : $(parsing_DIR)/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(rules_OBJ): $(rules_DIR)/%.o : $(rules_DIR)/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(ast_OBJ): $(ast_DIR)/%.o : $(ast_DIR)/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(pars_filter_DIR)/pars_gram.o: $(pars_filter_DIR)/ffilter_gram.y
$(pars_filter_DIR)/pars_gram.o:
	cd $(pars_filter_DIR) && \
	bison -Wno-deprecated -l -o ffilter_gram.c --defines=ffilter_gram.h ffilter_gram.y && \
	$(CC) $(CCFLAGS) $(WnoFLAGS) -c ffilter_gram.c -o pars_gram.o && \
	cd -

$(pars_filter_DIR)/pars_lex.o: $(pars_filter_DIR)/ffilter_lex.l
$(pars_filter_DIR)/pars_lex.o:
	cd $(pars_filter_DIR) && \
	flex -L -o ffilter_lex.c --header=ffilter_lex.h ffilter_lex.l && \
	mv lex.yy.c ffilter_lex.c && \
	$(CC) $(CCFLAGS) -c ffilter_lex.c -o pars_lex.o && \
	cd -

$(pars_filter_DIR)/pars_ffilter.o:
	cd $(pars_filter_DIR) && \
	$(CC) $(CCFLAGS) $(WnoFLAGS) -c ffilter.c -o pars_ffilter.o && \
	cd -

$(backend_OBJ): $(backend_DIR)/%.o : $(backend_DIR)/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(fields_OBJ): $(backend_DIR)/unirec_template.cpp
	$(ur_processor) -i $(backend_DIR) -o $(backend_DIR)
	$(CC) $(CCFLAGS) -c $(fields_DIR)/fields.c -o $(fields_OBJ)

$(ffilter_gram_DIR)/pars_gram.o: $(ffilter_gram_DIR)/ffilter_gram.y
$(ffilter_gram_DIR)/pars_gram.o:
	cd $(ffilter_gram_DIR) && \
	bison -Wno-deprecated -l -o ffilter_gram.c --defines=ffilter_gram.h ffilter_gram.y && \
	$(CC) $(CCFLAGS) $(WnoFLAGS) -c ffilter_gram.c -o pars_gram.o && \
	cd -

$(ffilter_gram_DIR)/pars_lex.o: $(ffilter_gram_DIR)/ffilter_lex.l
$(ffilter_gram_DIR)/pars_lex.o:
	cd $(ffilter_gram_DIR) && \
	flex -L -o ffilter_lex.c --header=ffilter_lex.h ffilter_lex.l && \
	mv lex.yy.c ffilter_lex.c && \
	$(CC) $(CCFLAGS) -c ffilter_lex.c -o pars_lex.o && \
	cd -

$(filter_DIR)/fcore.o:
	$(CC) $(CCFLAGS) -w -c $(filter_DIR)/fcore.c -o $(filter_DIR)/fcore.o

$(filter_DIR)/ffilter.o:
	$(CC) $(CCFLAGS) -w -c $(filter_DIR)/ffilter.c -o $(filter_DIR)/ffilter.o

$(filter_DIR)/filter.o:
	$(CPP) $(CPPFLAGS) -c $(filter_DIR)/filter.cpp -o $(filter_DIR)/filter.o

$(aggregator_OBJ): $(aggregator_DIR)/%.o : $(aggregator_DIR)/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(selector_OBJ): $(selector_DIR)/%.o : $(selector_DIR)/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	$(call clean_f,$(OBJ))
	rm -f $(REM)
	rm -f $(EXE)

# include dependency files + rename suffix .o to .d
-include $(OBJ:%.o=%.d)

