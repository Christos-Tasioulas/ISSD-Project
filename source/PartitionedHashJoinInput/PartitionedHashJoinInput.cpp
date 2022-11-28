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
