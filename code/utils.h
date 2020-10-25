/* date = October 9th 2020 11:00 am */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define localPersist static
#define global static
#define internal static
#define false 0
#define true 1

#define PI 3.14159265359f

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

#define kiloBytes(value) (value * 1024LL)
#define megaBytes(value) (kiloBytes(value) * 1024LL)
#define gigaBytes(value) (megaBytes(value) * 1024LL)
#define teraBytes(value) (gigaBytes(value) * 1024LL)

#define arrayCount(array) (sizeof(array) / sizeof(array[0]))

#if OTTER_DEBUG
#undef assert
#define assert(expression)\
if(!(expression)) {\
*(volatile int*)0 = 0;\
}
#else
#define assert(expression)
#endif

#define STRING(name) #name

//~------------ Dynamic Array -------------

// TODO(Jai): Implement own dynamic array

//~-------------Swap ----------------------
#define SWAP(a, b) do { \
u8 swapTemp[(sizeof(a) == sizeof(b)) ? sizeof(a) : -1]; \
memcpy(swapTemp, &a, sizeof(a)); \
memcpy(&b, &a, size(a)); \
memcpy(&a, swap_temp, sizeof(a)); \
} while(0)

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

#endif //UTILS_H
