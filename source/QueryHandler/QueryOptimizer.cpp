#include <iostream>
#include "QueryOptimizer.h"

/***************
 * Constructor *
 ***************/

QueryOptimizer::QueryOptimizer(List *tables, Query *query)
{
    this->tables = tables;
    this->query = query;
}

/**************
 * Destructor *
 **************/

QueryOptimizer::~QueryOptimizer() {}

/************************************************
 * Returns the most optimal cost of addressing  *
 * only the predicates in the given linked list *
 ************************************************/

unsigned long long QueryOptimizer::getOptimalPredicatesOrderRec(
    List *chosenPredicates) const
{
    return 0;
}

/****************************************************************************
 * Returns a list of predicates in the order that is estimated most optimal *
 ****************************************************************************/

List *QueryOptimizer::getOptimalPredicatesOrder() const
{
    /* We retrieve the amount of predicates in the query */
    // unsigned int predicatesNum = queryPredicates->getCounter();

    // if(predicatesNum == 1 || predicatesNum == 2)
    // {
    //     List *result = new List();
    //     result->append(queryPredicates);
    //     return result;
    // }

    // unsigned int i;
    // for(i = 1; i <= predicatesNum; i++)
    // {
    //     PredicatesParser *ith_predicate = (PredicatesParser *) queryPredicates->getItemInPos(i);
    //     queryPredicates->removePos(i);

        

    //     queryPredicates->insertBeforePos(ith_predicate, i);
    // }

    /* This is the list of the final result that we will return */
    List *result = new List();

    /* The list of predicates of the query contains both join and
     * filter queries. Our first job is to place all the filter
     * queries at the begining of the result list (those have the
     * highest priority of execution). Then, we need to place all
     * the join predicates in a seperate list and find the optimal
     * path of execution among those. In the end, we will concat
     * the optimal path of the join predicates to the result list.
     */
    List *joinPredicatesOnly = new List();

    /* We will start traversing the list of predicates from the head */
    Listnode *currentNode = query->getPredicates()->getHead();

    /* As long as we have not finished traversing the list */

    while(currentNode != NULL)
    {
        /* We retrieve the predicate stored in the current node */

        PredicatesParser *currentPredicate = (PredicatesParser *)
            currentNode->getItem();

        /* Case the current predicate is a filter predicate.
         *                                 ^^^^^^
         * We insert it instantly in the final list.
         */
        if(currentPredicate->hasConstant())
            result->insertLast(currentPredicate);

        /* Case the current predicate is a join predicate.
         *                                 ^^^^
         * We insert it in the new list containing only join predicates.
         */
        else
            joinPredicatesOnly->insertLast(currentPredicate);

        /* We proceed to the next node */
        currentNode = currentNode->getNext();
    }

    result->append(joinPredicatesOnly);
    delete joinPredicatesOnly;

    /* We return the final result */
    return result;
}

/****************************************************************************
 * Frees the allocated memory for the result of 'getOptimalPredicatesOrder' *
 ****************************************************************************/

void QueryOptimizer::deleteOptimalPredicatesOrder(List *optimalPredicatesOrder) const
{
    delete optimalPredicatesOrder;
}
