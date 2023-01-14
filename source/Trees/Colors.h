/*
 * Definitions of basic color codes for colored terminal output
 * 
 * By calling 'printf' with one of the arguments below, the
 * color of the output text is changed to what the name of
 * the argument implies
 * 
 * For example, the call 'printf(BLUE);' will change the
 * color output to blue.
 * 
 * By calling 'printf(RESET);' the default text color is
 * restored for any text that will be printed after.
 */

#define RESET_COLOR       "\033[0m"              /* Default */
#define BLACK_COLOR       "\033[30m"             /* Black */
#define RED_COLOR         "\033[31m"             /* Red */
#define GREEN_COLOR       "\033[32m"             /* Green */
#define YELLOW_COLOR      "\033[33m"             /* Yellow */
#define BLUE_COLOR        "\033[34m"             /* Blue */
#define MAGENTA_COLOR     "\033[35m"             /* Magenta */
#define CYAN_COLOR        "\033[36m"             /* Cyan */
#define WHITE_COLOR       "\033[37m"             /* White */
#define BOLDBLACK_COLOR   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED_COLOR     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN_COLOR   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW_COLOR  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE_COLOR    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA_COLOR "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN_COLOR    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE_COLOR   "\033[1m\033[37m"      /* Bold White */
