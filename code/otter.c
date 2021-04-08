#include "otter_platform.h"

#include "otter_math.h"
#include "otter_renderer.h"
#include "otter_file.h"

global f32 time;

#if 0
void
otterUpdateAndRender(otter_Memory* memory,
                     og_OffscreenBuffer* videoBackbuffer);
#endif
OTTER_UPDATE_AND_RENDER(otterUpdateAndRender) {
    otter_GameState* gameState = (otter_GameState*)memory->persistentStorage;
    // TODO(Jai): move away from using localPersist
    localPersist Mesh mesh;
    localPersist i32 depthBuffer[1080 * 1920];
	if (!memory->isInitialized) {
        // STUDY(Jai): Why there is a difference in the allocation address when viwed in the debugger, vs sizeof below
        og_arena_initialize(&gameState->worldArena,
                            ((u8*)memory->persistentStorage + sizeof(otter_GameState)),
                            (memory->persistentStorageSize - sizeof(otter_GameState)));
        og_arena_initialize(&gameState->scratch,
                            memory->transientStorage,
                            memory->transientStorageSize);
        og_file_load_objMesh(thread,
                             &gameState->scratch,
                             &gameState->worldArena,
                             &mesh,
                             memory->fileReadFull,
                             "../data/WeirdAssCat.obj");        
		
		time = 0;
        
        memory->isInitialized = true;
    }
    
    //~ // NOTE(Jai): REFERNCES
	//- https://github.com/OneLoneCoder/videos
    //- https://www.gabrielgambetta.com/computer-graphics-from-scratch/
    for (int i = 0; i < (1920 * 1080); ++i) {
        depthBuffer[i] = OG_INT_MAX;
    } 
    
    f32 screenWidth = (f32)videoBuffer->width;
	f32 screenHeight = (f32)videoBuffer->height;
    
	f32 near = 0.1f;
	f32 far = 1000.0f;
	f32 aspectRatio = screenHeight / screenWidth;
	f32 fovDegrees = 90.0f;
	f32 fovRadians = 1.0f / tanf(fovDegrees * 0.5f / 180.0f * PI);
	V3f camera = {0};
    
    /*
// NOTE(Jai): The Projection Matrix Strucure
               .              0                 1            2                 3
0 |(aspectRatio * fovRadians) ........... .................. ..... |
1 |.........................  fovRadians  .................. ..... |
2 |........................ .............         q            1.0 |
 3 |........................ .............    (-near * q)     ..... |

.      where, q = far / (far - near)
*/
    f32 q = far / (far - near);
	Mat4D projection = {
        .matrix[0][0] = aspectRatio * fovRadians,
        .matrix[1][1] = fovRadians,
        .matrix[2][2] = q,
        .matrix[2][3] = 1.0f,
        .matrix[3][2] = -near * q,
    };
    
    time += 0.009f;
    
    f32 theta = time / (2.0f * PI);
	if (time > 360.0f) { time = 0.0f; }
    
	// Rotation Matrices
	// Rotation Z
	Mat4D rotationZ = {
        .matrix[0][0] = cosf(theta),
        .matrix[0][1] = sinf(theta),
        .matrix[1][0] = -sinf(theta),
        .matrix[1][1] = cosf(theta),
        .matrix[2][2] = 1,
        .matrix[3][3] = 1,
    };
	// Rotation X
	Mat4D rotationX = {
        .matrix[0][0] = 1,
        .matrix[1][1] = cosf(theta * 0.5f),
        .matrix[1][2] = sinf(theta * 0.5f),
        .matrix[2][1] = -sinf(theta * 0.5f),
        .matrix[2][2] = cosf(theta * 0.5f),
        .matrix[3][3] = 1,
    };
    
    Mat4D  awayFromCamera = {
        .matrix[0][0] = 1.0f,
        .matrix[1][1] = 1.0f,
        .matrix[2][2] = 1.0f,
        .matrix[3][2] = 10.0f,
        .matrix[3][3] = 10.0f,
    };
    
    Mat4D translateByOne = {
        .matrix[0][0] = 1.0f,
        .matrix[1][1] = 1.0f,
        .matrix[2][2] = 1.0f,
        .matrix[3][0] = 1.0f,
        .matrix[3][1] = 1.0f,
        .matrix[3][3] = 1.0f,
    };
    
    Mat4D scaling = {
        .matrix[0][0] = (0.5f * screenWidth),
        .matrix[1][1] = (0.5f * screenHeight),
        .matrix[2][2] = 1.0f,
        .matrix[3][3] = 1.0f,
    };
    
	// Draw the triangles
	for (memoryIndex i = 0; i < mesh.triangleCount; ++i) {
		Triangle3f transformedTriangle = mesh.triangles[i];
        
		// Rotate Z
        og_triangle_transform(&transformedTriangle, &rotationZ);
        
		// Rotate X
        og_triangle_transform(&transformedTriangle, &rotationX);
        
		// Translate the triagle in z, away from the camera
        og_triangle_transform(&transformedTriangle, &awayFromCamera);
        
		// Calculate Normals
		V3f line1 = {
			.x = transformedTriangle.points[1].x - transformedTriangle.points[0].x,
			.y = transformedTriangle.points[1].y - transformedTriangle.points[0].y,
			.z = transformedTriangle.points[1].z - transformedTriangle.points[0].z
		};
		V3f line2 = {
			.x = transformedTriangle.points[2].x - transformedTriangle.points[0].x,
			.y = transformedTriangle.points[2].y - transformedTriangle.points[0].y,
			.z = transformedTriangle.points[2].z - transformedTriangle.points[0].z
		};
		V3f normal = og_v3f_cross(line1, line2);
		og_v3f_normalize(&normal);
        
		V3f viewFromCamera = {
			.x = transformedTriangle.points[0].x - camera.x,
			.y = transformedTriangle.points[0].y - camera.y,
			.z = transformedTriangle.points[0].z - camera.z
		};
		og_v3f_normalize(&viewFromCamera);
        
		// only draw the triangles if it is visible
		if (og_v3f_dot(normal, viewFromCamera) < 0) {
			// Illumination
			V3f lightDirection = { 0.0f, 0.0f, -1.0f};
			f32 luminence = og_v3f_dot(normal, lightDirection);
			u32 red = (u32)(128.0f * luminence);
			u32 green = (u32)(128.0f * luminence);
			u32 blue = (u32)(128.0f * luminence);
			u32 fillColour = og_rgba_to_hex(red,
                                            green,
                                            blue,
                                            255);
            
            f32 triangleZValues[3];
            triangleZValues[0] = transformedTriangle.a.z;
            triangleZValues[1] = transformedTriangle.b.z;
            triangleZValues[2] = transformedTriangle.c.z;
            
            // Project the triangle from 3D -> 2D
            og_triangle_transform(&transformedTriangle, &projection);
            
            // Shift the coordiantes from (-1 to +1) to (0 to 2)
            og_triangle_transform(&transformedTriangle, &translateByOne);
            
			// Scale the  triangles into view
            og_triangle_transform(&transformedTriangle, &scaling);
            
			Triangle2f drawTriangle = {
				.a = {transformedTriangle.points[0].x, transformedTriangle.points[0].y },
				.b = {transformedTriangle.points[1].x, transformedTriangle.points[1].y },
				.c = {transformedTriangle.points[2].x, transformedTriangle.points[2].y },
			};
            
            Triangle3f drawTriangle3D = {
				.a = {transformedTriangle.points[0].x, transformedTriangle.points[0].y, triangleZValues[0] },
				.b = {transformedTriangle.points[1].x, transformedTriangle.points[1].y, triangleZValues[1] },
				.c = {transformedTriangle.points[2].x, transformedTriangle.points[2].y, triangleZValues[2] },
			};
            
            f32 zTestF = 500.0f;
            zTestF *= 1024.0f;
            i32 zTest = og_round_floatToI32(zTestF);
            
            P3i testPoint1 = { .z = zTest };
            P3i testPoint = {0};
            
            
#if 0            
            og_renderer_fill_triangle(videoBuffer,
                                      drawTriangle,
                                      fillColour);
#else
            og_renderer_fill_triangle_3D(videoBuffer,
                                         drawTriangle3D,
                                         &depthBuffer[0],
                                         fillColour);
#endif
            
#if 0
            u32 drawColour = og_rgba_to_hex(0, 0, 0, 255);
            og_renderer_draw_triangle(videoBuffer,
                                      drawTriangle,
                                      drawColour);
#endif
		}
	}
    
    og_arena_free(&gameState->scratch);
	i32 end;
}
