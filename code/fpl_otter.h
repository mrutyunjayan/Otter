/* date = October 15th 2020 0:05 pm */

#ifndef FPL_OTTER_H
#define FPL_OTTER_H

#include "utils.h"

#if defined(FPL_PLATFORM_WINDOWS)

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
//~

typedef struct win32_GameCode{
    // either of these callbacks can be 0, check validity before call
    otter_updateAndRender* updateAndRender;
    
    FILETIME lastModifiedTime;
    HMODULE gameLibrary;
    
    b32 isValid;
} win32_GameCode;

#endif

#endif //FPL_OTTER_H
