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
    
    Point2f centre = {
        .x = 700.0f,
        .y = 375.0f
    };
    f32 radius = 350.0f;
    
    otter_drawCircleBresenham(videoBackbuffer,
                              centre,
                              radius,
                              1.0f, 1.0f, 1.0f);
}
