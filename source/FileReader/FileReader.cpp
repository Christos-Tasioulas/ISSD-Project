#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <bits/stdc++.h>
#include <sstream>
#include "FileReader.h"

/**********************************************************************
 * The maximum capacity of the buffer where the content of a specific *
 * line of the file is stored. Essentially, a line of file should not *
 *       have more characters than the specified message length       *
 **********************************************************************/

static unsigned int messageLength = 512;

/*****************************************
 * Converts a string to unsigned integer * 
 *****************************************/

static unsigned int atou(char *arithmetic_string, unsigned int *read_bytes = NULL)
{
	/* We convert the given argument to unsigned long integer
	 * by using the function 'strtoul'. Then we will cast that
	 * unsigned long integer to unsigned int and return it.
	 */
	char *parsingStopPoint;

	unsigned long unsignedLongResult = strtoul(arithmetic_string,
		&parsingStopPoint, 10);

    /* We store the amount of bytes that were successfully parsed
     * in case the user has given a no-null unsigned int address
     */
    if(read_bytes != NULL)
    {
        /* The amount of bytes parsed successfully is the address
         * where 'strtoul' stopped parsing minus the address where
         * 'strtou' started parsing.
         */
        (*read_bytes) = parsingStopPoint - arithmetic_string;
    }

	/* Finally, if no error has occured, we cast the unsigned
	 * long result to unsigned int and we return it.
	 */
	return (unsigned int) unsignedLongResult;
}

// Function that return count of the given
// character in the string
static int count(std::string s, char c)
{
    // Count variable
    int res = 0;
 
    for (long unsigned int i=0;i<s.length();i++)
 
        // checking character in string
        if (s[i] == c)
            res++;
 
    return res;
}

// function that has a text as input and returns an array with it
// Text has to be like this: <text> [i1,i2, ...  ,in] 
static int* line_to_array(std::string text)
{
    //Find the start and end of each table
    std::size_t l_bracket = text.find("[");
    std::size_t r_bracket = text.find("]");

    //Remove brackets from string
    std::string numbers = text.substr(l_bracket+1, r_bracket-l_bracket-1);

    // the size of our array will be equal to the number of commas in the text plus one
    int* array = new int[count(numbers, ',') + 1];

    
    std::string number = "";
    int i = 0;
    for (auto x: numbers)
    {
        // We detected a comma therefore we have found an element of the array
        if(x == ',')
        {
            // string to integer using stringstream
            std::stringstream alpharethmetic(number);
            int n = 0;
            alpharethmetic >> n;
            array[i] = n;
            i++;
            number = "";
        }
        // we have found part of the element of the array
        else number = number + x;
    }
    // This is the last element of the array
    std::stringstream alpharethmetic(number);
    int n = 0;
    alpharethmetic >> n;
    array[i] = n;

    return array;
}

/*********************************************************************
 *  Reads the "init" file that contains all the relation names that  *
 * will take part in the follow-up queries. Then it reads the binary *
 *  input file of each relation. Retuns a lined list that stores a   *
 *  (Table *) in each node. Each 'Table' object stores all the rows  *
 *                     and columns of a relation                     *
 *********************************************************************/

