call %cd%\build_dll.bat

clang-cl^
	%defines%^
	%include_directories%^
	%compiler_flags%^
	%libs%^
	%code%\fpl_otter.c^
	-Fegame^
	%linker_flags%

popd
