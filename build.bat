@echo off
echo Compilando BetterAltTab...

REM Crear directorio temporal para build
if not exist "temp_build" mkdir temp_build
cd temp_build

REM Configurar CMake para generar en la raíz
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=..

REM Compilar
cmake --build . --config Release

REM Verificar si se generó el ejecutable y copiarlo a la raíz
if exist "Release\BetterAltTab.exe" (
    copy "Release\BetterAltTab.exe" "..\BetterAltTab.exe"
    echo Ejecutable generado exitosamente: BetterAltTab.exe
) else if exist "..\BetterAltTab.exe" (
    echo Ejecutable generado exitosamente: BetterAltTab.exe
) else (
    echo Error: No se pudo generar el ejecutable
)

REM Limpiar directorio temporal
cd ..
rmdir /s /q temp_build

echo Compilacion completada.
pause
