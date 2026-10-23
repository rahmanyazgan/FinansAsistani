@echo off
chcp 65001 >nul
title Finans Asistani - APK Yap
cd /d "%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0apk_yap.ps1"
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Bir hata meydana geldi veya islem iptal edildi.
    pause
)
