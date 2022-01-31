cmake_minimum_required(VERSION 3.17)

set(CMAKE_TOOLCHAIN_FILE Toolchain.cmake)

project(beken378)

set(PLATFORMS ${CMAKE_CURRENT_SOURCE_DIR}/platform CACHE INTERNAL "")
set(ENCRYPT ${CMAKE_CURRENT_SOURCE_DIR}/tools/encrypt CACHE INTERNAL "")
set(BK_PACKAGER python ${CMAKE_CURRENT_SOURCE_DIR}/tools/beken_packager_wrapper CACHE INTERNAL "")
set(OTA_PACK_RT ${CMAKE_CURRENT_SOURCE_DIR}/tools/rt_ota_packaging_tool_cli CACHE INTERNAL "")
set(OTA_PACK_TY ${CMAKE_CURRENT_SOURCE_DIR}/tools/package CACHE INTERNAL "")

add_subdirectory(platform/bk7231t bk7231t_sdk)

function(beken378_executable TARGET VERSION PLATFORM)
    set(SRCS ${ARGN})
    set(FW "${TARGET}_${VERSION}")
    set(ELF "${TARGET}.elf")
    set(MAP "${FW}.map")
    set(ASM "${FW}.asm")
    set(BIN "${FW}.bin")
    set(BIN_UA "${FW}_UA.bin")
    set(BIN_UG "${FW}_UG.bin")
    set(BIN_QIO "${FW}_QIO.bin")

    set(CMAKE_EXE_LINKER_FLAGS "-g -Wl,--gc-sections -marm -mcpu=arm968e-s -mthumb-interwork -nostdlib -Xlinker -Map=beken.map -Wl,-wrap,malloc -Wl,-wrap,free -Wl,-wrap,zalloc -T${PLATFORMS}/${PLATFORM}.ld" PARENT_SCOPE)
    set_source_files_properties(${SRCS} PROPERTIES COMPILE_FLAGS "-g -mthumb -mcpu=arm968e-s -march=armv5te -mthumb-interwork -mlittle-endian -Os -std=c99 -ffunction-sections -Wall -fsigned-char -fdata-sections -Wunknown-pragmas -nostdlib -Wno-unused-function -Wno-unused-but-set-variable")
    add_executable(${ELF} ${SRCS})
    target_link_libraries(${ELF} PUBLIC ${PLATFORM}_sdk)

    add_custom_command(
            OUTPUT ${MAP}
            COMMAND ${CMAKE_NM} ${ELF} | sort > ${MAP}
            DEPENDS ${ELF}
    )
    add_custom_command(
            OUTPUT ${ASM}
            COMMAND ${CMAKE_OBJDUMP} -d ${ELF} > ${ASM}
            DEPENDS ${ELF}
    )
    add_custom_command(
            OUTPUT ${BIN}
            COMMAND ${CMAKE_OBJCOPY} -O binary ${ELF} ${BIN}
            DEPENDS ${ELF}
    )
    add_bk_encrypt(${FW})
    add_bk_packager(${FW})
    add_ota_package(${FW} ${VERSION})
    add_custom_target(
            ${TARGET}
            DEPENDS ${BIN_UA} ${BIN_UG} ${BIN_QIO} ${MAP} ${ASM}
            SOURCES ${SRCS}
    )
endfunction()

# beken378_executable(app_name 1.0.0 bk7231t main.c)
