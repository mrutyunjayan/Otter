#include "otter_platform.h"

#include "otter_math.h"
#include "otter_renderer.h"
#include "otter_file.h"

global f32 period;

#if 0
void
otterUpdateAndRender(otter_Memory* memory,
                     og_OffscreenBuffer* videoBuffer);
#endif
OTTER_UPDATE_AND_RENDER(otterUpdateAndRender) {
    otter_GameState* gameState = (otter_GameState*)memory->persistentStorage;
	if (!memory->isInitialized) {
        // STUDY(Jai): Why there is a difference in the allocation address when viwed in the debugger, vs sizeof below
        ogAlloc_arena_initialize(&gameState->worldArena,
                                 ((u8*)memory->persistentStorage + sizeof(otter_GameState)),
                                 (memory->persistentStorageSize - sizeof(otter_GameState)));
        ogAlloc_arena_initialize(&gameState->scratch,
                                 memory->transientStorage,
                                 memory->transientStorageSize);
        
        gameState->meshes = (Mesh*)ARENA_PUSH_ARRAY(&gameState->worldArena, 1, Mesh);
        ogFile_load_objMesh(thread,
                            &gameState->scratch,
                            &gameState->worldArena,
                            &gameState->meshes[0],
                            memory->fileReadFull,
                            "../data/african_head.obj");
        period = 0.0f;
        memory->isInitialized = true;
    }
    Mesh mesh = gameState->meshes[0];
    
    //~ // NOTE(Jai): REFERNCES
	//- https://github.com/OneLoneCoder/videos
    //- https://www.gabrielgambetta.com/computer-graphics-from-scratch/
    i32* depthBuffer = ARENA_PUSH_ARRAY(&gameState->scratch, (videoBuffer->width * videoBuffer->height), i32);
    memoryIndex pixelCount = videoBuffer->width * videoBuffer->height;
    memset(&depthBuffer[0], 0, pixelCount * sizeof(depthBuffer[OG_INT_MAX]));
    
    f32 screenWidth = (f32)videoBuffer->width;
    f32 screenHeight = (f32)videoBuffer->height;
    f32 aspectRatio = screenHeight / screenWidth;
    
    V3f camera = {0};
    //period += 0.009f;
    
    f32 theta =  PI;//period / (2.0f * PI);
    if (period > 360.0f) { period = 0.0f; }
    
    // Transformation Matrices
    Mat4D worldMatrix = ogMath_mat4d_make_identity();
    Mat4D rotationX = ogMath_mat4d_make_rotationX(theta);
    worldMatrix = ogMath_mat4d_multiply(rotationX, worldMatrix);
    Mat4D awayFromCamera = ogMath_mat4d_make_translation(0.0f, 0.0f, 1.2f);
    worldMatrix = ogMath_mat4d_multiply(awayFromCamera, worldMatrix);
    
    Mat4D projection = ogMath_mat4d_make_projection(aspectRatio, 106.0f, 0.1f, 1000.0f);
    Mat4D translateByOne = ogMath_mat4d_make_translation(1.0f, 1.0f, 0.0f);
    Mat4D scaling = ogMath_mat4d_make_scaling(0.5f * screenWidth, 0.5f * screenHeight, 1.0f);
    Mat4D projectIntoView = ogMath_mat4d_multiply(translateByOne, projection);
    projectIntoView = ogMath_mat4d_multiply(scaling, projectIntoView);
    
    // Draw the triangles
    for (memoryIndex i = 0; i <= mesh.triangleCount; ++i) {
        Triangle3f transformedTriangle = mesh.triangles[i];
        transformedTriangle = ogRenderer_triangle_transform(&transformedTriangle, &worldMatrix);
        // Calculate Normals
        V3f line1 = ogMath_v3f_subtract(transformedTriangle.p[1], transformedTriangle.p[0]);
        V3f line2 = ogMath_v3f_subtract(transformedTriangle.p[2], transformedTriangle.p[0]);
        V3f normal = ogMath_v3f_cross(line1, line2);
        normal = ogMath_v3f_normalize(normal);
        
        V3f viewFromCamera = ogMath_v3f_subtract(transformedTriangle.p[0], camera);
        viewFromCamera = ogMath_v3f_normalize(viewFromCamera);
        // only draw the triangles if it is visible
        if (ogMath_v3f_dot(normal, viewFromCamera) < 0) {
            // Illumination
            V3f lightDirection = { 0.0f, 0.0f, -1.0f, 0 };
            f32 luminence = ogMath_v3f_dot(normal, lightDirection);
            luminence = MAX(0.1f, luminence);
            Triangle2f textureCoordinates = mesh.textureCoords[i];
            
            u32 red = (u32)(128.0f * luminence);
            u32 green = (u32)(128.0f * luminence);
            u32 blue = (u32)(128.0f * luminence);
            u32 fillColour = ogUtils_rgba_to_hex(red, green, blue, 255);
            
            f32 triangleZValues[3];
            triangleZValues[0] = 1 / transformedTriangle.a.z;
            triangleZValues[1] = 1 / transformedTriangle.b.z;
            triangleZValues[2] = 1 / transformedTriangle.c.z;
            
            // Project the triangle from 3D -> 2D
            transformedTriangle = ogRenderer_triangle_transform_homogeneous(&transformedTriangle, &projectIntoView);
            
            Triangle3f drawTriangle = {
                .a = {transformedTriangle.p[0].x, transformedTriangle.p[0].y, triangleZValues[0], 0 },
                .b = {transformedTriangle.p[1].x, transformedTriangle.p[1].y, triangleZValues[1], 0 },
                .c = {transformedTriangle.p[2].x, transformedTriangle.p[2].y, triangleZValues[2], 0 },
            };
            
            ogRenderer_fill_triangle_3D(videoBuffer,
                                        &drawTriangle,
                                        &depthBuffer[0],
                                        fillColour);
        }
    }
    
    ogAlloc_arena_free(&gameState->scratch);
}
