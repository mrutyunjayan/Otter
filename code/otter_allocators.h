/* date = February 24th 2021 5:25 pm */

#ifndef OTTER_ALLOCATORS_H
#define OTTER_ALLOCATORS_H

#include "utils.h"
//~ CUSTOM ALLOCATORS
// http://www.gingerbill.org/series/memory-allocation-strategies/

//- Arena:
typedef struct {
	u8* buffer; // Pointer to the buffer, must point to an allocated block of 
	//             memory on initiliazation
    memoryIndex bufferSize; // Size of the buffer 
    memoryIndex currentOffset; // The current offset to the end of the used memory, initialize to zero
    memoryIndex memoryBlockStart; // The previous offset, set using function, initialize to zero
} Arena;

internal uintptr 
og_arena_alignForward(uintptr ptr,
                      memoryIndex align) {
	ASSERT(IS_POWER_OF_2(align));
	
	uintptr result, a, modulo;
	result = ptr;
	//modulo = p & 7;
	a = (uintptr)align;
	modulo = result & (a - 1);
	if (modulo != 0) { result += a - modulo; }
	
	return result;
}

internal void*
og_arena_allocAlign(Arena* arena,
                    memoryIndex allocation,
                    memoryIndex align) {
	void* result = 0;
    
    uintptr currentPtr = (uintptr)arena->buffer + (uintptr)arena->currentOffset;
	// Calculate the offset
	uintptr offset = og_arena_alignForward(currentPtr,
                                           align);
	// Change to relative offset
	offset -= (uintptr)arena->buffer; 
	// Check to see if the backing memory has space left
	if (offset + allocation < arena->bufferSize) {
		result = &arena->buffer[offset];
		arena->memoryBlockStart = arena->currentOffset;
		arena->currentOffset = allocation + offset;
        
        // Zero new memory by default
		memset(result, 0, allocation);
	}
	
	return result;
}

#ifndef DEFAULT_ARENA_ALIGNMENT
#define DEFAULT_ARENA_ALIGNMENT (2 * sizeof(void*))
#endif

#define ARENA_PUSH_STRUCT(arena, type) (type*)og_arena_alloc(arena, sizeof(type))
#define ARENA_PUSH_STRUCTALIGNED(arena, type, align) (type*)og_arena_allocAlign(arena, sizeof(type), align)
#define ARENA_PUSH_ARRAY(arena, count, type) (type*)og_arena_alloc(arena, (count * sizeof(type)))
#define ARENA_PUSH_ARRAYALIGNED(arena, count, type, align) (type*)og_arena_allocAlign(arena, (count * sizeof(type)), align)

// Because C doesn't allow default parameters
internal void*
og_arena_alloc(Arena* arena,
               memoryIndex size) {
	return og_arena_allocAlign(arena,
                               size,
                               DEFAULT_ARENA_ALIGNMENT);
}

internal void*
og_arena_resizeAlign(Arena* arena,
                     void* oldMemory,
                     memoryIndex oldSize,
                     memoryIndex newSize,
                     memoryIndex align) {
	ASSERT(IS_POWER_OF_2(align));
	
	if (oldMemory == NULL || oldSize == 0) {
		return og_arena_allocAlign(arena,
                                   newSize,
                                   align);
	} else if (arena->buffer <= (u8*)oldMemory
			   && (u8*)oldMemory < (arena->buffer + arena->bufferSize)) {
		if (arena->buffer + arena->memoryBlockStart == oldMemory) {
			arena->currentOffset = arena->memoryBlockStart + newSize;
			if (newSize > oldSize) {
				memset(&arena->buffer[arena->currentOffset],
					   0,
					   (newSize - oldSize));
			}
			
			return oldMemory;
		} else {
			void* newMemory = og_arena_allocAlign(arena,
                                                  newSize,
                                                  align);
			memoryIndex copySize = oldSize < newSize ? oldSize : newSize;
			memmove(newMemory,
					oldMemory,
					copySize);
			
			return newMemory;
		}
	} else {
		
		ASSERT(0 && "Memory is out of bounds of the buffer in this arena");
		return NULL;
	}
}

// Because C doesn't allow default parameters
internal void*
og_arena_resize(Arena* arena,
                void* oldMemory,
                memoryIndex oldSize,
                memoryIndex newSize) {
	return og_arena_resizeAlign(arena,
                                oldMemory,
                                oldSize,
                                newSize,
                                DEFAULT_ARENA_ALIGNMENT);
}

internal void
og_arena_initialize(Arena* arena,
                    void* backingBuffer,
                    memoryIndex backingBufferLength) {
    arena->buffer = (u8*)backingBuffer;
    arena->bufferSize = backingBufferLength;
    arena->currentOffset = 0;
    arena->memoryBlockStart = 0;
}

// NOTE(Jai): Completely frees the arena
internal void
og_arena_free(Arena* arena) {
	arena->currentOffset = 0;
	arena->memoryBlockStart = 0;
}

#endif //OTTER_ALLOCATORS_H
