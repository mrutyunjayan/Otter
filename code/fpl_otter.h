/* date = October 15th 2020 0:05 pm */

#ifndef FPL_OG_H
#define FPL_OG_H

#include "utils.h"

typedef struct win32_GameCode{
    // either of these callbacks can be 0, check validity before call
    otter_updateAndRender* updateAndRender;
    
    FILETIME lastModifiedTime;
    HMODULE gameLibrary;
    
    b8 isValid;
} win32_GameCode;

#endif

#endif //FPL_OG_H
