/* date = October 9th 2020 11:00 am */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

//~ TYPEDEFS

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t b8;
typedef unsigned int uint;

typedef float f32;
typedef double f64;
typedef uintptr_t uintptr;
typedef size_t memoryIndex;

//~ MACROS

#define localPersist static
#define global static
#define internal static
#define readOnly const
#define false 0
#define true 1

#define PI 3.14159265359f

#define KB(value) (value * 1024LL)
#define MB(value) (KB(value) * 1024LL)
#define GB(value) (MB(value) * 1024LL)
#define TB(value) (GB(value) * 1024LL)

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

#if OGDEBUG_RENDERER
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

#define OG_FLT_MAX FLT_MAX
#define OG_INT_MAX INT_MAX

#define MAX(a, b) a > b ? a : b
#define MIN(a, b) a < b ? a : b
//~ STRING STUFF
internal inline void
ogUtils_catStrings(i64 sourceACount, char* sourceA,
                   i64 sourceBCount, char* sourceB,
                   i64 destCount, char* dest) {
    // TODO(Jai): dest bounds checking
    for (int i = 0; i < sourceACount; ++i) {
        *dest++ = *sourceA++;
    }
    
    for (int i = 0; i < sourceBCount; ++i) {
        *dest++ = *sourceB++;
    }
    
    *dest++ = 0;
}

internal inline int
ogUtils_stringLength(char* string) {
    int count = 0;
    while (*string++) {
        ++count;
    }
    return count;
}

//~------------ Dynamic Array -------------

// TODO(Jai): Implement own dynamic array

//~-------------Swap ----------------------
#define SWAP(a, b) do { \
u8 _swapTemp[(sizeof(a) == sizeof(b)) ? sizeof(a) : -1]; \
memcpy(_swapTemp, &a, sizeof(a)); \
memcpy(&a, &b, sizeof(a)); \
memcpy(&b, _swapTemp, sizeof(a)); \
} while(0)


//~----------- Number Stuff ----------------
// NOTE(Jai): Round up the float value to int
internal inline i32
ogUtils_round_floatToI32(f32 number) {
    return ((i32)(number + 0.5f));
}

internal inline i32
ogUtils_truncate_floatToI32(f32 number) {
    return (i32)number;
}

internal inline i32
ogUtils_floor_floatToI32(f32 number) {
    return (i32)(floorf(number));
}

internal inline u32
ogUtils_rgba_to_hex(u32 red, u32 green, u32 blue, u32 alpha) {
    u32 result = alpha << 24
		| red << 16
        | green << 8
        | blue  << 0;
    
	return result;
}

//- Truncate
internal inline u32
ogUtils_truncate_safe_i64(i64 value) {
    // TODO(Jai): Defines for maximum values (u32_t)
    ASSERT(value <= 0xFFFFFFFF);
    u32 result = (u32)value;
    
    return result;
}

// TODO(Jai): reimplement myself
//- string to numbers
internal inline i64
ogUtils_strtoll(readOnly char* str, char** endPtr, i32 base) {
    return strtoll(str, endPtr, base);
}

internal inline f32
ogUtils_strtof(readOnly char* str, char** endPtr) {
    return strtof(str, endPtr);
}

#endif //UTILS_H
