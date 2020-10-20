@echo off

call J:\"Program Files (x86)"\"Microsoft Visual Studio"\2019\Community\VC\Auxiliary\Build\vcvarsall.bat x86_amd64

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

del *.pdb
del otter.dll

clang-cl^
	%defines%^
	%compiler_flags%^
	%code%\otter.c^
	-Feotter^
	%dll_flags%


clang-cl^
	%include_directories%^
	%defines%^
	%compiler_flags%^
	%libs%^
	%code%\fpl_otter.c^
	-Fegame^
	%linker_flags%

popd
