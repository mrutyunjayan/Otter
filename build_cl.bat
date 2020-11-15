@echo off

call "J:\Program Files (x86)\Microsoft Visual Studio\2019\Preview\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64 >nul

IF NOT EXIST build mkdir build

set common_compiler_flags=^
	-std:c11^
	-MTd^
	-nologo^
	-FC^
	-fp:fast^
	-Gm-^
	-GR-^
	-EHa-^
	-Od^
	-Oi^
	-wd4068

set dll_compiler_flags=^
	-WX^
	-W4^
	-wd4201^
	-wd4100^
	-wd4189^
	-wd4505

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

set linker_flags= -incremental:no -opt:ref 

set dll_flags= ^
	%linker_flags%^
	-PDB:%pdbFileName% ^
	%exports%

pushd build

del *.pdb >nul 2>nul
del otter.dll >nul 2>nul

cl^
	-LD^
	%code%\otter.c^
	%defines%^
	%include_directories%^
	%common_compiler_flags%^
	%dll_compiler_flags%^
	-Fmotter.map^
	/link^
	%dll_flags%^
	%linker_flags%

cl^
	%defines%^
	%include_directories%^
	%common_compiler_flags%^
	%code%\fpl_otter.c^
	-Fmgame.map^
	-Fegame^
	/link -SUBSYSTEM:WINDOWS^
	%libs%^
	%linker_flags% 

popd