List *FileReader::readInitFile(const char *init_file, const char *config_file)
{
    /* A list that will be storing the table of every relation */

    List *relations = new List();

    /* We read the desired maximum bitmap size from the config file.
     *
     * If no config file has been given, the size is initialized with 50000000.
     */
    unsigned int maxBitmapSize = 50000000;

    if(config_file != NULL) {
        readMaxBitmapSize(config_file, &maxBitmapSize);
    }

	/* We read the dataset type from the configuration file */

	char *dataset;
	readDataset(config_file, &dataset);

	/* We find the length of the string with the dataset type */

	unsigned int dataset_length = strlen(dataset);

    /* We open the initialization file */

	int fd = open(init_file, O_RDONLY);

	/* We examine if the opening was successful */

	if(fd == -1)
	{
		printf("Error opening \"%s\"\n", init_file);
		perror("open");
		return NULL;
	}

	/* We prepare the variables we will need to read the file */

	char read_char = 0;
	char buf[messageLength];
	unsigned int i = 0;
	unsigned int currentLine = 1;
	bool endOfFile = false;

	/* We will read the file character by character and each time
	 * we will save the read character in the 'read_char' variable
	 */
	while(1)
	{
		/* With the inner 'while' we read a single line of the file */

		while(1)
		{
			/* Here we read the next character */

			int read_bytes = read(fd, &read_char, 1);

			/* If there are no more characters in the file,
			 * (result from 'read' <= 0) we exit immediatelly
			 */

			if(read_bytes <= 0)
			{
				endOfFile = true;
				break;
			}

			/* If the read character was a new line, the loop ends */

			if(read_char == '\n')
			{
				/* We complete the string with final zero
		 		 * and reset 'i' for the next loop
				 */

				buf[i] = '\0';
				i = 0;

				break;
			}

			/* Else we store the character in the buffer and continue */

			buf[i++] = read_char;
		}

		/* If the end of file was reached, we stop the loop */

		if(endOfFile)
			break;

		/* Now the 'buf' array is storing the whole content of the current
		 * line of the file that we just read in the above inner 'while' loop.
		 *
		 * In this part we will do any actions we want with this line of file.
		 *
         * First, we retrieve the length of the buffer (in bytes).
         */
        unsigned int buffer_length = strlen(buf);

        /* We build the relative path to the binary filename */
        char currentBinaryFilename[strlen("../input/") + dataset_length + 1 + buffer_length + 1];
        sprintf(currentBinaryFilename, "../input/%s/%s", dataset, buf);

        /* We create a new Table giving it the binary file as input */
        Table *new_table = new Table(currentBinaryFilename, maxBitmapSize);

        /* We store the new Table we just created in the list */
        relations->insertLast(new_table);

        /* We proceed to the next line of the file */
        currentLine++;
    }

    /* Finally, we close the opened initialization file */

	int close_result = close(fd);

	/* We examine if the closing of the file was successful */

	if(close_result == -1)
	{
		printf("Error closing \"%s\"\n", init_file);
		perror("close");
	}

    /* We free the allocated memory for the dataset type */

    free(dataset);

    /* Finally we return the list of Tables of each relation */

    return relations;
}

/*****************************************************************
 * Reads the "work" file that contains all the queries that will *
 *  be given to the program. Every query is stored in a 'Query'  *
 *  object. A linked list of lists will be returned. Each inner  *
 * linked list contains a full batch of input queries. The outer *
 *              list contains all the input batches              *
 *****************************************************************/

List *FileReader::readWorkFile(const char *work_file)
{
    /* A list of lists - each inner list stores a batch of queries */

    List *allBatches = new List();

    /* A list that will be storing all the input queries of the current batch */

    List *currentBatchOfQueries = new List();

    /* We open the initialization file */

    int fd = open(work_file, O_RDONLY);

    /* We examine if the opening was successful */

    if(fd == -1)
    {
        printf("Error opening \"%s\"\n", work_file);
        perror("open");
        return NULL;
    }

    /* We prepare the variables we will need to read the file */

    char read_char = 0;
    char buf[messageLength];
    unsigned int i = 0;
    unsigned int currentLine = 1;
    bool endOfFile = false;

    /* We will read the file character by character and each time
     * we will save the read character in the 'read_char' variable
     */

    while(1)
    {
        /* With the inner 'while' we read a single line of the file */

        while(1)
        {
            /* Here we read the next character */

            int read_bytes = read(fd, &read_char, 1);

            /* If there are no more characters in the file,
             * (result from 'read' <= 0) we exit immediatelly
             */

            if(read_bytes <= 0)
            {
                endOfFile = true;
                break;
            }

            /* If the read character was a new line, the loop ends */

            if(read_char == '\n')
            {
                /* We complete the string with final zero
                 * and reset 'i' for the next loop
                 */

                buf[i] = '\0';
                i = 0;

                break;
            }

            /* Else we store the character in the buffer and continue */

            buf[i++] = read_char;
        }

        /* If the end of file was reached, we stop the loop */

        if(endOfFile)
            break;

        /* Now the 'buf' array is storing the whole content of the current
         * line of the file that we just read in the above inner 'while' loop.
         *
         * In this part we will do any actions we want with this line of file.
         *
         * Case we have reached the end of the current batch
         */
        if(!strcmp(buf, "F"))
        {
            /* We insert the complete batch in the list of all batches */
            allBatches->insertLast(currentBatchOfQueries);

            /* We prepare the helper list for the next batch */
            currentBatchOfQueries = new List();

            /* We proceed to the next line of the file */
            currentLine++;

            /* There is nothing else to do in this case */
            continue;
        }

        /* We store the current query in a 'Query' object */
        Query *new_query = new Query(buf);

        /* We insert the 'Query' object we just created in the current batch */
        currentBatchOfQueries->insertLast(new_query);

        /* We proceed to the next line of the file */
        currentLine++;
    }

    /* Finally, we close the opened initialization file */

    int close_result = close(fd);

    /* We examine if the closing of the file was successful */

    if(close_result == -1)
    {
        printf("Error closing \"%s\"\n", work_file);
        perror("close");
    }

    /* We delete the temporary helper list */

    delete currentBatchOfQueries;

    /* Finally we return the list of all batches */

    return allBatches;
}

