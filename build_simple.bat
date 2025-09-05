@echo off
echo Compilando BetterAltTab directamente...

REM Compilar directamente con cl.exe
cl.exe /O2 /DNDEBUG /DWIN32 /D_WINDOWS /EHsc ^
    main.cpp appicon.rc ^
    /link dwmapi.lib user32.lib gdi32.lib shell32.lib ole32.lib oleaut32.lib comdlg32.lib comctl32.lib shlwapi.lib ^
    /out:BetterAltTab.exe /subsystem:windows

if exist "BetterAltTab.exe" (
    echo Ejecutable generado exitosamente: BetterAltTab.exe
    del *.obj *.res 2>nul
) else (
    echo Error en la compilacion
)

pause
