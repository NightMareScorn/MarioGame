@echo off
setlocal enabledelayedexpansion

:: ===================================================
:: Build MarioGame - C++ only (fast build)
:: Run "build.bat --with-ui" to also build Next.js UI
:: ===================================================

set BUILD_UI=0
if "%1"=="--with-ui" set BUILD_UI=1

:: Restore NuGet packages
echo Restoring NuGet packages...
nuget.exe restore MarioGame.sln -PackagesDirectory packages
if !errorlevel! neq 0 (
    echo Warning: NuGet restore failed, continuing anyway...
)

:: Configure CMake
echo Configuring CMake...
set CMAKE="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
%CMAKE% -S . -B build -G "Visual Studio 17 2022" -A x64
if !errorlevel! neq 0 (
    echo Error: CMake configuration failed.
    exit /b 1
)

:: Build C++ project
echo Building C++ project...
%CMAKE% --build build --config Debug
if !errorlevel! neq 0 (
    echo Error: C++ build failed.
    exit /b 1
)

:: Optionally build Next.js UI
if "%BUILD_UI%"=="1" (
    echo Building Next.js UI...
    cd ui
    call npm install
    if !errorlevel! neq 0 (
        echo Error: npm install failed.
        cd ..
        exit /b 1
    )
    call npm run build
    if !errorlevel! neq 0 (
        echo Error: npm run build failed.
        cd ..
        exit /b 1
    )
    cd ..
    
    :: Re-run cmake build to copy UI now that ui/out exists
    echo Copying UI to build output...
    cmake --build build --config Debug --target MarioGame
) else (
    echo [INFO] Skipping UI build. Use "build.bat --with-ui" to include it.
)

echo.
echo Build successful!
echo Executable: build\Debug\MarioGame.exe
echo.
