@echo off

set start_minute=%time:~3,2%
set start_minunte=%start_minute:~0,4%
set start_second=%time:~6,2%
set /A start_minute = %start_minute% * 60 
set /A start_time = %start_minute% + %start_second%

call J:\"Program Files (x86)"\"Microsoft Visual Studio"\2019\Community\VC\Auxiliary\Build\vcvarsall.bat x86_amd64 >nul

IF NOT EXIST build mkdir build

set compiler_flags=^
	-nologo^
	-FC^
	-fp:fast^
	-GR-^
	-EHa-^
	-Od^
	-Oi^
	-MTd^
	-Z7^
	-TC^
	-Wall^
	-WX^
	-ferror-limit=50^
	-Wno-unused-parameter^
	-Wno-unused-variable^
	-Wno-unused-function^
	-Wno-nonportable-system-include-path^
	-Wno-missing-prototypes^
	-Wno-cast-align^
	-Wno-string-conversion^
	-Wno-extra-semi-stmt^
	-Wno-extra-semi^
	-Wno-missing-braces^
	-Wno-shift-sign-overflow

set pdbFileName=otter_%random%.pdb

set code=%cd%\code

set include_directories=^
	-I^
	%cd%\include

set exports=^
	-EXPORT:otterUpdateAndRender

set defines=^
	-DFPL_IMPLEMENTATION^
	-DOTTER_DEBUG

set libs=^
	User32.lib^
	Gdi32.lib^
	Winmm.lib

set linker_flags=^
	/link^
	-incremental:no^
	-opt:ref

set dll_flags= ^
	-LD^
	%linker_flags%^
	-PDB:%pdbFileName% ^
	%exports%

pushd build

del *.pdb >nul 2>nul
del otter.dll >nul 2>nul

clang-cl^
	%defines%^
	%include_directories%^
	%compiler_flags%^
	%code%\otter.c^
	-Feotter^
	%dll_flags%

set end_minute=%time:~3,2%
set end_second=%time:~6,2%
set /A end_minute = %end_minute% * 60
set /A end_time = %end_minute% + %end_second%

set /A compile_time = %end_time% - %start_time%

echo    Compiled DLL in: ~%compile_time% second(s)
popd
