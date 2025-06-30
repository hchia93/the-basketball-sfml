@echo off
echo ========================================
echo Solution Generator
echo ========================================
echo.

REM 1. Check vcpkg
if exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo [OK] vcpkg found at: %VCPKG_ROOT%
    goto :check_vcpkg_json
)

REM 1.2 vcpkg not installed, check git
git --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Git is not installed!
    echo Please install Git from: https://git-scm.com/download/win
    echo.
    pause
    exit /b 1
)

REM 1.2.1 git installed, install vcpkg
echo [INFO] Git found, installing vcpkg...
if not exist "C:\vcpkg" (
    git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
    if errorlevel 1 (
        echo [ERROR] Failed to clone vcpkg!
        echo.
        pause
        exit /b 1
    )
) else (
    echo [INFO] vcpkg directory already exists at C:\vcpkg
)

set VCPKG_ROOT=C:\vcpkg
echo [OK] vcpkg installed at: %VCPKG_ROOT%

:check_vcpkg_json
REM 2. Check vcpkg.json
if not exist "vcpkg.json" (
    echo [ERROR] vcpkg.json not found!
    echo.
    pause
    exit /b 1
)

echo [OK] vcpkg.json found

REM 2.1/2.2 Check if dependencies are installed
echo [INFO] Checking dependencies...
"%VCPKG_ROOT%\vcpkg.exe" install --triplet=x64-windows --dry-run >nul 2>&1
if errorlevel 1 (
    echo [INFO] Installing dependencies...
    "%VCPKG_ROOT%\vcpkg.exe" install --triplet=x64-windows
    if errorlevel 1 (
        echo [ERROR] Failed to install dependencies!
        echo.
        pause
        exit /b 1
    )
    echo [OK] Dependencies installed
) else (
    echo [OK] Dependencies already installed, skipping installation.
)

REM 3. Generate using cmake
if not exist "generated-vs" (
    mkdir generated-vs
)

cd generated-vs

echo [INFO] Generating Visual Studio solution...
cmake .. -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" -G "Visual Studio 17 2022" -A x64

if errorlevel 1 (
    echo [ERROR] CMake configuration failed!
    echo.
    pause
    exit /b 1
)

echo [SUCCESS] Solution generated successfully!
echo.

cd ..

pause 