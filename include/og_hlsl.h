/* date = May 14th 2021 9:39 am */

#ifndef OG_HLSL_H
#define OG_HLSL_H
typedef float[1] float1;
typedef float[2] float2;
typedef float[3] float3;
typedef float[4] float4;

typedef int[1] int1;
typedef int[2] int2;
typedef int[3] int3;
typedef int[4] int4;

typedef float[4][4] float4x4;
typedef float[3][3] float3x3;
typedef float[4][1] float4x1;
typedef float[1][4] float1x4;

typedef int[4][4] int4x4;
typedef int[3][3] int3x3;
typedef int[4][1] int4x1;
typedef int[1][4] int1x4;

typedef i32 dword;
typedef f32 half;

#define out out
#define SV_Position SV_Position
#define SV_Target SV_Target

#endif //OG_HLSL_H
