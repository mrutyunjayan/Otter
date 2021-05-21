/* date = October 15th 2020 0:09 pm */

#ifndef OG_PLATFORM_H
#define OG_PLATFORM_H

#include "otter_allocators.h"
#include "otter.h"

//~
//*************************************************************************
// ------Services that the platform player provides to the game------------
//*************************************************************************
typedef struct og_ButtonState {
    int halfTransitionCount;
    b8 endedDown;
} og_ButtonState;

typedef struct og_ControllerInput {
    b8 isConnected;
    b8 isAnalog;
    
    f32 stickAverageX;
    f32 stickAverageY;
    
    union {
        og_ButtonState buttons[12];
        
        struct {
            og_ButtonState moveUp;
            og_ButtonState moveDown;
            og_ButtonState moveLeft;
            og_ButtonState moveRight;
            
            og_ButtonState actionUp;
            og_ButtonState actionDown;
            og_ButtonState actionLeft;
            og_ButtonState actionRight;
            
            og_ButtonState leftShoulder;
            og_ButtonState rightShoulder;
            
            og_ButtonState start;
            og_ButtonState back;
        };
    };
} og_ControllerInput;

typedef struct og_Input {
    og_ButtonState mouseButtons[5];
    i32 mouseX;
    i32 mouseY;
    i32 mouseZ;
    
    f32 deltaTimeForFrame;
    og_ControllerInput controllers[5];
} og_Input;

typedef struct FileReadResult {
    u32 contentSize;
    void* contents;
} FileReadResult;

typedef struct ThreadContext {
    u8 placeholder;
} ThreadContext;

#if 0
FileReadResult
platformFileReadFull(char* fileName);

void
platformFileFreeMemory(void* bitmapMemory);

b8
platformFileWriteFull(char* fileName,
                      u32 memorySize,
                      void* memory);

i32
platformFileScanDelimited(ThreadContext* thread,
                          FileReadResult file,
                          char delimiter,
                          i32 startOffset,
                          char* out);
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"

#define PLATFORM_FILE_FREE_MEMORY(name) void name(ThreadContext* thread, void* memory)
typedef PLATFORM_FILE_FREE_MEMORY(platformFileFreeMemory);

#define PLATFORM_FILE_READ_FULL(name) FileReadResult name(ThreadContext* thread,\
char* fileName)
typedef PLATFORM_FILE_READ_FULL(platformFileReadFull);

#define PLATFORM_FILE_WRITE_FULL(name) b8 name(ThreadContext* thread,\
char* fileName,\
u32 memorySize,\
void* memory)
typedef PLATFORM_FILE_WRITE_FULL(platformFileWriteFull);

#pragma clang diagnostic pop

//~
//*************************************************************************
// ------Services that the game provides to the platform layer-------------
//*************************************************************************

typedef struct otter_Memory {
    void* transientStorage;
    void* persistentStorage;
    
    u64 transientStorageSize;
    u64 persistentStorageSize;
    
    b8 isInitialized;
    
    platformFileReadFull* fileReadFull;
    platformFileFreeMemory* fileFreeMemory;
    platformFileWriteFull* fileWriteFull;
} otter_Memory;

internal og_ControllerInput*
getController(og_Input* input,
              uint controllerIndex) {
    ASSERT(controllerIndex < ARRAY_COUNT(input->controllers));
    og_ControllerInput* result = &input->controllers[controllerIndex];
    
    return result;
}
//

#define OG_UPDATE_AND_RENDER(name) void name(ThreadContext* thread,\
otter_Memory* memory,\
og_OffscreenBuffer* videoBuffer,\
og_Input* input)
typedef OG_UPDATE_AND_RENDER(otter_updateAndRender);


#endif //OG_PLATFORM_H
