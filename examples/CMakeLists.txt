cmake_minimum_required(VERSION 3.17)

# Set toolchain file - otherwise CMake will complain
set(CMAKE_TOOLCHAIN_FILE ../Toolchain.cmake)

project(examples)

# Include the SDK
add_subdirectory(.. beken378)

# Create example executables
beken378_executable(hello_world 1.0.0 bk7231t hello_world/main.c)

beken378_executable(wifi_scan 1.0.0 bk7231t wifi_scan/wifi_scan.c)
target_include_directories(wifi_scan.elf PUBLIC wifi_scan/)

beken378_executable(wifi_softap 1.0.0 bk7231t wifi_softap/wifi_softap.c)
target_include_directories(wifi_softap.elf PUBLIC wifi_softap/)

beken378_executable(peri_flash 1.0.0 bk7231t peri_flash/test_flash.c)
target_include_directories(peri_flash.elf PUBLIC peri_flash/)

beken378_executable(net_mqtt 1.0.0 bk7231t net_mqtt/mqtt_test.c wifi_station/wifi_station.c)
target_include_directories(net_mqtt.elf PUBLIC net_mqtt/)
