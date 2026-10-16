@echo off
title Compilar Waterfox Lite Native UI (C)
echo Compilando interfaz nativa ultra-ligera en C...
gcc -O3 -s waterfox_lite_ui.c -o WaterfoxLite_NativeUI.exe -mwindows -lgdi32 -luser32 -lcomctl32 -lpsapi -lshell32 -ldwmapi
if %ERRORLEVEL% EQU 0 (
    echo [OK] Compilado con exito: WaterfoxLite_NativeUI.exe
) else (
    echo [ERROR] Fallo la compilacion con GCC.
)
pause
