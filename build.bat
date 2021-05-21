@ECHO OFF
ECHO  ==================
ECHO [ FULL COMPILATION ]
ECHO  ==================
ECHO .
CALL %cd%\compile_shaders.bat
CALL %cd%\build_dll.bat

PUSHD build

ECHO  =======================
ECHO  WIN32 LAYER COMPILATION
ECHO  =======================

clang-cl^
	%defines%^
	%include_directories%^
	%compiler_flags%^
	%libs%^
	%code%\win32_otter.c^
	-Fegame^
	%linker_flags%^
	-SUBSYSTEM:WINDOWS

POPD


