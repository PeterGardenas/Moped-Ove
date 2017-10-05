/**** Created by Squawk builder from "vmcore/src/vm/squawk.c.spp.preprocessed" ****/ /*
 * Copyright 2004-2010 Sun Microsystems, Inc. All Rights Reserved.
 * Copyright 2011 Oracle Corporation. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * only, as published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Oracle Corporation, 500 Oracle Parkway, Redwood
 * Shores, CA 94065 or visit www.oracle.com if you need additional
 * information or have any questions.
 */



#define CLASS_CACHE_SIZE 6


#define MONITOR_CACHE_SIZE 6

#include "platform.h"
#include "buildflags.h"


/**
 * Forward definition of printf/fprintf helper function.
 */
const char *format(const char* fmt);

/* used by io.c...*/
#include "io_native.h"

#include "os.c"

#ifndef DIAGNOSTICS_ENABLED
/* for ARM, defined in arm-library, otherwise do it here. */

#if defined(ASSUME) && ASSUME != 0
#define DIAG_INTRO	"[DIAG] "
void diagnosticPrimWithValue(char* msg, int len, int val) {
    printf("%s %.*s %#x\r\n", DIAG_INTRO, len, msg, val);
    fflush(stdout);
}
void diagnostic(char * msg) {
	diagnosticPrimWithValue(msg, strlen(msg), 0);
}
void diagnosticWithValue(char * msg, int val) {
	diagnosticPrimWithValue(msg, strlen(msg), val);
}
#else  /* no assume */
#define diagnostic(a) /**/
#define diagnosticWithValue(a,b) /**/
#endif /* no assume */
#endif /* DIAGNOSTICS_ENABLED */

#ifndef DEBUG_STARTUP
#define DEBUG_STARTUP 0
#endif


#ifndef KERNEL_SQUAWK
#define KERNEL_SQUAWK false
#endif

#ifndef TRACE
#define TRACE false
#endif

#ifdef PROFILING
#undef TRACE
#define TRACE true
#endif

#if PLATFORM_TYPE_DELEGATING
#define IODOTC "io_delegating.c"
#elif PLATFORM_TYPE_NATIVE
#define IODOTC "io_native.c"
#elif PLATFORM_TYPE_SOCKET
#define IODOTC "io_socket.c"
#elif PLATFORM_TYPE_BARE_METAL
/* Different bare metal platforms handle this differently - eg set IODOTC to a specific file.
 * Also include io_metal.c*/
#else
#error "unspecified platform type"
#endif

#if defined(TYPEMAP) && TYPEMAP != 0
#undef TYPEMAP
#define TYPEMAP true
#else
#define TYPEMAP false
#endif

/*
 * Include the type definitions and operations on machine word sized quantities
 */
#include "address.c"

/*
 * Forward refs.
 */
#ifndef FLASH_MEMORY
static void printCacheStats();
#endif

#if TRACE
boolean openTraceFile();
void printStackTrace(const char* msg);
void printStackTraceOpcode(int code);
void printProfileStackTrace(ByteAddress traceIP, UWordAddress traceFP, int lastOpcode);
void printStackTracePrim(int opcode, ByteAddress traceIP, UWordAddress traceFP, const char* msg, const char* mnemonic);
#else
#define openTraceFile() true
#define printStackTrace(msg)
#define printStackTraceOpcode(code)
#define printProfileStackTrace( traceIP, traceFP, lastOpcode)
#define printStackTracePrim(opcode, traceIP, traceFP, msg, mnemonic)
#endif /* TRACE */

static Address lookupKlass(int cid);
static Address lookupStaticMethod(int cid, int methodOffset);

/*
    This is the 'slow' Squawk VM that is implemented in C. The techniques used in
    this VM are such that it should be expected that it will be several times slower
    than other VMs implemented in C. Its purpose is only to have an initial Squawk
    implementation running quickly, and to provide a reference implementation
    to test the final system against.

    VM has five virtual machine registers.

      ip - the instruction pointer.
      fp - the frame pointer
      sp - the stack pointer
      sl - the stack limit
      bc - the branch counter

    The stack goes downwards, and activation records have the following format:

    High:
             -------------------------------
            |              P3               |
             -------------------------------
            |              P2               |
             -------------------------------
            |              P1               |
             -------------------------------
            |              P0               |
             -------------------------------
            |           Return IP           |
             -------------------------------
            |           Return FP           |
             -------------------------------
    fp ->   |         Current method        |
             -------------------------------
            |              L0               |
             -------------------------------
            |              L1               |
             -------------------------------
            |              ...              |
             -------------------------------
    sp ->   |              LN               |
             -------------------------------
            |                               |
            :        Evaluation stack       :
            :                               :
            |                               |
             -------------------------------
    Low:

*/

/*
 * Include the romized image.
 */
#include "rom.h"

/*
 * Add the monitor cache size to the global oop count
 */
#define GLOBAL_OOP_COUNT (ROM_GLOBAL_OOP_COUNT + MONITOR_CACHE_SIZE)
#define GLOBAL_INT_COUNT  ROM_GLOBAL_INT_COUNT
#define GLOBAL_ADDR_COUNT ROM_GLOBAL_ADDR_COUNT

/**
 * The default GC chunk, NVM and RAM sizes.
 */
#ifndef SERVICE_CHUNK_SIZE
#define SERVICE_CHUNK_SIZE (36*1024)
#endif

#define TWOWORDS (HDR_BYTES_PER_WORD * 2)
#define SERVICE_CHUNK_SIZE_MINUS2WORDS (SERVICE_CHUNK_SIZE - TWOWORDS)

#ifndef DEFAULT_RAM_SIZE
#define DEFAULT_RAM_SIZE   (256*1024) //73400320
#endif

/*
 * Note that DEFAULT_NVM_SIZE = 0 means don't support nvm at all
 */
#ifndef DEFAULT_NVM_SIZE
#define DEFAULT_NVM_SIZE   (100*1024)
#endif
#define TIMEQUANTA 1000

#ifndef MAX_BUFFERS
#define MAX_BUFFERS 1000
#endif

#define MAX_JVM_ARGS 20

/**
 * The tracing limits.
 */
const jlong MAX_UJLONG = (jlong)0x7fffffff << 32 | 0xffffffff;
#ifndef TRACESTART
#define TRACESTART MAX_UJLONG
#endif

/*
 * Forward declaration of VM exit routine.
 */
NORETURN void stopVM0(int, boolean);
NORETURN void stopVM(int);

/*
 * Include all the globals.
 */
#include "globals.h"

/*
 * Setup ss and sl.
 */
#define setStack(newss) {                                \
    ss = (UWordAddress)newss;                            \
    sl = ss + SC_limit;                                  \
}                                                        \

/*
 * Test to see if the interpreter is running in kernel mode.
 */
#if KERNEL_SQUAWK
#define inKernelMode() (gp == &kernelGlobals)
#else
#define inKernelMode() false
#endif

/*
 * Test to see if the interpreter is running on the GC stack
 */
#define usingServiceStack() (ss == (UWordAddress)com_sun_squawk_VMThread_serviceStack)

/*
 * Include utility functions.
 */
#include "util.h"

/*
 * Size of buffer used for pre-formatting printf/fprintf format specifications.
 */
#define FORMAT_BUF_LEN 1000

#ifdef _MSC_VER
#    define FORMAT_64  buf[bufPos++] = 'I'; buf[bufPos++] = '6'; buf[bufPos++] = '4';
#else
#    define FORMAT_64  buf[bufPos++] = 'l'; buf[bufPos++] = 'l';
#endif /* _MSC_VER */

/**
 * Preformats a format specification string given to 'printf' or 'fprintf'
 * so that the platform dependent part of printing of Offset/Word/Address/long
 * values is encapsulated here.
 *
 * This preformatter transforms the following format specifications:
 *
 *  '%A'  - formats the corresponding argument as an unsigned 32 or 64 bit value
 *  '%O'  - formats the corresponding argument as a signed 32 or 64 bit value
 *  '%L'  - formats the corresponding argument as a signed 64 bit value
 *  '%U'  - formats the corresponding argument as an unsigned 64 bit value
 *  '%D'  - formats the corresponding argument as a 64 bit floating point value
 *
 * @param  fmt  the format specification to be pre-formatted
 * @return the transformed version of 'fmt'
 */
const char *format(const char* fmt) {
    static char buf[FORMAT_BUF_LEN];
    int fmtPos = 0;
    int bufPos = 0;
    int fmtLen = strlen(fmt);

    while (fmtPos < fmtLen) {
        assume(bufPos < 1000);

        if (fmt[fmtPos] != '%') {
            buf[bufPos++] = fmt[fmtPos++];
        } else {
            fmtPos++;
            buf[bufPos++] = '%';
            switch (fmt[fmtPos++]) {
                case 'A': {
#if SQUAWK_64
                    FORMAT_64
#endif /* SQUAWK_64 */
                    buf[bufPos++] = 'u';
                    break;
                }
                case 'O': {
#if SQUAWK_64
                    FORMAT_64
#endif /* SQUAWK_64 */
                    buf[bufPos++] = 'd';
                    break;
                }
                case 'L': {
                    FORMAT_64
                    buf[bufPos++] = 'd';
                    break;
                }
                case 'U': {
                    FORMAT_64
                    buf[bufPos++] = 'u';
                    break;
                }
                case 'D': {
                    FORMAT_64




                    buf[bufPos++] = 'f';
                    break;
                }
                default: {
                    buf[bufPos++] = fmt[fmtPos - 1];
                }
            }
        }

    }
    buf[bufPos++] = 0;
    return buf;
}

/*
 * Define a few extra things.
 */
enum {EQ, NE, LT, GT, LE, GE};
#define USHORT CHAR
#define OOP com_sun_squawk_Object
typedef int Type;


/*
 * Define java-style FP conversions:
 *
 *** On Intel (and PPC it seems), the hardware does not produce the correct results
 *** for f.p. to int conversions when the value is NaN or out of
 *** range, so we need to explicitly handle the special cases.
 *** Don't know about ARM. Sparc only needs to check for NANs.
 *** Could make CPU-specific versions, but or now do most general case:
 ***/