/***********************************************************
 * Reads the input file and stores the user's input to the *
 *   given addresses passed as arguments to the function   *
 ***********************************************************/

void FileReader::readInputFile(
    const char *input_file,
    Relation **relR,
    Relation **relS)
{

    int r_size;
    int s_size;

    //Check if Input File exists
    if(input_file)
    {
        std::string filetext1, filetext2;
        std::ifstream inputFile(input_file);
        
        //Break the text file into two strings (The text file will have 2 lines)
        getline(inputFile, filetext1);
        getline(inputFile, filetext2);
        
        //Convert the two strings into 2 arrays
        int* array1 = line_to_array(filetext1);
        int* array2 = line_to_array(filetext2);

        // the size of the arrays will be equal to the number of commas in the text plus one 
        r_size = count(filetext1, ',') + 1;
        s_size = count(filetext2, ',') + 1;

        //Create an array of Tuples equal to the number of items in each item array
        Tuple *tuples_array_1 = new Tuple[r_size];
        Tuple *tuples_array_2 = new Tuple[s_size];

        //Insert items into tuples
        for(int i=0; i<r_size; i++)
        {
            int entry = array1[i];
            tuples_array_1[i] = Tuple(new int(entry), i+1);
        }

        for(int i=0; i<s_size; i++)
        {
            int entry = array2[i];
            tuples_array_2[i] = Tuple(new int(entry), i+1);
        }

        delete[] array1;
        delete[] array2;

        (*relR) = new Relation(tuples_array_1, r_size);
        (*relS) = new Relation(tuples_array_2, s_size);

    }
    // Hardcoding if an input file is not given
    else 
    {
        Tuple *tuples_array_1 = new Tuple[5];
        Tuple *tuples_array_2 = new Tuple[5];

        tuples_array_1[0] = Tuple(new int(1), 1);
        tuples_array_1[1] = Tuple(new int(8), 2);
        tuples_array_1[2] = Tuple(new int(8), 3);
        tuples_array_1[3] = Tuple(new int(7), 4);
        tuples_array_1[4] = Tuple(new int(3), 5);

        tuples_array_2[0] = Tuple(new int(1), 1);
        tuples_array_2[1] = Tuple(new int(9), 2);
        tuples_array_2[2] = Tuple(new int(2), 3);
        tuples_array_2[3] = Tuple(new int(8), 4);
        tuples_array_2[4] = Tuple(new int(8), 5);

        (*relR) = new Relation(tuples_array_1, 5);
        (*relS) = new Relation(tuples_array_2, 5);  
    }
    
}

/*************************************************************************
 *   Reads the given configuration file and returns the value of each    *
 * option in the file that is associated with the join execution through *
 *   the memory addresses that are given as arguments to the function    *
 *************************************************************************/

