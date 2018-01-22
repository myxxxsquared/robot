
#ifndef AUDIOTYPE_HEADER
#define AUDIOTYPE_HEADER

#include <vector>
#include <cstring>

typedef unsigned long PaSampleFormat;
#define paInt16          ((PaSampleFormat) 0x00000008)

const int INPUT_SAMPLERATE = 16000;
const int INPUT_TRUNKSIZE = 160;
const PaSampleFormat INPUT_FORMAT = paInt16;
using INPUT_TYPE = short;
const int INPUT_CHANNELS = 4;
const int INPUT_TRUNKSIZE_BYTE = INPUT_TRUNKSIZE*sizeof(short);

const int OUTPUT_SAMPLERATE = 16000;
const int OUTPUT_TRUNKSIZE = 1600;
const PaSampleFormat OUTPUT_FORMAT = paInt16;
using OUTPUT_TYPE = short;
const int OUTPUT_CHANNELS = 1;
const int OUTPUT_TRUNKSIZE_BYTE = OUTPUT_TRUNKSIZE*sizeof(short);

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

struct AudioOutputBuffer
{
    OUTPUT_TYPE data[OUTPUT_CHANNELS * OUTPUT_TRUNKSIZE];
    inline AudioOutputBuffer() {}
    inline AudioOutputBuffer(const void *buffer)
    {
        memcpy(data, buffer, sizeof(AudioOutputBuffer));
    }
};

#endif
