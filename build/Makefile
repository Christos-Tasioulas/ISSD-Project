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

# Name of the test executable
TEST=test

# Decide whether the commands will be shown or not
VERBOSE=FALSE

# Create the list of directories
DIRS=Bitmap FileReader HashTable List PartitionedHashJoin Relation RowIdPair RowIdRelation Tuple
SOURCEDIRS=$(foreach dir, $(DIRS), $(addprefix $(SOURCEDIR)/, $(dir)))
TARGETDIRS=$(foreach dir, $(DIRS), $(addprefix $(BUILDDIR)/, $(dir)))

# Create the main
MAINDIR=Main
SOURCEMAINDIR=$(foreach dir, $(MAINDIR), $(addprefix $(SOURCEDIR)/, $(dir)))
TARGETMAINDIR=$(foreach dir, $(MAINDIR), $(addprefix $(BUILDDIR)/, $(dir)))

# Create the test
TESTDIR=Test
SOURCETESTDIR=$(foreach dir, $(TESTDIR), $(addprefix $(SOURCEDIR)/, $(dir)))
TARGETTESTDIR=$(foreach dir, $(TESTDIR), $(addprefix $(BUILDDIR)/, $(dir)))

# Generate the GCC includes parameters by adding -I before each source folder
INCLUDES=$(foreach dir, $(SOURCEDIRS), $(addprefix -I, $(dir)))
INCLUDES+=$(foreach dir, $(SOURCETESTDIR), $(addprefix -I, $(dir)))

# Add this list to VPATH, the place make will look for the source files
VPATH=$(SOURCEDIRS)

# Create a list of *.cpp sources in DIRS
SOURCES=$(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.cpp))

MAINSOURCE=$(foreach dir,$(MAINDIR),$(wildcard $(dir)/*.cpp))

TESTSOURCE=$(foreach dir,$(TESTDIR),$(wildcard $(dir)/*.cpp))

# Create a list of *.h headers in DIRS
HEADERS=$(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.h))

TESTHEADER=$(foreach dir,$(TESTDIR),$(wildcard $(dir)/*.h))

# Define objects for all sources
OBJS := $(subst $(SOURCEDIR),$(BUILDDIR),$(SOURCES:.cpp=.o))

MAINOBJ := $(subst $(SOURCEDIR), $(BUILDDIR), $(MAINSOURCE:.cpp=.o))

TESTOBJ := $(subst $(SOURCEDIR), $(BUILDDIR), $(TESTSOURCE:.cpp=.o))

# Define dependencies files for all objects
DEPS=$(OBJS:.o=.d)

# Name the compiler
CC=g++

# Name the flags
CPPFLAGS=-g3 -Wall

# The command line arguments of the program
ARGUMENTS=

# The command line arguments of the test
TESTARGUMENTS=

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

all: directories $(TARGET) $(TEST)

$(TARGET) : $(MAINOBJ) $(OBJS)
	$(HIDE)echo Linking $@
	$(HIDE)$(CC) $(CPPFLAGS) $(MAINOBJ) $(OBJS) -o $(TARGET)

$(TEST) : $(TESTOBJ) $(OBJS)
	$(HIDE)echo Linking $@
	$(HIDE)$(CC) $(CPPFLAGS) $(TESTOBJ) $(OBJS) -o $(TEST)	

# Include dependencies
-include $(DEPS)

# Generate rules
$(foreach targetdir, $(TARGETDIRS), $(eval $(call generateRules, $(targetdir))))
$(foreach targetdir, $(TARGETMAINDIR), $(eval $(call generateRules, $(targetdir))))
$(foreach targetdir, $(TARGETTESTDIR), $(eval $(call generateRules, $(targetdir))))

# Creates just the target directories where each .o and .d will be saved
directories: 
	$(HIDE)$(MKDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)

# Runs the program. Sometimes useful to avoid rewriting long commands.
run_program: $(TARGET)
	$(HIDE)./$(TARGET) $(ARGUMENTS)

# Runs Valgrind for the program to check possible memory leaks.
run_program_valgrind: $(TARGET)
	$(HIDE)valgrind $(VALGRINDARGS) ./$(TARGET) $(ARGUMENTS)

# Runs the test. Sometimes useful to avoid rewriting long commands.
run_test: $(TEST)
	$(HIDE)./$(TEST) $(TESTARGUMENTS)

# Runs Valgrind for the test to check possible memory leaks.
run_test_valgrind: $(TEST)
	$(HIDE)valgrind $(VALGRINDARGS) ./$(TEST) $(TESTARGUMENTS)	

# Counts the lines, the amount of words and the quantity of characters in each .h and .cpp
count:
	$(HIDE)wc $(SOURCES) $(HEADERS) $(TESTSOURCE) $(TESTHEADER) $(MAINSOURCE)

# List all header and source files of the program
list:
	$(HIDE)make -s ls

# Helper command for the above 'list' command
ls:
	$(foreach file, $(HEADERS), $(info $(file)))
	$(foreach file, $(SOURCES), $(info $(file)))
	$(foreach file, $(TESTHEADER), $(info $(file)))
	$(foreach file, $(TESTSOURCE), $(info $(file)))
	$(foreach file, $(MAINSOURCE), $(info $(file)))

# Remove all objects, dependencies and executable files generated during the build
clean:
	$(HIDE)$(RMDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)
	$(HIDE)$(RMDIR) $(subst /,$(PSEP),$(TARGETMAINDIR)) $(ERRIGNORE)
	$(HIDE)$(RMDIR) $(subst /,$(PSEP),$(TARGETTESTDIR)) $(ERRIGNORE)
	$(HIDE)$(RM) $(TARGET) $(ERRIGNORE)
	$(HIDE)$(RM) $(TEST) $(ERRIGNORE)
	@echo Cleaning done!