void FileReader::readConfigFile(
    const char *config_file,
    unsigned int *bitsNumForHashing,
    bool *showInitialRelations,
    bool *showAuxiliaryArrays,
    bool *showHashTable,
    bool *showSubrelations,
    bool *showResult,
    unsigned int *hopscotchBuckets,
    unsigned int *hopscotchRange,
    bool *resizableByLoadFactor,
    double *loadFactor,
    double *maxAllowedSizeModifier,
    unsigned int *maxPartitionDepth)
{
    /* We open the configuration file */

	int fd = open(config_file, O_RDONLY);

	/* We examine if the opening was successful */

	if(fd == -1)
	{
		printf("Error opening \"%s\"\n", config_file);
		perror("open");
		return;
	}

	/* We prepare the variables we will need to read the file */

	char read_char = 0;
	char buf[messageLength];
	unsigned int i = 0;
	unsigned int currentLine = 1;
	bool endOfFile = false;

	/* We will read the file character by character and each time
	 * we will save the read character in the 'read_char' variable
	 */

	while(1)
	{
		/* With the inner 'while' we read a single line of the file */

		while(1)
		{
			/* Here we read the next character */

			int read_bytes = read(fd, &read_char, 1);

			/* If there are no more characters in the file,
			 * (result from 'read' <= 0) we exit immediatelly
			 */

			if(read_bytes <= 0)
			{
				endOfFile = true;
				break;
			}

			/* If the read character was a new line, the loop ends */

			if(read_char == '\n')
			{
				/* We complete the string with final zero
		 		 * and reset 'i' for the next loop
				 */

				buf[i] = '\0';
				i = 0;

				break;
			}

			/* Else we store the character in the buffer and continue */

			buf[i++] = read_char;
		}

		/* If the end of file was reached, we stop the loop */

		if(endOfFile)
			break;

		/* Now the 'buf' array is storing the whole content of the current
		 * line of the file that we just read in the above inner 'while' loop.
		 *
		 * In this part we will do any actions we want with this line of file.
		 */

        switch(currentLine)
        {
            /* Case the currently read line is the 1st line of the file */

            case 1:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *bitsNumForHashingAsString = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*bitsNumForHashingAsString != '=')
                    bitsNumForHashingAsString++;

                /* We go to the next character exactly after the '=' */
                bitsNumForHashingAsString++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read the amount of bits that will determine the hash
                 * code of each integer. This option is an integer value.
                 *
                 * We convert the arithmetic string to integer and we
                 * save that integer to the given address of that option.
                 */
                (*bitsNumForHashing) = atou(bitsNumForHashingAsString);

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is the 4th line of the file */

            case 4:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *showInitialRelationsAsString = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*showInitialRelationsAsString != '=')
                    showInitialRelationsAsString++;

                /* We go to the next character exactly after the '=' */
                showInitialRelationsAsString++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read whether the option to show the initial relations
                 * is "yes" or "no".
                 */
                (*showInitialRelations) = (!strcmp(
                    showInitialRelationsAsString, "yes")) ? true : false;

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is the 8th line of the file */

            case 8:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *showAuxiliaryArraysAsString = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*showAuxiliaryArraysAsString != '=')
                    showAuxiliaryArraysAsString++;

                /* We go to the next character exactly after the '=' */
                showAuxiliaryArraysAsString++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read whether the option to show the auxiliary arrays
                 * is "yes" or "no".
                 */
                (*showAuxiliaryArrays) = (!strcmp(
                    showAuxiliaryArraysAsString, "yes")) ? true : false;

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is the 12th line of the file */

            case 12:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *showHashTableAsString = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*showHashTableAsString != '=')
                    showHashTableAsString++;

                /* We go to the next character exactly after the '=' */
                showHashTableAsString++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read whether the option to display the contents of the
                 * hash table is "yes" or "no".
                 */
                (*showHashTable) = (!strcmp(
                    showHashTableAsString, "yes")) ? true : false;

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is the 16th line of the file */

            case 16:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *showSubrelationsAsString = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*showSubrelationsAsString != '=')
                    showSubrelationsAsString++;

                /* We go to the next character exactly after the '=' */
                showSubrelationsAsString++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read whether the option to display the contents of the
                 * subrelations is "yes" or "no".
                 */
                (*showSubrelations) = (!strcmp(
                    showSubrelationsAsString, "yes")) ? true : false;

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is the 26th line of the file */

            case 26:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *showResultAsString = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*showResultAsString != '=')
                    showResultAsString++;

                /* We go to the next character exactly after the '=' */
                showResultAsString++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read whether the option to display the final result
                 * is "yes" or "no".
                 */
                (*showResult) = (!strcmp(
                    showResultAsString, "yes")) ? true : false;

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is the 30th line of the file */

            case 30:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *hopscotchBucketsAsString = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*hopscotchBucketsAsString != '=')
                    hopscotchBucketsAsString++;

                /* We go to the next character exactly after the '=' */
                hopscotchBucketsAsString++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read the amount of initial buckets that each hash table
                 * created by the partitioned hash join algorithm will have.
                 *
                 * We convert the arithmetic string to integer and we
                 * save that integer to the given address of that option.
                 */
                (*hopscotchBuckets) = atou(hopscotchBucketsAsString);

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is the 34th line of the file */

            case 34:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *hopscotchRangeAsString = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*hopscotchRangeAsString != '=')
                    hopscotchRangeAsString++;

                /* We go to the next character exactly after the '=' */
                hopscotchRangeAsString++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read the initial size of neighborhood that each hash table
                 * created by the partitioned hash join algorithm will have.
                 *
                 * We convert the arithmetic string to integer and we
                 * save that integer to the given address of that option.
                 */
                (*hopscotchRange) = atou(hopscotchRangeAsString);

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is the 38th line of the file */

            case 38:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *resizableByLoadFactorAsString = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*resizableByLoadFactorAsString != '=')
                    resizableByLoadFactorAsString++;

                /* We go to the next character exactly after the '=' */
                resizableByLoadFactorAsString++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read whether the option to grow the hash table when
                 * the load factor is surpassed is "yes" or "no".
                 */
                (*resizableByLoadFactor) = (!strcmp(
                    resizableByLoadFactorAsString, "yes")) ? true : false;

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is the 46th line of the file */

            case 46:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *loadFactorAsString = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*loadFactorAsString != '=')
                    loadFactorAsString++;

                /* We go to the next character exactly after the '=' */
                loadFactorAsString++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read the load factor of the hash table.
                 *
                 * We convert the arithmetic string to double and we
                 * save that amount to the given address of that option.
                 */
                (*loadFactor) = strtod(loadFactorAsString, NULL);

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is the 50th line of the file */

            case 50:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *maxAllowedSizeModifierAsString = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*maxAllowedSizeModifierAsString != '=')
                    maxAllowedSizeModifierAsString++;

                /* We go to the next character exactly after the '=' */
                maxAllowedSizeModifierAsString++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read the ratio of the max allowed array size divided
                 * by the lvl-2 cache size.
                 *
                 * We convert the arithmetic string to double and we
                 * save that amount to the given address of that option.
                 */
                (*maxAllowedSizeModifier) = strtod(maxAllowedSizeModifierAsString, NULL);

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is the 57th line of the file */

            case 57:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *maxPartitionDepthAsString = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*maxPartitionDepthAsString != '=')
                    maxPartitionDepthAsString++;

                /* We go to the next character exactly after the '=' */
                maxPartitionDepthAsString++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read the initial size of neighborhood that each hash table
                 * created by the partitioned hash join algorithm will have.
                 *
                 * We convert the arithmetic string to integer and we
                 * save that integer to the given address of that option.
                 */
                (*maxPartitionDepth) = atou(maxPartitionDepthAsString);

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is none of the above */

            default:
            {
                break;
            }
        }

        /* We update the current line and proceed to the next loop */
        currentLine++;
    }

    /* Finally, we close the opened configuration file */

	int close_result = close(fd);

	/* We examine if the closing of the file was successful */

	if(close_result == -1)
	{
		printf("Error closing \"%s\"\n", config_file);
		perror("close");
	}
}

