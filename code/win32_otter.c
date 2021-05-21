#include "pch.h"

#include "utils.h"
#include "otter_platform.h"
#include "win32_otter.h"

#if OG_D3D11
#include "otter_d3d.h"
#include "d3dcompiler.h"
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3d10.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3dcompiler.lib")

global og_D3D d3d = {0};

#endif

//~ GLOBALS
global b8 running = true;
global b8 paused = false;
global win32_OffScreenBuffer win32_videoBuffer = { .pixelStride = 4 };
global WINDOWPLACEMENT global_windowPosition = { .length = sizeof(global_windowPosition) };

//~************ XInput Rerouting ***************************************

typedef DWORD WINAPI X_Input_Get_State(DWORD wUserIndex,
                                       XINPUT_STATE* pState);
typedef DWORD WINAPI X_Input_Set_State(DWORD dwUserIndex,
                                       XINPUT_VIBRATION* pVibration);
// NOTE(Jai): XINputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(X_Input_Get_State);
X_INPUT_GET_STATE(XInputGetState_Stub) {
    return (ERROR_DEVICE_NOT_CONNECTED);
}
global X_Input_Get_State* XInputGetState_ = XInputGetState_Stub;
#define XInputGetState XInputGetState_

// NOTE(Jai): XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, \
XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(X_Input_Set_State);
X_INPUT_SET_STATE(XInputSetState_Stub) {
    return (ERROR_DEVICE_NOT_CONNECTED);
}
global X_Input_Set_State* XInputSetState_ = XInputSetState_Stub;

#define XInputSetState XInputSetState_

internal void
win32_loadXInput(void) {
	HMODULE XInputLibrary = 0;
    XInputLibrary = LoadLibrary("xinput1_4.dll");
    if (!XInputLibrary) { XInputLibrary = LoadLibrary("xinput1_3.dll"); }
    else { /* TODO(Jai): Diagnostic */ }
    if (XInputLibrary) {
        XInputGetState = (X_Input_Get_State*)GetProcAddress(XInputLibrary,
                                                            "XInputGetState");
        XInputSetState = (X_Input_Set_State*)GetProcAddress(XInputLibrary,
                                                            "XInputSetState");
    } else { /* TODO(Jai): Diagnostic */ }
}

//~ Win32 Error Handling
internal void
win32_error_format(HRESULT hr, u32 line, char* file) {
    LPTSTR errorText = 0;
    FormatMessageA(// use system message tables to retrieve error text
                   FORMAT_MESSAGE_FROM_SYSTEM
                   // allocate buffer on local heap for error text
                   | FORMAT_MESSAGE_ALLOCATE_BUFFER
                   // Important! will fail otherwise, since we're not
                   // (and CANNOT) pass insertion parameters
                   | FORMAT_MESSAGE_IGNORE_INSERTS,
                   0,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
                   (DWORD)hr,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPTSTR)&errorText,  // output
                   0, // minimum size for output buffer
                   0);   // arguments
    
    if (errorText) {
        char message[512];
        sprintf (message, "%s\nLINE: %d\nFILE: %s\n", errorText, line, file);
        MessageBoxA(0,
                    message,
                    TEXT("Error"),
                    MB_OK);
        LocalFree(errorText);
        errorText = 0;
    }
}

#if OG_INTERNAL | DEBUG_RENDERER

#ifndef HR
#pragma clang diagnostic ignored "-Wunused-macros"
#define HR(x)\
{\
HRESULT hr = x;\
if (FAILED(hr)) {\
win32_error_format(hr, __LINE__, __FILE__);\
}\
}
#endif // ifndef HR

#else

#ifndef HR
#define HR(x) x
#endif // ifndef HR

#endif // OG_INTERNAL | DEBUG_RENDERER

//~ CONSOLE OUTPUT
internal void
win32_console_print(char* message) {
	OutputDebugStringA(message);
	DWORD length = (DWORD)strlen(message);
	LPDWORD numberWritten = 0;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),
                 message,
                 length,
                 numberWritten,
                 0);
}

