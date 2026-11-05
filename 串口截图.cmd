@echo off
setlocal
chcp 65001 >nul
cd /d "%~dp0"
set "BAMBU_SCREENSHOT_PYTHON=D:\1.Soft\Espressif\python_env\idf5.5_py3.11_env\Scripts\python.exe"
if defined IDF_PYTHON_ENV_PATH if exist "%IDF_PYTHON_ENV_PATH%\Scripts\python.exe" set "BAMBU_SCREENSHOT_PYTHON=%IDF_PYTHON_ENV_PATH%\Scripts\python.exe"
if not exist "%BAMBU_SCREENSHOT_PYTHON%" set "BAMBU_SCREENSHOT_PYTHON=python"
"%BAMBU_SCREENSHOT_PYTHON%" -X utf8 tools\capture_screenshot.py %*
if errorlevel 1 (
    echo.
    echo Screenshot failed. Close the serial monitor and check COM6 / USB.
    pause
    exit /b 1
)
exit /b 0
