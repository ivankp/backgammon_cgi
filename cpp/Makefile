SHELL := bash
CXX := g++
CPPFLAGS := -std=c++17 -Iinclude
CXXFLAGS := -Wall -O3 -flto -fmax-errors=3 $(CPPFLAGS)
# CXXFLAGS := -Wall -g -fmax-errors=3 $(CPPFLAGS)
LDFLAGS := # -Wl,-rpath=./lib
LDLIBS :=

BIN := ../cgi
BLD := .build
EXT := .cc

VPATH = $(BLD)

.PHONY: all clean

SRCS := $(shell find -L src -type f -name '*$(EXT)')
DEPS := $(patsubst src/%$(EXT),$(BLD)/%.d,$(SRCS))

GREP_EXES := grep -rl '^ *int \+main *(' src --include='*$(EXT)'
EXES := $(patsubst src%$(EXT),$(BIN)%, $(shell $(GREP_EXES)))

ifeq (0, $(words $(findstring $(MAKECMDGOALS), clean)))

all: $(EXES)

-include $(DEPS)

# -------------------------------------------------------------------
L_table := -lsqlite3
L_move := -lsqlite3
L_load_game := -lsqlite3

$(BIN)/move: \
  $(BLD)/cgi.o

$(BIN)/load_game: \
  $(BLD)/cgi.o \
  $(BLD)/encode.o

$(BIN)/pos: \
  $(BLD)/encode.o
# -------------------------------------------------------------------

$(DEPS): $(BLD)/%.d: src/%$(EXT)
	@mkdir -pv $(dir $@)
	$(CXX) $(CPPFLAGS) $(C_$*) -MM -MT '$(@:.d=.o)' $< -MF $@

$(BLD)/%.o:
	@mkdir -pv $(dir $@)
	$(CXX) $(CXXFLAGS) $(C_$*) -c $(filter %$(EXT),$^) -o $@

$(BIN)/%: $(BLD)/%.o
	@mkdir -pv $(dir $@)
	$(CXX) $(LDFLAGS) $(filter %.o,$^) -o $@ $(LDLIBS) $(L_$*)

endif

clean:
	@rm -rfv $(BLD) $(EXES)

