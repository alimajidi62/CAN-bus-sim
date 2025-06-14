cmake_minimum_required(VERSION 3.16)
project(cpp20Testnewfeature LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the source directory
set(SRC_DIR "${CMAKE_SOURCE_DIR}/testcpp20")

# Gather all .cpp, .cppm, .ixx files in testcpp20
file(GLOB_RECURSE SOURCES
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.cppm"
    "${SRC_DIR}/*.ixx"
)

# Add executable from all sources
add_executable(testcpp20 ${SOURCES})

# For MSVC and C++20 modules support (optional, but recommended)
if (MSVC)
    target_compile_options(testcpp20 PRIVATE /std:c++20 /experimental:module)
endif()