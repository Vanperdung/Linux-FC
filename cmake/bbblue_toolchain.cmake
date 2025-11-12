# =============================================================================
# BeagleBone Blue Toolchain File
# Target: ARMv7 / GCC 8.3
# =============================================================================

# Cross-system type
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX "arm-linux-gnueabihf" CACHE STRING "Target compiler prefix")

set(TOOLCHAIN "gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf")
set(TOOLCHAIN_DIR ${CMAKE_SOURCE_DIR}/.toolchain)
set(TOOLCHAIN_BIN ${TOOLCHAIN_DIR}/${TOOLCHAIN}/bin)

# Check if toolchain exists or not
if(NOT EXISTS ${TOOLCHAIN_BIN})
    message(STATUS "ARM toolchain not found, downloading...")
    file(DOWNLOAD
        "https://github.com/armbian/mirror/releases/download/_toolchain/${TOOLCHAIN}.tar.xz"
        "${TOOLCHAIN_DIR}/${TOOLCHAIN}.tar.xz"
        SHOW_PROGRESS
    )

    message(STATUS "Download ARM toolchain successfully, extracting...")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${TOOLCHAIN_DIR}/${TOOLCHAIN}.tar.xz
        WORKING_DIRECTORY ${TOOLCHAIN_DIR}
    )
endif()

# Set compiler
set(CMAKE_C_COMPILER "${TOOLCHAIN_BIN}/${TOOLCHAIN_PREFIX}-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_BIN}/${TOOLCHAIN_PREFIX}-g++")