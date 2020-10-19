#include <Windows.h>
#include <stdio.h>
#include <stdint.h>

#define true 1
#define false 0

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef i32 b32;
typedef unsigned int uint;

typedef float f32;
typedef double f64;

#define uint unsigned int

#define internal static
#define global static

#define GET_VARIABLE_NAME(name) getVariableName(#name)

inline internal char*
getVariableName(char* name) {
    return name;
}

internal inline void 
win32_handleError() {
    DWORD error = GetLastError();
    char debugOutputBuffer[256];
    snprintf(debugOutputBuffer,
             sizeof(debugOutputBuffer),
             "%d\n",
             (int)error);
    OutputDebugStringA(debugOutputBuffer);
}

global b32 global_running = true;

typedef struct {
    u32 width;
    u32 height;
    u32 pitch;
    
    const u32 BYTES_PER_PIXEL;
    
    void* memory;
    BITMAPINFO info;
} win32_OffscreenBuffer;

internal void
win32_resizeDIBSection(win32_OffscreenBuffer* buffer,
                       u32 width, u32 height) {
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
    buffer->info.bmiHeader.biWidth =(LONG)buffer->width;
    buffer->info.bmiHeader.biHeight = -(LONG)buffer->height;
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32; //we only need 24 bits (8 bits each for RGB
    //8 extra bits for padding to align ourselves to 4 bytes
    buffer->info.bmiHeader.biCompression = BI_RGB;
    
    size_t bitmapMemorySize = buffer->width * buffer->height * buffer->BYTES_PER_PIXEL;
    
    buffer->memory = VirtualAlloc(NULL, 
                                  bitmapMemorySize, 
                                  MEM_RESERVE | MEM_COMMIT, 
                                  PAGE_READWRITE);
    // TODO(Jai): Probably clear this to black
    
    buffer->pitch = width * buffer->BYTES_PER_PIXEL;
}

internal void
win32_updateWindow(HDC deviceContext,
                   u32 x, u32 y,
                   u32 width, u32 height) {
    StretchDIBits(deviceContext,
                  x, y, width, height,
                  x, y, width, height,
                  
                  
}

internal LRESULT CALLBACK 
win32_mainWindowCallback(HWND   window,
                         UINT   message,
                         WPARAM wParam,
                         LPARAM lParam) {
    LRESULT result = {0};
    
    switch (message) {
        case WM_CLOSE: {
            global_running = false;
        } break;
        
        case WM_DESTROY: {
            global_running = false;
        } break;
        
        case WM_SIZE: {
            RECT clientRect;
            GetClientRect(window, &clientRect);
            
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            
            win32_resizeDIBSection(width, height);
        } break;
        
        case WM_PAINT: {
            OutputDebugStringA("Paint\n");
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            
            int x = paint.rcPaint.left;
            int y = paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;
            int height = paint.rcPaint.bottom - paint.rcPaint.top;
            
            PatBlt(deviceContext,
                   0, 0,
                   width, height,
                   BLACKNESS);
            
            win32_updateWindow(deviceContext,
                               x, y,
                               
                               
                               width, height);
            
            EndPaint(window, &paint);
        } break;
        
        default : {
            result = DefWindowProcA(window,
                                    message,
                                    wParam,
                                    lParam);
        } break;
    }
    
    return result;
}

int CALLBACK
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR     lpCmdLine,
        int       nShowCmd) {
    
    WNDCLASSA windowClass = {
        //.style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = win32_mainWindowCallback,
        .hInstance = hInstance,
        .lpszClassName = "gameWindowClass"
    };
    
    
    if (!RegisterClassA(&windowClass)) {
        win32_handleError();
        return -1;
    }
    
    char GAME_NAME;
    char* gameName = GET_VARIABLE_NAME(GAME_NAME); 
    
    HWND window = CreateWindowExA(WS_EX_LAYERED,
                                  windowClass.lpszClassName,
                                  "test",
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                  NULL,
                                  NULL,
                                  hInstance,
                                  NULL);
    
    if (!window) {
        win32_handleError();
        return -1;
    }
    
    MSG message = {0};
    while (global_running) {
        while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
    
    return 0;
}
