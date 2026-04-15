@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul

:: 获取项目根目录
set "PROJECT_ROOT=%~dp0"
cd /d "%PROJECT_ROOT%"

echo [INFO] START: Windows Native Build

:: 1. 清理旧产物
echo START: Cleaning old build files...
if exist build rmdir /s /q build
if exist .venv rmdir /s /q .venv
if exist uv.lock del /f /q uv.lock

:: 2. 检测 vcpkg
set "VCPKG_EXE="
where vcpkg >nul 2>nul
if %errorlevel% equ 0 (
    for /f "delims=" %%i in ('where vcpkg') do set "VCPKG_EXE=%%i"
) else if exist "%PROJECT_ROOT%vcpkg\vcpkg.exe" (
    set "VCPKG_EXE=%PROJECT_ROOT%vcpkg\vcpkg.exe"
)

if "%VCPKG_EXE%"=="" (
    echo [ERROR] vcpkg not found. Please run vcpkg_install.bat first.
    pause & exit /b 1
)
echo [INFO] Using vcpkg: %VCPKG_EXE%

:: 3. 准备 C++ 依赖
echo START: Installing C++ dependencies via vcpkg...
"%VCPKG_EXE%" install --triplet=x64-windows-static || (echo [ERROR] vcpkg install failed & pause & exit /b 1)

:: 4. 配置 CMake 参数并运行 uv sync
:: 获取 vcpkg.exe 所在的目录
for %%i in ("%VCPKG_EXE%") do set "VCPKG_DIR=%%~dpi"
set "TOOLCHAIN=%VCPKG_DIR%scripts\buildsystems\vcpkg.cmake"

:: 核心：设置环境变量。注意路径中可能有空格，所以整个变量值用双引号包裹
set "SKBUILD_CMAKE_ARGS=-DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN%;-DVCPKG_TARGET_TRIPLET=x64-windows-static;-DPython_FIND_STRATEGY=LOCATION"

echo START: Running uv sync...
uv cache clean --force
uv sync --all-groups || (echo [ERROR] uv sync failed & pause & exit /b 1)

:: 5. 验证运行
echo.
echo DONE! Testing Demo...
uv run python main.py

pause