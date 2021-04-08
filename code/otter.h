/* date = October 12th 2020 2:39 pm */

#ifndef OTTER_H
#define OTTER_H

#include "utils.h"
#include "otter_allocators.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <stretchy_buffer.h>
#pragma clang diagnostic pop

typedef struct og_OffscreenBuffer {
    void* pixels;
    u32 width;
    u32 height;
	u32 aspectRatio;
    u32 pitch;
    u32 pixelStride;
} og_OffscreenBuffer;

typedef struct otter_GameState {
    void* assetData;
    
    Arena scratch;
    Arena worldArena;
} otter_GameState;

#endif //OTTER_H
