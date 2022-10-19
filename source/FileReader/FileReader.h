#ifndef _FILE_READER_H_
#define _FILE_READER_H_

/* A namespace that reades data from
 * the input and configuration files
 */
namespace FileReader {

void readInputFile(
    const char *input_file
);

void readConfigFile(
    const char *config_file,
    unsigned int *bitsNumForHashing,
    bool *showInitialRelations,
    bool *showAuxiliaryArrays,
    bool *showSubrelations
);

};

#endif