NOINLINE jlong float2ll_slow(float f) {
    static const unsigned long LongMin_f = 0xdf000000L;
    static const unsigned long LongMax_f = 0x5f000000L;
    jlong r;

    if (f != f)
        r = 0;
    else if ( f < *((float *) &LongMin_f))
        r = 0x8000000000000000LL;
    else if ( f >= *((float *) &LongMax_f))
        r = 0x7fffffffffffffffLL;
    else
        r = (jlong) f;
    return r;
}

NOINLINE jlong double2ll_slow(double d) {
#if ARM_FPA
    static const ujlong LongMin_d = 0x00000000c3e00000LL;
    static const ujlong LongMax_d = 0x0000000043e00000LL;
#else /* ARM_FPA */
    static const ujlong LongMin_d = 0xc3e0000000000000LL;
    static const ujlong LongMax_d = 0x43e0000000000000LL;
#endif /* ARM_FPA */

    jlong r;

    if (d != d)
        r = 0;
    else if ( d < *((double *) &LongMin_d))
        r = 0x8000000000000000LL;
    else if ( d >= *((double *) &LongMax_d))
        r = 0x7fffffffffffffffLL;
    else
        r = (jlong) d;
    return r;
}

NOINLINE int double2l_slow(double d) {
#if ARM_FPA
    static const ujlong IntegerMin_d = 0x00000000c1e00000LL;
    static const ujlong IntegerMax_d = 0x0000000041e00000LL;
#else /* ARM_FPA */
    static const ujlong IntegerMin_d = 0xc1e0000000000000LL;
    static const ujlong IntegerMax_d = 0x41e0000000000000LL;
#endif /* ARM_FPA */

    int r;

    if (d != d)
	r = 0;
    else if ( d < *((double *) &IntegerMin_d))
	r = 0x80000000U;
    else if ( d >= *((double *) &IntegerMax_d))
	r = 0x7fffffff;
    else
	r = (int) d;
    return r;

}

NOINLINE int float2l_slow(float f) {
    static const unsigned int IntegerMin_f = 0xcf000000L;
    static const unsigned long IntegerMax_f = 0x4f000000L;

    int r;

    if (f != f)
	r = 0;
    else if ( f < *((float *) &IntegerMin_f))
	r = 0x80000000U;
    else if ( f >= *((float *) &IntegerMax_f))
	r = 0x7fffffff;
    else
	r = (int) f;
    return r;
}


/**
 * Allocate a word aligned byte buffer from the C memory and zero its contents.
 * The call will exit the system if it could not allocate the buffer.
 *
 * NOTE: The current implementation actually allocates on a page boundary and buffer
 *       the size of the buffer allocated in a multiple of the system page size. This
 *       enables these buffers to be set as read-only using the system specific
 *       memory protection mechanism.
 *
 * @param size        the size (in bytes) to allocate.
 * @param desc        a description of what the buffer is for
 * @param fatalIfFail if true and the buffer cannot be allocated then cause a VM error
 * @return a word aligned buffer of the given size
 */
void *newBuffer(UWord size, const char *desc, boolean fatalIfFail) {
    void *buffer;

    /*
     * Adjust size so that it is a multiple of the page size of the machine
     */
    int pageSize = sysGetPageSize();
    int actualSize = (size + (pageSize - 1)) & ~(pageSize - 1);
    buffer = sysValloc(actualSize);

    /*
     * Ensure that the resulting buffer is word aligned (which is surely guaranteed
     * if it is page aligned!)
     */
    assume(isWordAligned((UWord)buffer));

    if (buffer == null) {
        if (fatalIfFail) {
            fprintf(stderr, format("Failed to allocate buffer of %A bytes for %s\n"), size, desc);
            stopVM(1);
        } else {
            return null;
        }
    }

    /*
     * Zero the bytes in the buffer.
     */
    #ifndef AUTOSAR
    	memset(buffer, 0, (int)size);
    #endif

    /*
     * Register the buffer.
     */
    if (BufferCount >= MAX_BUFFERS) {
        fatalVMError("exceeded MAX_BUFFERS allocations");
    }
    Buffers[BufferCount++] = buffer;

    return buffer;
}

/**
 * Free a given buffer that was allocated by 'newBuffer' and remove
 * it from the table of allocated buffers.
 */
void freeBuffer(Address buffer) {
    int i = 0;
    int insert = 0;

    while (i != BufferCount) {
        if (Buffers[i] == buffer) {

            sysVallocFree(buffer);
        } else {
            Buffers[insert] = Buffers[i];
            insert++;
        }
        i++;
    }

    if (BufferCount != insert + 1) {
        fatalVMError("buffer not in Buffers exactly once");
    }
    BufferCount = insert;
}

/**
 * Free all the buffers that were allocated by 'newBuffer'.
 */
void freeBuffers() {
    while (BufferCount != 0) {
        freeBuffer(Buffers[0]);
    }
}

/*
 * Include the memory interface.
 */
#include "memory.c"

/*
 * Include the bitmap used by the Lisp2 collectors.
 */
#ifdef LISP2_BITMAP
#include "lisp2.c"
#endif /* LISP2_BITMAP */

/*
 * Include the switch and bytecode routines.
 */
#include "bytecodes.c"

/*
 * Include java code translated into C code.
 */
#include "vm2c.c"


#if KERNEL_SQUAWK
/*
 * Include the device-related definitions
 */
#include "os_devices.h"
#else
#define deferInterruptsAndDo(action)        do { action; } while (0)
#endif

/*
 * Include the I/O system
 */
#include "cio.c"

#if KERNEL_SQUAWK
/*
 * Include the device-related routines
 */
#include "os_devices.c"
#include "devices.c"
#endif

/*
 * Include the instruction trace routine.
 */
#include "trace.c"

#ifdef DB_DEBUG
/*
 * Include support for low-level interactive debug
 */
#include "debug.c"
#endif

#if SDA_DEBUGGER
#include "debugger.c"
#endif


/**
 * Parse a string that specifies a quantity.
 *
 * @param p  the string to parse
 */
jlong parseQuantityLong(char *p, const char* arg) {
    jlong val = 0;
    for (;;) {
        int ch = *p++;
        if (ch == 0) {
            break;
        } else if (ch >= '0' && ch <= '9') {
            val = (val * 10) + (ch - '0');
        } else if (ch == 'K' || ch == 'k') {
            val *= 1024;
            break;
        } else if (ch == 'M' || ch == 'm') {
            val *= (1024*1024);
            break;
        } else {
            fprintf(stderr, "Badly formatted quantity for '%s' option\n", arg);
            stopVM(-1);
        }
    }
    return val;
}


/**
 * Parse a string that specifies a quantity.
 *
 * @param p  the string to parse
 */
int parseQuantity(char *p, const char* arg) {
    jlong res = parseQuantityLong(p, arg);
    if (res != (int)res) {
        fprintf(stderr, "parseQuantity overflow for '%s' option\n", arg);
        stopVM(-1);
    }
    return (int)res;
}

/**
 * Gets the size of a file.
 *
 * @param file     the file to inspect
 * @return the size of the file or -1 if it doesn't exist
 */
Offset getFileSize(const char *file) {
    struct stat buf;
    if (stat(file, &buf) == 0) {
        return buf.st_size;
    } else {
        if (errno != ENOENT) {
             fprintf(stderr, "Call to stat(%s) failed: %s\n", file, strerror(errno));
             stopVM(-1);
        }
        return -1;
    }
}

/**
 * Loads the contents of a file into a buffer.
 *
 * @param file     the file from which to load
 * @param buffer   the buffer into which the file should be loaded
 * @param size     the size of the buffer
 * @return the size of the file or -1 if 'file' does not exist. Any other errors cause
 *                 the system to exit.
 */
int readFile(const char *file, Address buffer, UWord size) {
    struct stat buf;
    int result = -1;
    if (stat(file, &buf) == 0) {
        int fd = open(file, O_RDONLY|O_BINARY);
        result = buf.st_size;
        if (fd != -1) {
            int count   = 0;
            int toRead  = buf.st_size;
            char *readPos = (char *)buffer;
            while (toRead > 0) {
                count = read(fd, readPos, toRead);
                if (count == -1) {
                    fprintf(stderr,format("Call to read() failed: %s (file size=%A, read=%d)\n"), strerror(errno), buf.st_size, buf.st_size-toRead);
                    stopVM(-1);
                }
                toRead -= count;
                readPos += count;
            }
            close(fd);
        } else {
             fprintf(stderr,"Call to open(%s) failed: %s\n", file, strerror(errno));
             stopVM(-1);
        }
    } else {
        if (errno != ENOENT) {
             fprintf(stderr,"Call to stat(%s) failed: %s\n", file, strerror(errno));
             stopVM(-1);
        }
    }
    return result;
}

/*
 * Include the functions for laoding and saving the bootstrap suite
 */
#include "suite.c"

/**
 * Gets the 2 integers representing a percent value.
 *
 * @param part   a value
 * @param whole  a value
 * @return  the non-fractional part of the value expressing 'part' as a percentage of 'whole' followed by the fractional part to 2 decimal places
 */
#define AS_PERCENT(part, whole)    (int)((((jlong)part) * 100) / (whole)), (int)(((((jlong)part) * 10000) / (whole)) % 100)

#ifndef FLASH_MEMORY

static void printCacheStat(const char *label, int part, jlong whole) {
    if (whole == 0) {
        fprintf(stderr, " %s:0%%", label);
    } else {
        fprintf(stderr, format(" %s:%u.%02u%%"), label, AS_PERCENT(part, whole));
    }
}

/**
 * Print cache stats.
 */
