cmake_minimum_required(VERSION 3.28)
project(cpp20Testnewfeature LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable C++20 modules support in CMake
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)

# Set Visual Studio 2022 specific settings
if(MSVC)
    # Ensure we're using Visual Studio 2022 (version 17.0 or later)
    if(MSVC_VERSION LESS 1930)
        message(FATAL_ERROR "Visual Studio 2022 (version 17.0 or later) is required for C++20 modules support")
    endif()
    
    # Set compiler flags for C++20 modules
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:module")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    
    # Enable parallel compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    
    # Set warning level
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
endif()

# Set the source directory
set(SRC_DIR "${CMAKE_SOURCE_DIR}/testcpp20")

# Define module interface files (.ixx and .cppm)
set(MODULE_SOURCES
    "${SRC_DIR}/parentModule.ixx"
    "${SRC_DIR}/TestModule.ixx"
    "${SRC_DIR}/TestModuleInCpp.cppm"
    "${SRC_DIR}/CoroutineTest.ixx"
    "${SRC_DIR}/compare_3_way.ixx"
    "${SRC_DIR}/ConstTest.ixx"
    "${SRC_DIR}/OldTestAndTemplate.ixx"
    "${SRC_DIR}/VariantTest.ixx"
    "${SRC_DIR}/STDExecution.ixx"
    "${SRC_DIR}/AtomicM.ixx"
    "${SRC_DIR}/GreedyActivity.ixx"
)

# Define implementation files (.cpp)
set(IMPLEMENTATION_SOURCES
    "${SRC_DIR}/Main.cpp"
    "${SRC_DIR}/TestTuple.cpp"
    "${SRC_DIR}/TestCalssInModule.cpp"
)

# Define header files (.h)
set(HEADER_FILES
    "${SRC_DIR}/TestTuple.h"
    "${SRC_DIR}/TestCalssInModule.h"
)

# Create the executable target
add_executable(testcpp20)

# Add sources to the target
target_sources(testcpp20 
    PRIVATE
        ${IMPLEMENTATION_SOURCES}
        ${HEADER_FILES}
    PUBLIC
        FILE_SET CXX_MODULES FILES
            ${MODULE_SOURCES}
)

# Set target properties
set_target_properties(testcpp20 PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include current source directory for headers
target_include_directories(testcpp20 PRIVATE "${SRC_DIR}")

# Additional compiler-specific settings
if(MSVC)
    target_compile_options(testcpp20 PRIVATE
        /std:c++20
        /experimental:module
        /utf-8
        /MP
        /W3
        /wd5276  # Suppress warning about /experimental:ifcDebugRecords
    )
    
    # Set runtime library to Multi-threaded DLL
    set_property(TARGET testcpp20 PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Set output directory
set_target_properties(testcpp20 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)

# Print some useful information
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Optional: Add custom target for cleaning module cache
if(MSVC)
    add_custom_target(clean-modules
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/CMakeFiles/testcpp20.dir"
        COMMENT "Cleaning module cache"
    )
endif()