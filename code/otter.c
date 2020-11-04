#include "otter_platform.h"

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
            Triangle3f triTemp = { 100.0f, 100.0f, 0.0f,    100.0f, 300.0f, 0.0f,    300.0f, 300.0f, 0.0f};
            sb_push(meshCube->triangles, triTemp);
        }
        
        {
            Triangle3f triTemp = { 100.0f, 100.0f, 0.0f,    300.0f, 300.0f, 0.0f,    300.0f, 100.0f, 0.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        //EAST
        {
            Triangle3f triTemp = { 300.0f, 100.0f, 0.0f,    300.0f, 300.0f, 0.0f,    300.0f, 300.0f, 1.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        {
            Triangle3f triTemp = { 300.0f, 100.0f, 0.0f,    300.0f, 300.0f, 1.0f,    300.0f, 100.0f, 1.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        //NORTH
        {
            Triangle3f triTemp ={ 300.0f, 100.0f, 1.0f,    300.0f, 300.0f, 1.0f,    100.0f, 300.0f, 1.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        {
            Triangle3f triTemp = { 300.0f, 100.0f, 1.0f,    100.0f, 300.0f, 1.0f,    100.0f, 100.0f, 1.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        //WEST
        {
            Triangle3f triTemp = { 100.0f, 100.0f, 1.0f,    100.0f, 300.0f, 1.0f,    100.0f, 300.0f, 0.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        {
            Triangle3f triTemp = { 100.0f, 100.0f, 1.0f,    100.0f, 300.0f, 0.0f,    100.0f, 100.0f, 0.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        //TOP
        {
            Triangle3f triTemp = { 100.0f, 300.0f, 0.0f,    100.0f, 300.0f, 1.0f,    300.0f, 300.0f, 1.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        {
            Triangle3f triTemp = { 100.0f, 300.0f, 0.0f,    300.0f, 300.0f, 1.0f,    300.0f, 300.0f, 0.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        //BOTTOM
        {
            Triangle3f triTemp = { 300.0f, 100.0f, 1.0f,    100.0f, 100.0f, 1.0f,    100.0f, 100.0f, 0.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        {
            Triangle3f triTemp = { 300.0f, 100.0f, 1.0f,    100.0f, 100.0f, 0.0f,    300.0f, 100.0f, 0.0f };
            sb_push(meshCube->triangles, triTemp);
        }
        
        memory->isInitialized = true;
    }
	
#if 0
	Point3f point1_3d = { 100.0f, 100.0f, 0.0f };
	Point3f point2_3d = { 100.0f, 300.0f, 0.0f };
	Point3f point3_3d = { 300.0f, 300.0f, 0.0f };
	Triangle3f triangle3d = { point1_3d, point2_3d, point3_3d };
	Triangle2f triangle = otter_project3DTriangleto2D(triangle3d,
													  videoBackbuffer->width, videoBackbuffer->height);
	otter_fillTriangleBresenham(videoBackbuffer,
								triangle,
								0.5f, 0.5f, 0.5f);
	otter_drawTriangle(videoBackbuffer,
					   triangle,
					   1.0f, 1.0f, 1.0f);
#else
	u32 count = (u32)sb_count(meshCube->triangles);
	for (uint i = 0; i < count; ++i) {
		
		Triangle2f tempTriangle = otter_project3DTriangleto2D(meshCube->triangles[i],
															  videoBackbuffer->width, videoBackbuffer->height);
		
		
#if 1
		otter_fillTriangleBresenham(videoBackbuffer,
									tempTriangle,
									0.5f, 0.5f, 0.5f);
		
#endif
		
#if 1
		otter_drawTriangle(videoBackbuffer,
						   tempTriangle,
						   1.0f, 1.0f, 1.0f);
#endif
	}
#endif
	
	int x = 5;
}
