/**
 * Minimal syscall stubs for RISC-V bare-metal with newlib/newlib-nano
 * 
 * These stubs provide the minimum required syscalls for newlib to function.
 * Implement these according to your hardware (UART, memory layout, etc.)
 */

#include <sys/stat.h>
#include <sys/times.h>
#include <sys/time.h>
#include <errno.h>
#undef errno
extern int errno;

/* Forward declarations */
void _exit(int status);
int _close(int file);
int _fstat(int file, struct stat *st);
int _isatty(int file);
int _lseek(int file, int ptr, int dir);
int _read(int file, char *ptr, int len);
void *_sbrk(int incr);
int _write(int file, char *ptr, int len);
int _kill(int pid, int sig);
int _getpid(void);
int _gettimeofday(struct timeval *tv, void *tz);

/* Environment variables (not used in bare metal) */
char *__env[1] = { 0 };
char **environ = __env;

/* Heap management - adjust these based on your linker script */
extern char _end;              /* Defined by linker script */
extern char _heap_end;         /* Defined by linker script */
static char *heap_ptr = NULL;

/**
 * _sbrk - Increase program data space
 * Newlib uses this for malloc/free
 */
void *_sbrk(int incr)
{
    char *prev_heap_ptr;
    
    if (heap_ptr == NULL) {
        heap_ptr = &_end;
    }
    
    prev_heap_ptr = heap_ptr;
    
    /* Check if we have enough heap space */
    if (heap_ptr + incr > &_heap_end) {
        /* Out of memory */
        errno = ENOMEM;
        return (void *) -1;
    }
    
    heap_ptr += incr;
    return (void *) prev_heap_ptr;
}

/**
 * _write - Write to a file/device
 * File descriptor 1 (stdout) and 2 (stderr) should write to UART/console
 */
int _write(int file, char *ptr, int len)
{
    int i;
    
    /* Implement UART output here for stdout/stderr */
    /* Example pseudo-code:
    if (file == 1 || file == 2) {  // stdout or stderr
        for (i = 0; i < len; i++) {
            uart_putc(ptr[i]);  // Your UART function
        }
        return len;
    }
    */
    
    /* For now, just return success without output */
    errno = EBADF;
    return -1;
}

/**
 * _read - Read from a file/device
 * File descriptor 0 (stdin) should read from UART/console
 */
int _read(int file, char *ptr, int len)
{
    /* Implement UART input here for stdin */
    /* Example pseudo-code:
    if (file == 0) {  // stdin
        *ptr = uart_getc();  // Your UART function
        return 1;
    }
    */
    
    errno = EBADF;
    return -1;
}

/**
 * _close - Close a file descriptor
 */
int _close(int file)
{
    return -1;
}

/**
 * _lseek - Set position in a file
 */
int _lseek(int file, int ptr, int dir)
{
    return 0;
}

/**
 * _fstat - Status of an open file
 */
int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;  /* Character device */
    return 0;
}

/**
 * _isatty - Query whether output stream is a terminal
 */
int _isatty(int file)
{
    return 1;  /* Assume all file descriptors are TTY for bare metal */
}

/**
 * _exit - Exit program (bare metal has nowhere to exit to)
 */
void _exit(int status)
{
    /* Hang in infinite loop or trigger system reset */
    while (1) {
        __asm__ volatile ("wfi");  /* Wait for interrupt */
    }
}

/**
 * _kill - Send a signal (not supported in bare metal)
 */
int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}

/**
 * _getpid - Get process ID (not meaningful in bare metal)
 */
int _getpid(void)
{
    return 1;
}

/**
 * _gettimeofday - Get current time
 * Implement using your hardware timer
 */
int _gettimeofday(struct timeval *tv, void *tz)
{
    if (tv) {
        /* TODO: Read from hardware timer/RTC */
        /* Example: Get cycle count and convert to time */
        unsigned long long cycles;
        __asm__ volatile ("rdcycle %0" : "=r" (cycles));
        
        /* Assume 1 GHz clock for simplicity - adjust for your hardware */
        tv->tv_sec = cycles / 1000000000ULL;
        tv->tv_usec = (cycles % 1000000000ULL) / 1000ULL;
    }
    return 0;
}

/* Additional stubs that might be needed */

int _open(const char *name, int flags, int mode)
{
    errno = ENOSYS;
    return -1;
}

int _link(const char *old, const char *new)
{
    errno = ENOSYS;
    return -1;
}

int _unlink(const char *name)
{
    errno = ENOSYS;
    return -1;
}

int _stat(const char *file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _fork(void)
{
    errno = ENOSYS;
    return -1;
}

int _execve(const char *name, char *const argv[], char *const env[])
{
    errno = ENOSYS;
    return -1;
}

int _wait(int *status)
{
    errno = ENOSYS;
    return -1;
}

