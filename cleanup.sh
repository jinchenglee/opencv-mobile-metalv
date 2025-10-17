#!/bin/bash
# Cleanup script for OpenCV bare-metal repository
# Removes build artifacts, temporary files, and generated files

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}OpenCV Repository Cleanup${NC}"
echo -e "${BLUE}========================================${NC}"

# Function to remove directory with size info
remove_dir() {
    local dir=$1
    if [ -d "$dir" ]; then
        local size=$(du -sh "$dir" 2>/dev/null | cut -f1)
        echo -e "${YELLOW}Removing:${NC} $dir (${size})"
        rm -rf "$dir"
    fi
}

# Function to remove file with confirmation
remove_file() {
    local file=$1
    if [ -f "$file" ]; then
        echo -e "${YELLOW}Removing:${NC} $file"
        rm -f "$file"
    fi
}

echo ""
echo -e "${GREEN}Cleaning build directories...${NC}"
remove_dir "build_baremetal"
remove_dir "install_baremetal"
remove_dir "build"
remove_dir ".cache"

echo ""
echo -e "${GREEN}Cleaning CMake files...${NC}"
remove_file "CMakeCache.txt"
remove_dir "CMakeFiles"
remove_file "cmake_install.cmake"
remove_file "Makefile"

echo ""
echo -e "${GREEN}Cleaning generated files...${NC}"
remove_file "build.log"
remove_file "output.map"
remove_file "opencv_example.elf"
remove_file "opencv_example.bin"
remove_file "opencv_example.hex"
remove_file "opencv_example.dis"

echo ""
echo -e "${GREEN}Cleaning object files...${NC}"
find . -name "*.o" -type f -delete 2>/dev/null || true
find . -name "*.obj" -type f -delete 2>/dev/null || true
find . -name "*.a" -type f -not -path "./install_baremetal/*" -delete 2>/dev/null || true

echo ""
echo -e "${GREEN}Cleaning temporary files...${NC}"
find . -name "*~" -type f -delete 2>/dev/null || true
find . -name "*.swp" -type f -delete 2>/dev/null || true
find . -name "*.swo" -type f -delete 2>/dev/null || true
find . -name ".DS_Store" -type f -delete 2>/dev/null || true

echo ""
echo -e "${GREEN}Cleaning 3rdparty generated files...${NC}"
# Note: 3rdparty/flatbuffers is auto-recreated by build_baremetal.sh if needed
remove_dir "3rdparty/flatbuffers"

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}Cleanup complete!${NC}"
echo -e "${BLUE}========================================${NC}"

# Show remaining size
echo ""
echo -e "${YELLOW}Repository size after cleanup:${NC}"
du -sh . 2>/dev/null

echo ""
echo -e "${YELLOW}Tip:${NC} To start fresh, run: ${GREEN}./build_baremetal.sh${NC}"