/*****************************************************************************
 *  Reads the number of threads from the configuration file and places the   *
 * value of the option to the address pointed by the given 'result' argument *
 *****************************************************************************/

void FileReader::readNumOfThreads(const char *config_file, unsigned int *result)
{
    /* We open the configuration file */

	int fd = open(config_file, O_RDONLY);

	/* We examine if the opening was successful */

	if(fd == -1)
	{
		printf("Error opening \"%s\"\n", config_file);
		perror("open");
		return;
	}

	/* We prepare the variables we will need to read the file */

	char read_char = 0;
	char buf[messageLength];
	unsigned int i = 0;
	unsigned int currentLine = 1;
	bool endOfFile = false;

	/* We will read the file character by character and each time
	 * we will save the read character in the 'read_char' variable
	 */

	while(1)
	{
		/* With the inner 'while' we read a single line of the file */

		while(1)
		{
			/* Here we read the next character */

			int read_bytes = read(fd, &read_char, 1);

			/* If there are no more characters in the file,
			 * (result from 'read' <= 0) we exit immediatelly
			 */

			if(read_bytes <= 0)
			{
				endOfFile = true;
				break;
			}

			/* If the read character was a new line, the loop ends */

			if(read_char == '\n')
			{
				/* We complete the string with final zero
		 		 * and reset 'i' for the next loop
				 */

				buf[i] = '\0';
				i = 0;

				break;
			}

			/* Else we store the character in the buffer and continue */

			buf[i++] = read_char;
		}

		/* If the end of file was reached, we stop the loop */

		if(endOfFile)
			break;

		/* Now the 'buf' array is storing the whole content of the current
		 * line of the file that we just read in the above inner 'while' loop.
		 *
		 * In this part we will do any actions we want with this line of file.
		 */

        switch(currentLine)
        {
            /* Case the currently read line is the 1st line of the file */

            case 60:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *threadsNum = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*threadsNum != '=')
                    threadsNum++;

                /* We go to the next character exactly after the '=' */
                threadsNum++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read the amount of threads that will be executing the
                 * tasks of each join. This option is an integer value.
                 *
                 * We convert the arithmetic string to integer and we
                 * save that integer to the given address of that option.
                 */
                (*result) = atou(threadsNum);

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is none of the above */

            default:
            {
                break;
            }
        }

        /* We update the current line and proceed to the next loop */
        currentLine++;
    }

    /* Finally, we close the opened configuration file */

	int close_result = close(fd);

	/* We examine if the closing of the file was successful */

	if(close_result == -1)
	{
		printf("Error closing \"%s\"\n", config_file);
		perror("close");
	}
}

