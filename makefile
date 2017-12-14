#Input
MKDIR_P         ?=mkdir -p
COMPILER        ?=g++
TOP             ?=$(shell pwd)
SCALAR	        ?= FLOAT
HAVE_TERM       := $(shell echo $$TERM)
VERION_COMPILER := `$(COMPILER) --version`
#undef to none (linux)
ifndef HAVE_TERM
	HAVE_TERM = none
endif
#dump to none (macOS)
ifeq ($(HAVE_TERM),dumb)
	HAVE_TERM = none
endif
#program name
S_DIR  = $(TOP)/source/
S_INC  = $(TOP)/include/

#global include
DIPS_INCLUDE = $(TOP)/dips/include/

# C++ files
SOURCE_FILES = $(wildcard $(S_DIR)/*.cpp) $(wildcard $(S_DIR)/*/*.cpp)
SOURCE_DEBUG_OBJS = $(addprefix $(O_DEBUG_DIR)/,$(notdir $(SOURCE_FILES:.cpp=.o)))
SOURCE_RELEASE_OBJS = $(addprefix $(O_RELEASE_DIR)/,$(notdir $(SOURCE_FILES:.cpp=.o)))

# C FLAGS
C_FLAGS = -fPIC -D_FORCE_INLINES
# CPP FLAGS
CC_FLAGS = -std=c++14 -I $(DIPS_INCLUDE) -I $(S_INC)
# RELEASE_FLAGS
RELEASE_FLAGS = -O3 -DNDEBUG -fomit-frame-pointer -Wno-unused-result
# DEBUG_FLAGS
DEBUG_FLAGS = -g -D_DEBUG -Wall 
# Linker
LDFLAGS += -lz -lm -lutil 

ifeq ($(SCALAR),FLOAT)
O_DEBUG_DIR    = $(TOP)/Debug/obj-float
O_RELEASE_DIR  = $(TOP)/Release/obj-float
O_DEBUG_PROG   = $(TOP)/Debug/DENN-float
O_RELEASE_PROG = $(TOP)/Release/DENN-float
C_FLAGS		  += -DUSE_FLOAT
else ifeq ($(SCALAR),DOUBLE)
O_DEBUG_DIR    = $(TOP)/Debug/obj-double
O_RELEASE_DIR  = $(TOP)/Release/obj-double
O_DEBUG_PROG   = $(TOP)/Debug/DENN-double
O_RELEASE_PROG = $(TOP)/Release/DENN-double
C_FLAGS		  += -DUSE_DOUBLE
else ifeq ($(SCALAR),LONG_DOUBLE)
O_DEBUG_DIR    = $(TOP)/Debug/obj-long-double
O_RELEASE_DIR  = $(TOP)/Release/obj-long-double
O_DEBUG_PROG   = $(TOP)/Debug/DENN-long-double
O_RELEASE_PROG = $(TOP)/Release/DENN-long-double
C_FLAGS		  += -DUSE_LONG_DOUBLE
else 
	$(error Set SCALAR=FLOAT|DOUBLE|LONG_DOUBLE)
endif


# Linux flags
ifeq ($(shell uname -s),Linux)
# too slow -fopenmp 
C_FLAGS += -pthread 
#no clang
ifneq ($(findstring clang,$(VERION_COMPILER)), clang)
# must to be a release mode (no checking)
# RELEASE_FLAGS += --enable-checking=release
# remove gcc warning (eigen)
C_FLAGS += -Wno-int-in-bool-context
# only linux + gcc
C_FLAGS += -lpthread
endif
endif


# MacOS flags
ifeq ($(shell uname -s),Darwin)
#No OpenMP 
RELEASE_FLAGS += -march=native
endif
##
# Support function for colored output
# Args:
#     - $(1) = Color Type
#     - $(2) = String to print
ifneq ($(HAVE_TERM),none)
define colorecho
	@tput setaf $(1)
	@echo $(2)
	@tput sgr0
