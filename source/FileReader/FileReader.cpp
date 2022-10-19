#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
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

static unsigned int atou(char *arithmeticString)
{
	/* We convert the given argument to unsigned long integer
	 * by using the function 'strtoul'. Then we will cast that
	 * unsigned long integer to unsigned int and return it.
	 */
	char *conversionErrorMessage;

	unsigned long unsignedLongResult = strtoul(arithmeticString,
		&conversionErrorMessage, 10);

	/* Finally, if no error has occured, we cast the unsigned
	 * long result to unsigned int and we return it.
	 */
	return (unsigned int) unsignedLongResult;
}

/*********************************************************************
 * Reads the given configuration file and returns the value of each  *
 * option in the file through the memory addresses that are given as *
 *                     arguments to the function                     *
 *********************************************************************/

void FileReader::readConfigFile(
    const char *config_file,
    unsigned int *bitsNumForHashing,
    bool *showInitialRelations,
    bool *showAuxiliaryArrays,
    bool *showSubrelations)
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

            /* Case the currently read line is none of the above */

            default:
            {
                break;
            }
        }

        /* We update the current line and proceed to the next loop */
        currentLine++;
    }
}
