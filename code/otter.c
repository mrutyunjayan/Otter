#include "otter_platform.h"

//void
//otter_updateAndRender(otter_Memory* memory,
//                      otter_OffscreenBuffer* videoBackbuffer) 
OTTER_UPDATE_AND_RENDER(otterUpdateAndRender) {
    
    otter_GameState* gameState = (otter_GameState*)memory->persistentStorage;
    
    if (!memory->isInitialized) {
        gameState->assetData = gameState + sizeof(otter_GameState);
        
        Mesh* meshCube = (Mesh*)gameState->assetData;
        
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
        
        memory->isInitialized = true;
    }
    
    Point2f point1 = { 600.0f, 300.0f };
    Point2f point2 = { 300.0f, 300.0f };
    Point2f point3 = { 900.0f, 600.0f };
    
#if 1
    otter_fillTriangleBresenham(videoBackbuffer,
                                point1, point2, point3,
                                0.5f, 1.0f, 0.5f);
#endif
    
    otter_drawTriangle(videoBackbuffer,
                       point1, point2, point3,
                       1.0f, 0.5f, 1.0f);
    
    int x = 5;
}
