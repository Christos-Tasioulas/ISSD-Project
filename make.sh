#!/bin/bash

# this bash script compiles the two processes from the different directories simultaneously and with the same commands
count=$#

# checking the number of arguments it should be one or two 
if [[ ($count -gt 3 && $count -lt 1) ]];
then
    echo "Wrong number of arguments"
    exit -1
fi

# "make" command
if [[ ($count -eq 1 && $1 == "make") ]];
then
    cd build/
    make
    cd ..
    cd test_build/
    make
    cd ..
# "make program" command
elif [[ ($count -eq 2 && $1 == "make" && $2 == "program") ]]
then
    cd build/
    make
    cd ..
# "make test" command
elif [[ ($count -eq 2 && $1 == "make" && $2 == "test") ]]
then
    cd test_build/
    make
    cd ..    
# "make run" command    
elif [[ ($count -eq 2 && $1 == "make" && $2 == "run") ]]
then    
    cd build/
    make run
    cd ..
    cd test_build/
    make run_test
    cd ..
# "make run program" command    
elif [[ ($count -eq 3 && $1 == "make" && $2 == "run" && $3 == "program") ]]
then    
    cd build/
    make run
    cd ..
# "make run test" command    
elif [[ ($count -eq 3 && $1 == "make" && $2 == "run" && $3 == "test") ]]
then
    cd test_build/
    make run_test
    cd ..    
# "make clean" command    
elif [[ ($count -eq 2 && $1 == "make" && $2 == "clean") ]]
then
    cd build/
    make clean 
    cd ..  
    cd test_build/
    make clean
    cd ..
# "make clean program" command    
elif [[ ($count -eq 3 && $1 == "make" && $2 == "clean" && $3 == "program") ]]
then
    cd build/
    make clean 
    cd ..
# "make clean test" command    
elif [[ ($count -eq 3 && $1 == "make" && $2 == "clean" && $3 == "test") ]]
then  
    cd test_build/
    make clean
    cd ..       	
# "make valgrind" command    
elif [[ ($count -eq 2 && $1 == "make" && $2 == "valgrind") ]]
then    
    cd build/
    make run_valgrind
    cd ..
    cd test_build/
    make run_valgrind_test 
    cd ..
# "make valgrind program" command    
elif [[ ($count -eq 3 && $1 == "make" && $2 == "valgrind" && $3 == "program") ]]
then    
    cd build/
    make run_valgrind
    cd ..
# "make valgrind test" command    
elif [[ ($count -eq 3 && $1 == "make" && $2 == "valgrind" && $3 == "test") ]]
then
    cd test_build/
    make run_valgrind_test 
    cd ..
# "make count" command    
elif [[ ($count -eq 2 && $1 == "make" && $2 == "count") ]]
then    
    cd build/
    make count
    cd ..
    cd test_build/
    make count
    cd ..
# "make count program" command    
elif [[ ($count -eq 3 && $1 == "make" && $2 == "count" && $3 == "program") ]]
then    
    cd build/
    make count
    cd ..
# "make count test" command    
elif [[ ($count -eq 3 && $1 == "make" && $2 == "count" && $3 == "test") ]]
then
    cd test_build/
    make count
    cd ..
# "make list" command    
elif [[ ($count -eq 2 && $1 == "make" && $2 == "list") ]]
then    
    cd build/
    make list
    cd ..
    cd test_build/
    make list 
    cd ..
# "make list program" command    
elif [[ ($count -eq 3 && $1 == "make" && $2 == "list" && $3 == "program") ]]
then    
    cd build/
    make list
    cd ..
# "make list test" command    
elif [[ ($count -eq 3 && $1 == "make" && $2 == "list" && $3 == "test") ]]
then
    cd test_build/
    make list 
    cd ..          
else
    echo "Invalid arguments"    
fi

