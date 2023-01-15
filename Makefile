# # # # # # # # # # # # # # # # # # # # # # # # # # #
#                  Configurations                   #
# # # # # # # # # # # # # # # # # # # # # # # # # # #

# We store in the 'CONFIGS' variable the options we
# will use to execute each of the following commands

CONFIGS=--no-print-directory

# # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Commands associated with the main program     #
# # # # # # # # # # # # # # # # # # # # # # # # # # #

# Builds the executable of the main program
program:
	@cd build && $(MAKE) $(CONFIGS)

# Runs the executable of the main program
run:
	@cd build && $(MAKE) run $(CONFIGS)

# Runs valgrind for the executable of the main program
valgrind:
	@cd build && $(MAKE) run_valgrind $(CONFIGS)

# Counts the number of lines, words & characters of
# the main program and prints them in the screen
count:
	@cd build && $(MAKE) count $(CONFIGS)

# Prints all the *.h and *.cpp files of the main program
list:
	@cd build && $(MAKE) list $(CONFIGS)

# Removes all the temporary files of the main program
# (*.o, *.d, the executable and the temporary directories)
clean:
	@cd build && $(MAKE) clean $(CONFIGS)

# # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Commands associated with the unit testing     #
# # # # # # # # # # # # # # # # # # # # # # # # # # #

# Builds the executable of the test program
test:
	@cd test_build && $(MAKE) $(CONFIGS)

# Runs the executable of the test program
run_test:
	@cd test_build && $(MAKE) run $(CONFIGS)

# Runs valgrind for the executable of the test program
valgrind_test:
	@cd test_build && $(MAKE) run_valgrind $(CONFIGS)

# Counts the number of lines, words & characters of
# the test program and prints them in the screen
count_test:
	@cd test_build && $(MAKE) count $(CONFIGS)

# Prints all the *.h and *.cpp files of the test program
list_test:
	@cd test_build && $(MAKE) list $(CONFIGS)

# Removes all the temporary files of the test program
# (*.o, *.d, the executable and the temporary directories)
clean_test:
	@cd test_build && $(MAKE) clean $(CONFIGS)

# # # # # # # # # # # # # # # # # # # # # # # # # # #
#      Commands associated with both programs       #
# # # # # # # # # # # # # # # # # # # # # # # # # # #

# Builds the executable of both programs
all: program test

# Runs the executable of both programs
run_all: run run_test

# Runs valgrind for the executable of both programs
valgrind_all: valgrind valgrind_test

# Counts the number of lines, words & characters
# of both programs and prints them in the screen
count_all: count count_test

# Prints all the *.h and *.cpp files of both programs
list_all: list list_test

# Removes all the temporary files of both programs
# (*.o, *.d, the executable and the temporary directories)
clean_all: clean clean_test

# # # # # # # # # # # # # # # # # # # # # # # # # # #
#       Commands associated with the harness        #
# # # # # # # # # # # # # # # # # # # # # # # # # # #

# Compiles the harness program and creates its executable
# The harness program is used to test our main program
harness:
	@cd build && g++ -o harness harness.cpp

# Runs the harness executable with the small input files
# and our main program as command line arguments
run_harness_small:
	@cd build && ./harness ../input/small/small.init ../input/small/small.work ../input/small/small.result phj

# Runs the harness executable with the public input files
# and our main program as command line arguments
run_harness_public:
	@cd build && ./harness ../input/public/public.init ../input/public/public.work ../input/public/public.result phj

# Removes the harness executable from the 'build' directory
clean_harness:
	@cd build && rm harness