static void printCacheStats() {
    jlong count = 0;
    fprintf(stderr, "----------------------------------");
#ifdef PROFILING
    count = instructionCount-lastStatCount;
    fprintf(stderr, " %lld.%02lld M Instructions ", count/1000000, (count % 1000000) / 10000);
    lastStatCount = instructionCount;
#else
#if TRACE
    count = getBranchCount()-lastStatCount;
    fprintf(stderr, " %lld.02%lld M Branches ", count/1000000, (count % 1000000) / 10000);
    lastStatCount = getBranchCount();
#endif /* TRACE */
#endif /* PROFILING */
    fprintf(stderr, "----------------------------------");

#ifdef INTERPRETER_STATS
    if (count > 0) {
        fprintf(stderr, "\nTotals - ");
        fprintf(stderr, " Class: %d",   cachedClassAccesses);
        fprintf(stderr, " Monitor: %d", pendingMonitorAccesses);
        fprintf(stderr, " Exit: %d",    com_sun_squawk_GC_monitorExitCount);
        fprintf(stderr, " New: %d",     com_sun_squawk_GC_newCount);
    }

    fprintf(stderr, "\nHits   - ");
    printCacheStat(" Class",   cachedClassHits,                  cachedClassAccesses);
    printCacheStat(" Monitor", pendingMonitorHits,               pendingMonitorAccesses);
    printCacheStat(" Exit",    com_sun_squawk_GC_monitorReleaseCount, com_sun_squawk_GC_monitorExitCount);
    printCacheStat(" New",     com_sun_squawk_GC_newHits,        com_sun_squawk_GC_newCount);
    fprintf(stderr, "\n");
    cachedClassHits = cachedClassAccesses = 0;
    pendingMonitorHits = pendingMonitorAccesses = 0;
#else /* INTERPRETER_STATS */
    fprintf(stderr, "\n");
#endif /* INTERPRETER_STATS */

    com_sun_squawk_GC_monitorExitCount = com_sun_squawk_GC_monitorReleaseCount = 0;
    com_sun_squawk_GC_newCount = com_sun_squawk_GC_newHits = 0;

#if TRACE
    if (total_extends > 0) {
        fprintf(stderr, format("Extends: %d (slots as %% of extends = %d.%02d%%)\n"), total_extends, AS_PERCENT(total_slots, total_extends));
    }
#endif /* TRACE */
    fprintf(stderr, format("GCs: %d full, %d partial\n"), com_sun_squawk_GC_fullCollectionCount, com_sun_squawk_GC_partialCollectionCount);
}

#endif /* !FLASH_MEMORY */

/**
 * Stops the VM running.
 *
 * @param exitCode the exit code
 */
NORETURN void stopVM(int exitCode) {
    stopVM0(exitCode, true);
}

/**
 * Stops the VM running.
 *
 * @param exitCode the exit code
 */
NORETURN void stopVM0(int exitCode, boolean cleanup) {
    static boolean inStop = false;

/*
if ( com_sun_squawk_VM_isBlocked) {
fprintf(stderr, "VM is in a blocking system call!!! WILL CRASH????");
}
*/

    if (!inStop) {
        inStop = true;
        finalizeStreams();

	jnaSetLED(-1, 0x121218);

        if (cleanup) {
            freeBuffers();
        }

        fprintf(stderr, "\n\n");
#ifndef FLASH_MEMORY
        printCacheStats();
#endif /* !FLASH_MEMORY */
        fprintf(stderr, "** VM stopped");
#ifdef PROFILING
        fprintf(stderr, format(" after %L instructions"), instructionCount);
#endif /* PROFILING */
#if TRACE
        fprintf(stderr, format(" after %L branches"), getBranchCount());
#endif /* TRACE */
        fprintf(stderr, format(": exit code = %d ** "), exitCode);
#if PLATFORM_TYPE_SOCKET
        if (ioport != null) {
            jlong average = (io_ops_count == 0 ? 0 : io_ops_time / io_ops_count);
            fprintf(stderr, format(" (average time for %d I/O operation: %L ms)"), io_ops_count, average);
        }
#endif
        fprintf(stderr, "\n");
        fflush(stderr);



#ifdef DB_DEBUG
        db_vm_exiting();
#endif
    }
#ifdef _MSC_VER
    if (notrap) {
        _asm{ int 3 };
    }
#endif

#if PLATFORM_TYPE_NATIVE
    IO_shutdown();
#endif

    osfinish();
	inStop = false;
    exit(exitCode);
}

/**
 * Shows the usage message for passing flags to the embedded JVM.
 */
#if PLATFORM_TYPE_DELEGATING
void jvmUsage() {
    printf("    -J<flag>       pass <flag> to the embedded Java VM. Some common usages include:\n");
    printf("                       -J-Xdebug -J-Xrunjdwp:transport=dt_socket,server=y,suspend=y,address=9999\n");
    printf("                                [enables debugging of embedded JVM]\n");
    printf("                       -J-Dcio.tracing=true [enables tracing in embedded JVM]\n");
    printf("                       -J-Dcio.logging=true [enables logging in embedded JVM]\n");
}
#endif /* PLATFORM_TYPE_DELEGATING */

/**
 * Shows the usage message.
 */
void usage() {
    printf("Usage: \n");
    printf("    -Xmx:<size>    set RAM size (%dKb)\n", DEFAULT_RAM_SIZE/1024);
#if DEFAULT_NVM_SIZE
    printf("    -Xmxnvm:<size> set NVM size (%dKb)\n", DEFAULT_NVM_SIZE/1024);
#endif
    printf("    -Xboot:<file>  load bootstrap suite from file (squawk.suite)\n");
    printf("    -Xtgc:<n>      set GC trace flags:\n");
    printf("                     1: trace mem config and GC events\n");
#if com_sun_squawk_GC_GC_TRACING_SUPPORTED
    printf("                     2: trace allocations\n");
    printf("                     4: detailed trace of garbage collector\n");
    printf("                     8: detailed trace of object graph copying\n");
    printf("                    16: heap trace before each GC\n");
    printf("                    32: heap trace after each GC\n");
    printf("                    64: include heap contents in heap trace\n");
#endif
#if (com_sun_squawk_GC_GC_TRACING_SUPPORTED | com_sun_squawk_GarbageCollector_HEAP_TRACE)
    printf("    -Xtgca:<n>     start GC tracing at collection 'n' (0)\n");
#endif
#if TRACE
    printf("    -Xts:<n>       start tracing after 'n' backward branches\n");
    printf("    -Xte:<n>       stop tracing after 'n' backward branches\n");
    printf("    -Xtr:<n>       trace 1000 backward branches after 'n' backward branches\n");
    printf("    -Xtnoservice   do not trace when executing on the service thread\n");
    printf("    -Xterr         trace to standard error output stream\n");
    printf("    -Xstats:<n>    dump a cache stats every 'n' backward branches\n");
#endif /* TRACE */

#ifdef PROFILING
#ifdef OSPROF
    printf("    -Xprof:<n>     take profile sample every 'n' milliseconds\n");
#else
    printf("    -Xprof:<n>     take profile sample every 'n' instructions\n");
#endif /* OSPROF */
#endif /* PROFILING */

#if PLATFORM_TYPE_SOCKET
    printf("    -Xioport:[host:]port  connect to an I/O server via a socket\n");
#endif /* PLATFORM_TYPE_SOCKET */

#if KERNEL_SQUAWK
    printf("    -Xkernel[:<n>] support async handling of interrupts (with signal number <n>)\n");
    printf("    -K<flags>      arguments to pass to kernel VM\n");
#endif /* KERNEL_SQUAWK */

#ifndef FLASH_MEMORY
    printf("    -Xnotrap       don't trap VM crashes\n");
#endif /* FLASH_MEMORY */

#if PLATFORM_TYPE_DELEGATING
    jvmUsage();
#endif /* PLATFORM_TYPE_DELEGATING */
}

/**
 * Determines if the contents of a given string start with a given prefix.
 *
 * @param  line    the (null terminated) string to check
 * @param  prefix  the (null terminated) prefix to check against
 * @return true if line starts with prefix
 */
boolean startsWith(char *line, char *prefix) {
    int i;
    for (i = 0 ;; i++) {
        char ch = line[i];
        int prefixCh = prefix[i];
        if (prefixCh == 0) {
            return true;
        }
        if (ch == 0) {
            return false;
        }
        if (prefixCh != ch) {
            return false;
        }
    }
}

/**
 * Determines if the length and contents of two given strings are equals.
 *
 * @param  s1    the first (null terminated) string to check
 * @param  s2    the second (null terminated) string to check
 * @return true if s1 and s2 are the length and have matching contents
 */
boolean equals(char *s1, char *s2) {
    int i;
    for (i = 0 ;; i++) {
        int ch = s1[i];
        if (ch == 0) {
            return s2[i] == 0;
        }
        if (s2[i] != ch) {
            return false;
        }
    }
}

/**
 * Calculates the space required to make a deep copy of an array of C strings.
 *
 * @param  length   the number of elements in the array
 * @param  array    the array
 * @return the size (in bytes) required for a deep copy of the array
 */
int calculateSizeForCopyOfCStringArray(int length, char **array) {
    int total = length * sizeof(char *);
    while(length-- > 0) {
        total += strlen(array[length]) + 1;
    }
    return total;
}

/**
 * Writes a C string (i.e. a null-terminated char array) into the VM's memory buffer.
 *
 * @param  string  the C string to write
 * @param  offset  the offset in the VM's memory buffer at which to write
 * @return the offset one past the last byte written to the VM's memory buffer
 */
int writeCString(char *string, int offset) {
    while (*string != 0) {
        setByte(memory, offset++, *string++);
    }
    setByte(memory, offset++, 0); // write null-terminator
    return offset;
}

/**
 * Writes an array of C strings into a given buffer. The provided
 * buffer must be within the type checked memory buffer.
 *
 * @param  length   the number of elements in the array
 * @param  array    the array
 * @param  offset   the offset in the VM's memory buffer at which to write
 * @return the offset one past the last byte written to the VM's memory buffer
 */
int writeCStringArray(int length, char **array, int offset) {
    char **arrayCopy = (char **)Address_add(memory, offset);

    // Find the end of the array copy which is where the elements will be copied
    offset += length * sizeof(Address);
    while (length-- > 0) {
        setObject(arrayCopy++, 0, Address_add(memory, offset));
        offset = writeCString(*array++, offset);
    }
    return offset;
}

/** 
 * Return another path to find the bootstrap suite with the given name.
 * On some platforms the suite might be stored in an odd location
 * 
 * @param bootstrapSuiteName the name of the boostrap suite
 * @return full or partial path to alternate location, or null
 */
static char* getAlternateBootstrapSuiteLocation(char* bootstrapSuiteName);

