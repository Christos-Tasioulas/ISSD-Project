#include <iostream>
#include "ColumnSubset.h"

/***************
 * Constructor *
 ***************/

ColumnSubset::ColumnSubset(
    ColumnIdentity *firstColId,
    ColumnIdentity *neighborOfFirst,
    PredicatesParser *predBetweenTheTwo,
    ColumnStatistics *subsetStats)
{
    /* We initialize the list of columns */
    columnIdentities = new List();
    columnIdentities->insertLast(firstColId);
    columnIdentities->insertLast(neighborOfFirst);

    /* We initialize the list of predicates */
    predicatesOrder = new List();
    predicatesOrder->insertLast(predBetweenTheTwo);

    /* We initialize the statistics of the subset */
    this->subsetStats = subsetStats;

    /* We initialize the cost to the estimated
     * number of tuples of the intermediate result
     * of join between the two given columns
     */
    totalCost = subsetStats->getElementsNum();
}

/***************
 * Constructor *
 ***************/

ColumnSubset::ColumnSubset(
    ColumnSubset *existingSubset,
    ColumnIdentity *nextColId,
    PredicatesParser *predBetweenLastAndNext,
    ColumnStatistics *newStats)
{
    /* We initialize the list of columns */
    columnIdentities = new List();
    columnIdentities->append(existingSubset->columnIdentities);
    columnIdentities->insertLast(nextColId);

    /* We initialize the list of predicates */
    predicatesOrder = new List();
    predicatesOrder->append(existingSubset->predicatesOrder);
    predicatesOrder->insertLast(predBetweenLastAndNext);

    /* We initialize the statistics of the subset */
    this->subsetStats = newStats;

    /* The cost of the new subset is the cost of the previously existing subset plus
     * the cost of the intermediate result of join between the subset and 'nextColId'
     */
    totalCost = existingSubset->subsetStats->getElementsNum() + newStats->getElementsNum();
}

/**************
 * Destructor *
 **************/

ColumnSubset::~ColumnSubset()
{
    /* We delete the stats because in the constructor
     * the given stats were allocated with 'new' or
     * they were changed with other stats that were
     * allocated with 'new' as well.
     */
    delete subsetStats;

    /* Also we delete the list of column identities */
    delete columnIdentities;

    /* We delete the list of predicates order as well */
    delete predicatesOrder;
}

/**************************************************
 * Getter - Returns the list of column identities *
 **************************************************/

List *ColumnSubset::getColumnsIdentities() const
{
    return columnIdentities;
}

/********************************************
 * Getter - Returns the order of predicates *
 ********************************************/

List *ColumnSubset::getPredicatesOrder() const
{
    return predicatesOrder;
}

/****************************************************
 * Getter - Returns the current stats of the subset *
 ****************************************************/

ColumnStatistics *ColumnSubset::getSubsetStats() const
{
    return subsetStats;
}

/********************************************
 * Getter - Returns the total cost to reach *
 *  the intermediate result of this subset  *
 ********************************************/

unsigned long long ColumnSubset::getTotalCost() const
{
    return totalCost;
}

/********************************************
 * Adds a new column identity to the subset *
 ********************************************/

void ColumnSubset::insertColumnIdentity(ColumnIdentity *colId)
{
    columnIdentities->insertLast(colId);
}

/********************************************************
 * Returns the most newly inserted column of the subset *
 ********************************************************/

ColumnIdentity *ColumnSubset::getLastColumn() const
{
    return (ColumnIdentity *) columnIdentities->getTail()->getItem();
}

/********************************************
 * Returns the 'pos'-th identity of the set *
 ********************************************/

ColumnIdentity *ColumnSubset::getColumnIdentityInPos(unsigned int pos) const
{
    return (ColumnIdentity *) columnIdentities->getItemInPos(pos);
}

/******************************************************************************
 * Updates the given 'neighborsList' with all the available column identities *
 *   that can be joined to the subset and the 'neighborPredsList' with the    *
 *   corresponding join predicates that connect these columns with the set    *
 ******************************************************************************/

