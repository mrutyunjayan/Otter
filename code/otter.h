/* date = October 12th 2020 2:39 pm */

#ifndef OTTER_H
#define OTTER_H

#include "utils.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <stretchy_buffer.h>
#pragma clang diagnostic pop

typedef struct {
    void* pixels;
    u32 width;
    u32 height;
	u32 aspectRatio;
    u32 pitch;
    u32 pixelStride;
} otter_OffscreenBuffer;

typedef struct {
    void* transientStorage;
    void* persistentStorage;
    
    u64 transientStorageSize;
    u64 persistentStorageSize;
    
    b32 isInitialized;
} otter_Memory;

typedef struct {
    void* assetData;
} otter_GameState;

#include "otter_renderer.h"
#endif //OTTER_H
