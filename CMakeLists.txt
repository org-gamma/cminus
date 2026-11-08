
cmake_minimum_required(VERSION 3.16)

project(CMinus
    VERSION 0.1.0
    DESCRIPTION "CMinus programming language transpiler"
    LANGUAGES CXX
)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler executable
add_executable(cminus
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/transpiler.cpp
    src/errors.cpp
)

# Include directories
target_include_directories(cminus PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# Compiler warnings
if(MSVC)
    target_compile_options(cminus PRIVATE /W4)
else()
    target_compile_options(cminus PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# Output directory
set_target_properties(cminus PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build"
)

# Installation
install(TARGETS cminus
    RUNTIME DESTINATION bin
)