void ColumnSubset::getNeighbors(List **neighborsList, List **neighborPredsList) const
{
    /* We initialize both lists */
    (*neighborsList) = new List();
    (*neighborPredsList) = new List();

    /* We will traverse the list of column identities */
    Listnode *current = columnIdentities->getHead();

    /* As long as we have not reached the end of the list */
    while(current != NULL)
    {
        /* We retrieve the column stored in the current node */
        ColumnIdentity *colId = (ColumnIdentity *) current->getItem();

        /* For each neighbor of the column, we examine which ones do not
         * exist already in the subset. All those are possible successors
         * for the next join in the subset.
         */
        Listnode *neighborNode = colId->getNeighbors()->getHead();
        Listnode *neighborPredNode = colId->getNeighborPredicates()->getHead();

        /* As long as we have not examined all the neighbors */
        while(neighborNode != NULL)
        {
            /* We retrieve the neighbor in the current node */
            ColumnIdentity * currentNeighbor = (ColumnIdentity *)
                neighborNode->getItem();

            /* We retrieve the corresponding join predicate */
            PredicatesParser *currentNeighborPred = (PredicatesParser *)
                neighborPredNode->getItem();

            /* If the neighbor exists in the subset, we
             * proceed immediatelly to the next neighbor
             */
            if(!exists(currentNeighbor))
            {
                (*neighborsList)->insertLast(currentNeighbor);
                (*neighborPredsList)->insertLast(currentNeighborPred);
            }

            /* We proceed to the next neighbor */
            neighborNode = neighborNode->getNext();
            neighborPredNode = neighborPredNode->getNext();
        }

        /* We proceed to the next node */
        current = current->getNext();
    }
}

/********************************************************************
 * Frees the allocated memory for the lists built by 'getNeighbors' *
 ********************************************************************/

void ColumnSubset::freeNeighbors(List *neighborsList, List *neighborPredsList)
{
    delete neighborsList;
    delete neighborPredsList;
}

/***********************************
 * Changes the stats of the subset *
 ***********************************/

void ColumnSubset::changeStats(
    unsigned int newMinElement,
    unsigned int newMaxElement,
    unsigned int newElementsNum,
    unsigned int newDistinctElementsNum)
{
    subsetStats->setMinElement(newMinElement);
    subsetStats->setMaxElement(newMaxElement);
    subsetStats->setElementsNum(newElementsNum);
    subsetStats->setDistinctElementsNum(newDistinctElementsNum);
}

/********************************************************************
 * Returns 'true' if the given column identity exists in the subset *
 ********************************************************************/

bool ColumnSubset::exists(ColumnIdentity *colId) const
{
    /* We retrieve the ID of the given column identity */
    unsigned int idOfCol = colId->getId();

    /* We will traverse the list of column identities of the set */
    Listnode *currentNode = columnIdentities->getHead();

    /* As long as we have not finished traversing the list */
    while(currentNode != NULL)
    {
        /* We retrieve the column identity stored in the current node */
        ColumnIdentity *nextColId = (ColumnIdentity *) currentNode->getItem();

        /* If the ID of the current column matches the given one,
         * the given column exists in the set, so we return 'true'
         */
        if(nextColId->getId() == idOfCol)
            return true;

        /* We proceed to the next node */
        currentNode = currentNode->getNext();
    }

    /* If this part is reached, that means no column identity
     * with the same ID as the given one was found in the subset.
     * Consequently, the given column does not exist in the set.
     * In this case, we return 'false'.
     */
    return false;
}








/*

bool is_connected(ColumnSubset *cs1, ColumnSubset *cs2);

unsigned int hf(void *cs);

int compare(void *l1, void *l2);

unsigned int Find_Cost(List* columns);

List* find_Subsets(ColumnSubset **R, int n, int i);

HashTable *JoinEnumeration(int n, ColumnSubset **R)
{
    int i,j, size=1;
    for(i=0; i<n; ++i)
    {
        size=2*size;
    }

    HashTable *BestTree = new HashTable(size);
    for(i=0; i<n; ++i)
    {
        List *R_Value = new List();
        R_value->InsertLast(R[i]); 
        BestTree->insert(R_value, R[i], hf);
    }

    for(i=0; i<n; ++i)
    {
        ColunmSubset *S;
        List *Subsets = new List();
        Subsets = find_Subsets(R, n, i);
        ListNode* node = Subsets->get_head();

        while(node != NULL)
        {
            List* Subset = node->get_value();
            S = new ColumnSubset(Subset);

            for(j=0; j<n; ++j)
            {
                if(Subset->search(R[j])) continue;

                if(!is_connected(S, R[j])) continue;

                ColumnSubset *new_S = new ColumnSubset(S, R[j]);
                List *S_Value = BestTree->search_item(S, hf, compare);
                S_Value->InsertLast(R[j]);
                List *CurrTree = S_Value;

                if(BestTree->search(new_S) == NULL || Find_Cost(BestTree->get_value(new_S)) > Find_Cost(CurrTree->getCost()))
                {
                    BestTree->insert(CurrTree, new_S, hf);
                }
            }

            node=node->get_next();
        }

        destroy_Subsets(Subsets);
        delete Subsets;
    }

    return BestTree;
}   
    

*/
