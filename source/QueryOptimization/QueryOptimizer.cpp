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

List *QueryOptimizer::getOptimalPredicatesOrderRec(
    List *chosenPredicates,
    unsigned long long *cost) const
{
    // /* We retrieve the amount of predicates in the query */
    // unsigned int predicatesNum = chosenPredicates->getCounter();

    // /* Case there is only one predicate */

    // if(predicatesNum == 1)
    // {
    //     /* We will return the given list itself */
    //     List *result = new List();
    //     result->append(chosenPredicates);

    //     /* We update the cost if the user wants */
    //     if(cost != NULL)
    //         (*cost) = 0;

    //     return result;
    // }

    // unsigned int i;

    // for(i = 1; i <= predicatesNum; i++)
    // {
    //     PredicatesParser *ith_predicate = (PredicatesParser *) queryPredicates->getItemInPos(i);
    //     queryPredicates->removePos(i);

        

    //     queryPredicates->insertBeforePos(ith_predicate, i);
    // }
    List *result = new List();
    result->append(chosenPredicates);
    return result;
}

/****************************************************************************
 * Returns a list of predicates in the order that is estimated most optimal *
 ****************************************************************************/

List *QueryOptimizer::getOptimalPredicatesOrder() const
{
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
            joinPredicatesOnly->insertLast(currentPredicate);

        /* We proceed to the next node */
        currentNode = currentNode->getNext();
    }

    

    /* We find the optimal order among the given join predicates */
    List *joinPredicatesInOptimalOrder =
        getOptimalPredicatesOrderRec(joinPredicatesOnly);

    /* We concatenate that order to the filter predicates */
    result->append(joinPredicatesInOptimalOrder);

    /* We free the allocated memory for the temporary lists */
    delete joinPredicatesInOptimalOrder;
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