endef
else
define colorecho
	@echo $(2)
endef
endif

# Color Types
COLOR_BLACK = 0
COLOR_RED = 1
COLOR_GREEN = 2
COLOR_YELLOW = 3
COLOR_BLUE = 4
COLOR_MAGENTA = 5
COLOR_CYAN = 6
COLOR_WHITE = 7

all: directories show_debug_flags debug release

directories: ${O_DEBUG_DIR} ${O_RELEASE_DIR}

rebuild: clean directories debug release

rebuild_debug: clean_debug debug

rebuild_release: clean_release release

debug: directories show_debug_flags $(SOURCE_DEBUG_OBJS)
	$(COMPILER) $(C_FLAGS) $(CC_FLAGS) $(SOURCE_DEBUG_OBJS) $(LDFLAGS) -o $(O_DEBUG_PROG)
	
release: directories show_release_flags $(SOURCE_RELEASE_OBJS)
	$(COMPILER) $(C_FLAGS) $(CC_FLAGS) $(SOURCE_RELEASE_OBJS) $(LDFLAGS) -o $(O_RELEASE_PROG)

# makedir
${O_DEBUG_DIR}:
	$(call colorecho,$(COLOR_CYAN),"[ Create $(O_DEBUG_DIR) directory ]")
	@${MKDIR_P} ${O_DEBUG_DIR}

# makedir
${O_RELEASE_DIR}:
	$(call colorecho,$(COLOR_CYAN),"[ Create $(O_RELEASE_DIR) directory ]")
	@${MKDIR_P} ${O_RELEASE_DIR}

show_debug_flags:
	$(call colorecho,$(COLOR_YELLOW),"[ Debug flags: $(C_FLAGS) $(CC_FLAGS) $(DEBUG_FLAGS) ]")

show_release_flags:
	$(call colorecho,$(COLOR_YELLOW),"[ Release flags: $(C_FLAGS) $(CC_FLAGS) $(RELEASE_FLAGS) ]")

$(O_DEBUG_DIR)/%.o: $(S_DIR)/%.cpp
	$(call colorecho,$(COLOR_GREEN),"[ Make debug object: $(@) ]")
	@$(COMPILER) $(C_FLAGS) $(CC_FLAGS) $(DEBUG_FLAGS) -c $< -o $@

$(O_RELEASE_DIR)/%.o: $(S_DIR)/%.cpp
	$(call colorecho,$(COLOR_GREEN),"[ Make release object: $(@) ]")
	@$(COMPILER) $(C_FLAGS) $(CC_FLAGS) $(RELEASE_FLAGS) -c $< -o $@

$(O_DEBUG_DIR)/%.o: $(S_DIR)/*/%.cpp
	$(call colorecho,$(COLOR_GREEN),"[ Make debug object: $(@) ]")
	@$(COMPILER) $(C_FLAGS) $(CC_FLAGS) $(DEBUG_FLAGS) -c $< -o $@

$(O_RELEASE_DIR)/%.o: $(S_DIR)/*/%.cpp
	$(call colorecho,$(COLOR_GREEN),"[ Make release object: $(@) ]")
	@$(COMPILER) $(C_FLAGS) $(CC_FLAGS) $(RELEASE_FLAGS) -c $< -o $@

# Clean
clean: clean_debug clean_release

clean_debug:
	$(call colorecho,$(COLOR_MAGENTA),"[ Delete debug obj files ]")
	@rm -f -R $(O_DEBUG_DIR)
	$(call colorecho,$(COLOR_MAGENTA),"[ Delete debug executable files ]")
	@rm -f $(O_DEBUG_PROG)
	
clean_release:
	$(call colorecho,$(COLOR_MAGENTA),"[ Delete release obj files ]")
	@rm -f -R $(O_RELEASE_DIR)
	$(call colorecho,$(COLOR_MAGENTA),"[ Delete release executable files ]")
	@rm -f $(O_RELEASE_PROG)
