global b32 global_running = true;

global fplVideoBackbuffer* global_videoBackbufferPtr;

global u32 playerX = 100;
global u32 playerY = 100;
global u32 playerSize = 10;

//~ LOAD ENGINE DLL

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

//~

win32_handleEvent() {
	
}

win32_clearVideoBackbuffer() {
	
}

win32_mainWindowCallBack() {
	
}

INT 
WinMain(HINSTANCE hInstance, 
		HINSTANCE hPrevInstance,
		PSTR lpCmdLine, 
		INT nCmdShow) {
	
	// Register the window class.
	WNDCLASSA windowClass = {0};
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = win32_mainWindowCallBack;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = "OtterWindowClass";
	
	if (!RegisterClassA(&windowClass)) {
		// TODO(Jai): Handle Error
		return -1;
	}
	
    HWND window = CreateWindowExA(WS_EX_TOPMOST | WS_EX_LAYERED,//0,
                                  windowClass.lpszClassName,
                                  "Otter Engine",
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  NULL,
                                  NULL,
                                  instance,
                                  NULL);
    if (!window) {
        // TODO(Jai): handle error
        return -1;
    }
    
    SetWindowPos(window,
                 HWND_NOTOPMOST,
                 0, 0,
                 1000, 600,
                 0);
    
    return 0;
}