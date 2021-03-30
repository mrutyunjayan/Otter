#include "pch.h"

#include "utils.h"
#include "otter_platform.h"
#include "win32_otter.h"

//~ GLOBALS
global b32 running = true;
global win32_OffScreenBuffer win32_videoBuffer = { .pixelStride = 4 };

//~ CONSOLE OUTPUT
internal void
win32_console_print(char* message) {
	OutputDebugString(message);
	DWORD length = (DWORD)strlen(message);
	LPDWORD numberWritten = 0;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),
                 message,
                 length,
                 numberWritten,
                 NULL);
}

internal void
win32_console_printError(char* message) {
	OutputDebugString(message);
	DWORD length = (DWORD)strlen(message);
	LPDWORD numberWritten = 0;
	WriteConsole(GetStdHandle(STD_ERROR_HANDLE),
                 message,
                 length,
                 numberWritten,
                 NULL);
}

//*************************************************************************
// ------------------GET BUILD DIRECTORY-----------------------------------
//*************************************************************************

inline internal void
win32_getExeFileName(win32_State* platformState) {
	// NOTE(Jai): never use MAX_PATH (aka WIN32_STATE_FILE_NAME_COUNT code that is user facing because it
    // can be dangerous and lead to bad results
    DWORD sizeOfFileName = GetModuleFileName(0,
                                             platformState->exeFileName,
                                             sizeof(platformState->exeFileName));
    
    platformState->onePastLastSlash = platformState->exeFileName;
    for (char* scan = platformState->exeFileName; *scan; ++scan) {
		if (*scan == '\\') { platformState->onePastLastSlash = scan + 1; }
    }
}

inline internal void
win32_buildExePathFileName(win32_State* platformState,
                           char* fileName,
                           int destCount, char* dest) {
	og_catStrings((platformState->onePastLastSlash - platformState->exeFileName), platformState->exeFileName,
                  og_stringLength(fileName), fileName,
                  destCount, dest);
}

//~ FILE I/O

