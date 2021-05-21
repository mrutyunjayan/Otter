/* date = January 11th 2021 9:53 pm */

#ifndef WIN32_OG_H
#define WIN32_OG_H

#define WIN32_STATE_FILE_NAME_COUNT MAX_PATH

//~ CLOCK
typedef struct Clock {
    f64 frequency;
    f64 startTime; // in seconds
    f64 elapsed; // in seconds
} Clock;

//~ BACKBUFFER
typedef struct win32_OffScreenBuffer {
    // NOTE(Jai): Pixels are always 32 bits wide, Memory Order: BB GG RR xx
    u32 width;
    u32 height;
    u32 pitch;
    u32 pixelStride;
    BITMAPINFO info;
    void* memory;
} win32_OffScreenBuffer;

typedef struct win32_WindowDimensions{
    int width;
    int height;
} win32_WindowDimensions;

//~ PLATFORM STATE
typedef struct win32_State {
    void* gameMemoryBlock;
    u64 totalSize;
    
    char exeFileName[WIN32_STATE_FILE_NAME_COUNT];
    char* onePastLastSlash;
    
    Clock clock;
    f64 lastTime;
} win32_State;

typedef struct{
    // either of these callbacks can be 0, check validity before call
    otter_updateAndRender* updateAndRender;
    
    FILETIME lastModifiedTime;
    HMODULE gameLibrary;
    
    b8 isValid;
} win32_GameCode;

#endif //WIN32_OG_H
