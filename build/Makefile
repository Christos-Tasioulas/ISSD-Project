# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Sample Makefile that you can quickly adapt to your needs. #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# Set project directory one level above of Makefile directory. $(CURDIR) is
# a GNU make variable containing the path to the current working directory
PROJDIR := $(realpath $(CURDIR)/..)
SOURCEDIR := $(PROJDIR)/source
BUILDDIR := $(PROJDIR)/build

# Name of the final executable
TARGET=phj

# Decide whether the commands will be shown or not
VERBOSE=FALSE

# Create the list of directories
DIRS=Bitmap HashTable Main PartitionedHashJoin Relation RowIdPair RowIdRelation Tuple
SOURCEDIRS=$(foreach dir, $(DIRS), $(addprefix $(SOURCEDIR)/, $(dir)))
TARGETDIRS=$(foreach dir, $(DIRS), $(addprefix $(BUILDDIR)/, $(dir)))

# Generate the GCC includes parameters by adding -I before each source folder
INCLUDES=$(foreach dir, $(SOURCEDIRS), $(addprefix -I, $(dir)))

# Add this list to VPATH, the place make will look for the source files
VPATH=$(SOURCEDIRS)

# Create a list of *.cpp sources in DIRS
SOURCES=$(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.cpp))

# Create a list of *.h headers in DIRS
HEADERS=$(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.h))

# Define objects for all sources
OBJS := $(subst $(SOURCEDIR),$(BUILDDIR),$(SOURCES:.cpp=.o))

# Define dependencies files for all objects
DEPS=$(OBJS:.o=.d)

# Name the compiler
CC=g++

# Name the flags
CPPFLAGS=-g3 -Wall

# The command line arguments of the program
ARGUMENTS=

# The options to run the Valgrind program
VALGRINDARGS=--track-origins=yes --leak-check=full

# OS specific part
ifeq ($(OS),Windows_NT)
	RM=del /F /Q 
	RMDIR=-RMDIR /S /Q
	MKDIR=-mkdir
	ERRIGNORE=2>NUL || true
	SEP=\\
else
	RM=rm -rf 
	RMDIR=rm -rf 
	MKDIR=mkdir -p
	ERRIGNORE=2>/dev/null
	SEP=/
endif

# Remove space after separator
PSEP=$(strip $(SEP))

# Hide or not the calls depending on VERBOSE
ifeq ($(VERBOSE),TRUE)
	HIDE=  
else
	HIDE=@
endif

# Define the function that will generate each rule
define generateRules
$(1)/%.o: %.cpp
	@echo Building $$@
	$(HIDE)$(CC) $(CPPFLAGS) -c $$(INCLUDES) -o $$(subst /,$$(PSEP),$$@) $$(subst /,$$(PSEP),$$<) -MMD
endef

.PHONY: all clean directories 

all: directories $(TARGET)

$(TARGET) : $(OBJS)
	$(HIDE)echo Linking $@
	$(HIDE)$(CC) $(CPPFLAGS) $(OBJS) -o $(TARGET)

# Include dependencies
-include $(DEPS)

# Generate rules
$(foreach targetdir, $(TARGETDIRS), $(eval $(call generateRules, $(targetdir))))

# Creates just the target directories where each .o and .d will be saved
directories: 
	$(HIDE)$(MKDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)

# Runs the program. Sometimes useful to avoid rewriting long commands.
run: $(TARGET)
	$(HIDE)./$(TARGET) $(ARGUMENTS)

# Runs Valgrind for the program to check possible memory leaks.
run_valgrind: $(TARGET)
	$(HIDE)valgrind $(VALGRINDARGS) ./$(TARGET) $(ARGUMENTS)

# Counts the lines, the amount of words and the quantity of characters in each .h and .cpp
count:
	$(HIDE)wc $(SOURCES) $(HEADERS)

# List all header and source files of the program
list:
	$(HIDE)make -s ls

# Helper command for the above 'list' command
ls:
	$(foreach file, $(HEADERS), $(info $(file)))
	$(foreach file, $(SOURCES), $(info $(file)))

# Remove all objects, dependencies and executable files generated during the build
clean:
	$(HIDE)$(RMDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)
	$(HIDE)$(RM) $(TARGET) $(ERRIGNORE)
	@echo Cleaning done!
