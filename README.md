# C++20 Test Features Project

This project demonstrates various C++20 features including modules, coroutines, three-way comparison, concepts, and more.

## Prerequisites

- **Visual Studio 2022** (version 17.0 or later) with C++ development tools
- **CMake 3.28 or later** (for C++20 modules support)

## Features Tested

- C++20 Modules (`.ixx` and `.cppm` files)
- Coroutines
- Three-way comparison operator (`<=>`)
- Concepts and constraints
- std::execution policies
- Atomic operations
- Template improvements
- Variant types
- And more...

## Building the Project

### Option 1: Using the Build Scripts

**Windows Batch Script:**
```cmd
build.bat
```

**PowerShell Script:**
```powershell
.\build.ps1
```

### Option 2: Manual CMake Build

1. Create a build directory:
```cmd
mkdir build
cd build
```

2. Configure the project:
```cmd
cmake -G "Visual Studio 17 2022" -A x64 ..
```

3. Build the project:
```cmd
cmake --build . --config Debug
```

4. Run the executable:
```cmd
bin\Debug\testcpp20.exe
```

### Option 3: Open in Visual Studio

1. Open Visual Studio 2022
2. Select "Open a local folder"
3. Navigate to the project root directory
4. Visual Studio will automatically detect the CMakeLists.txt file
5. Build and run the project using Visual Studio's interface

## Project Structure

```
cpp20Testnewfeature/
├── CMakeLists.txt          # Main CMake configuration
├── build.bat              # Windows batch build script
├── build.ps1              # PowerShell build script
├── README.md               # This file
└── testcpp20/             # Source code directory
    ├── Main.cpp           # Main entry point
    ├── *.ixx              # C++20 module interface files
    ├── *.cppm             # C++20 module files
    ├── *.cpp              # Implementation files
    └── *.h                # Header files
```

## Module Dependencies

The project includes several C++20 modules:
- `parentModule` - Base interface module
- `childmodule` - Implementation module
- `testIncpp` - Test module in .cppm format
- `CoroutineTest` - Coroutine demonstrations
- `compare_3_way` - Three-way comparison examples
- `ConstTest` - Const usage examples
- `OldTestAndTemplate` - Template improvements
- `VariantTest` - Variant type examples
- `STDExecution` - Execution policy examples
- `AtomicM` - Atomic operation examples
- `GreedyActivity` - Algorithm examples

## Troubleshooting

1. **CMake version too old**: Ensure you have CMake 3.28 or later for proper C++20 modules support
2. **Visual Studio version**: Make sure you're using Visual Studio 2022 (MSVC 193 or later)
3. **Module compilation errors**: Clean the build directory and rebuild from scratch
4. **Link errors**: Ensure all module dependencies are properly specified

## Notes

- This project requires C++20 modules support, which is experimental in Visual Studio 2022
- Module compilation order is automatically handled by CMake
- The project uses `/experimental:module` flag for MSVC
