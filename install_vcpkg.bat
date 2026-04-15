@echo off
:: Move to the script's directory
cd /d "%~dp0"

echo [1/3] Detecting vcpkg...

if not exist "vcpkg\" (
    echo [2/3] Cloning vcpkg from GitHub...
    git clone --depth=1 https://github.com/microsoft/vcpkg.git
) else (
    echo [2/3] vcpkg folder already exists, skipping clone.
)

if exist "vcpkg\bootstrap-vcpkg.bat" (
    echo [3/3] Running vcpkg bootstrap...
    cd vcpkg
    call bootstrap-vcpkg.bat -disableMetrics
    cd ..
    echo SUCCESS: vcpkg is ready.
) else (
    echo ERROR: vcpkg/bootstrap-vcpkg.bat not found.
)

pause