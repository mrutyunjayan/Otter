@ECHO OFF

IF NOT DEFINED DevEnvDir (
	CALL "D:\VS\2019\VC\Auxiliary\Build\vcvars64.bat" >NUL
)

IF NOT EXIST build MKDIR build

SET compiler_flags=^
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

SET pdbFileName=otter_%random%.pdb

SET code=%cd%\code

SET include_directories=^
	-I^
	%cd%\include

SET exports=^
	-EXPORT:otterUpdateAndRender

SET defines=^
	-DOTTER_DEBUG=1^
	-DOTTER_INTERNAL=1

SET libs=^
	User32.lib^
	Gdi32.lib^
	Winmm.lib

SET linker_flags=^
	/link^
	-incremental:no^
	-opt:ref

SET dll_flags= ^
	-LD^
	%linker_flags%^
	-PDB:%pdbFileName% ^
	%exports%

PUSHD build

DEL *.pdb >NUL 2>NUL
DEL otter.dll >NUL 2>NUL

ECHO waiting for pdb... >lock.tmp

clang-cl^
	%defines%^
	%include_directories%^
	%compiler_flags%^
	%code%\otter.c^
	-Feotter^
	%dll_flags%

DEL lock.tmp >NUL 2>NUL

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


