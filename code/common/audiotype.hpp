
#ifndef AUDIOTYPE_HEADER
#define AUDIOTYPE_HEADER

#include <vector>

typedef unsigned long PaSampleFormat;
#define paInt16          ((PaSampleFormat) 0x00000008)

const int INPUT_SAMPLERATE = 16000;
const int INPUT_TRUNKSIZE = 160;
const PaSampleFormat INPUT_FORMAT = paInt16;
using INPUT_TYPE = short;
const int INPUT_CHANNELS = 4;
const int INPUT_TRUNKSIZE_BYTE = INPUT_TRUNKSIZE*sizeof(short);

const int OUTPUT_SAMPLERATE = 16000;
const int OUTPUT_TRUNKSIZE = 160;
const PaSampleFormat OUTPUT_FORMAT = paInt16;
using OUTPUT_TYPE = short;
const int OUTPUT_CHANNELS = 1;

typedef std::vector<INPUT_TYPE> AudioInputArray;
typedef std::vector<OUTPUT_TYPE> AudioOutputArray;

typedef struct
{
    AudioInputArray data[4];
} AudioInputStruct;

typedef struct
{
    INPUT_TYPE data[INPUT_CHANNELS * INPUT_TRUNKSIZE];
} AudioInputBuffer;

#endif
