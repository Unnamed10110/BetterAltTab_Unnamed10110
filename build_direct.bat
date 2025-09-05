@echo off
echo Compilando BetterAltTab directamente en la raiz...

REM Usar CMake con configuración mínima
cmake -B . -S . -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=.

REM Compilar
cmake --build . --config Release

REM Limpiar archivos temporales
if exist "CMakeCache.txt" del CMakeCache.txt
if exist "CMakeFiles" rmdir /s /q CMakeFiles
if exist "*.vcxproj" del *.vcxproj
if exist "*.vcxproj.filters" del *.vcxproj.filters
if exist "*.sln" del *.sln
if exist "*.cmake" del *.cmake

if exist "BetterAltTab.exe" (
    echo Ejecutable generado exitosamente: BetterAltTab.exe
) else (
    echo Error: No se pudo generar el ejecutable
)

pause