internal void
win32_console_printError(char* message) {
	OutputDebugStringA(message);
	DWORD length = (DWORD)strlen(message);
	LPDWORD numberWritten = 0;
	WriteConsole(GetStdHandle(STD_ERROR_HANDLE),
                 message,
                 length,
                 numberWritten,
                 0);
}

//*************************************************************************
// ------------------GET BUILD DIRECTORY-----------------------------------
//*************************************************************************

internal inline void
win32_getExeFileName(win32_State* platformState) {
	// NOTE(Jai): never use MAX_PATH (aka WIN32_STATE_FILE_NAME_COUNT code that is user facing because it
    // can be dangerous and lead to bad results
    DWORD sizeOfFileName = GetModuleFileNameA(0,
                                              platformState->exeFileName,
                                              sizeof(platformState->exeFileName));
    
    platformState->onePastLastSlash = platformState->exeFileName;
    for (char* scan = platformState->exeFileName; *scan; ++scan) {
		if (*scan == '\\') { platformState->onePastLastSlash = scan + 1; }
    }
}

internal inline void
win32_buildModulePathFileName(win32_State* platformState,
                              char* fileName,
                              int destCount, char* dest) {
	ogUtils_catStrings((platformState->onePastLastSlash - platformState->exeFileName), platformState->exeFileName,
                       ogUtils_stringLength(fileName), fileName,
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
                                   (LPSECURITY_ATTRIBUTES)0,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   0);
    
    if (fileHandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fileSize64;
        if(GetFileSizeEx(fileHandle, &fileSize64)) {
            u32 fileSize = ogUtils_truncate_safe_i64(fileSize64.QuadPart);
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
                             (LPOVERLAPPED)0)) { result.contentSize = fileSize;  }
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
b8
platform_file_write_full(ThreadContext* thread,
                         char* fileName,
                         u32 memorySize,
                         void* memory);
#endif
PLATFORM_FILE_WRITE_FULL(platform_file_write_full) {
    b8 result = false;
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
    if (!result.isValid) { result.updateAndRender = 0; }
    
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

// HACK(Jai): works for now
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
win32_window_getDimensions(HWND window) {
    win32_WindowDimensions result;
    RECT clientRect;
    GetClientRect(window, &clientRect);
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
    
    buffer->memory = VirtualAlloc(0,
                                  bitmapMemorySize,
                                  MEM_RESERVE | MEM_COMMIT,
                                  PAGE_READWRITE);
    
    // TODO(Jai): Probably clear this to black
    buffer->pitch = width * buffer->pixelStride;
}

internal void
win32_window_update(HDC deviceContext,
                    int windowWidth, int windowHeight) {
#if OG_D3D11
    f32 d3d_clearColour[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    ID3D11DeviceContext_ClearRenderTargetView(d3d.deviceContext,
                                              d3d.renderTargetView,
                                              d3d_clearColour);
    ID3D11DeviceContext_ClearDepthStencilView(d3d.deviceContext,
                                              d3d.depthStencilView,
                                              D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                              1.0f, 0);
    //- Bind the view to the output merger
    // NOTE(Jai): Need to bind this every frame as calling present unbinds it
    ID3D11DeviceContext_OMSetRenderTargets(d3d.deviceContext,
                                           1, &d3d.renderTargetView, d3d.depthStencilView);
    ID3D11DeviceContext_IASetInputLayout(d3d.deviceContext,
                                         d3d.inputLayout);
    ID3D11DeviceContext_IASetPrimitiveTopology(d3d.deviceContext,
                                               D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    ID3D11DeviceContext_Draw(d3d.deviceContext,
                             d3d.vertexCount,
                             0);
    HR(IDXGISwapChain1_Present(d3d.swapChain,
                               0, 0));
#else
    PatBlt(deviceContext,
           0, 0,
           windowWidth, windowHeight,
           BLACKNESS);
    StretchDIBits(deviceContext,
                  0 ,0,
                  windowWidth, windowHeight,
                  0, 0,
                  (int)win32_videoBuffer.width, (int)win32_videoBuffer.height,
                  win32_videoBuffer.memory,
                  &win32_videoBuffer.info,
                  DIB_RGB_COLORS,
                  SRCCOPY);
    win32_videoBuffer_clear();
#endif
}

//- D3D11
#if OG_D3D11

internal void
win32_d3d_init(HWND window) {
    UINT factoryFlags = 0;
#if DEBUG_RENDERER
    factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
    HR(CreateDXGIFactory2(factoryFlags,
                          &IID_IDXGIFactory4,
                          (void**)&d3d.factory));
    
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };
    D3D_FEATURE_LEVEL featureLevelRecieved = D3D_FEATURE_LEVEL_9_1;
    
    UINT deviceFlags = 0;
#if DEBUG_RENDERER
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    HR(D3D11CreateDevice(0,
                         D3D_DRIVER_TYPE_HARDWARE,
                         0,
                         deviceFlags,
                         featureLevels,
                         ARRAY_COUNT(featureLevels),
                         D3D11_SDK_VERSION,
                         &d3d.device,
                         &featureLevelRecieved,
                         &d3d.deviceContext));
    
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
        .Height = 0,
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .SampleDesc.Count = 1,
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 2,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
    };
    HR(IDXGIFactory5_CreateSwapChainForHwnd(d3d.factory,
                                            (IUnknown*)d3d.device,
                                            window,
                                            &swapChainDesc,
                                            0,
                                            0,
                                            &d3d.swapChain));
    
    //- Render Target Init
    HR(IDXGISwapChain_GetBuffer(d3d.swapChain,
                                0, &IID_ID3D11Texture2D, (void**)&d3d.backBuffer));
    HR(ID3D11Device5_CreateRenderTargetView(d3d.device,
                                            (ID3D11Resource*)d3d.backBuffer, 0, &d3d.renderTargetView));
    ID3D11Texture2D_Release(d3d.backBuffer);
    /* NOTE(Jai): GetBuffer() increases the ref. count of d3d.backbuffer
     .             - releasing it to set it back to 1 */
    
    win32_WindowDimensions windowDimensions = win32_window_getDimensions(window);
    //- Depth Buffer Init
    D3D11_TEXTURE2D_DESC depthStencilBufferDesc = {
        .Width = (UINT)windowDimensions.width,
        .Height = (UINT)windowDimensions.height,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_D32_FLOAT,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_DEPTH_STENCIL,
        .SampleDesc.Count = 1
    };
    
    HR(ID3D11Device5_CreateTexture2D(d3d.device,
                                     &depthStencilBufferDesc, 0, &d3d.depthStencilBuffer));
    HR(ID3D11Device5_CreateDepthStencilView(d3d.device,
                                            (ID3D11Resource*)d3d.depthStencilBuffer, 0,
                                            &d3d.depthStencilView));
    
    //- State Management
    // Blend
    D3D11_BLEND_DESC blendDesc = {
        .RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL
    };
    HR(ID3D11Device_CreateBlendState(d3d.device,
                                     &blendDesc,
                                     &d3d.blendState));
    // Depth Stencil
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {
        .DepthEnable = true,
        .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
        .DepthFunc = D3D11_COMPARISON_LESS_EQUAL
    };
    HR(ID3D11Device_CreateDepthStencilState(d3d.device,
                                            &depthStencilDesc,
                                            &d3d.depthStencilState));
    // Rasterizer
    D3D11_RASTERIZER_DESC rasterDesc = {
        .CullMode = D3D11_CULL_BACK,
        .DepthClipEnable = true,
        .FillMode = D3D11_FILL_SOLID
    };
    HR(ID3D11Device_CreateRasterizerState(d3d.device,
                                          &rasterDesc, &d3d.rasterizerState));
    
    //- Set The Viewport
    D3D11_VIEWPORT viewPort = {
        .Width = (f32)windowDimensions.width,
        .Height = (f32)windowDimensions.height,
        .MaxDepth = 1.0f
    };
    
    ID3D11DeviceContext_RSSetViewports(d3d.deviceContext,
                                       1, &viewPort);
    
}

internal inline void
win32_d3d_window_resize(HWND window) {
    win32_WindowDimensions windowDimensions = win32_window_getDimensions(window);
    // NOTE(Jai): Release all the old buffers and views
    ID3D11RenderTargetView_Release(d3d.renderTargetView);
    ID3D11DepthStencilView_Release(d3d.depthStencilView);
    ID3D11Resource_Release(d3d.depthStencilBuffer);
    
    // NOTE(Jai): Resize the swapchain and recreate the render target view
    HR(IDXGISwapChain1_ResizeBuffers(d3d.swapChain,
                                     0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));
    HR(IDXGISwapChain_GetBuffer(d3d.swapChain,
                                0, &IID_ID3D11Texture2D, (void**)&d3d.backBuffer));
    HR(ID3D11Device_CreateRenderTargetView(d3d.device,
                                           (ID3D11Resource*)d3d.backBuffer, 0, &d3d.renderTargetView));
    ID3D11Texture2D_Release(d3d.backBuffer);
    
    // NOTE(Jai): Create the depth/stencil buffer and view
    D3D11_TEXTURE2D_DESC d3d_depthStencilBufferDesc = {
        .Width = (UINT)windowDimensions.width,
        .Height = (UINT)windowDimensions.height,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_D32_FLOAT,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_DEPTH_STENCIL,
        .SampleDesc.Count = 1
    };
    HR(ID3D11Device5_CreateTexture2D(d3d.device,
                                     &d3d_depthStencilBufferDesc, 0, &d3d.depthStencilBuffer));
    HR(ID3D11Device5_CreateDepthStencilView(d3d.device,
                                            (ID3D11Resource*)d3d.depthStencilBuffer, 0,
                                            &d3d.depthStencilView));
    ID3D11DeviceContext_OMSetRenderTargets(d3d.deviceContext,
                                           1, &d3d.renderTargetView, d3d.depthStencilView);
    // NOTE(Jai): Set The Viewport
    D3D11_VIEWPORT d3d_viewPort = {
        .Width = (f32)windowDimensions.width,
        .Height = (f32)windowDimensions.height,
        .MaxDepth = 1.0f
    };
    
    ID3D11DeviceContext_RSSetViewports(d3d.deviceContext,
                                       1, &d3d_viewPort);
}

internal inline void
win32_d3d_shader_load(char* compiledShaderFileName, win32_State* platformState, ID3DBlob** shaderBlob) {
    char blobFullPath[WIN32_STATE_FILE_NAME_COUNT];
    win32_buildModulePathFileName(platformState,
                                  compiledShaderFileName,
                                  sizeof(blobFullPath), blobFullPath);
    i32 blobPathWideSize = MultiByteToWideChar(CP_UTF8, 0, blobFullPath, -1, 0, 0);
#pragma clang diagnostic ignored "-Wvla"
    wchar_t blobFullPath_wide[blobPathWideSize];
    MultiByteToWideChar(CP_UTF8, 0, blobFullPath, -1, blobFullPath_wide, blobPathWideSize);
    HR(D3DReadFileToBlob(blobFullPath_wide, shaderBlob));
}
#endif

//~ INPUT HANDLING

internal void
win32_message_process_keyboard(og_ButtonState* newState,
                               b8 isDown) {
	if (newState->endedDown != isDown) {
        newState->endedDown = isDown;
        ++newState->halfTransitionCount;
    }
}

internal void
win32_xInput_process_digitialButton(og_ButtonState* newState,
                                    og_ButtonState* oldState,
                                    DWORD xInputButtonState,
                                    DWORD buttonBit) {
	newState->endedDown = ((xInputButtonState & buttonBit) == buttonBit);
    newState->halfTransitionCount = (oldState->endedDown != newState->endedDown) ? 1 : 0;
}

internal f32
win32_xInput_process_stickValue(SHORT value,
                                SHORT deadZoneThreshold) {
	f32 result = 0;
    if (value < -deadZoneThreshold) {
        result = (f32)((value + deadZoneThreshold) / (32768.0f - deadZoneThreshold));
    } else if (value > deadZoneThreshold) {
        result = (f32)((value - deadZoneThreshold) / (32767.0f - deadZoneThreshold));
    }
    
    return result;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wsign-conversion"
internal void
win32_fullscreen_toggle(HWND window) {
	// NOTE(Jai): This follows Raymond Chen's prescription for fullscreen
	// toggling, see: https://devblogs.monitorInfocrosoft.com/oldnewthing/20100412-00/?p=14353
	DWORD style = (DWORD)GetWindowLong(window, GWL_STYLE);
	if (style & WS_OVERLAPPEDWINDOW) {
		MONITORINFO monitorInfo = { sizeof(monitorInfo) };
		if (GetWindowPlacement(window, &global_windowPosition) &&
			GetMonitorInfo(MonitorFromWindow(window,
											 MONITOR_DEFAULTTOPRIMARY), &monitorInfo)) {
			SetWindowLong(window, GWL_STYLE,
						  style & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(window, HWND_TOP,
						 monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
						 monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
						 monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
						 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	} else {
		SetWindowLong(window, GWL_STYLE,
					  style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(window, &global_windowPosition);
		SetWindowPos(window, NULL, 0, 0, 0, 0,
					 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
					 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}
#pragma clang diagnostic pop

//~ WIN32 MESSAGE PROCESSING
internal void
win32_messages_processPending(win32_State* platformState,
                              og_ControllerInput* keyboardController,
                              HWND window) {
    MSG message;
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
        switch (message.message) {
            case WM_QUIT: {
                running = false;
            } break;
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                u32 VKCode = (u32)message.wParam;
                /* NOTE(Jai):
                the 30th bit in lParam:  The previous key state. The value is 1 if the key is down before
the message is sent, or it is zero if the key is up.*/
#define keyMessageWasDownBit (1 << 30)
                /*NOTE(Jai):the 31st bit in lParam: The transition state. The value is always 0 for a
WM_KEYDOWN message.*/
#define keyMessageWasUpBit (1 << 31)
                b8 wasDown = (message.lParam & keyMessageWasDownBit) != 0;
                b8 isDown = (message.lParam & keyMessageWasUpBit) == 0;
                if (wasDown != isDown) {
                    switch (VKCode) {
                        case 'W': {
                            win32_message_process_keyboard(&keyboardController->moveUp,
                                                           isDown);
                        } break;
                        case 'A': {
                            win32_message_process_keyboard(&keyboardController->moveLeft,
                                                           isDown);
                        } break;
                        case 'S': {
                            win32_message_process_keyboard(&keyboardController->moveDown,
                                                           isDown);
                        } break;
                        case 'D': {
                            win32_message_process_keyboard(&keyboardController->moveRight,
                                                           isDown);
                        } break;
                        case 'Q': {
                            win32_message_process_keyboard(&keyboardController->leftShoulder,
                                                           isDown);
                        } break;
                        case 'E': {
                            win32_message_process_keyboard(&keyboardController->rightShoulder,
                                                           isDown);
                        } break;
                        case VK_UP: {
                            win32_message_process_keyboard(&keyboardController->actionUp,
                                                           isDown);
                        } break;
                        case VK_DOWN: {
                            win32_message_process_keyboard(&keyboardController->actionDown,
                                                           isDown);
                        } break;
                        case VK_LEFT: {
                            win32_message_process_keyboard(&keyboardController->actionLeft,
                                                           isDown);
                        } break;
                        case VK_RIGHT: {
                            win32_message_process_keyboard(&keyboardController->actionRight,
                                                           isDown);
                        } break;
                        case VK_ESCAPE: {
                            win32_message_process_keyboard(&keyboardController->back,
                                                           isDown);
                        } break;
                        case VK_SPACE: {
                            win32_message_process_keyboard(&keyboardController->start,
                                                           isDown);
                        } break;
						// NOTE(Jai): The 29th bit in the lParam: The context code. The value is always 0 for a
						// WM_KEYDOWN message.; 1 if the ALT key is down while the key is
						// pressed
#define altKeyWasDownBit (1 << 29)
						case VK_RETURN: {
							if (isDown) {
								b8 altKeyWasDown = (b8)(message.lParam & altKeyWasDownBit);
                                if (altKeyWasDown) {
                                    if (message.hwnd) { win32_fullscreen_toggle(message.hwnd); }
                                }
							}
						} break;
                        case VK_F4: {
                            if (isDown) {
								b8 altKeyWasDown = (b8)(message.lParam & altKeyWasDownBit);
								if (altKeyWasDown) { running = false; }
							}
						} break;
                        default: {} break;
                    }
                }
            } break;
            default: {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }break;
        }
        break;
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
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            ASSERT(!"Keyboard input came in through a non-dispatch message");
        } break;
        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            win32_WindowDimensions windowDimensions = win32_window_getDimensions(window);
            win32_window_update(deviceContext,
                                windowDimensions.width, windowDimensions.height);
            EndPaint(window, &paint);
        } break;
#if OG_D3D11
        case WM_SIZE: {
            if (d3d.swapChain) {
                win32_d3d_window_resize(window);
            }
        } break;
        case WM_ENTERSIZEMOVE: {
            paused = true;
        } break;
        case WM_EXITSIZEMOVE: {
            paused = false;
            if (d3d.swapChain) {
                win32_d3d_window_resize(window);
            }
        } break;
#endif
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
    win32_buildModulePathFileName(&platformState,
                                  "otter.dll",
                                  sizeof(sourceGameCodeLibFullPath), sourceGameCodeLibFullPath);
    
    char tempGameCodeLibFullPath[WIN32_STATE_FILE_NAME_COUNT];
    win32_buildModulePathFileName(&platformState,
                                  "otter_temp.dll",
                                  sizeof(tempGameCodeLibFullPath), tempGameCodeLibFullPath);
    
    char gameCodeLockFullPath[WIN32_STATE_FILE_NAME_COUNT];
    win32_buildModulePathFileName(&platformState,
                                  "lock.tmp",
                                  sizeof(gameCodeLockFullPath), gameCodeLockFullPath);
    
    win32_window_resizeDIBSection(&win32_videoBuffer,
                                  960, 540);
    
    // Register the window class.
    WNDCLASSA windowClass = {0};
    
    windowClass.lpfnWndProc   = win32_mainWindowCallback;
    windowClass.hInstance     = instance;
    windowClass.lpszClassName = "OtterWindow";
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    
    RegisterClassA(&windowClass);
    
    // Create the window.
    HWND window = CreateWindowExA(0,                                 // Optional window styles.
                                  windowClass.lpszClassName,         // Window class
                                  "Otter",                     // Window text
                                  WS_OVERLAPPEDWINDOW,               // Window style
                                  // Size and position
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                  0,       // Parent window
                                  0,       // Menu
                                  instance,   // Instance handle
                                  0        // Additional application data
                                  );
    
    if (!window) {
        MessageBoxA(0,
                    "Window creation failed",
                    "Error",
                    MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }
    ShowWindow(window, showCommand);
    win32_WindowDimensions windowDimensions = win32_window_getDimensions(window);
    
#if OG_D3D11
    //~ D3D 11
    win32_d3d_init(window);
    ogD3D_render_hello_triangle(&d3d, &platformState);
    //~ D3D 11 END
#endif
    
#if OG_INTERNAL
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
        og_Input input[2] = {0};
        og_Input* newInput = &input[0];
        og_Input* oldInput = &input[1];
        
        win32_GameCode game = win32_gameCode_load(sourceGameCodeLibFullPath,
                                                  tempGameCodeLibFullPath,
                                                  gameCodeLockFullPath);
        LONG fileModified = false;
        b8 needToReload = false;
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
            
            og_ControllerInput* oldKeyboardController = getController(oldInput, 0);
            og_ControllerInput* newKeyboardController = getController(newInput, 0);
            og_ControllerInput zeroController = {0};
            *newKeyboardController = zeroController;
            newKeyboardController->isConnected = true;
            win32_messages_processPending(&platformState, newKeyboardController, window);
            if (!paused) {
                POINT mouseLocation;
                GetCursorPos(&mouseLocation);
                ScreenToClient(window, &mouseLocation);
                newInput->mouseX = mouseLocation.x;
                newInput->mouseY = mouseLocation.y;
                newInput->mouseZ = 0; // TODO(Jai): Support mousewheel?
                win32_message_process_keyboard(&newInput->mouseButtons[0],
                                               (b8)(GetKeyState(VK_LBUTTON) & (1 << 15)));
                win32_message_process_keyboard(&newInput->mouseButtons[1],
                                               (b8)(GetKeyState(VK_RBUTTON) & (1 << 15)));
                win32_message_process_keyboard(&newInput->mouseButtons[2],
                                               (b8)(GetKeyState(VK_MBUTTON) & (1 << 15)));
                win32_message_process_keyboard(&newInput->mouseButtons[3],
                                               (b8)(GetKeyState(VK_XBUTTON1) & (1 << 15)));
                win32_message_process_keyboard(&newInput->mouseButtons[4],
                                               (b8)(GetKeyState(VK_XBUTTON2) & (1 << 15)));
                // TODO(Jai): Need to not poll disconnected controllers to avoid
                // xinput frame rate hit on older libraries...
                // TODO(Jai): Should we poll this more frequently
                DWORD maxControllerCount = XUSER_MAX_COUNT;
                if (maxControllerCount > ARRAY_COUNT(input->controllers) - 1) {
                    maxControllerCount = ARRAY_COUNT(input->controllers) - 1;
                }
                for (DWORD controllerIndex = 0; controllerIndex < maxControllerCount; controllerIndex++) {
                    DWORD ourControllerIndex = controllerIndex + 1;
                    og_ControllerInput* oldController = getController(oldInput, ourControllerIndex);
                    og_ControllerInput* newController = getController(newInput, ourControllerIndex);
                    XINPUT_STATE controllerState;
                    if (XInputGetState(controllerIndex,
                                       &controllerState) == ERROR_SUCCESS) {
                        newController->isConnected = true;
                        newController->isAnalog = oldController->isAnalog;
                        XINPUT_GAMEPAD* gamePad = &controllerState.Gamepad;
                        newController->stickAverageX =
                            win32_xInput_process_stickValue(gamePad->sThumbLX,
                                                            XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                        newController->stickAverageY =
                            win32_xInput_process_stickValue(gamePad->sThumbLY,
                                                            XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                        if ((newController->stickAverageX != 0.0f) ||
							(newController->stickAverageY != 0.0f)) {
                            
                            newController->isAnalog = true;
                        }
                        if (gamePad->wButtons & XINPUT_GAMEPAD_DPAD_UP) {
                            newController->stickAverageY = 1.0f;
                        }
                        if (gamePad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
                            newController->stickAverageY = -1.0f;
                        }
                        if (gamePad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
                            newController->stickAverageX = -1.0f;
                        }
                        if (gamePad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
                            newController->stickAverageX = 1.0f;
                        }
                        f32 threshold = 0.5f;
                        DWORD fakeMoveButtons = (newController->stickAverageX > threshold ? 0 : 1);
                        win32_xInput_process_digitialButton(&newController->moveLeft,
                                                            &oldController->moveLeft,
                                                            fakeMoveButtons,
                                                            1);
                        fakeMoveButtons = (newController->stickAverageX < threshold ? 0 : 1);
                        win32_xInput_process_digitialButton(&newController->moveRight,
                                                            &oldController->moveRight,
                                                            fakeMoveButtons,
                                                            1);
                        fakeMoveButtons = (newController->stickAverageX > threshold ? 0 : 1);
                        win32_xInput_process_digitialButton(&newController->moveDown,
                                                            &oldController->moveDown,
                                                            fakeMoveButtons,
                                                            1);
                        fakeMoveButtons = (newController->stickAverageX < threshold ? 0 : 1);
                        win32_xInput_process_digitialButton(&newController->moveUp,
                                                            &oldController->moveUp,
                                                            fakeMoveButtons,
                                                            1);
                        
                        win32_xInput_process_digitialButton(&newController->actionDown,
                                                            &oldController->actionDown,
                                                            gamePad->wButtons,
                                                            XINPUT_GAMEPAD_A);
                        win32_xInput_process_digitialButton(&newController->actionRight,
                                                            &oldController->actionRight,
                                                            gamePad->wButtons,
                                                            XINPUT_GAMEPAD_B);
                        win32_xInput_process_digitialButton(&newController->actionLeft,
                                                            &oldController->actionLeft,
                                                            gamePad->wButtons,
                                                            XINPUT_GAMEPAD_X);
                        win32_xInput_process_digitialButton(&newController->actionUp,
                                                            &oldController->actionUp,
                                                            gamePad->wButtons,
                                                            XINPUT_GAMEPAD_Y);
                        win32_xInput_process_digitialButton(&newController->leftShoulder,
                                                            &oldController->leftShoulder,
                                                            gamePad->wButtons,
                                                            XINPUT_GAMEPAD_LEFT_SHOULDER);
                        win32_xInput_process_digitialButton(&newController->rightShoulder,
                                                            &oldController->rightShoulder,
                                                            gamePad->wButtons,
                                                            XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        win32_xInput_process_digitialButton(&newController->moveUp,
                                                            &oldController->moveUp,
                                                            gamePad->wButtons,
                                                            XINPUT_GAMEPAD_DPAD_UP);
                        win32_xInput_process_digitialButton(&newController->moveDown,
                                                            &oldController->moveDown,
                                                            gamePad->wButtons,
                                                            XINPUT_GAMEPAD_DPAD_DOWN);
                        win32_xInput_process_digitialButton(&newController->moveLeft,
                                                            &oldController->moveLeft,
                                                            gamePad->wButtons,
                                                            XINPUT_GAMEPAD_DPAD_LEFT);
                        win32_xInput_process_digitialButton(&newController->moveRight,
                                                            &oldController->moveRight,
                                                            gamePad->wButtons,
                                                            XINPUT_GAMEPAD_DPAD_RIGHT);
                        win32_xInput_process_digitialButton(&newController->start,
                                                            &oldController->start,
                                                            gamePad->wButtons,
                                                            XINPUT_GAMEPAD_START);
                        win32_xInput_process_digitialButton(&newController->back,
                                                            &oldController->back,
                                                            gamePad->wButtons,
                                                            XINPUT_GAMEPAD_BACK);
                    } else { newController->isConnected = false; }
                }
                
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
                                         &otter_videoBuffer,
                                         newInput);
                }
                
            }
            HDC deviceContext = GetDC(window);
            windowDimensions = win32_window_getDimensions(window);
            win32_window_update(deviceContext,
                                windowDimensions.width, windowDimensions.height);
            ReleaseDC(window, deviceContext);
            
            platformState.lastTime = currentTime;
        }
        
        PostMessageA(window,
                     WM_DESTROY,
                     0, 0);
    }
    
#if OG_D3D11
    if (d3d.swapChain) { IDXGISwapChain1_Release(d3d.swapChain); }
    if (d3d.deviceContext) { ID3D11DeviceContext_Release(d3d.deviceContext); }
    if (d3d.device) { ID3D11Device5_Release(d3d.device); }
#endif
}
