# ------------------ CREDIT -----------------

# Original makefile made by Jeisson Hidalgo
# From Universidad de Costa Rica, ECCI
# C/C++ Makefile v2.4.1 2022-Jun-04 CC-BY 4.0

# This is a modified, reduced version of it
# It is yet very early and prone to bugs,
# so check out the original at his site!

# https://jeisson.ecci.ucr.ac.cr/

# ------------------ DEBUG ------------------

# Print variable values invoking, the command
# make print-VARNAME, which prints VARNAME'S
# value

print-% : ;
	@echo $* = $($*)

# ------------------ CONFIG -----------------

# C and C++ compilers
CC = gcc
CSTD = -std=c11

XC = g++
XSTD = -std=c++20

# Flags and definitions
DEFS =

COMPILER_FLAGS =
LINKER_FLAGS =

DEFINED =$(DEFS:%=-D%)
C_FLAGS =$(strip $(CSTD) $(DEFINED) $(COMPILER_FLAGS))
X_FLAGS =$(strip $(XSTD) $(DEFINED) $(COMPILER_FLAGS))
L_FLAGS =$(strip $(DEFINED) $(LINKER_FLAGS))

# Dependencies
LIB_DIR =
INC_DIR =

SRC_DIR =src
SRC_SUBDIRS =$(shell find -L $(SRC_DIR) -type d)

# Main ouputs
OBJ_DIR =obj
BIN_DIR =bin

APP_NAME =$(shell basename $(shell pwd))
EXE_APP =$(BIN_DIR)/$(APP_NAME)
ifeq ($(OS), Windows_NT)
	EXE_APP := $(EXE_APP).exe
endif

EXE_ARGS =

# ------------------ FILES ------------------

# Headers, sources and object files
C_HEADERS =$(wildcard $(SRC_SUBDIRS:%=%*.h))
C_MODULES =$(wildcard $(SRC_SUBDIRS:%=%*.c))
C_OBJS =$(C_MODULES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

X_HEADERS =$(wildcard $(SRC_SUBDIRS:%=%/*.hpp))
X_MODULES =$(wildcard $(SRC_SUBDIRS:%=%/*.cpp))
X_OBJS =$(X_MODULES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

OBJS =$(strip $(C_OBJS) $(X_OBJS))
INCLUDED =$(INC_DIR:%=-I%) $(SRC_SUBDIRS:%=-I%)
LINKED =$(if $(LIB_DIR), -L$(LIB_DIR),)

# Dependency information files for helping out make
MAKE_DEPENDENCIES =$(OBJS:%.o=%.d)

# Ignore bin, object and documentation directories
IGNORED =$(BIN_DIR) $(OBJ_DIR)

# ------------------ RULES ------------------

# Include Make dependency information files
-include $(MAKE_DEPENDENCIES)

# Decide linker on whether there's C sources
# present, or not
LINKER =$(if $(C_MODULES), $(CC), $(XC))

# Expand every following command's pre-requisites
# a second time. 
# Used for targets and directory which parameters
# are solved at the second expansion
.SECONDEXPANSION:

# Don't delete directories that were created
# as result of missing in the first place
.PRECIOUS: %/.

# Create directories if missing
%/.:
	mkdir -p $(dir $@)

# ------------------ TARGETS ----------------

# Build application and other files
default: $(EXE_APP)

# Statitcally link object files into the
# application, without minding its parent
# directory updating
# The linker was decided upon beforehand
$(EXE_APP): $(OBJS) | $$(@D)/.
	$(LINKER) $(LINKER_FLAGS) $(INCLUDED) $^ -o $@ $(LINKED)

# Compile c source files, without minding 
# their parent directory updating
# Also create dependency information files
# to help Make
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $$(@D)/.
	$(CC) -c $(C_FLAGS) $(INCLUDED) -MMD $< -o $@

# Same thing, but for cpp source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $$(@D)/.
	$(XC) -c $(X_FLAGS) $(INCLUDED) -MMD $< -o $@

# Run application. Also, prompt its construction
# if absent
run: $(EXE_APP)
	$(strip $(EXE_APP) $(EXE_ARGS))

# Mark these targets as mere utilities.
# These do not represent recipes
.PHONY: clean help

# CLean all files to be ignored on repo push
clean:
	rm -rf $(IGNORED)

# Display brief info about use
help:
	@echo "help - Display this information"
	@echo "run - Run application. Build if missing"
	@echo "clean - Clean bin/, obj/ folders and their contents"
	@echo "default - Build application"
	@echo "print-VAR - Print VAR's evaluated contents on the Makefile (debug)"