/**
 * Sets up the memory buffer.
 * 
 * All "Start" values are rounded up to the system page size (note that page size on SPOT is 4 BYTES!)
 * 
 * Desktop machine memory layout:
 *
 * Memory_start
 *    ROM_start
 *         Bootstrap_suite object
 *    ROM_end
 *    NVM_start [8MB]  NVM Used for loading extra suites (such as translator)
 *    NVM_end
 *    RAM_start [8MB]
 *    RAM_end
 *    serviceStack [24KB]
 *    argv copy
 * Memory_end
 * [TYPEMAP]
 *
 * Sun SPOT memory layout:
 *
 * ROM_start
 *     Bootstrap_suite object
 * ROM_end
 * Memory_start
 *    NVM_start [128 B]  - NVM not used on SPOT
 *    NVM_end
 *    RAM_start [470000 B]
 *    RAM_end
 *    serviceStack [24KB]
 *    argv copy
 * Memory_end
 *
 * @param ramSize   either the size (in bytes) requested for RAM or for the SPOT the total memory available
 * @param nvmSize   the size (in bytes) requested for NVM
 * @param argv      the command line options after the -X and -J options have been stripped
 * @param argc      the number of components in argv
 */
 Address appSuite;
 
Address setupMemory(int ramSize, int nvmSize, int argc, char *argv[], char *bootstrapSuiteFile) {
    int pageSize = sysGetPageSize();

    int serviceChunkSize = SERVICE_CHUNK_SIZE;
    int realMemorySize;
    int bootstrapSize;
    int appSize;


    
    Address suite;
    int argvTotalSize = calculateSizeForCopyOfCStringArray(argc, argv);
    int offset;

    diagnosticWithValue("setupMemory: ram size", ramSize);
    diagnosticWithValue("setupMemory: nvmSize size", nvmSize);





#ifdef FLASH_MEMORY
    memorySize = roundUp(ramSize - (pageSize-1), pageSize);
    ramSize = memorySize - roundUp(serviceChunkSize, pageSize) - roundUp(argvTotalSize, pageSize) - roundUp(nvmSize, pageSize);
    diagnosticWithValue("ram size", ramSize);
	diagnosticWithValue("page size", pageSize);
	diagnosticWithValue("memory size", memorySize);
    assume(!TYPEMAP);
#else /* FLASH_MEMORY */	

	/* load squawk.suite array */
	#include "../rts/gcc-rpi/squawkSuiteArray.c"
	
	int sizeOfSquawkSuiteArray;
    memorySize = sizeOfSquawkSuiteArray = sizeof(squawkSuiteArray)/sizeof(squawkSuiteArray[0]);
    if (memorySize == -1) {
        char* altName = sysGetAlternateBootstrapSuiteLocation(bootstrapSuiteFile);
        if (altName) {
            printf("bootstrap suite not found, trying %s\n", altName);
            bootstrapSuiteFile = altName;
            memorySize = getFileSize(bootstrapSuiteFile);
        }
    }
    memorySize = roundUp(memorySize + ramSize, pageSize);
    memorySize = roundUp(memorySize + nvmSize, pageSize);



    memorySize = roundUp(memorySize + serviceChunkSize, pageSize);
    memorySize = roundUp(memorySize + argvTotalSize, pageSize);
#endif /* FLASH_MEMORY */

       printf("squawk.c memorySize %d\r\n", memorySize);

     // Double the memory buffer to allocate the type map if necessary
    realMemorySize = TYPEMAP ? memorySize * 2 : memorySize;
    
    // Allocate the memory buffer
    memory = newBuffer(realMemorySize, "memory", true);
    
    memoryEnd = Address_add(memory, memorySize);
#ifdef FLASH_MEMORY
       printf("flash memory\r\n");
    bootstrapSize = loadBootstrapSuiteFromFlash(bootstrapSuiteFile, &com_sun_squawk_VM_romStart, &suite, &com_sun_squawk_VM_bootstrapHash);
    com_sun_squawk_VM_bootstrapStart = com_sun_squawk_VM_romStart;

#ifdef VIRTUAL_ADDRESS_SPACE_LOWER_BOUND
    // on SPOTs and machines with MMU, flash memory may be virtually allocated to a larger range than specified by get_flash_base() + get_flash_size()
    assume((Address)VIRTUAL_ADDRESS_SPACE_LOWER_BOUND > com_sun_squawk_VM_romStart);
    com_sun_squawk_VM_romEnd = (Address)VIRTUAL_ADDRESS_SPACE_UPPER_BOUND;
    //com_sun_squawk_VM_romEnd = (Address)(FLASH_BASE_ADDR + FLASH_BASE_SIZE);
#else /* VIRTUAL_ADDRESS_SPACE_LOWER_BOUND */
    com_sun_squawk_VM_romEnd = (Address)(get_flash_base() + get_flash_size());
#endif /* VIRTUAL_ADDRESS_SPACE_LOWER_BOUND */
    
#else /* FLASH_MEMORY */
    // ROM starts at the begining of the VM's memory buffer
    com_sun_squawk_VM_romStart = com_sun_squawk_VM_bootstrapStart = memory;
#ifdef BYPASS_FILELOADER
       printf("bypass_fileloader\r\n");
	//output_val(sizeof(squawkSuiteArray)/sizeof(squawkSuiteArray[0]));
	bootstrapSize = new_loadBootstrapSuite(squawkSuiteArray, sizeOfSquawkSuiteArray, memory, memorySize, &suite, &com_sun_squawk_VM_bootstrapHash);
#else
	bootstrapSize = loadBootstrapSuite(bootstrapSuiteFile, memory, memorySize, &suite, &com_sun_squawk_VM_bootstrapHash);
#endif /* BYPASS_FILELOADER */
    
#endif /* FLASH_MEMORY */
    com_sun_squawk_VM_bootstrapEnd = Address_add(com_sun_squawk_VM_bootstrapStart, bootstrapSize);
    com_sun_squawk_VM_romEnd = Address_add(com_sun_squawk_VM_romStart, bootstrapSize);

#ifdef FLASH_MEMORY
    // NVM starts at the beginning of the memory buffer
    com_sun_squawk_GC_nvmStart = (Address)roundUp((UWord)memory, pageSize);
#else
    // NVM starts on the next page after the end of ROM
    com_sun_squawk_GC_nvmStart = (Address)roundUp((UWord)com_sun_squawk_VM_romEnd, pageSize);
#endif /* FLASH_MEMORY */
    com_sun_squawk_GC_nvmEnd = Address_add(com_sun_squawk_GC_nvmStart, nvmSize);
    com_sun_squawk_GC_nvmAllocationPointer = com_sun_squawk_GC_nvmStart;

    // RAM starts on the next page after the end of NVM
    com_sun_squawk_GC_ramStart = (Address)roundUp((UWord)com_sun_squawk_GC_nvmEnd, pageSize);
    com_sun_squawk_GC_ramEnd = Address_add(com_sun_squawk_GC_ramStart, ramSize);  
    





    com_sun_squawk_VM_bootstrapSuite = suite;

    // The stack for the service thread starts on the next page after the end of RAM.
    // The length of the stack in logical slots is written into the first word of the
    // block allocate for the stack. This length is later used in Thread.initializeThreading()
    // to format the stack as a Java object of type Klass.LOCAL_ARRAY



	com_sun_squawk_VMThread_serviceStack = Address_add(roundUp((UWord)com_sun_squawk_GC_ramEnd, pageSize), TWOWORDS);

    setArrayLength(com_sun_squawk_VMThread_serviceStack, SERVICE_CHUNK_SIZE_MINUS2WORDS / HDR_BYTES_PER_WORD);
    setUWord(com_sun_squawk_VMThread_serviceStack, SC_guard, 0);
    setObject(com_sun_squawk_VMThread_serviceStack, SC_owner, 0);
    setObject(com_sun_squawk_VMThread_serviceStack, SC_lastFP, 0);
    setUWord(com_sun_squawk_VMThread_serviceStack, SC_lastBCI, 0);

    // The command line arguments for the JAM start on the next page after the end of the stack for the service thread.
    com_sun_squawk_VM_argc = argc;
    com_sun_squawk_VM_argv = (Address)roundUp((UWord)Address_add(com_sun_squawk_VMThread_serviceStack, SERVICE_CHUNK_SIZE_MINUS2WORDS), pageSize);
    offset = writeCStringArray(argc, argv, Address_diff(com_sun_squawk_VM_argv, memory));
    
    // Ensure that the buffer did not overflow
    assumeAlways(loeq(Address_add(memory, offset), memoryEnd));

    // Ensure all the buffers start at word aligned addresses.
    assume(isWordAligned((UWord)com_sun_squawk_VM_romStart));
    assume(isWordAligned((UWord)com_sun_squawk_GC_nvmStart));
    assume(isWordAligned((UWord)com_sun_squawk_GC_ramStart));
    assume(isWordAligned((UWord)com_sun_squawk_VMThread_serviceStack));



    
    if (DEBUG_STARTUP || com_sun_squawk_GC_traceFlags != 0) {
        printRange("ROM       ", (char*)com_sun_squawk_VM_romStart, (char*)com_sun_squawk_VM_romEnd);
        printRange("Suite     ", (char*)suite, (char*)((char*) suite + bootstrapSize));
        printRange("Memory    ", (char*)memory, (char*)memoryEnd);
#if TYPEMAP
        printRange("Type map  ", (char*)getTypePointer(memory), (char*)getTypePointer(memoryEnd));
#endif /* TYPEMAP */
#if DEFAULT_NVM_SIZE
        printRange("NVM       ", (char*)com_sun_squawk_GC_nvmStart, (char*)com_sun_squawk_GC_nvmEnd);
#endif
        printRange("RAM       ", (char*)com_sun_squawk_GC_ramStart, (char*)com_sun_squawk_GC_ramEnd);
        printRange("ServiceStk", (char*)com_sun_squawk_VMThread_serviceStack, (char*)Address_add(com_sun_squawk_VMThread_serviceStack, SERVICE_CHUNK_SIZE_MINUS2WORDS));
#if DEBUG_STARTUP
        printRange("argv      ", (char*)com_sun_squawk_VM_argv, (char*)Address_add(memory, offset));
#endif
    }
    
    return suite;
}

#if KERNEL_SQUAWK
/**
 * Preprocess the command line arguments.
 *
 * @param argv    the original command line options
 * @param argc    the number of components in argv
 * @return the pointer to the relocated bootstrap suite in ROM
 */
