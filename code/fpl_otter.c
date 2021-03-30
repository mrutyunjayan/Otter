#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <final_platform_layer.h>
#pragma clang diagnostic pop

#include "otter_platform.h"
#include "fpl_otter.h"

global b32 global_running = true;

global fplVideoBackbuffer* global_videoBackbufferPtr;

global u32 playerX = 100;
global u32 playerY = 100;
global u32 playerSize = 10;

//~ LOAD ENGINE DLL
#if defined(FPL_PLATFORM_WINDOWS)

internal inline FILETIME
win32_getFileModifiedTime(char* fileName) {
    FILETIME result = {0};
    
    WIN32_FIND_DATAA findData;
    HANDLE findHandle = FindFirstFileA(fileName,
                                       &findData);
    if (findHandle != INVALID_HANDLE_VALUE) {
        result= findData.ftLastWriteTime;
        FindClose(findHandle);
    }
    return result;
}

internal win32_GameCode
win32_loadGameCode(char* sourceLibraryName,
                   char* tempLibraryName) {
    
    win32_GameCode result = {0};
    
    result.lastModifiedTime = win32_getFileModifiedTime(sourceLibraryName);
    if (!CopyFileA(sourceLibraryName,
                   tempLibraryName,
                   false)) {
        DWORD error = GetLastError();
    }
    
    result.gameLibrary = LoadLibraryA(tempLibraryName);
    
    if (result.gameLibrary) {
        result.updateAndRender = (otter_updateAndRender*)GetProcAddress(result.gameLibrary,
                                                                        "otterUpdateAndRender");
        
        result.isValid = result.updateAndRender || 0;
    }
    
    if (!result.isValid) {
        result.updateAndRender = 0;
    }
    
    return result;
}

internal void
win32_unloadGameCode(win32_GameCode* gameCode) {
    
    if (gameCode->gameLibrary) {
        
        FreeLibrary(gameCode->gameLibrary);
        gameCode->gameLibrary = 0;
    }
    
    gameCode->isValid = false;
    gameCode->updateAndRender = 0;
}

internal inline i32
fpl_handleError() {
    
    fplPlatformResultType initResult = fplGetPlatformResult();
    const char* initResultStr = fplGetPlatformResultTypeString(initResult);
    const char* errStr = fplGetLastError();
    fplConsoleFormatError("FPL-ERROR[%s]: %s\n", initResultStr, errStr);
    
    return -1;
}

//*************************************************************************
// ------------------FILE I/O--------------------------------------
//*************************************************************************

// void platform_file_freeMemory(ThreadContext* thread, void* memory)
PLATFORM_FILE_FREE_MEMORY(platform_file_freeMemory) {
    
	if (memory) {
        VirtualFree(memory, 0,MEM_RELEASE);
    }
}

// FileReadResult platform_file_readEntire(ThreadContext* thread,
//                                         char* fileName)
PLATFORM_FILE_READ_ENTIRE(platform_file_readEntire) {
    
    FileReadResult result = {0};
    
    HANDLE fileHandle = CreateFileA(fileName,
                                    GENERIC_READ,
                                    FILE_SHARE_READ,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
    
    if (fileHandle != INVALID_HANDLE_VALUE) {
        
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(fileHandle,
                          &fileSize)) {
            
            u32 fileSize32 = safeTruncate_i64(fileSize.QuadPart);
            result.contents = VirtualAlloc(0,
                                           fileSize32,
                                           MEM_RESERVE | MEM_COMMIT,
                                           PAGE_READWRITE);
            if (result.contents) {
                
                DWORD bytesRead;
                if ((ReadFile(fileHandle,
                              result.contents,
                              fileSize32,
                              &bytesRead,
                              NULL) && (fileSize32 == bytesRead))) {
                    // NOTE(Jai): File read successfully
                    
                    result.contentSize = fileSize32;
                } else {
                    DEBUG_platform_freeFileMemory(thread, result.contents);
                    result.contents = 0;
                } 
            } else {
                // TODO(Jai): logging
            }
        } else {
            // TODO(Jai): logging
        }
        CloseHandle(fileHandle);
    } else {
        // TODO(Jai): logging
    }
    return result;
}

