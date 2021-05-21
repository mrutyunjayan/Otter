#include "otter_platform.h"

#include "otter_math.h"
#include "otter_renderer.h"
#include "otter_file.h"

#if 0
void
otterUpdateAndRender(ThreadContext* thread,
                     otter_Memory* memory,
                     og_OffscreenBuffer* videoBuffer,
                     og_Input* input);
#endif
OG_UPDATE_AND_RENDER(otterUpdateAndRender) {
    otter_GameState* gameState = (otter_GameState*)memory->persistentStorage;
	if (!memory->isInitialized) {
        // STUDY(Jai): Why there is a difference in the allocation address when viwed in the debugger, vs sizeof below
        ogAlloc_arena_initialize(&gameState->worldArena,
                                 ((u8*)memory->persistentStorage + sizeof(otter_GameState)),
                                 (memory->persistentStorageSize - sizeof(otter_GameState)));
        ogAlloc_arena_initialize(&gameState->scratch,
                                 memory->transientStorage,
                                 memory->transientStorageSize);
        
#if 0
        gameState->meshes = (Mesh*)ARENA_PUSH_ARRAY(&gameState->worldArena, 1, Mesh);
        ogFile_load_objMesh(thread,
                            &gameState->scratch,
                            &gameState->worldArena,
                            &gameState->meshes[0],
                            memory->fileReadFull,
                            "../data/african_head.obj");
#endif
        memory->isInitialized = true;
    }
    
    
    ogAlloc_arena_free(&gameState->scratch);
}
