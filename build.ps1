# PowerShell Build script for C++20 Test Project with Visual Studio 2022

Write-Host "Building C++20 Test Project..." -ForegroundColor Green

# Clean previous build
if (Test-Path "build") {
    Write-Host "Cleaning previous build..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force "build"
}

# Create build directory
New-Item -ItemType Directory -Name "build"
Set-Location "build"

try {
    # Configure with Visual Studio 2022
    Write-Host "Configuring project with CMake for Visual Studio 2022..." -ForegroundColor Cyan
    cmake -G "Visual Studio 17 2022" -A x64 ..
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed!"
    }

    # Build the project
    Write-Host "Building the project..." -ForegroundColor Cyan
    cmake --build . --config Debug
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed!"
    }

    Write-Host "Build completed successfully!" -ForegroundColor Green
    Write-Host "Executable location: build\bin\Debug\testcpp20.exe" -ForegroundColor Yellow

    # Optional: Run the executable
    $choice = Read-Host "Do you want to run the executable? (y/n)"
    if ($choice -eq "y" -or $choice -eq "Y") {
        Write-Host "Running the executable..." -ForegroundColor Cyan
        & "bin\Debug\testcpp20.exe"
    }
}
catch {
    Write-Host "Error: $_" -ForegroundColor Red
    exit 1
}
finally {
    Set-Location ".."
}

Write-Host "Press any key to continue..." -ForegroundColor Gray
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
