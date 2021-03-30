#ifndef OTTER_FILE_H
#define OTTER_FILE_H

#include "utils.h"
#include "otter_platform.h"
#include "otter_math.h"

internal i32
og_file_scan_delimited(FileReadResult file,
                       char delimiter,
                       i32 startOffset,
                       char* out) {
    i32 charCount = startOffset;
    if (file.contents) {
        if (startOffset > (i32)file.contentSize) {
            // TODO(Jai): Logging
            return -1;
        }
        char* readChar = (char*)file.contents + startOffset;
        while (*readChar != delimiter) {
            if (readChar == ((char*)file.contents + file.contentSize)) { return -1; } // NOTE(Jai): EOF
            *out++ = *readChar++;
            charCount++;
        }
        
        *out = 00;
        charCount++;
    } else {
        // TODO(Jai): Logging
        return -1;
    }
    
    return charCount;
}

internal inline i32
og_file_scan_line(FileReadResult file,
                  char* out,
                  i32 startOffset) {
    return og_file_scan_delimited(file, '\n', startOffset, out);
}

internal b32
og_file_load_objMesh(ThreadContext* thread,
                     Arena* scratch,
                     Arena* assetData,
                     Mesh* mesh,
                     platformFileReadFull* fileReadFull,
                     char* fileName) {
	b32 result = false;
    FileReadResult file = fileReadFull(thread, fileName);
    
    if (!file.contentSize) { return false; }
    
    V3f* vertices = 0;
    b32 eof = false;
    i32 charsReadFromFile = 0;
    i32 charsPrevReadFromFile = 0;
    V3f* vertexArray = 0;
    b32 vertexArraySet = false;
    Triangle3f* triangleArray = 0;
    b32 triangleArraySet = false;
    char* endPtr;
    i32 verticesFound = 0;
    memoryIndex triangleCount = 0;
    
    while (!eof) {
        char line[128];
        charsReadFromFile = og_file_scan_line(file,
                                              &line[0],
                                              charsReadFromFile);
        eof = (charsReadFromFile == -1);
        
        if (line[0] == 'v') {
            V3f vertexTemp;
            verticesFound++;
            
            vertexTemp.x = og_strtof(&line[2], &endPtr);
            vertexTemp.y = og_strtof(endPtr, &endPtr);
            vertexTemp.z = og_strtof(endPtr, 0);
            
            V3f* scratchLocation = ARENA_PUSH_STRUCTALIGNED(scratch, V3f, 2);
            if (!vertexArraySet) { vertexArray = scratchLocation; vertexArraySet = true; }
            *scratchLocation = vertexTemp;
        }
        charsPrevReadFromFile = charsReadFromFile;
        
        if (line[0] == 'f' && verticesFound > 2) {
            memoryIndex indexTemp[3];
            
            indexTemp[0] = (memoryIndex)og_strtoll(&line[2], &endPtr, 10);
            indexTemp[1] = (memoryIndex)og_strtoll(endPtr, &endPtr, 10);
            indexTemp[2] = (memoryIndex)og_strtoll(endPtr, 0, 10);
            
            Triangle3f triangleTemp = {
                vertexArray[indexTemp[0] - 1],
                vertexArray[indexTemp[1] - 1],
                vertexArray[indexTemp[2] -1]
            };
            
            Triangle3f* lastTriangleLocation = ARENA_PUSH_STRUCTALIGNED(assetData, Triangle3f, 2);
            if (!triangleArraySet) { triangleArray = lastTriangleLocation; triangleArraySet = true;}
            
            *lastTriangleLocation = triangleTemp;
            
            triangleCount++;
        }
    }
    // TODO(Jai): Proper error handling
    if (!triangleCount) { return false; }
    
    assetData->memoryBlockStart = (memoryIndex)((u8*)triangleArray - assetData->buffer);
    
    mesh->triangles = triangleArray;
    mesh->triangleCount = triangleCount;
    result = true;
    
    og_arena_free(scratch);
    
    return result;
}

#endif // OTTER_FILE
