#include "otter_platform.h"

#include "otter_math.c"
#include "otter_renderer.c"

global f32 time;

//void
//otter_updateAndRender(otter_Memory* memory,
//                      otter_OffscreenBuffer* videoBackbuffer) 
OTTER_UPDATE_AND_RENDER(otterUpdateAndRender) {
    
    otter_GameState* gameState = (otter_GameState*)memory->persistentStorage;
	gameState->assetData = gameState + sizeof(otter_GameState);
    Mesh* meshCube = (Mesh*)gameState->assetData;
	
	
	// NOTE(Jai): Using dynamic array for faster protyping for now
	// TODO(Jai): Get rid dynamic array and switch to using the arena for asset data
	if (!memory->isInitialized) {
        //Generate Cube
        //SOUTH
        {
            Triangle3f triTemp = { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f};
            sb_push(meshCube->triangles, triTemp);
        }
        
        {
            Triangle3f triTemp = { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        //EAST
        {
            Triangle3f triTemp = { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        {
            Triangle3f triTemp = { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        //NORTH
        {
            Triangle3f triTemp ={ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        {
            Triangle3f triTemp = { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        //WEST
        {
            Triangle3f triTemp = { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        {
            Triangle3f triTemp = { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        //TOP
        {
            Triangle3f triTemp = { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        {
            Triangle3f triTemp = { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        //BOTTOM
        {
            Triangle3f triTemp = { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        {
            Triangle3f triTemp = { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
		time = 0;
		
        memory->isInitialized = true;
    }
	
	f32 screenWidth = (f32)videoBackbuffer->width;
	f32 screenHeight = (f32)videoBackbuffer->height;
	
	f32 near = 0.1f;
	f32 far = 1000.0f;
	f32 aspectRatio = screenHeight / screenWidth;
	f32 fovDegrees = 90.0f;
	f32 fovRadians = 1.0f / tanf(fovDegrees * 0.5f / 180.0f * PI);
	
	Mat4x4 projection = {0};
	projection.matrix[0][0] = aspectRatio * fovRadians;
	projection.matrix[1][1] = fovRadians;
	projection.matrix[3][2] = far / (far - near);
	projection.matrix[2][3] = 1.0f;
	projection.matrix[3][3] = 0.0f;
	
	// Rotation Matrices
	Mat4x4 rotationZ = {0};
	Mat4x4 rotationX = {0};
	
	time += 0.01f;
	f32 theta = time / (2.0f * PI);
	if (time > 360.0f) { time = 0.0f; }
	
	// Rotation Z
	rotationZ.matrix[0][0] = cosf(theta);
	rotationZ.matrix[0][1] = sinf(theta);
	rotationZ.matrix[1][0] = -sinf(theta);
	rotationZ.matrix[1][1] = cosf(theta);
	rotationZ.matrix[2][2] = 1;
	rotationZ.matrix[3][3] = 1;
	
	// Rotation X
	rotationX.matrix[0][0] = 1;
	rotationX.matrix[1][1] = cosf(theta * 0.5f);
	rotationX.matrix[1][2] = sinf(theta * 0.5f);
	rotationX.matrix[2][1] = -sinf(theta * 0.5f);
	rotationX.matrix[2][2] = cosf(theta * 0.5f);
	rotationX.matrix[3][3] = 1;
	
	// Draw the triangles
	for (u32 i = 0; i < 12; ++i) {
		
		Triangle3f projectedTriangle = {0};
		Triangle3f translatedTriangle = {0};
		Triangle3f rotatedTriangleZ = {0};
		Triangle3f rotatedTriangleZX = {0};
#if 1
		
		// Rotate Z
		rotatedTriangleZ.points[0] = transformVector3D(meshCube->triangles[i].points[0],
													   &rotationZ);
		rotatedTriangleZ.points[1] = transformVector3D(meshCube->triangles[i].points[1],
													   &rotationZ);
		rotatedTriangleZ.points[2] = transformVector3D(meshCube->triangles[i].points[2],
													   &rotationZ);
		
		// Rotate X
		rotatedTriangleZX.points[0] = transformVector3D(rotatedTriangleZ.points[0],
														&rotationX);
		rotatedTriangleZX.points[1] = transformVector3D(rotatedTriangleZ.points[1],
														&rotationX);
		rotatedTriangleZX.points[2] = transformVector3D(rotatedTriangleZ.points[2],
														&rotationX);
		
		// Translate the triagle in z, away from the camera
		translatedTriangle = rotatedTriangleZX;
#else
		translatedTriangle = meshCube->triangles[i];
#endif
		translatedTriangle.points[0].z += 2.0f;
		translatedTriangle.points[1].z += 2.0f;
		translatedTriangle.points[2].z += 2.0f;
		
		// Project triangles from 3D -> 2D
		projectedTriangle.points[0] = transformVector3D(translatedTriangle.points[0],
														&projection);
		projectedTriangle.points[1] = transformVector3D(translatedTriangle.points[1],
														&projection);
		projectedTriangle.points[2] = transformVector3D(translatedTriangle.points[2],
														&projection);
		// Scale the  triangles into view
		projectedTriangle.points[0].x += 1.0f;
		projectedTriangle.points[0].y += 1.0f;
		projectedTriangle.points[0].x *= 0.5f * (f32)screenWidth;
		projectedTriangle.points[0].y *= 0.5f * (f32)screenHeight;
		
		projectedTriangle.points[1].x += 1.0f;
		projectedTriangle.points[1].y += 1.0f;
		projectedTriangle.points[1].x *= 0.5f * (f32)screenWidth;
		projectedTriangle.points[1].y *= 0.5f * (f32)screenHeight;
		
		projectedTriangle.points[2].x += 1.0f;
		projectedTriangle.points[2].y += 1.0f;
		projectedTriangle.points[2].x *= 0.5f * (f32)screenWidth;
		projectedTriangle.points[2].y *= 0.5f * (f32)screenHeight;
		
		Triangle2f drawTriangle = {
			.a = {projectedTriangle.points[0].x, projectedTriangle.points[0].y },
			.b = {projectedTriangle.points[1].x, projectedTriangle.points[1].y },
			.c = {projectedTriangle.points[2].x, projectedTriangle.points[2].y },
		};
		
#if 0		
		otter_fillTriangleBresenham(videoBackbuffer,
									drawTriangle,
									1.0f, 0.0f, 1.0f);
#endif
		
		otter_drawTriangle(videoBackbuffer,
						   drawTriangle,
						   1.0f, 1.0f, 1.0f);
	}
	
	i32 end;
}
