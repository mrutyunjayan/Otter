/* date = October 15th 2020 0:09 pm */

#ifndef OTTER_PLATFORM_H
#define OTTER_PLATFORM_H

#include "otter_allocators.h"
#include "otter.h"

//~
//*************************************************************************
// ------Services that the platform player provides to the game------------
//*************************************************************************

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

b32 
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

#define PLATFORM_FILE_WRITE_FULL(name) b32 name(ThreadContext* thread,\
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
    
    b32 isInitialized;
    
    platformFileReadFull* fileReadFull;
    platformFileFreeMemory* fileFreeMemory;
    platformFileWriteFull* fileWriteFull;
} otter_Memory;

#define OTTER_UPDATE_AND_RENDER(name) void name(ThreadContext* thread,\
otter_Memory* memory,\
og_OffscreenBuffer* videoBuffer)
typedef OTTER_UPDATE_AND_RENDER(otter_updateAndRender);
#endif //OTTER_PLATFORM_H