internal FILETIME
win32_file_getModifiedTime(char* fileName) {
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


#if 0
inline void
platform_file_memory_free(void* memory);
#endif
PLATFORM_FILE_FREE_MEMORY(platform_file_memory_free) {
    if (memory) { VirtualFree(memory, 0, MEM_RELEASE); }
}

#if 0
FileReadResult
platform_file_read_full(char* filenameFull);
#endif
PLATFORM_FILE_READ_FULL(platform_file_read_full) {
    FileReadResult result = {0};
    
    HANDLE fileHandle = CreateFile(fileName,
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   (LPSECURITY_ATTRIBUTES)NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    
    if (fileHandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fileSize64;
        if(GetFileSizeEx(fileHandle, &fileSize64)) {
            
            u32 fileSize = og_truncate_safe_i64(fileSize64.QuadPart);
            result.contents = VirtualAlloc(0,
                                           fileSize,
                                           MEM_RESERVE | MEM_COMMIT,
                                           PAGE_READWRITE);
            if (result.contents) {
                
                DWORD bytesRead;
                if (ReadFile(fileHandle,
                             result.contents,
                             fileSize,
                             &bytesRead,
                             (LPOVERLAPPED)NULL)) { result.contentSize = fileSize;  }
                else { VirtualFree(result.contents, 0, MEM_RELEASE); }
            }  else {
                // TODO(Jai): logging
            }
        }  else {
            // TODO(Jai): logging
        }
        
        CloseHandle(fileHandle);
    }  else {
        // TODO(Jai): logging
    }
    
    return result;
}

#if 0
b32
platform_file_write_full(ThreadContext* thread,
                         char* fileName,
                         u32 memorySize,
                         void* memory);
#endif
PLATFORM_FILE_WRITE_FULL(platform_file_write_full) {
    b32 result = false;
    
    HANDLE fileHandle = CreateFileA(fileName,
                                    GENERIC_WRITE,
                                    0,
                                    0,
                                    CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL,
                                    0);
    
    if (fileHandle != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten;
        
        if (WriteFile(fileHandle,
                      memory,
                      memorySize,
                      &bytesWritten,
                      0)) {
            // NOTE(Jai): File wrote successfully
            result = (bytesWritten == memorySize);
        } else {
            // TODO(Jai): Logging
        }
        
        CloseHandle(fileHandle);
    } else {
        // TODO(Jai): Logging
    }
    
    return result;
}

//~ LOAD ENGINE DLL

internal win32_GameCode
win32_gameCode_load(char* sourceLibraryName,
                    char* tempLibraryName,
                    char* lockFileName) {
    win32_GameCode result = {0};
    
	WIN32_FILE_ATTRIBUTE_DATA ignored;
	if (!GetFileAttributesExA(lockFileName,
							  GetFileExInfoStandard,
							  &ignored)) {
		
		result.lastModifiedTime = win32_file_getModifiedTime(sourceLibraryName);
		if (!CopyFileA(sourceLibraryName,
					   tempLibraryName,
					   FALSE)) {
			DWORD error = GetLastError();
		}
		
		result.gameLibrary = LoadLibraryA(tempLibraryName);
		
		if (result.gameLibrary) {
			result.updateAndRender = (otter_updateAndRender*)GetProcAddress(result.gameLibrary,
                                                                            "otterUpdateAndRender");
			DWORD error = GetLastError();
			result.isValid = result.updateAndRender ? true : false;
		}
	}
    
    if (!result.isValid) {
        result.updateAndRender = 0;
    }
    
    return result;
}

internal void
win32_gameCode_unload(win32_GameCode* gameCode) {
    if (gameCode->gameLibrary) {
        FreeLibrary(gameCode->gameLibrary);
        gameCode->gameLibrary = 0;
    }
    
    gameCode->isValid = false;
    gameCode->updateAndRender = 0;
}

#include <string.h>

//~ Clear Video Buffer
internal void
win32_videoBuffer_clear() {
#if 0
    for (u32 y = 0; y < win32_videoBuffer.height; ++y) {
        u32* pixel = (u32*)((u8*)win32_videoBuffer.memory 
                            + y * win32_videoBuffer.width);
        
        for (u32 x = 0; x < win32_videoBuffer.width; ++x) {
            *pixel++ = 0xFF000000;
        }
    }
#endif
    memset(win32_videoBuffer.memory,
           (int)0xFF000000,
           (win32_videoBuffer.width * win32_videoBuffer.height * win32_videoBuffer.pixelStride));
}

//~ CLOCK

internal void
win32_clock_initialize(Clock* clock) {
    LARGE_INTEGER startTime;
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);
    clock->frequency = 1.0 / (f64)frequency.QuadPart;
    clock->startTime = (f64)startTime.QuadPart * clock->frequency;
}

internal f64
win32_clock_absoluteTime(Clock* clock) {
    LARGE_INTEGER timeNow;
    QueryPerformanceCounter(&timeNow);
    
    return ((f64)timeNow.QuadPart * clock->frequency);
}

internal void
win32_clock_update(Clock* clock) {
    if (clock->startTime != 0.0) { clock->elapsed = win32_clock_absoluteTime(clock) - clock->startTime; }
}

internal void
win32_clock_start(Clock* clock) {
    clock->startTime = win32_clock_absoluteTime(clock);
    clock->elapsed = 0;
}

internal void
win32_clock_stop(Clock* clock) {
    clock->startTime = 0;
}

//~ WINDOWING

internal win32_WindowDimensions 
win32_window_getDimensions(HWND windowHandle) {
    win32_WindowDimensions result;
    
    RECT clientRect;
    GetClientRect(windowHandle, &clientRect);
    result.width = clientRect.right - clientRect.left;
    result.height = clientRect.bottom - clientRect.top;
    
    return result;
}

internal void
win32_window_resizeDIBSection(win32_OffScreenBuffer* buffer,
                              u32 width,
                              u32 height){
    if (buffer->memory) {
        VirtualFree(buffer->memory, 
                    0, 
                    MEM_RELEASE);
    }
    
    buffer->width = width;
    buffer->height = height;
    
    // NOTE(Jai): When the 'bmiHeight' field is negative, this is a clue to
    // Windows to treat this bitmap as top-down, and not bottom-down - meaning that
    // the first 3 bytes of the image are the color for the opt left pixel
    // in the bitmap, not the bottom left!
    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biWidth = (LONG)buffer->width;
    buffer->info.bmiHeader.biHeight = -(LONG)buffer->height;
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32; // we only need 24 bits (8 bits each for RGB
    // 8 extra bits for padding to align ourselves to 4 bytes
    buffer->info.bmiHeader.biCompression = BI_RGB;
    
    size_t bitmapMemorySize = buffer->width * buffer->height * buffer->pixelStride;
    
    buffer->memory = VirtualAlloc(NULL, 
                                  bitmapMemorySize, 
                                  MEM_RESERVE | MEM_COMMIT, 
                                  PAGE_READWRITE);
    
    // TODO(Jai): Probably clear this to black
    buffer->pitch = width * buffer->pixelStride;
}

internal void
win32_window_update(HDC deviceContext,
                    int windowWidth, int windowHeight) {
#if 0
    if (windowWidth >= ((int)buffer->width * 2)
        && windowHeight >= ((int)buffer->height * 2)) {
        StretchDIBits(deviceContext,
                      0, 0,
                      windowWidth * 2, windowHeight * 2,
                      0, 0,
                      windowWidth, windowHeight,
                      buffer->memory,
                      &buffer->info,
                      DIB_RGB_COLORS,
                      SRCCOPY);
    } else {
        int offsetX = 10;
		int offsetY = 10;
		// NOTE(Jai): For protyping purposes we are always blitting 
		// 1 - 1 pixels to make sure we don't introduce artifacts with
		// stretching while we are learning to code the renderer
		StretchDIBits(deviceContext,
					  offsetX, offsetY,
                      (int)(buffer->width * 2), (int)(buffer->height * 2),
                      0, 0,
                      (int)buffer->width, (int)buffer->height,
					  buffer->memory,
					  &buffer->info,
					  DIB_RGB_COLORS,
					  SRCCOPY);
	}
#else
    StretchDIBits(deviceContext,
                  0 ,0,
                  windowWidth, windowHeight,
                  
                  0, 0,
                  (int)win32_videoBuffer.width, (int)win32_videoBuffer.height,
                  win32_videoBuffer.memory,
                  &win32_videoBuffer.info,
                  DIB_RGB_COLORS,
                  SRCCOPY);
#endif
    
    win32_videoBuffer_clear();
}

//~ WIN32 MESSAGE PROCESSING
internal void
win32_messages_processPending(win32_State* platformState) {
    MSG message;
    
    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
        switch (message.message) {
            case WM_QUIT: {
                running = false;
            } break;
            
            case WM_DESTROY: {
                running = false;
                PostQuitMessage(0);
            } break;
            
            default: {
                TranslateMessage(&message);
                DispatchMessage(&message);
            } break;
        }
    }
}

