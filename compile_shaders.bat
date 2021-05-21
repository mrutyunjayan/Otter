@ECHO OFF
ECHO ==================
ECHO SHADER COMPILATION
ECHO ==================
ECHO .

IF NOT DEFINED DevEnvDir (
	CALL "D:\VS\2019\VC\Auxiliary\Build\vcvars64.bat" >NUL
)

SET code=%cd%\code\shaders
SET outputDir=%cd%\build

SET compiler_flags=^
    -nologo^
    -Od^
    -Zi

ECHO VERTEX SHADERS
ECHO --------------
fxc %compiler_flags%^
    -T vs_5_0^
    -Fc %code%\vertex_assembly.txt^
    -Fo %outputDir%\vertex.cso^
    %code%\vertex.hlsl

ECHO .
ECHO PIXEL SHADERS
ECHO -------------
fxc %compiler_flags%^
    -T ps_5_0^
    -Fc %code%\pixel_assembly.txt^
    -Fo %outputDir%\pixel.cso^
    %code%\pixel.hlsl
ECHO .
ECHO .
ECHO .
ECHO .