void preprocessArgs(char *argv[], int *argc,
                    char *kernelArgs[], int *kernelArgsCount) {
    int newIndex = 0;
    int oldIndex = 0;
    int indexLim = *argc;

    *kernelArgsCount = 0;
    while (oldIndex != indexLim) {
        char *arg = argv[oldIndex];
        if (arg[0] != '-') {
            break; /* finished VM options part */
        }

        if (arg[1] != 'X' && arg[1] != 'K') {
            argv[newIndex++] = arg;
        } else {
            if (arg[1] == 'X') {
                if (startsWith(arg + 2, "kernel")) {
                    kernelMode = true;
                    if (arg[8] == ':') {
                        int n = atoi(arg + 9);
                        kernelSignal = (n == 0) ? SIGSYS : n;
                    } else {
                        kernelSignal = SIGSYS;
                    }
                } else {
                    argv[newIndex++] = arg;
                }
            } else {
                /* '-K' flag */
                /* Use "MAX_JVM_ARGS - 1" to reserve space for
                 * "java.lang.JavaDriverManager". */
                if (*kernelArgsCount >= MAX_JVM_ARGS - 1) {
                    fatalVMError("too many '-K' flags");
                }
                kernelArgs[(*kernelArgsCount)++] = arg + 2;
            }
        }
        oldIndex++;
    }

    /* Copy main class and it args */
    while (oldIndex != indexLim) {
        argv[newIndex++] = argv[oldIndex++];
    }

    *argc = newIndex;
}
#endif /* KERNEL_SQUAWK */

#ifndef FLASH_MEMORY
/********* SIGNAL HANDLING *********
 * Use OS provided signal handling to catch interrupts, bus errors, and segmentation violations.
 **********************************/
#include <signal.h>


#if 1 // the old way

void signalHandler(int signum) {
    char* strsignal(int signum);
    /* WARNING: ARE THESE SAFE TO CALL FROM A SIGNAL HANDLER??? */
    fprintf(stderr, "\ncaught signal %d\n", signum);
    fatalVMError(strsignal(signum));
}

void setupSignals() {
    signal(SIGSEGV, signalHandler);
#ifndef _MSC_VER
    signal(SIGBUS,  signalHandler);
#endif
    signal(SIGINT,  signalHandler);
}


#else   // the new way

#include <dlfcn.h>


void termHandler(int signum, siginfo_t * pInfo, void * pContext) {
/* WARNING: ARE THESE SAFE TO CALL FROM A SIGNAL HANDLER??? */
    fprintf(stderr, "\nSquawk VM caught signal %d\nTERMINATIN©G\n", signum);
  
    fatalVMError(strsignal(signum));

}

void crashHandler(int signum, siginfo_t * pInfo, void * pContext) {
    /* WARNING: ARE THESE SAFE TO CALL FROM A SIGNAL HANDLER??? */
    fprintf(stderr, "\nSquawk VM caught signal %d\nCRASH\n", signum);

    fprintf(stderr, "    si_code: 0x%x\n", pInfo->si_code);
#ifdef VXWORKS
    fprintf(stderr, "    si_value.sival_int: 0x%x\n", pInfo->si_value.sival_int);
#else
    fprintf(stderr, "    si_errno: %d\n", pInfo->si_errno);
    fprintf(stderr, "    si_addr: %p\n", pInfo->si_addr);
#endif

    if (nativeFuncPtr == NULL) {
        fprintf(stderr, "    Not in callout to native function.\n");
    } else {
        fprintf(stderr, "    In callout to native function: %p\n", nativeFuncPtr);
         Dl_info dlinfo;
         if (dladdr(nativeFuncPtr, &dlinfo)) {
            fprintf(stderr, "   named: %s\n", dlinfo.dli_sname);
         } else {
            fprintf(stderr, "   named: unknown\n");
         }
    }
    fprintf(stderr, "Resuming system signal handler...\n");
}

void setupSignals() {
    struct sigaction sa;
    
    nativeFuncPtr = NULL;

    sa.sa_flags = SA_SIGINFO | SA_RESETHAND;
    sa.sa_sigaction = crashHandler;

    if(sigaction(SIGSEGV, &sa, NULL)) {
      perror("sigaction");
      exit(0);
   }

#ifndef _MSC_VER
    if(sigaction(SIGBUS, &sa, NULL)) {
      perror("sigaction");
      exit(0);
   }
#endif
    
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = termHandler;
    if(sigaction(SIGINT, &sa, NULL)) {
      perror("sigaction");
      exit(0);
   }

}

#endif  // the new way
#endif // FLASH_MEMORY


/**
 * Process the command line arguments.
 *
 * @param argv    the original command line options
 * @param argc    the number of components in argv
 * @return the pointer to the relocated bootstrap suite in ROM
 */
Address processArgs(char *argv[], const int argc) {
    int newIndex = 0;
    int oldIndex = 0;
    char *javaVMArgs[MAX_JVM_ARGS];
    int   javaVMArgsCount = 0;
    char *bootstrapSuiteFile = "squawk.suite";

    int nvmSize = DEFAULT_NVM_SIZE;
    int ramSize = DEFAULT_RAM_SIZE;

    diagnostic("in processArgs");


#ifdef PROFILING
    fprintf(stderr, "*************** Profiling version ***************\n");
#endif

#ifdef DB_DEBUG
    fprintf(stderr, "*************** Debug version ***************\n");
#endif

#if TRACE
    fprintf(stderr, "*************** Tracing version ***************\n");
#endif

    while (oldIndex != argc) {
        char *arg = argv[oldIndex];
        char *wholeArg = arg;

        if (arg[0] != '-') {
            break; /* finished VM options part */
        }
diagnostic("c, ");
        if (arg[1] == 'X') {
            arg += 2; /* skip the '-X' */
#if PLATFORM_TYPE_SOCKET
            if (startsWith(arg, "ioport:")) {
                ioport = arg + 7;
            } else
#endif
#if DEFAULT_NVM_SIZE
            if (startsWith(arg, "mxnvm:")) {
                nvmSize = parseQuantity(arg+6, wholeArg);
            } else
#endif
            if (startsWith(arg, "mx:")) {
                ramSize = roundDownToWord(parseQuantity(arg+3, wholeArg));
            } else if (startsWith(arg, "boot:")) {
                bootstrapSuiteFile = arg + 5;
#if (com_sun_squawk_GC_GC_TRACING_SUPPORTED | com_sun_squawk_GarbageCollector_HEAP_TRACE)
            } else if (startsWith(arg, "tgca:")) {
                com_sun_squawk_GC_traceThreshold = parseQuantity(arg+5, wholeArg);
#endif
            } else if (startsWith(arg, "tgc:")) {
                com_sun_squawk_GC_traceFlags = parseQuantity(arg+4, wholeArg);
#ifndef FLASH_MEMORY
            } else if (equals(arg, "notrap")) {
                notrap = true;
#endif /* FLASH_MEMORY */
#if TRACE
            } else if (equals(arg, "tnoservice")) {
                traceServiceThread = false;
            } else if (equals(arg, "terr")) {
                traceFile = stderr;
            } else if (startsWith(arg, "ts:")) {
                setTraceStart(parseQuantityLong(arg+3, wholeArg));
            } else if (startsWith(arg, "te:")) {
                setTraceEnd(parseQuantityLong(arg+3, wholeArg));
            } else if (startsWith(arg, "tr:")) {
                jlong start = parseQuantityLong(arg+3, wholeArg);
                setTraceStart(start);
                setTraceEnd(start + 1000);
            } else if (startsWith(arg, "stats:")) {
                statsFrequency = parseQuantity(arg+6, wholeArg);
                if (statsFrequency == 0) {
                    fprintf(stderr,"-Xstats:0 is invalid\n");
                    stopVM(-1);
                }
#endif /* TRACE */
#ifdef PROFILING
            } else if (startsWith(arg, "prof:")) {
                sampleFrequency = parseQuantity(arg+5, wholeArg);
                if (sampleFrequency == 0) {
                    fprintf(stderr,"-Xprof:0 is invalid\n");
                    stopVM(-1);
                }
#endif /* PROFILING */
            } else {
                if (arg[0] != 0) {
                    fprintf(stderr,"Unrecognised option: %s\n", wholeArg);
                }
                usage();
                stopVM(0);
            }
        } else if (PLATFORM_TYPE_DELEGATING && arg[1] == 'J') {
            /* '-J' flag */
            if (javaVMArgsCount >= MAX_JVM_ARGS) {
                fatalVMError("too many '-J' flags");
            }
            javaVMArgs[javaVMArgsCount++] = arg + 2;
#ifdef __APPLE__
        } else if (startsWith(arg, "-psn")) {
            // skip this (process id number?)
#endif /* __APPLE__ */
        } else {
            argv[newIndex++] = arg;
        }
        oldIndex++;
    }

    /* Copy main class and it args */
    while (oldIndex != argc) {
        argv[newIndex++] = argv[oldIndex++];
    }




#ifndef FLASH_MEMORY
    if (!notrap) {
        setupSignals();
    } else {
        printf("Trap handling disabled\n");
    }
#endif /* FLASH_MEMORY */

#if PLATFORM_TYPE_DELEGATING
    /*
     * Startup the embedded Hotspot VM if Squawk was not launched via a JNI call
     */
    CIO_initialize("squawk.jar", javaVMArgs, javaVMArgsCount);
#elif PLATFORM_TYPE_NATIVE
    IO_initialize();
#elif PLATFORM_TYPE_SOCKET
    IO_initialize();
#endif

    /*
     * Set up the buffer that will be used for the ROM, NVM and RAM, remaining.
     */
    return setupMemory(ramSize, nvmSize, newIndex, argv, bootstrapSuiteFile);
}

#if RUN_UNIT_TESTS
/**
 * A structure used to verify that the PLATFORM_BIG_ENDIAN constant is correct.
 */
typedef union {
    int i;
    char c[4];
} EndianTest;

/**
 * Verifies that the PLATFORM_BIG_ENDIAN constant is correct, that the
 * ROM image was built with the correct endianess and that loads can be
 * unaligned if the PLATFORM_UNALIGNED_LOADS constant is true. If any of
 * these tests fail, an error message is printed and the VM will exit.
 */
