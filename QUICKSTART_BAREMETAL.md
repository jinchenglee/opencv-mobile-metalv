# Quick Start Guide: OpenCV on RISC-V Bare Metal

## TL;DR - Get Started in 5 Minutes

```bash
# 1. Build OpenCV
./build_baremetal.sh

# 2. Build example application
make -f Makefile.example

# 3. Your application binary is ready
# opencv_example.elf - Load onto your hardware
# opencv_example.bin - Raw binary format
```

## File Overview

| File | Purpose |
|------|---------|
| `toolchains/riscv64-baremetal-newlib.toolchain.cmake` | CMake toolchain config |
| `bare_metal_syscalls.c` | Newlib syscall implementations |
| `riscv_baremetal.ld` | Linker script (memory layout) |
| `start.S` | Startup assembly code |
| `example_app.cpp` | Example OpenCV application |
| `build_baremetal.sh` | Build script for OpenCV |
| `Makefile.example` | Makefile for your applications |

## Customization Checklist

### Before Building OpenCV

1. ✅ **Install toolchain**
   ```bash
   sudo apt-get install gcc-riscv-none-elf
   ```

2. ✅ **Configure architecture** (edit toolchain file)
   ```cmake
   set(RISCV_ARCH "rv64gc")     # Your ISA
   set(RISCV_ABI "lp64d")       # Your ABI
   ```

3. ✅ **Choose newlib variant** (edit toolchain file)
   ```cmake
   option(USE_NEWLIB_NANO "Use newlib-nano" ON)  # ON for smaller size
   ```

### Before Building Your Application

4. ✅ **Configure memory layout** (edit `riscv_baremetal.ld`)
   ```ld
   RAM : ORIGIN = 0x80000000, LENGTH = 256M  # Match your hardware
   HEAP_SIZE = 0x1000000  # 16MB - adjust based on image sizes
   STACK_SIZE = 0x10000   # 64KB - adjust if needed
   ```

5. ✅ **Implement UART** (edit `bare_metal_syscalls.c`)
   ```c
   int _write(int file, char *ptr, int len) {
       // Add your UART code here for printf() support
   }
   ```

6. ✅ **Implement timer** (edit `bare_metal_syscalls.c`)
   ```c
   int _gettimeofday(struct timeval *tv, void *tz) {
       // Add your timer code here for timing functions
   }
   ```

## Common Use Cases

### Case 1: Minimal Size (< 1MB code)

```bash
# Build with newlib-nano and size optimization
cmake ... -DUSE_NEWLIB_NANO=ON ...
make -f Makefile.example OPT_FLAGS=-Os
```

### Case 2: With Vector Extensions

```bash
# Edit toolchain: set(RISCV_ARCH "rv64gcv")
./build_baremetal.sh
# Add to CMake: -DWITH_HAL_RVV=ON -DCPU_BASELINE=RVV
```

### Case 3: 32-bit RISC-V

```bash
# Use riscv32-unknown-elf-gcc
# Edit toolchain: 
#   set(CMAKE_SYSTEM_PROCESSOR riscv32)
#   set(RISCV_ARCH "rv32gc")
#   set(RISCV_ABI "ilp32d")
```

## Minimal Code Example

```cpp
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

int main() {
    // Create 100x100 grayscale image
    cv::Mat img(100, 100, CV_8UC1, cv::Scalar(128));
    
    // Apply Gaussian blur
    cv::GaussianBlur(img, img, cv::Size(5, 5), 1.5);
    
    return 0;
}
```

## Building Your Own Application

### Method 1: Using Makefile

```bash
# Copy and edit Makefile.example
cp Makefile.example Makefile
# Edit SOURCES to include your .cpp files
make
```

### Method 2: Manual gcc

```bash
# Compile
riscv-none-elf-g++ -c myapp.cpp \
    --specs=nano.specs -march=rv64gc -mabi=lp64d \
    -O2 -fno-exceptions -fno-rtti \
    -I install_baremetal/include/opencv4

# Link
riscv-none-elf-g++ \
    start.o bare_metal_syscalls.o myapp.o \
    -o myapp.elf --specs=nano.specs -march=rv64gc -mabi=lp64d \
    -T riscv_baremetal.ld \
    -L install_baremetal/lib -lopencv_imgproc -lopencv_core \
    -Wl,--gc-sections -static
```

## Memory Budget Planning

| Component | Typical Size | Notes |
|-----------|-------------|-------|
| OpenCV code | 300-600 KB | With core+imgproc, -Os optimization |
| Newlib | 50-150 KB | Nano vs standard |
| Your code | Variable | Your application logic |
| Stack | 64 KB | Minimum, increase if deep recursion |
| Heap | 1-16 MB | Depends on image sizes |

**Total minimum**: ~1.5 MB RAM for basic usage

## Troubleshooting Quick Fixes

| Problem | Solution |
|---------|----------|
| "M_PI was not declared" | Already fixed! Toolchain defines `-D_GNU_SOURCE` |
| "undefined reference to `_sbrk`" | Compile and link `bare_metal_syscalls.c` |
| "undefined reference to pthread" | Add `-DOPENCV_DISABLE_THREAD_SUPPORT=ON` |
| Stack overflow | Increase `STACK_SIZE` in linker script |
| Heap exhausted | Increase `HEAP_SIZE` in linker script |
| No printf output | Implement UART in `_write()` function |
| Build takes forever | Reduce parallel jobs: `make -j1` |

## Testing Without Hardware

Use QEMU to test your binary:

```bash
# Install QEMU
sudo apt-get install qemu-system-riscv64

# Run your application
qemu-system-riscv64 -machine virt -nographic \
    -bios none -kernel opencv_example.elf

# With GDB debugging
qemu-system-riscv64 -machine virt -nographic \
    -bios none -kernel opencv_example.elf -s -S

# In another terminal
riscv-none-elf-gdb opencv_example.elf
(gdb) target remote :1234
(gdb) break main
(gdb) continue
```

## Performance Tips

1. **Enable optimizations**: Use `-O2` or `-O3` (not `-Os` for speed)
2. **Use vector extensions**: Enable RVV if available
3. **Align data**: Ensure Mat data is aligned for DMA/cache
4. **Profile**: Use cycle counters to find bottlenecks
   ```cpp
   unsigned long long start, end;
   __asm__ volatile ("rdcycle %0" : "=r" (start));
   // Your code here
   __asm__ volatile ("rdcycle %0" : "=r" (end));
   printf("Cycles: %llu\n", end - start);
   ```

## Size Optimization Tips

1. **Use newlib-nano**: Saves ~100KB
2. **Compile with -Os**: Optimize for size
3. **Enable LTO**: Add `-flto` to compiler and linker flags
4. **Strip symbols**: Use `--strip-all` in linker
5. **Reduce modules**: Only build core+imgproc
6. **Remove unused code**: Linker `--gc-sections` flag

## Next Steps

- Read `BAREMETAL_README.md` for detailed documentation
- Examine `example_app.cpp` for API usage examples
- Customize `bare_metal_syscalls.c` for your hardware
- Adjust `riscv_baremetal.ld` memory layout
- Profile and optimize your application

## Support

- OpenCV docs: https://docs.opencv.org/
- RISC-V toolchain: https://github.com/riscv-collab/riscv-gnu-toolchain
- Newlib docs: https://sourceware.org/newlib/

## License

Same as OpenCV (Apache 2.0)

