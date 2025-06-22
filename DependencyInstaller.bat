@echo off
echo ========================================
echo Dependency Installer
echo ========================================
echo.

REM Check if Git is installed
git --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Git is not installed!
    echo Git is required to clone vcpkg and manage dependencies.
    echo.
    echo Please install Git from: https://git-scm.com/download/win
    echo After installing Git, run this script again.
    echo.
    pause
    exit /b 1
)

echo [OK] Git

REM Check if vcpkg is installed
if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo [WARNING] vcpkg not found at %VCPKG_ROOT%
    echo.
    set /p choice="Do you want to clone vcpkg to C:/? (Y/N): "
    if /i "%choice%"=="Y" (
        echo.
        echo [INFO] Cloning vcpkg to C:/...
        if not exist "C:\vcpkg" (
            git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
            if errorlevel 1 (
                echo [ERROR] Failed to clone vcpkg!
                echo Please check your internet connection and try again.
                echo.
                pause
                exit /b 1
            )
        ) else (
            echo [INFO] vcpkg directory already exists at C:\vcpkg
        )
        
        echo [INFO] Setting VCPKG_ROOT environment variable...
        set VCPKG_ROOT=C:\vcpkg
        echo [OK] vcpkg installed at: %VCPKG_ROOT%
    ) else (
        echo.
        echo Dependency Installation Aborted.
        echo.
        pause
        exit /b 1
    )
) else (
    echo [OK] vcpkg at: %VCPKG_ROOT%
)

REM Check if vcpkg.json exists
if not exist "vcpkg.json" (
    echo [ERROR] vcpkg.json not found!
    echo This file is required to define project dependencies.
    echo.
    pause
    exit /b 1
)

echo [OK] vcpkg.json found at project root

REM Install dependencies from vcpkg.json
echo [INFO] Installing dependencies from vcpkg.json...
echo.
"%VCPKG_ROOT%\vcpkg.exe" install --triplet=x64-windows

if errorlevel 1 (
    echo [ERROR] Failed to install dependencies!
    echo Please check your vcpkg installation and try again.
    echo.
    pause
    exit /b 1
)

echo [OK] Dependencies installed successfully

REM Display installed packages
echo [INFO] Installed packages:
"%VCPKG_ROOT%\vcpkg.exe" list
echo.

REM Check if CMake is available
cmake --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake not found!
    echo Please install CMake and add it to your PATH.
    echo Visit: https://cmake.org/download/
    echo.
    pause
    exit /b 1
)

echo [OK] CMake: 
echo.
cmake --version
echo.

echo Dependency installation completed successfully!
echo.
pause 