void verifyBuildFlags() {
    EndianTest et;
    boolean bigEndian;

    et.i = 1;
    bigEndian = (et.c[3] == 1);

    if (bigEndian != PLATFORM_BIG_ENDIAN) {
        fprintf(stderr, "PLATFORM_BIG_ENDIAN constant is incorrect: should be %s\n", bigEndian ? "true" : "false");
        stopVM(-1);
    }

    if (SQUAWK_64 != (HDR_BYTES_PER_WORD == 8)) {
        fprintf(stderr, "A %d bit squawk executable cannot be run with a %d bit image\n", (SQUAWK_64 ? 64 : 32), HDR_BYTES_PER_WORD*8);
        stopVM(-1);
    }

    if (PLATFORM_UNALIGNED_LOADS) {
        unsigned char bytecode[] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };
        int i;

        for (i = 0; i != 8; ++i) {
            ByteAddress xxip;
            unsigned char *ptr = bytecode + i;
            int b1 = *ptr++;
            int b2 = *ptr++;
            int b3 = *ptr++;
            int b4 = *ptr++;
            int expect;

            if (PLATFORM_BIG_ENDIAN) {
                expect = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
            } else {
                expect = (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
            }

            xxip = bytecode + i;
            if (expect != *((int *)xxip)) {
                fprintf(stderr, "PLATFORM_UNALIGNED_LOADS constant is incorrect: should be false\n");
                stopVM(-1);
            }
        }
    }

    if (MONITOR_CACHE_SIZE > com_sun_squawk_VMThread_MAXDEPTH) {
        // this should never happen, but check once at startup, instead of
        // at every lock operation.
        fprintf(stderr, "MONITOR_CACHE_SIZE is too large for maximum lock depth\n");
        stopVM(-1);
    }
}

#define assertEquals4(msg, expected, received) \
    if (received != expected) { \
        fprintf(stderr, "Test failed. Expected 0x%x, received 0x%x at %s:%d\n    %s\n", expected, received, __FILE__, __LINE__, msg); \
        exit(EXIT_FAILURE); \
    } \
    
#define assertEquals8(msg, expected, received) \
    if (received != expected) { \
        fprintf(stderr, "Test failed. Expected 0x%llx, received 0x%llx at %s:%d\n    %s\n", expected, received, __FILE__, __LINE__, msg); \
        exit(EXIT_FAILURE); \
    } \

void testLongs() {
    const jlong PATTERN1 = 0xAAAAAAAAAAAAAAAALL;
    const jlong PATTERN2 = 0xAAAAAAAAFFFFFFFFLL;
    const int   PATTERN3 = 0xAAAAAAAA;
    const int   MAX      = 0xFFFFFFFF;
    
    jlong l1 = makeLong(PATTERN3, PATTERN3);
    assertEquals8("makeLong() 1", PATTERN1, l1);

    assertEquals4("getHi() 1", PATTERN3, getHi(l1));
    assertEquals4("getLo() 1", PATTERN3, getLo(l1));

    l1 = makeLong(PATTERN3, MAX);
    assertEquals8("makeLong() 2", PATTERN2, l1);

    assertEquals4("getHi() 2", PATTERN3, getHi(l1));
    assertEquals4("getLo() 2", MAX,      getLo(l1));
}

void testUnaligned() {
    const jlong PATTERN1 = 0x1122334455667788LL;
    const jlong PATTERN1_SWAP = 0x8877665544332211LL;
    const jlong PATTERN2 = 0xAAAAAAAAFFFFFFFFLL;
    const jlong PATTERN2_SWAP = 0xFFFFFFFFAAAAAAAALL;
    const int   PATTERN3 = 0x55667788;
    const int   PATTERN3_SWAP = 0x88776655;
    jlong tmp[2];
    Address buf = ((char*)&tmp); // first check aligned:
    tmp[0] = 0;
    tmp[1] = 0;
    
    {
        jlong val;
        jlong valCheck;

        // compare results of the aligned and unaligned accessors
        setLong(buf, 0, PATTERN1);
        assertEquals8("overwrite", 0ll, tmp[1]);
        valCheck = getLong(buf, 0);
        assertEquals8("getLong", PATTERN1, valCheck);
        
        setUnalignedLong(buf, 0, PATTERN1);
        assertEquals8("overwrite", 0ll, tmp[1]);
        valCheck = getLong(buf, 0);
        assertEquals8("getLong 2", PATTERN1, valCheck);
        val = getUnalignedLong(buf, 0);
        assertEquals8("getUnalignedLong check", valCheck, val);
        swap8(buf);
        valCheck = getLong(buf, 0);
        assertEquals8("swap8 aligned", PATTERN1_SWAP, valCheck);
        val = getUnalignedLong(buf, 0);
        assertEquals8("swap8", PATTERN1_SWAP, val);

        setUnalignedLong(buf, 0, PATTERN2);
        assertEquals8("overwrite", 0ll, tmp[1]);
        val = getUnalignedLong(buf, 0);
        assertEquals8("getUnalignedLong 2", PATTERN2, val);
        swap8(buf);
        valCheck = getLong(buf, 0);
        assertEquals8("swap8 p2 aligned", PATTERN2_SWAP, valCheck);
        val = getUnalignedLong(buf, 0);
        assertEquals8("swap8 p2 2", PATTERN2_SWAP, val);
    }

    {
        int val;
        int valCheck;

        setInt(buf, 0, PATTERN3);
        valCheck = getInt(buf, 0);
        assertEquals4("getInt", PATTERN3, valCheck);

        setUnalignedInt(buf, 0, PATTERN3);
        valCheck = getInt(buf, 0);
        assertEquals4("getInt 2", PATTERN3, valCheck);
        val = getUnalignedInt(buf, 0);
        assertEquals4("getUnalignedInt", PATTERN3, val);
        swap4(buf);
        val = getUnalignedInt(buf, 0);
        assertEquals4("swap4", PATTERN3_SWAP, val);
    }
    
    // -------- force unaligned -----------------
    buf = ((char*)&tmp) + 1;

    {
        const int REMAINDER_MASK = PLATFORM_BIG_ENDIAN ? 0x00FFFFFFLL: 0xFFFFFF00LL;
        jlong val;

        setUnalignedLong(buf, 0, PATTERN1);
        assertEquals8("overwrite", 0LL, (tmp[1] & REMAINDER_MASK));
        val = getUnalignedLong(buf, 0);
        assertEquals8("getUnalignedLong", PATTERN1, val);
        swap8(buf);
        val = getUnalignedLong(buf, 0);
        assertEquals8("swap8", PATTERN1_SWAP, val);

        setUnalignedLong(buf, 0, PATTERN2);
        assertEquals8("overwrite", 0LL, (tmp[1] & REMAINDER_MASK));
        val = getUnalignedLong(buf, 0);
        assertEquals8("getUnalignedLong p2", PATTERN2, val);
        swap8(buf);
        val = getUnalignedLong(buf, 0);
        assertEquals8("swap8 p2", PATTERN2_SWAP, val);
    }

    {
        int val;
        setUnalignedInt(buf, 0, PATTERN3);
        val = getUnalignedInt(buf, 0);
        assertEquals4("getUnalignedInt", PATTERN3, val);
        swap4(buf);
        val = getUnalignedInt(buf, 0);
        assertEquals4("swap4", PATTERN3_SWAP, val);
    }
}

void testWordAt() {
    const jlong PATTERN1 = 0x1122334455667788LL;
    const jlong PATTERN1_SWAP = 0x8877665544332211LL;
    const jlong PATTERN2 = 0xAAAAAAAAFFFFFFFFLL;
    const jlong PATTERN2_SWAP = 0xFFFFFFFFAAAAAAAALL;
    const int   PATTERN3 = 0x55667788;
    const int   PATTERN3_SWAP = 0x88776655;
    jlong tmp;
    Address buf = &tmp;
    jlong val;

    setLongAtWord(buf, 0, PATTERN1);
    val = getLongAtWord(buf, 0);
    assertEquals8("getLongAtWord", PATTERN1, val);
    swap8(buf);
    val = getLongAtWord(buf, 0);
    assertEquals8("swap8", PATTERN1_SWAP, val);
    
    setLongAtWord(buf, 0, PATTERN2);
    val = getLongAtWord(buf, 0);
    assertEquals8("getLongAtWord 2", PATTERN2, val);
    swap8(buf);
    val = getLongAtWord(buf, 0);
    assertEquals8("swap8 2", PATTERN2_SWAP, val);
}


