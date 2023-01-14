#include <iostream>
#include "ColumnSubset.h"

/***************
 * Constructor *
 ***************/

ColumnSubset::ColumnSubset(
    ColumnIdentity *firstColId,
    unsigned int minElement,
    unsigned int maxElement,
    unsigned int elementsNum,
    unsigned int distinctElementsNum)
{
    columnIdentities = new List();

    subsetStats = new ColumnStatistics(
        minElement,
        maxElement,
        elementsNum,
        distinctElementsNum
    );

    totalCost = 0;
}

/**************
 * Destructor *
 **************/

ColumnSubset::~ColumnSubset()
{
    delete subsetStats;
    delete columnIdentities;
}

/**************************************************
 * Getter - Returns the list of column identities *
 **************************************************/

List *ColumnSubset::getColumnsIdentities() const
{
    return columnIdentities;
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

/********************************************
 * Returns the 'pos'-th identity of the set *
 ********************************************/

ColumnIdentity *ColumnSubset::getColumnIdentityInPos(unsigned int pos) const
{
    return (ColumnIdentity *) columnIdentities->getItemInPos(pos);
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