//~MAIN WINDOW CALLBACK
internal LRESULT CALLBACK
win32_mainWindowCallback(HWND window,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam) {
    LRESULT result = 0;
    
    switch (message) {
        
        case WM_CLOSE: {
            running = false;
        } break;
        
        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            win32_WindowDimensions windowDimensions = win32_window_getDimensions(window);
            PatBlt(deviceContext,
                   0, 0,
                   windowDimensions.width, windowDimensions.height,
                   BLACKNESS);
            win32_window_update(deviceContext,
                                windowDimensions.width, windowDimensions.height);
            EndPaint(window, &paint);
        } break;
        
        default: { 
            result = DefWindowProc(window, message, wParam, lParam); 
        } break;
    }
    
    return result;
}

//*************************************************************************
// ------------------MAIN--------------------------------------
//*************************************************************************
int CALLBACK
WinMain(HINSTANCE instance,
        HINSTANCE prevInstance,
        LPSTR commandLine,
        int showCommand) {
    win32_State platformState = {0};
    
    win32_clock_initialize(&platformState.clock);
    win32_clock_update(&platformState.clock);
    
    win32_getExeFileName(&platformState);
    
    char sourceGameCodeLibFullPath[WIN32_STATE_FILE_NAME_COUNT];
    win32_buildExePathFileName(&platformState,
                               "otter.dll",
                               sizeof(sourceGameCodeLibFullPath), sourceGameCodeLibFullPath);
    
    char tempGameCodeLibFullPath[WIN32_STATE_FILE_NAME_COUNT];
    win32_buildExePathFileName(&platformState,
                               "otter_temp.dll",
                               sizeof(tempGameCodeLibFullPath), tempGameCodeLibFullPath);
    
    char gameCodeLockFullPath[WIN32_STATE_FILE_NAME_COUNT];
    win32_buildExePathFileName(&platformState,
                               "lock.tmp",
                               sizeof(gameCodeLockFullPath), gameCodeLockFullPath);
    
    win32_window_resizeDIBSection(&win32_videoBuffer,
                                  960, 540);
    
    // Register the window class.
    WNDCLASS windowClass = {0};
    
    windowClass.lpfnWndProc   = win32_mainWindowCallback;
    windowClass.hInstance     = instance;
    windowClass.lpszClassName = "OtterWindow";
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    
    RegisterClass(&windowClass);
    
    // Create the window.
    HWND window = CreateWindowEx(0,                                 // Optional window styles.
                                 windowClass.lpszClassName,         // Window class
                                 "Otter",                     // Window text
                                 WS_OVERLAPPEDWINDOW,               // Window style
                                 // Size and position
                                 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                 NULL,       // Parent window    
                                 NULL,       // Menu
                                 instance,   // Instance handle
                                 NULL        // Additional application data
                                 );
    
    if (!window) {
        MessageBox(0, 
                   "Window creation failed",
                   "Error",
                   MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }
    
    ShowWindow(window, showCommand);
    
#if OTTER_INTERNAL
    LPVOID baseAddress = (LPVOID)TB(2);
#else
    LPVOID baseAddress = (LPVOID)0;
#endif
    otter_Memory otter_memory  = {
        .persistentStorageSize = MB(64),
        .transientStorageSize = GB(2),
        .fileReadFull = platform_file_read_full,
        .fileFreeMemory = platform_file_memory_free,
        .fileWriteFull = platform_file_write_full
    };
    
    platformState.totalSize = otter_memory.persistentStorageSize + otter_memory.transientStorageSize;
    platformState.gameMemoryBlock = VirtualAlloc(baseAddress,
                                                 (size_t)platformState.totalSize,
                                                 MEM_RESERVE | MEM_COMMIT, 
                                                 PAGE_READWRITE);
	
    
    otter_memory.persistentStorage = platformState.gameMemoryBlock;
    otter_memory.transientStorage = (u8*)(otter_memory.persistentStorage) + otter_memory.persistentStorageSize;
    
    if (otter_memory.persistentStorage) {
        win32_GameCode game = win32_gameCode_load(sourceGameCodeLibFullPath,
                                                  tempGameCodeLibFullPath,
                                                  gameCodeLockFullPath);
        
        
        LONG fileModified = false;
        b32 needToReload = false;
        i32 counter = 0;
        
        MSG message = {0};
        while (running) {
            if (!needToReload) {
                FILETIME newLibModifiedTime = win32_file_getModifiedTime(sourceGameCodeLibFullPath);
                fileModified = CompareFileTime(&newLibModifiedTime,
                                               &game.lastModifiedTime);
                //if (fileModified) { needToReload = true; }
            } else {
                game = win32_gameCode_load(sourceGameCodeLibFullPath,
                                           tempGameCodeLibFullPath,
                                           gameCodeLockFullPath);
                fileModified = false;
                needToReload = false;
            }
            
            if (fileModified) {
                if (counter == 0) { win32_gameCode_unload(&game); }
                counter++;
                if (counter > 10) {
                    counter = 0;
                    needToReload = true;
                }
            }
            
            win32_clock_update(&platformState.clock);
            f64 currentTime = platformState.clock.elapsed;
            // NOTE(Jai): Calculate the delta in ms and convert to seconds
            f64 deltaTime = currentTime - platformState.lastTime;
            
            win32_messages_processPending(&platformState);
            
            ThreadContext thread = {0};
            
            og_OffscreenBuffer otter_videoBuffer = {
                .pixels = win32_videoBuffer.memory,
                .width = win32_videoBuffer.width,
                .height = win32_videoBuffer.height,
                .pitch = win32_videoBuffer.pitch,
                .pixelStride = win32_videoBuffer.pixelStride
            };
            
            if (game.updateAndRender) {
                game.updateAndRender(&thread,
                                     &otter_memory, 
                                     &otter_videoBuffer);
            }
            
            HDC deviceContext = GetDC(window);
            win32_WindowDimensions windowDimensions = win32_window_getDimensions(window);
            win32_window_update(deviceContext,
                                windowDimensions.width, windowDimensions.height);
            ReleaseDC(window, deviceContext);
            
            platformState.lastTime = currentTime;
        }
        
        PostMessageA(window,
                     WM_DESTROY,
                     0, 0);
    }
    
    return 0;
}

