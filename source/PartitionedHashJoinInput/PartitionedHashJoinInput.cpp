#include <iostream>
#include "PartitionedHashJoinInput.h"

/***************
 * Constructor *
 ***************/

PartitionedHashJoinInput::PartitionedHashJoinInput(const char *config_file)
{
    FileReader::readConfigFile(
        config_file,
        &bitsNumForHashing,
        &showInitialRelations,
        &showAuxiliaryArrays,
        &showHashTable,
        &showSubrelations,
        &showResult,
        &hopscotchBuckets,
        &hopscotchRange,
        &resizableByLoadFactor,
        &loadFactor,
        &maxAllowedSizeModifier,
        &maxPartitionDepth
    );
}

/**************
 * Destructor *
 **************/

PartitionedHashJoinInput::~PartitionedHashJoinInput() {}

/***********************************************
 * Prints the value of each field of the class *
 ***********************************************/

void PartitionedHashJoinInput::print() const
{
    std::cout << "\n       Parameters of the Join input"
        << "\n=========================================="
        << "\nBits num for hashing: " << bitsNumForHashing
        << "\nInitial relations are shown: " << showInitialRelations
        << "\nAuxiliary arrays are shown: " << showAuxiliaryArrays
        << "\nHash tables are shown: " << showHashTable
        << "\nContents of subrelations are shown: " << showSubrelations
        << "\nThe join result is shown: " << showResult
        << "\nAmount of Hopscotch buckets: " << hopscotchBuckets
        << "\nHopscotch range: " << hopscotchRange
        << "\nHash tables can grow by load factor: " << resizableByLoadFactor
        << "\nIf the above is true, the load factor is: " << loadFactor
        << "\nPercentage of the usable cache for probing: " << maxAllowedSizeModifier
        << "\nMaximum partition depth of relations: " << maxPartitionDepth
        << "\n" << std::endl;
}