/**********************************************************************
 * Reads the maximum size of each bitmap that we will use to estimate *
 *      the amount of distinct elements per column of each table      *
 **********************************************************************/

void FileReader::readMaxBitmapSize(const char *config_file, unsigned int *result)
{
    /* We open the configuration file */

	int fd = open(config_file, O_RDONLY);

	/* We examine if the opening was successful */

	if(fd == -1)
	{
		printf("Error opening \"%s\"\n", config_file);
		perror("open");
		return;
	}

	/* We prepare the variables we will need to read the file */

	char read_char = 0;
	char buf[messageLength];
	unsigned int i = 0;
	unsigned int currentLine = 1;
	bool endOfFile = false;

	/* We will read the file character by character and each time
	 * we will save the read character in the 'read_char' variable
	 */

	while(1)
	{
		/* With the inner 'while' we read a single line of the file */

		while(1)
		{
			/* Here we read the next character */

			int read_bytes = read(fd, &read_char, 1);

			/* If there are no more characters in the file,
			 * (result from 'read' <= 0) we exit immediatelly
			 */

			if(read_bytes <= 0)
			{
				endOfFile = true;
				break;
			}

			/* If the read character was a new line, the loop ends */

			if(read_char == '\n')
			{
				/* We complete the string with final zero
		 		 * and reset 'i' for the next loop
				 */

				buf[i] = '\0';
				i = 0;

				break;
			}

			/* Else we store the character in the buffer and continue */

			buf[i++] = read_char;
		}

		/* If the end of file was reached, we stop the loop */

		if(endOfFile)
			break;

		/* Now the 'buf' array is storing the whole content of the current
		 * line of the file that we just read in the above inner 'while' loop.
		 *
		 * In this part we will do any actions we want with this line of file.
		 */

        switch(currentLine)
        {
            /* Case the currently read line is the 1st line of the file */

            case 63:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *maxBitmapSize = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*maxBitmapSize != '=')
                    maxBitmapSize++;

                /* We go to the next character exactly after the '=' */
                maxBitmapSize++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read the maximum size of each bitmap that will be used
                 * to estimate the number of distinct elements of each
                 * column of every table. This option is an integer value.
                 *
                 * We convert the arithmetic string to integer and we
                 * save that integer to the given address of that option.
                 */
                (*result) = atou(maxBitmapSize);

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is none of the above */

            default:
            {
                break;
            }
        }

        /* We update the current line and proceed to the next loop */
        currentLine++;
    }

    /* Finally, we close the opened configuration file */

	int close_result = close(fd);

	/* We examine if the closing of the file was successful */

	if(close_result == -1)
	{
		printf("Error closing \"%s\"\n", config_file);
		perror("close");
	}
}

