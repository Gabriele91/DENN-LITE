#Input
MKDIR_P       ?=mkdir -p
COMPILER      ?=emcc
TOP           ?=$(shell pwd)
USE_ASM_JS    ?=no
#program name
S_DIR_EMSCRIPTEN  = $(TOP)
S_DIR          = $(TOP)/../
S_INC          = $(TOP)/../
O_DEBUG_DIR    = $(TOP)/Debug/obj
O_RELEASE_DIR  = $(TOP)/Release/obj
O_DEBUG_PROG   = $(TOP)/DENN.js
O_RELEASE_PROG = $(TOP)/DENN.js

#global include
DIPS_INCLUDE = $(TOP)/../dips/include/

# C++ files
SOURCE_FILES = $(S_DIR_EMSCRIPTEN)/main_web.cpp $(S_DIR)/TicksTime.cpp $(S_DIR)/RandomIndices.cpp
SOURCE_DEBUG_OBJS = $(addprefix $(O_DEBUG_DIR)/,$(notdir $(SOURCE_FILES:.cpp=.o)))
SOURCE_RELEASE_OBJS = $(addprefix $(O_RELEASE_DIR)/,$(notdir $(SOURCE_FILES:.cpp=.o)))

# C FLAGS
C_FLAGS = -fPIC -D_FORCE_INLINES
# CPP FLAGS
CC_FLAGS = -lstdc++ -std=c++14 -I $(DIPS_INCLUDE) -I $(S_INC)
# RELEASE_FLAGS
RELEASE_FLAGS = -O3
# DEBUG_FLAGS
DEBUG_FLAGS = -g -D_DEBUG -Wall 
# Linker  #
LDFLAGS += -lm \
  -s USE_ZLIB=1\
  -s INLINING_LIMIT=50 \
  -s TOTAL_MEMORY=134217728 \
  -s INVOKE_RUN=0 \
  -s NO_EXIT_RUNTIME=1\
  -s 'EXTRA_EXPORTED_RUNTIME_METHODS=["FS"]'\
  -s FORCE_FILESYSTEM=1
# Emscripten FLAGS
JS_LDFLAGS = --preload-file $(TOP)/../assets@/assets

# ASM_JS_FLAGS
ifeq ($(USE_ASM_JS),yes)
	CC_FLAGS+= -DPTHREADS_SUPPORTED
	LDFLAGS += -s USE_PTHREADS=2 
else 
	# WEB_ASSEMBLY
	LDFLAGS += -s WASM=1
endif

##
# Support function for colored output
# Args:
#     - $(1) = Color Type
#     - $(2) = String to print
define colorecho
      @tput setaf $(1)
      @echo $(2)
      @tput sgr0
endef

# Color Types
COLOR_BLACK = 0
COLOR_RED = 1
COLOR_GREEN = 2
COLOR_YELLOW = 3
COLOR_BLUE = 4
COLOR_MAGENTA = 5
COLOR_CYAN = 6
COLOR_WHITE = 7

#default release
all: directories release

directories: ${O_DEBUG_DIR} ${O_RELEASE_DIR}

#default release
rebuild: clean directories release

rebuild_debug: clean_debug debug

rebuild_release: clean_release release

debug: directories show_debug_flags $(SOURCE_DEBUG_OBJS)
	$(COMPILER) $(C_FLAGS) $(CC_FLAGS) $(SOURCE_DEBUG_OBJS) $(LDFLAGS) -o $(O_DEBUG_PROG) $(JS_LDFLAGS)
	
release: directories show_release_flags $(SOURCE_RELEASE_OBJS)
	$(COMPILER) $(C_FLAGS) $(CC_FLAGS) $(SOURCE_RELEASE_OBJS) $(LDFLAGS) -o $(O_RELEASE_PROG) $(JS_LDFLAGS)

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
	
$(O_RELEASE_DIR)/%.o: $(S_DIR_EMSCRIPTEN)/%.cpp
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
