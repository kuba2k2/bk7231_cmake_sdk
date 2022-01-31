# System Generic - no OS bare-metal application
set(CMAKE_SYSTEM_NAME Generic)
# Setup arm processor
set(CMAKE_SYSTEM_PROCESSOR arm)
# When trying to link cross compiled test program, error occurs, so setting test compilation to static library
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