/**************************************************************
 * Reads the type of input dataset that will be used as input *
 **************************************************************/

void FileReader::readDataset(const char *config_file, char **result)
{
    /* We open the configuration file */

	int fd = open(config_file, O_RDONLY);

	/* We examine if the opening was successful */

	if(fd == -1)
	{
		printf("Error opening \"%s\"\n", config_file);
		perror("open");
		return;
	}

	/* We prepare the variables we will need to read the file */

	char read_char = 0;
	char buf[messageLength];
	unsigned int i = 0;
	unsigned int currentLine = 1;
	bool endOfFile = false;

	/* We will read the file character by character and each time
	 * we will save the read character in the 'read_char' variable
	 */

	while(1)
	{
		/* With the inner 'while' we read a single line of the file */

		while(1)
		{
			/* Here we read the next character */

			int read_bytes = read(fd, &read_char, 1);

			/* If there are no more characters in the file,
			 * (result from 'read' <= 0) we exit immediatelly
			 */

			if(read_bytes <= 0)
			{
				endOfFile = true;
				break;
			}

			/* If the read character was a new line, the loop ends */

			if(read_char == '\n')
			{
				/* We complete the string with final zero
		 		 * and reset 'i' for the next loop
				 */

				buf[i] = '\0';
				i = 0;

				break;
			}

			/* Else we store the character in the buffer and continue */

			buf[i++] = read_char;
		}

		/* If the end of file was reached, we stop the loop */

		if(endOfFile)
			break;

		/* Now the 'buf' array is storing the whole content of the current
		 * line of the file that we just read in the above inner 'while' loop.
		 *
		 * In this part we will do any actions we want with this line of file.
		 */

        switch(currentLine)
        {
            /* Case the currently read line is the 1st line of the file */

            case 67:
            {
                /* We create a new variable that points to the base address
                 * of the buffer storing the content of the current line
                 */
                char *dataset = buf;

                /* As long as we do not encounter the '=' symbol,
                 * we go to the next character of the string
                 */
                while(*dataset != '=')
                    dataset++;

                /* We go to the next character exactly after the '=' */
                dataset++;

                /* Now the variable is pointing to the start of the data
                 * we are interested in reading. In this line we want to
                 * read the dataset that will be used as input.
                 */
                unsigned int datasetNameLen = strlen(dataset);
                (*result) = (char *) malloc(1 + datasetNameLen);

                /* We copy the dataset to the result string */
                strcpy((*result), dataset);

                /* There is nothing more to do with this line of file */
                break;
            }

            /* Case the currently read line is none of the above */

            default:
            {
                break;
            }
        }

        /* We update the current line and proceed to the next loop */
        currentLine++;
    }

    /* Finally, we close the opened configuration file */

	int close_result = close(fd);

	/* We examine if the closing of the file was successful */

	if(close_result == -1)
	{
		printf("Error closing \"%s\"\n", config_file);
		perror("close");
	}
}