void fpTest() {
    float f0 = 0.0;
    float f1 = 1.0;
    double d0 = 0.0;
    double d1 = 1.0;
    FILE *vmOut = streams[currentStream];

#if 0
    fprintf(vmOut, format("f0 = %i\n"), f2ib(f0));
    fprintf(vmOut, format("f1 = %i\n"), f2ib(f1));
    fprintf(vmOut, format("f0 + f0 = %i\n"), f2ib(f0 + f0));
    fprintf(vmOut, format("f0 + f1 = %i\n"), f2ib(f0 + f1));
    fprintf(vmOut, format("f0 * f1 = %i\n"), f2ib(f0 * f1));
    fprintf(vmOut, format("f1 * f1 = %i\n"), f2ib(f1 * f1));
    fprintf(vmOut, format("f1 + f1 = %i\n"), f2ib(f1 + f1));
    fflush(vmOut);

    fprintf(vmOut, format("f0 = %f\n"), f0);fflush(vmOut);
    fprintf(vmOut, format("f1 = %f\n"), f1);fflush(vmOut);
    fprintf(vmOut, format("f0 + f0 = %f\n"), (f0 + f0));fflush(vmOut);
    fprintf(vmOut, format("f0 + f1 = %f\n"), (f0 + f1));fflush(vmOut);
    fprintf(vmOut, format("f0 * f1 = %f\n"), (f0 * f1));fflush(vmOut);
    fprintf(vmOut, format("f1 * f1 = %f\n"), (f1 * f1));fflush(vmOut);
    fprintf(vmOut, format("f1 + f1 = %f\n"), (f1 + f1));fflush(vmOut);

    fprintf(vmOut, format("d0 = %D\n"), d0);fflush(vmOut);
    fprintf(vmOut, format("d1 = %D\n"), d1);fflush(vmOut);
    fprintf(vmOut, format("d0 + d0 = %D\n"), (d0 + d0));fflush(vmOut);
    fprintf(vmOut, format("d0 + d1 = %D\n"), (d0 + d1));fflush(vmOut);
    fprintf(vmOut, format("d0 * d1 = %D\n"), (d0 * d1));fflush(vmOut);
    fprintf(vmOut, format("d1 * d1 = %D\n"), (d1 * d1));fflush(vmOut);
    fflush(vmOut);
#endif
    
    if (f0 == 0.0) {
        // fprintf(vmOut, "fpTest passed 1\n");
    } else {
        fprintf(vmOut, "fpTest failed 1\n");
    }

    if (f1 > f0) {
        // fprintf(vmOut, "fpTest passed 2\n");
    } else {
        fprintf(vmOut, "fpTest failed 2\n");
    }

#if C_FP_CONVERSIONS_OK
    // Test that the normal C conversions are OK to use, or if the 
    // functions float2l, float2ll, double2l, double2ll are needed on this platform:

    //fprintf(vmOut, "Running C_FP_CONVERSIONS_OK tests...\n");
    assertEquals4("float2l(1.0f)", float2l_slow(1.0f), (int)1.0f);
    assertEquals4("float2l(F_L_POS_NAN)", float2l_slow(ib2f(F_L_POS_NAN)), (int)(ib2f(F_L_POS_NAN)));
    assertEquals4("float2l(F_H_POS_NAN)", float2l_slow(ib2f(F_H_POS_NAN)), (int)(ib2f(F_H_POS_NAN)));
    assertEquals4("float2l(F_L_NEG_NAN)", float2l_slow(ib2f(F_L_NEG_NAN)), (int)(ib2f(F_L_NEG_NAN)));
    assertEquals4("float2l(F_H_NEG_NAN)", float2l_slow(ib2f(F_H_NEG_NAN)), (int)(ib2f(F_H_NEG_NAN)));
    assertEquals4("float2l(F_POS_INFINITY)", float2l_slow(ib2f(F_POS_INFINITY)), (int)(ib2f(F_POS_INFINITY)));
    assertEquals4("float2l(F_NEG_INFINITY)", float2l_slow(ib2f(F_NEG_INFINITY)), (int)(ib2f(F_NEG_INFINITY)));

    assertEquals8("float2ll(1.0f)", float2ll_slow(1.0f), (jlong)1.0f);
    assertEquals8("float2ll(F_L_POS_NAN)", float2ll_slow(ib2f(F_L_POS_NAN)), (jlong)(ib2f(F_L_POS_NAN)));
    assertEquals8("float2ll(F_H_POS_NAN)", float2ll_slow(ib2f(F_H_POS_NAN)), (jlong)(ib2f(F_H_POS_NAN)));
    assertEquals8("float2ll(F_L_NEG_NAN)", float2ll_slow(ib2f(F_L_NEG_NAN)), (jlong)(ib2f(F_L_NEG_NAN)));
    assertEquals8("float2ll(F_H_NEG_NAN)", float2ll_slow(ib2f(F_H_NEG_NAN)), (jlong)(ib2f(F_H_NEG_NAN)));
    assertEquals8("float2ll(F_POS_INFINITY)", float2ll_slow(ib2f(F_POS_INFINITY)), (jlong)(ib2f(F_POS_INFINITY)));
    assertEquals8("float2ll(F_NEG_INFINITY)", float2ll_slow(ib2f(F_NEG_INFINITY)), (jlong)(ib2f(F_NEG_INFINITY)));
    
    assertEquals4("double2l(1.0)", double2l_slow(1.0), (int)1.0);
    assertEquals4("double2l(D_L_POS_NAN)", double2l_slow(lb2d(D_L_POS_NAN)), (int)(lb2d(D_L_POS_NAN)));
    assertEquals4("double2l(D_H_POS_NAN)", double2l_slow(lb2d(D_H_POS_NAN)), (int)(lb2d(D_H_POS_NAN)));
    assertEquals4("double2l(D_L_NEG_NAN)", double2l_slow(lb2d(D_L_NEG_NAN)), (int)(lb2d(D_L_NEG_NAN)));
    assertEquals4("double2l(D_H_NEG_NAN)", double2l_slow(lb2d(D_H_NEG_NAN)), (int)(lb2d(D_H_NEG_NAN)));
    assertEquals4("double2l(D_POS_INFINITY)", double2l_slow(lb2d(D_POS_INFINITY)), (int)(lb2d(D_POS_INFINITY)));
    assertEquals4("double2l(D_NEG_INFINITY)", double2l_slow(lb2d(D_NEG_INFINITY)), (int)(lb2d(D_NEG_INFINITY)));

    assertEquals8("double2ll(1.0)", double2ll_slow(1.0), (jlong)1.0);
    assertEquals8("double2ll(D_L_POS_NAN)", double2ll_slow(lb2d(D_L_POS_NAN)), (jlong)(lb2d(D_L_POS_NAN)));
    assertEquals8("double2ll(D_H_POS_NAN)", double2ll_slow(lb2d(D_H_POS_NAN)), (jlong)(lb2d(D_H_POS_NAN)));
    assertEquals8("double2ll(D_L_NEG_NAN)", double2ll_slow(lb2d(D_L_NEG_NAN)), (jlong)(lb2d(D_L_NEG_NAN)));
    assertEquals8("double2ll(D_H_NEG_NAN)", double2ll_slow(lb2d(D_H_NEG_NAN)), (jlong)(lb2d(D_H_NEG_NAN)));
    assertEquals8("double2ll(D_POS_INFINITY)", double2ll_slow(lb2d(D_POS_INFINITY)), (jlong)(lb2d(D_POS_INFINITY)));
    assertEquals8("double2ll(D_NEG_INFINITY)", double2ll_slow(lb2d(D_NEG_INFINITY)), (jlong)(lb2d(D_NEG_INFINITY)));

    assertEquals8("(jlong)1E+100D == max long",     (jlong)1E+100D, 0x7fffffffffffffffLL);
#endif
}




void unitTests() {
    testLongs();
    testUnaligned();
    testWordAt();
    fpTest();
}

#endif /* RUN_UNIT_TESTS */

/**
 * Program entrypoint.
 *
 * @param argc the number of command line parameters
 * @param argv the parameter argument vector
 */
Address Squawk_setup(int argc, char *argv[]) {

    Address bootstrapSuite;

    /*
     * Sanity check.
     */
    assume(sizeof(UWord) == sizeof(Address));
    diagnostic("in Squawk_setup");

#if RUN_UNIT_TESTS
    /*
     * Check that the build flags were correct.
     */
    verifyBuildFlags();
    
    unitTests();
#endif

    /*
     * Extract the native VM options
     */
    bootstrapSuite = processArgs(argv, argc);

    /*
     * Set the global informing the VM that memory access type checking is enabled
     */
    com_sun_squawk_VM_usingTypeMap = TYPEMAP;

    /*
     * Make ROM and NVM be read-only
     */
    sysToggleMemoryProtection(com_sun_squawk_GC_nvmStart, com_sun_squawk_GC_nvmEnd, true);
    sysToggleMemoryProtection(com_sun_squawk_VM_romStart, com_sun_squawk_VM_romEnd, true);

    initMethods();

    /*
     * Return the bootstrap suite.
     */
    return bootstrapSuite;
}

/**
 * Saves the values of the 'ip, 'sp' and 'fp' registers into the data structure
 * encapsulating the current execution context (uer or kernel).
 */
#define saveContextRegisters() {    \
    saved_ip = ip;          \
    saved_fp = fp;          \
    saved_sp = sp;          \
}

/**
 * Implements the OPC.PAUSE bytecode. This must be a macro as it
 * executes a return from the main interpreter loop.
 */
#if KERNEL_SQUAWK
#define do_pause() {        \
fprintf(stderr, format("VM.pause called after %L branches (kernelMode %d)\n"), getBranchCount(), inKernelMode()); \
    saveContextRegisters(); \
    return;                 \
}
#else /* KERNEL_SQUAWK */
#define do_pause() {        \
    fatalInterpreterError("Illegal Squawk bytecode (OPC.PAUSE = 0xFF)"); \
}
#endif /* KERNEL_SQUAWK */

/**
 * Continues execution of the VM from the last OPC.PAUSE.
 *
 * @param gp the global pointer
 */
void Squawk_continue(Globals *gp) {
    int opcode = -1;
#if TRACE
    int opcodeCopy = opcode;
#endif /* TRACE */

#ifdef MACROIZE
    int          iparm;             /* The immediate operand value of the current bytecode. */
    ByteAddress  ip;                /* The instruction pointer. */
    UWordAddress fp;                /* The frame pointer. */
    UWordAddress sp;                /* The stack pointer. */
#if SDA_DEBUGGER
    int sda_bp_set_or_stepping = false;     /* TRUE if we have any breakpoints set or are in a stepping mode [optimisation] */
#endif /* SDA_DEBUGGER */
#endif /* MACROIZE */

    ip = saved_ip;                  /* Restore the instruction pointer. */
    fp = saved_fp;                  /* Restore the frame pointer. */
    sp = saved_sp;                  /* Restore the stack pointer. */





    /*
     * This is the main bytecode execution loop.
     */
   while (true) {
#if TRACE
        const int lastOpcode = opcodeCopy;
        ByteAddress ipCopy = ip;
#ifdef MACROIZE
        /*
         * Copy the variable values into corresponding globals so that
         * printStackTrace() works in the macroized version of the VM
         */
        lastIP = ip;
        lastFP = fp;
#endif /* MACROIZE */

#endif /* TRACE */

#if SDA_DEBUGGER
        /* Check for breakpoint or single step before decoding. If breakpoint is hit, then sda_checkBreakOrStep()
         * will update frame and ip, so that breakpoint handling code is called instead.  Likewise, if a step occurs,
         * single step handling code is called.
         */
        sda_checkBreakOrStep(ip, fp, sp);
#endif

        opcode = fetchUByte();

#ifdef PROFILING
        opcodeCopy = opcode;
#endif
        osloop();
#ifdef DB_DEBUG
        db_checkBreak(opcode, ip, fp);
#endif


#if TRACE
        if (com_sun_squawk_VM_tracing) {
            trace(ipCopy, fp, sp);
        }
#endif /* TRACE */

#ifdef PROFILING
        instructionCount++;
#ifdef OSPROF
        OSPROF(ipCopy, fp, lastOpcode);
#else
        if (sampleFrequency > 0 && (instructionCount % sampleFrequency) == 0) {
            printProfileStackTrace(ipCopy, fp, lastOpcode);
        }
#endif /* OSPROF */
#endif /* PROFILING */
        next:
#include "switch.c"
            continue;

#ifdef MACROIZE
        threadswitchstart: {
            threadswitchmain();
            continue;
        }

        invokenativestart: {
            invokenativemain();
            continue;
        }

        throw_nullPointerException: {
            saveContextRegisters();
            resetStackPointer();
            call(com_sun_squawk_VM_nullPointerException);
            continue;
        }

        throw_arrayIndexOutOfBoundsException: {
            saveContextRegisters();
            resetStackPointer();
            call(com_sun_squawk_VM_arrayIndexOutOfBoundsException);
            continue;
        }

#endif /* MACROIZE */
    }
}

