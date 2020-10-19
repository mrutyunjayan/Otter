/* date = October 12th 2020 2:39 pm */

#ifndef OTTER_H
#define OTTER_H

#include "utils.h"

typedef struct {
    void* pixels;
    u32 width;
    u32 height;
    u64 pitch;
    u64 pixelStride;
} otter_OffscreenBuffer;

typedef struct {
    void* transientStorage;
    void* persistentStorage;
    
    u64 transientStorageSize;
    u64 persistentStorageSize;
    
    b32 isInitialized;
} otter_Memory;

#endif //OTTER_H
