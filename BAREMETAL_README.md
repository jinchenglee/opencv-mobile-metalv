# OpenCV on RISC-V Bare Metal with Newlib

This guide explains how to build and use OpenCV on RISC-V bare-metal systems using newlib or newlib-nano.

## Prerequisites

### 1. RISC-V Bare-Metal Toolchain

You need a RISC-V bare-metal toolchain (riscv-none-elf or riscv32-unknown-elf).

**Installation:**

```bash
# Ubuntu/Debian
sudo apt-get install gcc-riscv-none-elf

# Or build from source
git clone https://github.com/riscv-collab/riscv-gnu-toolchain
cd riscv-gnu-toolchain
./configure --prefix=/opt/riscv --with-arch=rv64gc --with-abi=lp64d
make -j$(nproc)
export PATH=/opt/riscv/bin:$PATH
```

### 2. CMake

```bash
sudo apt-get install cmake
```

## Quick Start

### 1. Build OpenCV

```bash
./build_baremetal.sh
```

This will:
- Configure OpenCV with bare-metal settings
- Build only `core` and `imgproc` modules
- Install to `install_baremetal/` directory

### 2. Customize the Build

Edit `toolchains/riscv64-baremetal-newlib.toolchain.cmake` to adjust:
- Architecture: Change `RISCV_ARCH` (rv64gc, rv64gcv, etc.)
- ABI: Change `RISCV_ABI` (lp64, lp64d, lp64f)
- Newlib variant: Set `USE_NEWLIB_NANO` to ON/OFF

### 3. Hardware-Specific Configuration

**Important files to customize:**

1. **riscv_baremetal.ld** - Memory layout
   - Set RAM origin and length for your board
   - Adjust heap and stack sizes

2. **bare_metal_syscalls.c** - System calls
   - Implement UART I/O for `_write()` and `_read()`
   - Implement timer for `_gettimeofday()`

## Using OpenCV in Your Application

### Example Application

Create `my_opencv_app.c`:

```c
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>

using namespace cv;

// Example: Simple image processing
void process_image() {
    // Create a 640x480 grayscale image
    Mat img(480, 640, CV_8UC1);
    
    // Fill with gradient
    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            img.at<uint8_t>(y, x) = (x + y) / 2;
        }
    }
    
    // Apply Gaussian blur
    Mat blurred;
    GaussianBlur(img, blurred, Size(5, 5), 1.5);
    
    printf("Image processed: %dx%d\n", blurred.cols, blurred.rows);
}

int main() {
    printf("OpenCV Bare Metal Test\n");
    
    try {
        process_image();
        printf("Success!\n");
    } catch (const cv::Exception& e) {
        printf("Error: %s\n", e.what());
        return 1;
    }
    
    return 0;
}
```

### Build Your Application

Create `build_app.sh`:

```bash
#!/bin/bash

OPENCV_DIR="install_baremetal"
TOOLCHAIN="riscv-none-elf"

# Compile syscalls
${TOOLCHAIN}-gcc -c bare_metal_syscalls.c \
    --specs=nano.specs \
    -march=rv64gc -mabi=lp64d \
    -O2 -ffunction-sections -fdata-sections

# Compile startup
${TOOLCHAIN}-gcc -c start.S \
    -march=rv64gc -mabi=lp64d

# Compile application
${TOOLCHAIN}-g++ -c my_opencv_app.c \
    --specs=nano.specs \
    -march=rv64gc -mabi=lp64d \
    -I${OPENCV_DIR}/include/opencv4 \
    -O2 -ffunction-sections -fdata-sections \
    -fno-exceptions -fno-rtti

# Link everything
${TOOLCHAIN}-g++ \
    start.o \
    bare_metal_syscalls.o \
    my_opencv_app.o \
    -o my_opencv_app.elf \
    --specs=nano.specs \
    -march=rv64gc -mabi=lp64d \
    -T riscv_baremetal.ld \
    -L${OPENCV_DIR}/lib \
    -lopencv_imgproc -lopencv_core \
    -Wl,--gc-sections \
    -static

# Generate binary
${TOOLCHAIN}-objcopy -O binary my_opencv_app.elf my_opencv_app.bin

# Show size
${TOOLCHAIN}-size my_opencv_app.elf

echo "Built: my_opencv_app.elf and my_opencv_app.bin"
```

## Memory Requirements

### Minimum Requirements

- **Code size**: ~500KB (core + imgproc, optimized with -Os)
- **Heap**: 1-16MB (depends on image sizes processed)
- **Stack**: 64KB minimum (adjust based on recursion depth)

### Optimization Tips

