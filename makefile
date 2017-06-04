#Input
MKDIR_P       ?=mkdir -p
CC            ?=g++
TOP           ?=$(shell pwd)

#program name
S_DIR  = $(TOP)/
S_INC  = $(TOP)/
O_DIR  = $(TOP)/obj/
O_PROG = $(TOP)/DENN

#global include
DIPS_INCLUDE = $(TOP)/dips/include/

# C++ files
SOURCE_FILES = $(S_DIR)/main.cpp TicksTime.cpp
SOURCE_OBJS = $(addprefix $(O_DIR)/,$(notdir $(SOURCE_FILES:.cpp=.o)))

# C FLAGS
C_FLAGS = -Wall -fPIC -pthread -D_FORCE_INLINES -fopenmp -Ofast
# CPP FLAGS
CC_FLAGS = -lstdc++ -std=c++14 -I $(DIPS_INCLUDE)

# Linker
LDFLAGS += -lz -lm -lpthread -lutil 

# Color Types
COLOR_BLACK = 0
COLOR_RED = 1
COLOR_GREEN = 2
COLOR_YELLOW = 3
COLOR_BLUE = 4
COLOR_MAGENTA = 5
COLOR_CYAN = 6
COLOR_WHITE = 7

all: directories denn

directories: ${O_DIR}

denn: directories $(SOURCE_OBJS)
	$(CC) $(C_FLAGS) $(CC_FLAGS) $(SOURCE_OBJS) -o $(O_PROG) $(LDFLAGS)
	
# makedir
${O_DIR}:
	$(call colorecho,$(COLOR_GREEN),"[ Create $(O_DIR) directory ]")
	@${MKDIR_P} ${O_DIR}

$(O_DIR)/%.o: $(S_DIR)/%.cpp
	$(call colorecho,$(COLOR_GREEN),"[ Make object $(@) ]")
	$(CC) $(C_FLAGS) $(CC_FLAGS) -c $< -o $@

# Clean
clean:
	$(call colorecho,$(COLOR_MAGENTA),"[ Delete obj files ]")
	@rm -f -R $(O_DIR)
	$(call colorecho,$(COLOR_MAGENTA),"[ Delete executable files ]")
	@rm -f $(O_PROG)

