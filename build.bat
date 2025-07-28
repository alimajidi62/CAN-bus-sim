@echo off
REM Build script for C++20 Test Project with Visual Studio 2022

echo Building C++20 Test Project...

REM Clean previous build
if exist build (
    echo Cleaning previous build...
    rmdir /s /q build
)

REM Create build directory
mkdir build
cd build

REM Configure with Visual Studio 2022
echo Configuring project with CMake for Visual Studio 2022...
cmake -G "Visual Studio 17 2022" -A x64 ..

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b %ERRORLEVEL%
)

REM Build the project
echo Building the project...
cmake --build . --config Debug

if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo Build completed successfully!
echo Executable location: build\bin\Debug\testcpp20.exe

REM Optional: Run the executable
set /p choice="Do you want to run the executable? (y/n): "
if /i "%choice%"=="y" (
    echo Running the executable...
    bin\Debug\testcpp20.exe
)

pause
