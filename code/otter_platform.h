/* date = October 15th 2020 0:09 pm */

#ifndef OTTER_PLATFORM_H
#define OTTER_PLATFORM_H

#include "utils.h"
#include "otter_math.h"
#include "otter.h"

#define OTTER_UPDATE_AND_RENDER(name) void name(otter_Memory* memory,\
otter_OffscreenBuffer* videoBackbuffer)
typedef OTTER_UPDATE_AND_RENDER(otter_updateAndRender);

#endif //OTTER_PLATFORM_H
