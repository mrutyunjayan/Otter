/* date = January 11th 2021 9:53 pm */

#ifndef WIN32_OTTER_H
#define WIN32_OTTER_H

//~ NOTE(Jai): Recursive macro definition only for 
// editor syntax highlighting
#define DWORD DWORD
#define FILETIME FILETIME
#define HMODULE HMODULE
#define HANDLE HANDLE
#define BITMAPINFO BITMAPINFO
#define HDC HDC
#define LARGE_INTEGER LARGE_INTEGER
#define XINPUT_STATE XINPUT_STATE
#define LONG LONG
#define SHORT SHORT
#define LPVOID LPVOID
#define HINSTANCE HINSTANCE
#define PSTR PSTR
#define HRESULT HRESULT
#define LPCGUID LPCGUID
#define LPUNKNOWN LPUNKNOWN
#define XINPUT_VIBRATION XINPUT_VIBRATION
#define WIN32_FIND_DATAA WIN32_FIND_DATAA
#define WIN32_FILE_ATTRIBUTE_DATA WIN32_FILE_ATTRIBUTE_DATA
#define LPDIRECTSOUNDBUFFER LPDIRECTSOUNDBUFFER
#define DSBUFFERDESC DSBUFFERDESC
#define WAVEFORMATEX WAVEFORMATEX
#define LPDIRECTSOUND LPDIRECTSOUND
#define MSG MSG
#define HCURSOR HCURSOR
#define PAINTSTRUCT PAINTSTRUCT
#define MONITORINFO MONITORINFO
#define LPARAM LPARAM
#define WINDOWPLACEMENT WINDOWPLACEMENT
#define WPARAM WPARAM
#define UINT UINT
#define HWND HWND
#define LRESULT LRESULT
#define SIZE_T SIZE_T
#define WNDCLASSA WNDCLASSA
#define WNDCLASS WNDCLASS
#define INT INT
#define WNDPROC WNDPROC
#define HMENU HMENU
#define LPDWORD LPDWORD
#define RECT RECT
#define LONG_PTR LONG_PTR
#define LPSECURITY_ATTRIBUTES LPSECURITY_ATTRIBUTES
#define LPOVERLAPPED LPOVERLAPPED
#define LPSTR LPSTR 

#define WIN32_STATE_FILE_NAME_COUNT MAX_PATH

//~ CLOCK
typedef struct Clock {
    f64 frequency;
    f64 startTime; // in seconds
    f64 elapsed; // in seconds
} Clock;

//~ BACKBUFFER
typedef struct win32_OffScreenBuffer {
    // NOTE(Jai): Pixels are always 32 bits wide, Memory Order: BB GG RR xx
    u32 width;
    u32 height;
    u32 pitch;
    u32 pixelStride;
    BITMAPINFO info;
    void* memory;
} win32_OffScreenBuffer;

typedef struct win32_WindowDimensions{
    int width;
    int height;
} win32_WindowDimensions;

//~ PLATFORM STATE
typedef struct win32_State {
    void* gameMemoryBlock;
    u64 totalSize;
    
    char exeFileName[WIN32_STATE_FILE_NAME_COUNT];
    char* onePastLastSlash;
    
    Clock clock;
    f64 lastTime;
} win32_State;

typedef struct{
    // either of these callbacks can be 0, check validity before call
    otter_updateAndRender* updateAndRender;
    
    FILETIME lastModifiedTime;
    HMODULE gameLibrary;
    
    b32 isValid;
} win32_GameCode;

#endif //WIN32_OTTER_H
