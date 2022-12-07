#ifndef _INTERMEDIATE_RELATION_H_
#define _INTERMEDIATE_RELATION_H_

class IntermediateRelation {

private:

    /* The position of the relation in the input structure.
     *
     * It can be treated as a "name" for the relation.
     *
     * For example, "r0" has name 0, "r1" has name 1 and so on.
     */
    unsigned int name;

    /* The priority of the relation in the query.
     *
     * For example in the query: 1 0 3 1|...|...
     *
     * In the above query there are two 1s. The left
     * one has prio 1 and the right one has prio 2.
     */
    unsigned int priority;

public:

    /* Constructor & Destructor */
    IntermediateRelation(unsigned int name, unsigned int priority = 1);
    ~IntermediateRelation();

    /* Getter - Returns the name of the relation */
    unsigned int getName() const;

    /* Getter - Returns the priority of the relation */
    unsigned int getPriority() const;

    /* Prints the data of an intermediate relation */
    void print() const;

};

#endif
