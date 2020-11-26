/* date = October 15th 2020 0:05 pm */

#ifndef FPL_OTTER_H
#define FPL_OTTER_H

#include "utils.h"

#if defined(FPL_PLATFORM_WINDOWS)

typedef struct win32_GameCode{
    // either of these callbacks can be 0, check validity before call
    otter_updateAndRender* updateAndRender;
    
    FILETIME lastModifiedTime;
    HMODULE gameLibrary;
    
    b32 isValid;
} win32_GameCode;

#endif

#endif //FPL_OTTER_H
