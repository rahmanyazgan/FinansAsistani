@echo off
chcp 65001 >nul
title Finans Asistani - AAB Yap (Google Play)
cd /d "%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0aab_yap.ps1"
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Bir hata meydana geldi veya islem iptal edildi.
    pause
)
