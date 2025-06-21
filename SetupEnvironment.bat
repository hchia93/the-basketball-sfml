@echo off
echo Setting up development environment for Basketball SFML project...
echo.

REM Check if vcpkg is available
if not exist "C:\vcpkg\vcpkg.exe" (
    echo ERROR: vcpkg not found at C:\vcpkg\vcpkg.exe
    echo Please make sure vcpkg is installed at: C:\vcpkg
    echo Or update the path in this script to point to your vcpkg installation.
    pause
    exit /b 1
)

echo vcpkg found. Installing dependencies...
echo.

REM Install SFML
echo Installing SFML...
C:\vcpkg\vcpkg.exe install sfml:x64-windows
C:\vcpkg\vcpkg.exe install sfml:x86-windows

REM Install Box2D
echo Installing Box2D...
C:\vcpkg\vcpkg.exe install box2d:x64-windows
C:\vcpkg\vcpkg.exe install box2d:x86-windows

REM Integrate with Visual Studio
echo Integrating with Visual Studio...
C:\vcpkg\vcpkg.exe integrate install

echo.
echo Environment setup complete!
echo You can now run GenerateVisualStudioSolution.bat to create the Visual Studio solution.
pause 