// b32 platform_file_writeEntire(ThreadContext* thread, char* fileName,
//                               u32 memorySize, void* memory)
PLATFORM_FILE_WRITE_ENTIRE(platform_file_writeEntire) {
    
	b32 result = false;
    
    HANDLE fileHandle = CreateFileA(fileName,
                                    GENERIC_WRITE,
                                    0,
                                    NULL,
                                    CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
    if (fileHandle != INVALID_HANDLE_VALUE) {
        
        DWORD bytesWritten;
        if (WriteFile(fileHandle,
                      memory,
                      memorySize,
                      &bytesWritten,
                      NULL)) {
            
            // NOTE(Jai): File wrote to successfully
            result = (bytesWritten == memorySize);
        } else {
            // TODO(Jai): logging
        } 
        
        CloseHandle(fileHandle);
    } else {
        // TODO(Jai): logging
    }
    
    return result;
}
#endif

//~
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
internal void
fpl_handleEvent(fplEvent* currentEvent) {
    
    while (fplPollEvent(currentEvent)) {
		
        switch (currentEvent->type) {
			
            case fplEventType_Window: {
                // A window event, like resize, lost/got focus, etc.
                switch (currentEvent->window.type) {
					
                    case fplWindowEventType_Closed: {
                        global_running = false;
                    } break;
                    
                    case fplWindowEventType_Resized: {
                        // ... Compute new rectangle here (Letterbox or something)
                        fplVideoRect newRect = {
                            //! Left position in pixels
                            .x = 0,
                            //! Top position in pixels
                            .y = 0,
                            //! Width in pixels
                            .width = (i32)currentEvent->window.size.width,
                            //! Height in pixels
                            .height = (i32)currentEvent->window.size.height
                        };
                        global_videoBackbufferPtr->outputRect = newRect;
                    } break;
                    
                    default: break;
                }
            } break;
            case fplEventType_Keyboard: {
                // A keyboard event, like key down/up, pressed, etc.
                switch (currentEvent->keyboard.type) {
					
                    case fplKeyboardEventType_Button: {
                        fplButtonState state = currentEvent->keyboard.buttonState;
                        
                        // ... Handle the key code
                        uint64_t keyCode = currentEvent->keyboard.keyCode;
                        if (state >= fplButtonState_Press) {
                            if (keyCode == 87 || keyCode == 119) {
                                // Letter W is held down
                                playerY -= 10;
                            } else if (keyCode == 65 || keyCode == 97) {
                                // Letter A is held down
                                playerX -= 10;
                            } else if (keyCode == 83 || keyCode == 115) {
                                // Letter S is held down
                                playerY += 10;
                            } else if (keyCode == 68 || keyCode == 100) {
                                // Letter D is held down
                                playerX += 10;
                            } 
                        }
                        
                        // or
                        
                        // ... handle the mapped key
                        fplKey mappedKey = currentEvent->keyboard.mappedKey;
                        if (state == fplButtonState_Release) {
							
							// F4 key pressed
                            if (mappedKey == fplKey_F4) {
								
#if 0                                
								fplKeyboardModifierFlags keyModifier = currentEvent->keyboard.modifiers;
                                if (keyModifier == fplKeyboardModifierFlags_LAlt
									|| keyModifier == fplKeyboardModifierFlags_RAlt) {
									
                                    global_running = false;
                                }
#endif
                            }
                        }
                    } break;
                    
                    case fplKeyboardEventType_Input:
                    {
                        if(currentEvent->keyboard.keyCode > 0 
						   && currentEvent->keyboard.keyCode < 0x10000) {
                            // Handle character input
							
                        }
                    } break;
                    default: break;
                }
            } break;
            case fplEventType_Mouse: {
                // A mouse event, like mouse button down/up, mouse move, etc.
                switch (currentEvent->mouse.type) {
                    // ...
                    default: break;
                }
            } break;
            case fplEventType_Gamepad: {
                // A gamepad event, like connected/disconnected, state-updated etc.
                switch (currentEvent->gamepad.type) {
                    // ...
                    default: break;
                }
            } break;
            
            default: break;
        }
    }
}
#pragma clang diagnostic pop

internal void
fpl_clearVideoBackbuffer() {
    
    global_videoBackbufferPtr = fplGetVideoBackbuffer();
    
    for (u32 y = 0; y < global_videoBackbufferPtr->height; ++y) {
        u32* pixel = (u32*)((u8*)global_videoBackbufferPtr->pixels 
                            + y * global_videoBackbufferPtr->lineWidth);
        for (u32 x = 0; x < global_videoBackbufferPtr->width; ++x) {
            
            *pixel++ = 0xFF000000;
        }
    }
}

//~    --------MAIN-----
int 
main(int argc, char** argv) {
    
    fplSettings settings;
    fplSetDefaultSettings(&settings);
    
    // Forcing the video driver to be software
    settings.video.driver = fplVideoDriverType_Software;
    
    // NOTE(Jai): Changing the window settings
    fplWindowSettings windowSettings = {
        .title = "Otter Engine",
        .icons = (const u8*)settings.window.icons,
        //! Window callbacks
        .callbacks = settings.window.callbacks,
        //! Window size in screen coordinates
        .windowSize = settings.window.windowSize,
        //! Fullscreen size in screen coordinates
        .fullscreenSize = settings.window.fullscreenSize,
        //! Fullscreen refresh rate in Hz
        .fullscreenRefreshRate = settings.window.fullscreenRefreshRate,
        //! Is window resizable
        .isResizable = settings.window.isResizable,
        //! Is window decorated
        .isDecorated = settings.window.isDecorated,
        //! Is floating
        .isFloating = settings.window.isFloating,
        //! Is window in fullscreen mode
        .isFullscreen = settings.window.isFullscreen
    };
    settings.window = windowSettings;
    
    if (!fplPlatformInit(fplInitFlags_All,
                         &settings)) {
        
        fpl_handleError();
    }
    
    otter_Memory otter_memory = {
        .persistentStorageSize = MB(64),
        .transientStorageSize = GB(1)
    };
    
    u64 totalMemorySize = otter_memory.persistentStorageSize
        + otter_memory.transientStorageSize;
    
	Arena arena = {
		.buffer = fplMemoryAllocate((size_t)totalMemorySize),
		.bufferSize = totalMemorySize
	};
	
    og_arena_alloc(&arena,
                   otter_memory.persistentStorageSize);
	otter_memory.persistentStorage = &arena.buffer[arena.memoryBlockStart];
    og_arena_alloc(&arena,
                   otter_memory.transientStorageSize);
	otter_memory.transientStorage = &arena.buffer[arena.memoryBlockStart];
	
    global_videoBackbufferPtr = fplGetVideoBackbuffer();
    og_OffscreenBuffer otter_videoBackbuffer = {
        .pixels = global_videoBackbufferPtr->pixels, 
        .width = global_videoBackbufferPtr->width,
        .height = global_videoBackbufferPtr->height,
        .pitch = (u32)(global_videoBackbufferPtr->lineWidth),
        .pixelStride = (u32)global_videoBackbufferPtr->pixelStride
    };
	otter_videoBackbuffer.aspectRatio = 
		otter_videoBackbuffer.height / otter_videoBackbuffer.width;
    
    if (otter_memory.persistentStorage) {
        
        char exeFilePath[FPL_MAX_PATH_LENGTH];
        fplGetExecutableFilePath((char*)&exeFilePath,
                                 fplArrayCount(exeFilePath));
        fplExtractFilePath((char*)&exeFilePath,
                           (char*)&exeFilePath,
                           fplArrayCount(exeFilePath));
        
        char sourceGameCodeLibFullPath[FPL_MAX_PATH_LENGTH];
        if (!fplPathCombine((char*)&sourceGameCodeLibFullPath,
                            fplArrayCount(sourceGameCodeLibFullPath),
                            2,
                            exeFilePath, "otter.dll")) {
            
            fpl_handleError();
        }
        
        char tempGameCodeLibFullPath[FPL_MAX_PATH_LENGTH];
        if (!fplPathCombine((char*)&tempGameCodeLibFullPath,
                            fplArrayCount(tempGameCodeLibFullPath),
                            2,
                            exeFilePath, "otter_temp.dll")) {
            
            fpl_handleError();
        }
        
#if defined(FPL_PLATFORM_WINDOWS)
        win32_GameCode game = win32_loadGameCode(sourceGameCodeLibFullPath,
                                                 tempGameCodeLibFullPath);
#endif
        
        b32 reload = false;
        i32 fileModified = false;
        
        while (global_running) {
            
#if defined(FPL_PLATFORM_WINDOWS)
            if (!reload) {
                
                FILETIME newLibModifiedTime = win32_getFileModifiedTime(sourceGameCodeLibFullPath);
                fileModified = CompareFileTime(&newLibModifiedTime,
                                               &game.lastModifiedTime);
            } else {
                
                game = win32_loadGameCode(sourceGameCodeLibFullPath,
                                          tempGameCodeLibFullPath);
                reload = false;
            }
            
            if (fileModified) {
                
                win32_unloadGameCode(&game);
                reload = true;
                fileModified = false;
            }
#endif
            
			fplEvent currentEvent;
            fpl_handleEvent(&currentEvent);
            
			fpl_clearVideoBackbuffer();
			
            if(game.updateAndRender) {
                otter_videoBackbuffer.width = global_videoBackbufferPtr->width;
				otter_videoBackbuffer.height = global_videoBackbufferPtr->height;
				otter_videoBackbuffer.pitch = (u32)(global_videoBackbufferPtr->lineWidth);
				otter_videoBackbuffer.pixels = global_videoBackbufferPtr->pixels; 
				otter_videoBackbuffer.aspectRatio = 
					otter_videoBackbuffer.height / otter_videoBackbuffer.width;
				
				game.updateAndRender(&thread,
                                     &otter_memory, 
									 &otter_videoBackbuffer);
			}
			
			fplVideoFlip();
		}
	} else {
		
		fplPlatformResultType initResult = fplGetPlatformResult();
		const char* initResultStr = fplGetPlatformResultTypeString(initResult);
		const char* errStr = fplGetLastError();
		fplConsoleFormatError("FPL-ERROR[%s]: %s\n", initResultStr, errStr);
		
		return -1;
	}
	
	fplPlatformRelease();
	return 0;
}
