#ifndef OTTER_FILE_H
#define OTTER_FILE_H

#include "utils.h"
#include "otter_platform.h"
#include "otter_math.h"
internal i32
ogFile_scan_delimited(FileReadResult file,
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
ogFile_scan_line(FileReadResult file,
                 char* out,
                 i32 startOffset) {
    return ogFile_scan_delimited(file, '\n', startOffset, out);
}

internal b32
ogFile_load_objMesh(ThreadContext* thread,
                    Arena* scratch,
                    Arena* assetData,
                    Mesh* mesh,
                    platformFileReadFull* fileReadFull,
                    char* fileName) {
	b32 result = false;
    FileReadResult file = fileReadFull(thread, fileName);
    
    if (!file.contentSize) { return false; }
    
    b32 eof = false;
    i32 charsReadFromFile = 0;
    i32 charsPrevReadFromFile = 0;
    
    V3f* vertexArray = 0;
    b32 vertexArraySet = false;
    i32 verticesFound = 0;
    
    V2f* textureCoordsArray = 0;
    b32 textureCoordsArraySet = false;
    
    Triangle3f* triangleArray = 0;
    b32 triangleArraySet = false;
    memoryIndex triangleCount = 0;
    memoryIndex textureCoordCount = 0;
    Triangle2f* textureCoordsTempArray = 0;
    b32 textureCoordsTempArraySet = false;
    char* endPtr;
    
    while (!eof) {
        char line[128];
        charsReadFromFile = ogFile_scan_line(file, &line[0], charsReadFromFile);
        eof = (charsReadFromFile == -1);
        if (line[0] == 'v') {
            if (line[1] == 't') {
                V2f textureTemp = {0};
                
                textureTemp.u = ogUtils_strtof(&line[2], &endPtr);
                textureTemp.v = ogUtils_strtof(endPtr, &endPtr);
                f32 junk = ogUtils_strtof(endPtr, 0);
                
                V2f* textureScratch = ARENA_PUSH_STRUCTALIGNED(scratch, V2f, 2);
                if (!textureCoordsArraySet) { textureCoordsArray = textureScratch; textureCoordsArraySet = true; }
                *textureScratch = textureTemp;
                textureCoordCount++;
            } else if (line[1] == 'n') {
                // TODO(Jai): Load normal data
            } else {
                V3f vertexTemp = { .w = 1.0f };
                verticesFound++;
                
                vertexTemp.x = ogUtils_strtof(&line[2], &endPtr);
                vertexTemp.y = ogUtils_strtof(endPtr, &endPtr);
                vertexTemp.z = ogUtils_strtof(endPtr, 0);
                
                V3f* vertexScratch = ARENA_PUSH_STRUCTALIGNED(scratch, V3f, 2);
                if (!vertexArraySet) { vertexArray = vertexScratch; vertexArraySet = true; }
                *vertexScratch = vertexTemp;
            }
        }
        charsPrevReadFromFile = charsReadFromFile;
        
        if (line[0] == 'f' && verticesFound > 2) {
            // TODO(Jai): Read Normal data properly
            memoryIndex vIndexTemp[3];
            memoryIndex tIndexTemp[3];
            
            vIndexTemp[0] = (memoryIndex)ogUtils_strtoll(&line[2], &endPtr, 10);
            if (*endPtr == '/') { tIndexTemp[0] = (memoryIndex)ogUtils_strtoll(endPtr + 1, &endPtr, 10); }
            if (*endPtr == '/') { i64 junk = ogUtils_strtoll(endPtr + 1, &endPtr, 10); }
            
            vIndexTemp[1] = (memoryIndex)ogUtils_strtoll(endPtr, &endPtr, 10);
            if (*endPtr == '/') { tIndexTemp[1] = (memoryIndex)ogUtils_strtoll(endPtr + 1, &endPtr, 10); }
            if (*endPtr == '/') { i64 junk = ogUtils_strtoll(endPtr + 1, &endPtr, 10); }
            
            vIndexTemp[2] = (memoryIndex)ogUtils_strtoll(endPtr, &endPtr, 10);
            if (*endPtr == '/') { tIndexTemp[2] = (memoryIndex)ogUtils_strtoll(endPtr + 1, &endPtr, 10); }
            if (*endPtr == '/') { i64 junk = ogUtils_strtoll(endPtr + 1, &endPtr, 10); }
            
            Triangle3f triangleTemp = {
                vertexArray[vIndexTemp[0] - 1],
                vertexArray[vIndexTemp[1] - 1],
                vertexArray[vIndexTemp[2] - 1]
            };
            Triangle2f textureCoordsTemp = {
                textureCoordsArray[tIndexTemp[0] - 1],
                textureCoordsArray[tIndexTemp[1] - 1],
                textureCoordsArray[tIndexTemp[2] - 1]
            };
            
            Triangle3f* lastTriangleLocation = ARENA_PUSH_STRUCTALIGNED(assetData, Triangle3f, 2);
            Triangle2f* textureCoordsLocation = ARENA_PUSH_STRUCTALIGNED(scratch, Triangle2f, 2);
            if (!triangleArraySet) { triangleArray = lastTriangleLocation; triangleArraySet = true;}
            if (!textureCoordsTempArraySet) { textureCoordsTempArray = textureCoordsLocation; textureCoordsTempArraySet = true;}
            *lastTriangleLocation = triangleTemp;
            *textureCoordsLocation = textureCoordsTemp;
            triangleCount++;
        }
    }
    // TODO(Jai): Proper error handling
    if (!triangleCount) { return false; }
    
    ARENA_PUSH_ARRAY(assetData, 1, u64); // NOTE(Jai): Padding
    Triangle2f* triTextureCoords = ARENA_PUSH_ARRAYALIGNED(assetData, textureCoordCount, Triangle2f, 2);
    memcpy(triTextureCoords, textureCoordsTempArray, triangleCount * sizeof(Triangle2f));
    assetData->memoryBlockStart = (memoryIndex)((u8*)triangleArray - assetData->buffer);
    
    mesh->triangles = triangleArray;
    mesh->textureCoords = triTextureCoords;
    mesh->triangleCount = triangleCount;
    mesh->textureCoordCount = textureCoordCount;
    result = true;
    
    ogAlloc_arena_free(scratch);
    
    return result;
}

#endif // OTTER_FILE