void printRange(char* label, void* start, void* end) {
    fprintf(stderr, "%s  : %p~%p [size:%d]\r\n", label, start, end, ((char*)end - (char*)start));
}

int printValueInRange(Address value, char* valueName, Address low, Address high, char* rangeName) {
    int result = (value >= low && value < high);
    fprintf(stderr, "    %s: %p. Is %sin %s range %p .. %p\n", valueName, value, (result ? "" : "NOT "), rangeName, low, high);
    return result;
}

void printStrSafely(Address value, char* valueName, Address low, Address high, char* rangeName) {
    int result = (value >= low && value < high);
    fprintf(stderr, "    %s: ", valueName);
    if (value == 0) {
        fprintf(stderr, "NULL.\n");
    } else if (result) {
        printJavaString(value, stderr);
        fprintf(stderr, "\n");
    } else {
        fprintf(stderr, "%p is NOT in %s range %p .. %p\n", value, rangeName, low, high);
    }
}

void printJavaStrSafely(Address value, char* valueName) {
    printStrSafely(value, valueName, com_sun_squawk_VM_romStart, com_sun_squawk_GC_ramEnd, "RAM or ROM");
}


/**
 * Exits the VM with an error message including various interpreter state.
 */
extern  void fatalInterpreterError0(char *msg, int opcode, int _iparm, ByteAddress _ip, UWordAddress _fp, UWordAddress _sp, char* file, int line) {
    Address stackHigh = com_sun_squawk_GC_ramEnd;
    boolean userStack = FALSE;

    jnaSetLED(-1, 0x4141);

    fprintf(stderr, "fatalInterpreterError %s -- %s:%d\n", msg, file, line);
    fprintf(stderr, "    opcode: 0x%x\n", opcode);
#if com_sun_squawk_Klass_DEBUG_CODE_ENABLED
    if (opcode != (opcode & 0xFF)) {
           fprintf(stderr, "    opcode overflowed!\n");
    }
#endif
    fprintf(stderr, "    iparm: 0x%x\n", _iparm);

    if (com_sun_squawk_GC_collecting) {
        fprintf(stderr, "    in GC\n");
    }

#if com_sun_squawk_Klass_ENABLE_DYNAMIC_CLASSLOADING
    printValueInRange(_ip, "ip", com_sun_squawk_VM_romStart, com_sun_squawk_GC_ramEnd, "code");
#else
    printValueInRange(_ip, "ip", com_sun_squawk_VM_romStart, com_sun_squawk_VM_romEnd, "code");
#endif

    if (ss == com_sun_squawk_VMThread_serviceStack) {
        fprintf(stderr, "    stack: %p. current stack is service stack\n", ss);
    } else if (printValueInRange(ss, "stack", com_sun_squawk_GC_ramStart, com_sun_squawk_GC_ramEnd, "RAM")) {
        int cid = com_sun_squawk_Klass_id(getClass(ss));
        if (cid == CID_LOCAL_ARRAY) {
            userStack = TRUE;
        } else {
            fprintf(stderr, "stack has bad class id: %x\n", cid);
        }
    }
    stackHigh = Address_add(ss, getArrayLength(ss) * HDR_BYTES_PER_WORD);
    printValueInRange(_sp, "sp", sl, stackHigh, "current stack");
    printValueInRange(_fp, "fp", sl, stackHigh, "current stack");
    
    // saved values not always valid. 
    fprintf(stderr, "        next three saved values may not be valid:\n");
#if com_sun_squawk_Klass_ENABLE_DYNAMIC_CLASSLOADING
    printValueInRange(saved_ip, "saved_ip", com_sun_squawk_VM_romStart, com_sun_squawk_GC_ramEnd, "code");
#else
    printValueInRange(saved_ip, "saved_ip", com_sun_squawk_VM_romStart, com_sun_squawk_VM_romEnd, "code");
#endif
    printValueInRange(saved_sp, "saved_sp", sl, stackHigh, "current stack");
    printValueInRange(saved_fp, "saved_fp", sl, stackHigh, "current stack");
    
    if (userStack) {
        Address thread = getObject(ss, SC_owner);

        printValueInRange(thread, "thread", com_sun_squawk_GC_ramStart, com_sun_squawk_GC_ramEnd, "RAM");
        printStrSafely(com_sun_squawk_VMThread_name(thread), "thread name", com_sun_squawk_VM_romStart, com_sun_squawk_GC_ramEnd, "RAM or ROM");
#if com_sun_squawk_Klass_DEBUG_CODE_ENABLED
        {
            Address isolate = com_sun_squawk_VMThread_isolate(thread);
            if (printValueInRange(isolate, "isolate", com_sun_squawk_GC_ramStart, com_sun_squawk_GC_ramEnd, "RAM")) {
                printStrSafely(com_sun_squawk_Isolate_mainClassName(isolate), "isolate main class",
                               com_sun_squawk_GC_ramStart, com_sun_squawk_GC_ramEnd, "RAM");
            }
        }
#endif
    } else {
        if (com_sun_squawk_GC_collecting) {
            fprintf(stderr, "    in garbage collection\n");
        }
    }
    
    if (!com_sun_squawk_VM_extendsEnabled) {
        fprintf(stderr, "    stack extension disabled\n");
    }

    fatalVMError("");
}

/**
 * Runs the VM.
 *
 * @param argc the number of command line parameters
 * @param argv the parameter argument vector
 */
void Squawk_run(int argc, char *argv[]) {
    Address bootstrapSuite = Squawk_setup(argc, argv);
    


#ifdef MACROIZE
    ByteAddress  ip = 0;            /* The instruction pointer. */
    UWordAddress fp = 0;            /* The frame pointer. */
    UWordAddress sp = 0;            /* The stack pointer. */
#endif

    /*
     * Set up the VM entry point.
     */
    ip = (ByteAddress)com_sun_squawk_VM_startup;
    sp = (UWordAddress)Address_add(com_sun_squawk_VMThread_serviceStack, SERVICE_CHUNK_SIZE_MINUS2WORDS);
    setStack(com_sun_squawk_VMThread_serviceStack);

    /*
     * Push the parameters.
     */
    downPushAddress(bootstrapSuite);
    downPushAddress(0); /* Dummy return address */

#ifdef PROFILING
#ifdef OSPROF
    osprofstart(sampleFrequency);
#endif
#endif

#ifdef DB_DEBUG
    db_prepare();
#endif

    saveContextRegisters();
    Squawk_continue(gp);
}

/**
 * Normal program startup.
 *
 * @param argc the number of command line parameters
 * @param argv the parameter argument vector
 */
void Squawk_main(int argc, char *argv[]) {
#if KERNEL_SQUAWK
    /* Doing this here guaratees that all configurations (including
     * Java_com_sun_squawk_vm_Main_squawk() via JNI) have access to
     * a kernel/user split. However, Nik felt this option should be
     * done explicitly for only those platforms that will use this
     * mode. This seems good enough for now. */
    char *kernelArgs[MAX_JVM_ARGS];
    int   kernelArgsCount = 0;

    preprocessArgs(argv, &argc, kernelArgs, &kernelArgsCount);
    if (kernelMode) {
        Squawk_setSignalHandlers();
        Squawk_kernelMain(kernelArgsCount, kernelArgs);
    }
#endif /* KERNEL_SQUAWK */
    diagnostic("in Squawk_main");
    Squawk_run(argc, argv);
    fatalVMError("return from interpreter loop in non-kernel mode");
}

/**
 * Program entrypoint.
 *
 * @param argc the number of command line parameters
 * @param argv the parameter argument vector
 */
int Squawk_main_wrapper(int argc, char *argv[]) {
    int ignore = initializeGlobals(&userGlobals);
    
































    diagnostic("in Squawk_main_wrapper");

    /*
     * Lose the first argument (the path to the executable program).
     */
    argv++;
    argc--;

    Squawk_main(argc, argv);
    
    return 0;
}

/**
 * Some platforms have non-standard startup requirements. If so, define os_main() to handle
 * the issues.
 */
#if defined(__APPLE__) || defined(VXWORKS)
#include "os_main.c"
#endif

#ifndef VXWORKS
	#ifndef AUTOSAR
		int main(int argc, char *argv[]) {
    	diagnostic("In main - squawk.c");
		#ifdef __APPLE__
	    	os_main(argc, argv);
		#else
	    	return Squawk_main_wrapper(argc, argv);
		#endif /* !__APPLE__ */ 
		}
	#endif /* not AUTOSAR */
#endif /* not VXWORKS */

/**
 * lookup a klass from the bootstrap suite
 * @param cid
 * @return klass
 */
static Address lookupKlass(int cid) {
    Address classes;
    Address class;
    assume(com_sun_squawk_VM_bootstrapSuite != null);
    assume(inROM(com_sun_squawk_VM_bootstrapSuite));
    classes = com_sun_squawk_Suite_classes(com_sun_squawk_VM_bootstrapSuite);
    assume(inROM(classes));
    boundsAssume(classes, cid);
    class = getObject(classes, cid);
    return class;
}

static Address lookupStaticMethod(int cid, int methodOffset) {
    Address klass;
    Address method;
    //fprintf(stderr, "Lookup of %d in class %d = ", methodOffset, cid);
    klass = lookupKlass(cid);
    assume(inROM(klass));
    method = getStaticMethod(klass, methodOffset);
    assume(inCode(method));
    //fprintf(stderr, "%x\n", method);
    return method;
}
