# RISC-V 64-bit Bare Metal Toolchain with Newlib/Newlib-nano
# For use with riscv-none-elf-gcc toolchain

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

# Specify the cross compiler
set(CMAKE_C_COMPILER riscv-none-elf-gcc)
set(CMAKE_CXX_COMPILER riscv-none-elf-g++)
set(CMAKE_ASM_COMPILER riscv-none-elf-gcc)

# Where is the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Processor-specific flags
# Adjust -march based on your hardware:
# - rv64gc: RV64 with compressed instructions and standard extensions
# - rv64gcv: Add vector extensions (RVV)
# - rv64imafdc: Explicit list of extensions
set(RISCV_ARCH "rv64gc")
set(RISCV_ABI "lp64d")

# Common flags for both C and C++
set(COMMON_FLAGS "-march=${RISCV_ARCH} -mabi=${RISCV_ABI}")

# Choose newlib or newlib-nano
option(USE_NEWLIB_NANO "Use newlib-nano instead of newlib" ON)

if(USE_NEWLIB_NANO)
    # Newlib-nano: smaller, optimized for size
    set(COMMON_FLAGS "${COMMON_FLAGS} --specs=nano.specs")
    message(STATUS "Using newlib-nano")
else()
    # Standard newlib
    message(STATUS "Using newlib")
endif()

# Additional bare-metal flags
set(COMMON_FLAGS "${COMMON_FLAGS} -ffunction-sections -fdata-sections")
set(COMMON_FLAGS "${COMMON_FLAGS} -fno-exceptions -fno-rtti")

# Enable math constants (M_PI, M_E, etc.) - required for OpenCV
# This defines _GNU_SOURCE which enables POSIX and BSD extensions in newlib
set(COMMON_FLAGS "${COMMON_FLAGS} -D_GNU_SOURCE -D_USE_MATH_DEFINES")

# C specific flags
set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")

# C++ specific flags
set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -fno-use-cxa-atexit")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "${COMMON_FLAGS} -Wl,--gc-sections")

# For static libraries only (bare metal)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Optional: Disable some checks that require executable compilation
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# OpenCV-specific: indicate cross-compilation
set(CMAKE_CROSSCOMPILING TRUE)

# Set installation prefix
if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory" FORCE)
endif()

