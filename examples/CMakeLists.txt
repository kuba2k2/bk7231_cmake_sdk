cmake_minimum_required(VERSION 3.17)

# Set toolchain file - otherwise CMake will complain
set(CMAKE_TOOLCHAIN_FILE ../Toolchain.cmake)

project(examples)

# Include the SDK
add_subdirectory(.. beken378)

# Create example executables
beken378_executable(hello_world 1.0.0 bk7231t hello_world/main.c)
