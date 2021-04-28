CALL %cd%\build_dll.bat

PUSHD build

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


