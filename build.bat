@echo off
setlocal enabledelayedexpansion

:: Configure CMake (this will also handle NuGet restore via CMakeLists.txt)
echo Configuring CMake...
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
if !errorlevel! neq 0 (
    echo Error: CMake configuration failed.
    exit /b 1
)

:: Build using CMake
echo Building project...
cmake --build build --config Debug
if !errorlevel! neq 0 (
    echo Error: Build failed.
    exit /b 1
)

:: Build Next.js UI
echo Building Next.js UI...
cd ui
call npm install
call npm run build
cd ..
if not exist "ui\out" (
    echo Error: UI build failed.
    exit /b 1
)

echo.
echo Build successful! 
echo Solution file: build/MarioGame.sln
echo Executable: build/Debug/MarioGame.exe
echo.
pause
