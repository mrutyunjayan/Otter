/* date = October 9th 2020 11:00 am */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

//~ TYPEDEFS

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int32_t b32;
typedef unsigned int uint;

typedef float f32;
typedef double f64;
typedef uintptr_t p64;

//~ MACROS

#define localPersist static
#define global static
#define internal static
#define false 0
#define true 1

#define PI 3.14159265359f

#define KB(value) (value * 1024LL)
#define MB(value) (KB(value) * 1024LL)
#define GB(value) (MB(value) * 1024LL)
#define TB(value) (GB(value) * 1024LL)

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

#if OTTER_DEBUG
#undef ASSERT
#define ASSERT(expression)\
if(!(expression)) {\
*(volatile int*)0 = 0;\
}
#else
#define ASSERT(expression)
#endif

#define STRING(name) #name

#define IS_POWER_OF_2(x) ((x & (x - 1)) == 0) 

//~------------ Dynamic Array -------------

// TODO(Jai): Implement own dynamic array

//~-------------Swap ----------------------
#define SWAP(a, b) do { \
u8 swapTemp[(sizeof(a) == sizeof(b)) ? sizeof(a) : -1]; \
memcpy(swapTemp, &a, sizeof(a)); \
memcpy(&a, &b, sizeof(a)); \
memcpy(&b, swapTemp, sizeof(a)); \
} while(0)


//~ CUSTOM ALLOCATORS
// http://www.gingerbill.org/series/memory-allocation-strategies/

//~ Arena:

typedef struct {
	u8* buffer; // Pointer to the buffer, must point to an allocated block of 
	//             memory on initiliazation
	u64 bufferSize; // Size of the buffer 
	u64 currentOffset; // The current offset to the end of the used memory, initialize to zero
	u64 memoryBlockStart; // The previous offset, set using function, initialize to zero
} Arena;

internal p64 
alignForward(p64 ptr,
			 u64 align) {
	
	ASSERT(IS_POWER_OF_2(align));
	
	p64 result, a, modulo;
	
	result = ptr;
	size_t size = sizeof(void*);
	
	//modulo = p & 7;
	a = (p64)align;
	modulo = result & (a - 1);
	
	
	if (modulo != 0) {
		
		result += a - modulo;
	}
	
	return result;
}

internal void*
arena_allocAlign(Arena* arena,
				 u64 allocation,
				 u64 align) {
	
	p64 currentPtr = (p64)arena->buffer + (p64)arena->currentOffset;
	// Calculate the offset
	p64 offset = alignForward(currentPtr,
							  align);
	
	// Change to relative offset
	offset -= (p64)arena->buffer; 
	
	// Check to see if the backing memory has space left
	if (offset + allocation < arena->bufferSize) {
		
		void* ptr = &arena->buffer[offset];
		arena->memoryBlockStart = arena->currentOffset;
		arena->currentOffset = offset + allocation;
		
		return ptr;
	}
	
	return NULL;
}

#ifndef DEFAULT_ARENA_ALIGNMENT
#define DEFAULT_ARENA_ALIGNMENT (2 * sizeof(void*))
#endif

// Because C doesn't allow default parameters
internal void*
arena_alloc(Arena* arena,
			size_t size) {
	
	return arena_allocAlign(arena,
							size,
							DEFAULT_ARENA_ALIGNMENT);
}

internal void*
arena_resizeAlign(Arena* arena,
				  void* oldMemory,
				  u64 oldSize,
				  u64 newSize,
				  u64 align) {
	
	ASSERT(IS_POWER_OF_2(align));
	
	if (oldMemory == NULL || oldSize == 0) {
		
		return arena_allocAlign(arena,
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
			
			void* newMemory = arena_allocAlign(arena,
											   newSize,
											   align);
			u64 copySize = oldSize < newSize ? oldSize : newSize;
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
arena_resize(Arena* arena,
			 void* oldMemory,
			 u64 oldSize,
			 u64 newSize) {
	
	return arena_resizeAlign(arena,
							 oldMemory,
							 oldSize,
							 newSize,
							 DEFAULT_ARENA_ALIGNMENT);
}

void
arena_free(Arena* arena) {
	
	memset(&arena->buffer,
		   0,
		   arena->bufferSize);
	arena->currentOffset = 0;
	arena->memoryBlockStart = 0;
}

//~----------- Number Stuff ----------------
// NOTE(Jai): Round up the float value to int
inline internal i32
round_floatToI32(f32 number) {
    return ((i32)(number + 0.5f));
}

inline internal i32
truncate_floatToI32(f32 number) {
    return (i32)number;
}

inline internal i32
floor_floatToI32(f32 number) {
    return (i32)(floorf(number));
}

inline internal u32
rgbaToHex(u32 red, u32 green, u32 blue, u32 alpha) {
	
    u32 result = alpha << 24
		| red << 16
        | green << 8
        | blue  << 0;
    
	return result;
}
#endif //UTILS_H
