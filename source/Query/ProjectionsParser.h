#ifndef _PROJECTIONS_PARSER_H_
#define _PROJECTIONS_PARSER_H_

/* A structure used to parse the projections of a query
 * and store the implied relations and columns in seperate
 * variables
 *
 * Each projection has the form "x.y", where 'x' is a
 * relation and 'y' the column of that relation. We
 * will save these two values in seperate variables.
 */
class ProjectionsParser {

private:

    /* The array of the current projection */
    unsigned int array;

    /* The column of the above array
     * pointed by the projection
     */
    unsigned int column;

public:

    /* Constructor & Destructor */
    ProjectionsParser(char *projection_string);
    ~ProjectionsParser();

    /* Getter - Returns the array of the projection */
    unsigned int getArray() const;

    /* Getter - Returns the column of the projection */
    unsigned int getColumn() const;

    /* Prints a projection (its array and its column) */
    void print() const;

};

#endif
