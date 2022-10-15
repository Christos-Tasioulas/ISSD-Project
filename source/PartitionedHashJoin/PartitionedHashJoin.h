#ifndef _PARTITIONED_HASH_JOIN_H
#define _PARTITIONED_HASH_JOIN_H

#include "HashTable.h"
#include "Relation.h"
#include "RowIdPair.h"

/* Reads the input relational arrays and executes
 * the Partitioned Hash Join Algorithm to perform
 * the Join operation between them
 */
void execute_PHJ();



#endif
