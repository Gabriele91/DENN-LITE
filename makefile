#Input
MKDIR_P       ?=mkdir -p
COMPILER      ?=g++
TOP           ?=$(shell pwd)

#program name
S_DIR  = $(TOP)/
S_INC  = $(TOP)/
O_DEBUG_DIR    = $(TOP)/Debug/obj
O_RELEASE_DIR  = $(TOP)/Release/obj
O_DEBUG_PROG   = $(TOP)/Debug/DENN
O_RELEASE_PROG = $(TOP)/Release/DENN

#global include
DIPS_INCLUDE = $(TOP)/dips/include/

# C++ files
SOURCE_FILES = $(S_DIR)/main.cpp $(S_DIR)/TicksTime.cpp $(S_DIR)/RandomIndices.cpp
SOURCE_DEBUG_OBJS = $(addprefix $(O_DEBUG_DIR)/,$(notdir $(SOURCE_FILES:.cpp=.o)))
SOURCE_RELEASE_OBJS = $(addprefix $(O_RELEASE_DIR)/,$(notdir $(SOURCE_FILES:.cpp=.o)))

# C FLAGS
C_FLAGS = -fPIC -D_FORCE_INLINES
# CPP FLAGS
CC_FLAGS = -lstdc++ -std=c++14 -I $(DIPS_INCLUDE)
# RELEASE_FLAGS
RELEASE_FLAGS = -O3
# DEBUG_FLAGS
DEBUG_FLAGS = -g -D_DEBUG -Wall 
# Linker
LDFLAGS += -lz -lm -lutil 

# Linux flags
ifeq ($(shell uname -s),Linux)
# too slow -fopenmp 
C_FLAGS += -lpthread -pthread 
endif

# MacOS flags
ifeq ($(shell uname -s),Darwin)
#No OpenMP 
RELEASE_FLAGS += -march=native
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

all: directories debug release

directories: ${O_DEBUG_DIR} ${O_RELEASE_DIR}

rebuild: clean directories debug release

rebuild_debug: clean_debug directories debug

rebuild_release: clean_release directories release

debug: directories $(SOURCE_DEBUG_OBJS)
	$(COMPILER) $(C_FLAGS) $(CC_FLAGS) $(SOURCE_DEBUG_OBJS) $(LDFLAGS) -o $(O_DEBUG_PROG)
	
release: directories $(SOURCE_RELEASE_OBJS)
	$(COMPILER) $(C_FLAGS) $(CC_FLAGS) $(SOURCE_RELEASE_OBJS) $(LDFLAGS) -o $(O_RELEASE_PROG)

# makedir
${O_DEBUG_DIR}:
	$(call colorecho,$(COLOR_GREEN),"[ Create $(O_DEBUG_DIR) directory ]")
	@${MKDIR_P} ${O_DEBUG_DIR}

# makedir
${O_RELEASE_DIR}:
	$(call colorecho,$(COLOR_GREEN),"[ Create $(O_RELEASE_DIR) directory ]")
	@${MKDIR_P} ${O_RELEASE_DIR}

$(O_DEBUG_DIR)/%.o: $(S_DIR)/%.cpp
	$(call colorecho,$(COLOR_GREEN),"[ Make debug object $(@) ]")
	$(COMPILER) $(C_FLAGS) $(CC_FLAGS) $(DEBUG_FLAGS) -c $< -o $@

$(O_RELEASE_DIR)/%.o: $(S_DIR)/%.cpp
	$(call colorecho,$(COLOR_GREEN),"[ Make release object $(@) ]")
	$(COMPILER) $(C_FLAGS) $(CC_FLAGS) $(RELEASE_FLAGS) -c $< -o $@

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
