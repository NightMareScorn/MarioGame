@echo off
setlocal enabledelayedexpansion

:: Check for NuGet.exe
if not exist "nuget.exe" (
    echo Downloading nuget.exe...
    powershell -Command "Invoke-WebRequest -Uri 'https://dist.nuget.org/win-x86-commandline/latest/nuget.exe' -OutFile 'nuget.exe'"
    if !errorlevel! neq 0 (
        echo Error: Failed to download nuget.exe
        exit /b 1
    )
)

:: Restore packages
echo Restoring NuGet packages...
nuget.exe restore MarioGame.sln
if !errorlevel! neq 0 (
    echo Error: NuGet restore failed.
    exit /b 1
)

:: Find MSBuild
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "!VSWHERE!" (
    echo Error: vswhere.exe not found at !VSWHERE!
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"!VSWHERE!" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
    set "MSBUILD=%%i"
)

if not defined MSBUILD (
    echo Error: MSBuild.exe not found.
    exit /b 1
)

echo Using MSBuild: "!MSBUILD!"

set "CONFIG=Debug"
set "PLATFORM=x64"

if "%~1" neq "" set "CONFIG=%~1"
if "%~2" neq "" set "PLATFORM=%~2"

echo Building configuration: !CONFIG! - !PLATFORM!
"!MSBUILD!" MarioGame.sln /property:Configuration=!CONFIG! /property:Platform=!PLATFORM!
