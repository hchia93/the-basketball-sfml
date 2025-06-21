@echo off
echo Generating Visual Studio solution for Basketball SFML project...
echo.

REM Check if vcpkg is available
if not exist "C:\vcpkg\vcpkg.exe" (
    echo ERROR: vcpkg not found at C:\vcpkg\vcpkg.exe
    echo Please make sure vcpkg is installed at: C:\vcpkg
    echo Or update the path in this script to point to your vcpkg installation.
    pause
    exit /b 1
)

REM Create build directory
if not exist "Build" mkdir Build
cd Build

REM Configure with CMake
echo Configuring project with CMake...
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -G "Visual Studio 17 2022" -A x64

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed.
    echo Please make sure you have CMake installed and Visual Studio 2022 is available.
    pause
    exit /b 1
)

echo.
echo Solution generated successfully in the Build directory!
echo You can now open BasketballSFML.sln in Visual Studio.
echo.
echo Build the project by running:
echo   cmake --build . --config Release
echo.
pause 