#!/bin/bash
# Build script for OpenCV on RISC-V bare-metal with newlib

set -e  # Exit on error

# Configuration
BUILD_DIR="build_baremetal"
INSTALL_DIR="install_baremetal"
TOOLCHAIN_FILE="toolchains/riscv64-baremetal-newlib.toolchain.cmake"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}OpenCV Bare-Metal RISC-V Build${NC}"
echo -e "${GREEN}========================================${NC}"

# Check if toolchain is available
if ! command -v riscv-none-elf-gcc &> /dev/null; then
    echo -e "${RED}ERROR: riscv-none-elf-gcc not found in PATH${NC}"
    echo "Please install RISC-V bare-metal toolchain first"
    echo "  Ubuntu/Debian: sudo apt-get install gcc-riscv-none-elf"
    echo "  Or download from: https://github.com/riscv-collab/riscv-gnu-toolchain"
    exit 1
fi

echo -e "${YELLOW}Toolchain found:${NC}"
riscv-none-elf-gcc --version | head -n 1

# Clean previous build
if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Workaround: Create placeholder for flatbuffers license (required by install target)
echo -e "${YELLOW}Creating 3rdparty license placeholders...${NC}"
mkdir -p 3rdparty/flatbuffers
cat > 3rdparty/flatbuffers/LICENSE.txt << 'EOF'
FlatBuffers License
-------------------
This is a placeholder license file for the FlatBuffers library.
OpenCV uses FlatBuffers internally but does not require the full library installation.

FlatBuffers is licensed under the Apache License, Version 2.0.
For the actual license, see: https://github.com/google/flatbuffers
EOF

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
echo -e "${GREEN}Configuring CMake...${NC}"
cmake \
    -DCMAKE_TOOLCHAIN_FILE="../${TOOLCHAIN_FILE}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="../${INSTALL_DIR}" \
    \
    `# Use newlib-nano for smaller size` \
    -DUSE_NEWLIB_NANO=ON \
    \
    `# Static linking for bare metal` \
    -DBUILD_SHARED_LIBS=OFF \
    \
    `# Disable OS-dependent features` \
    -DOPENCV_DISABLE_THREAD_SUPPORT=ON \
    -DOPENCV_DISABLE_FILESYSTEM_SUPPORT=ON \
    -DOPENCV_DISABLE_ENV_SUPPORT=ON \
    \
    `# Disable parallel frameworks` \
    -DWITH_PTHREADS_PF=OFF \
    -DWITH_OPENMP=OFF \
    \
    `# Disable all external dependencies` \
    -DWITH_EIGEN=OFF \
    -DWITH_IPP=OFF \
    -DWITH_ITT=OFF \
    -DWITH_LAPACK=OFF \
    \
    `# Module selection - only core and imgproc` \
    -DBUILD_LIST=core,imgproc \
    \
    `# Disable all optional modules` \
    -DBUILD_opencv_apps=OFF \
    -DBUILD_opencv_java=OFF \
    -DBUILD_opencv_python=OFF \
    -DBUILD_opencv_js=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_PERF_TESTS=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_DOCS=OFF \
    \
    `# Disable 3rdparty libraries` \
    -DBUILD_ZLIB=OFF \
    -DBUILD_JPEG=OFF \
    -DBUILD_PNG=OFF \
    -DBUILD_TIFF=OFF \
    -DBUILD_WEBP=OFF \
    \
    `# Additional optimizations` \
    -DENABLE_FAST_MATH=ON \
    -DCV_ENABLE_INTRINSICS=ON \
    -DENABLE_PRECOMPILED_HEADERS=OFF \
    \
    `# RISC-V vector extensions (if supported by your hardware)` \
    -DWITH_HAL_RVV=OFF \
    `# Set to ON and uncomment if you have RVV support:` \
    `# -DCPU_BASELINE=RVV` \
    \
    ..

if [ $? -ne 0 ]; then
    echo -e "${RED}CMake configuration failed!${NC}"
    exit 1
fi

# Build
echo -e "${GREEN}Building OpenCV...${NC}"
cmake --build . --config Release -j$(nproc)

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

# Install
echo -e "${GREEN}Installing...${NC}"
cmake --build . --target install

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Build completed successfully!${NC}"
echo -e "${GREEN}========================================${NC}"
echo -e "Installation directory: ${YELLOW}${INSTALL_DIR}${NC}"
echo ""
echo -e "${YELLOW}Important files:${NC}"
echo "  Libraries:   ${INSTALL_DIR}/lib/"
echo "  Headers:     ${INSTALL_DIR}/include/"
echo "  Syscalls:    bare_metal_syscalls.c (compile with your project)"
echo "  Linker:      riscv_baremetal.ld (use with -T flag)"
echo ""
echo -e "${YELLOW}To use in your project:${NC}"
echo "  1. Compile bare_metal_syscalls.c with your application"
echo "  2. Link with: -L${INSTALL_DIR}/lib -lopencv_imgproc -lopencv_core"
echo "  3. Use linker script: -T riscv_baremetal.ld"
echo "  4. Include headers: -I${INSTALL_DIR}/include/opencv4"