1. **Use newlib-nano** for smaller binary (~150KB savings)
2. **Enable link-time optimization**: Add `-flto` to CFLAGS/LDFLAGS
3. **Optimize for size**: Use `-Os` instead of `-O2`
4. **Strip symbols**: Use `--strip-all` in linker flags
5. **Reduce heap**: Limit maximum image sizes in your application

Example optimized flags:

```cmake
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -flto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -flto")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto -Wl,--strip-all")
```

## Hardware Integration

### UART Output

Implement in `bare_metal_syscalls.c`:

```c
// Example for a memory-mapped UART at address 0x10000000
#define UART_BASE 0x10000000
#define UART_TX   (*((volatile uint8_t*)(UART_BASE + 0)))
#define UART_SR   (*((volatile uint8_t*)(UART_BASE + 4)))

static inline void uart_putc(char c) {
    while (!(UART_SR & 0x01));  // Wait for TX ready
    UART_TX = c;
}

int _write(int file, char *ptr, int len) {
    if (file == 1 || file == 2) {  // stdout or stderr
        for (int i = 0; i < len; i++) {
            uart_putc(ptr[i]);
        }
        return len;
    }
    errno = EBADF;
    return -1;
}
```

### Timer Implementation

```c
// Example using RISC-V mcycle counter
int _gettimeofday(struct timeval *tv, void *tz) {
    if (tv) {
        unsigned long long cycles;
        __asm__ volatile ("rdcycle %0" : "=r" (cycles));
        
        // Convert to time (adjust CLOCK_HZ for your system)
        #define CLOCK_HZ 1000000000ULL  // 1 GHz
        tv->tv_sec = cycles / CLOCK_HZ;
        tv->tv_usec = ((cycles % CLOCK_HZ) * 1000000ULL) / CLOCK_HZ;
    }
    return 0;
}
```

## Troubleshooting

### Build Errors

1. **"M_PI was not declared in this scope"**
   - **Solution**: Already fixed in the toolchain! The toolchain file defines `-D_GNU_SOURCE` and `-D_USE_MATH_DEFINES`
   - If still occurs, ensure you're using the latest toolchain file
   - Alternative: Include `math_constants.h` before OpenCV headers
   - This happens because newlib on bare-metal doesn't define POSIX math constants by default

2. **"undefined reference to `__dso_handle`"**
   - Add `-fno-use-cxa-atexit` to C++ flags

3. **"undefined reference to pthread functions"**
   - Ensure `-DOPENCV_DISABLE_THREAD_SUPPORT=ON`

4. **"undefined reference to `_sbrk`"**
   - Make sure `bare_metal_syscalls.c` is compiled and linked

### Runtime Errors

1. **Heap exhausted**
   - Increase `HEAP_SIZE` in linker script
   - Reduce image sizes in your application

2. **Stack overflow**
   - Increase `STACK_SIZE` in linker script
   - Reduce recursion depth

3. **Math function errors**
   - Link with libm: `-lm` (included in newlib)

## Advanced Topics

### Using RISC-V Vector Extensions (RVV)

If your hardware supports RVV:

```bash
# Build OpenCV with RVV support
cmake ... \
    -DWITH_HAL_RVV=ON \
    -DCPU_BASELINE=RVV \
    ...
```

Update toolchain file:
```cmake
set(RISCV_ARCH "rv64gcv")  # Add 'v' for vector extension
```

### Custom Memory Allocator

For deterministic memory allocation, implement a custom allocator:

```cpp
// In your application
void* operator new(size_t size) {
    return my_custom_malloc(size);
}

void operator delete(void* ptr) noexcept {
    my_custom_free(ptr);
}
```

### Debugging

Use GDB with QEMU for testing:

```bash
# Run in QEMU
qemu-system-riscv64 -machine virt -nographic \
    -bios none -kernel my_opencv_app.elf -s -S

# In another terminal
riscv-none-elf-gdb my_opencv_app.elf
(gdb) target remote :1234
(gdb) break main
(gdb) continue
```

## Module Support

### Supported Modules

- ✅ **core**: Basic data structures (Mat, algorithms)
- ✅ **imgproc**: Image processing (filters, transforms)

### Unsupported Modules (Bare Metal)

- ❌ **imgcodecs**: Requires file I/O
- ❌ **videoio**: Requires file/device I/O
- ❌ **highgui**: Requires windowing system
- ❌ **dnn**: Too large for most bare-metal systems
- ❌ **ml**: Requires file I/O for model loading

## References

- [Newlib Documentation](https://sourceware.org/newlib/)
- [RISC-V Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)
- [OpenCV Documentation](https://docs.opencv.org/)
- [ARM Semihosting](https://developer.arm.com/documentation/100863/)

## License

Same as OpenCV (Apache 2.0 License)

