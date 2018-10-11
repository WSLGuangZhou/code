/*
 * File: include/nsl/internal/nsl_port.h
 *
 * Defines of the platform ports. 
 *
 * Copyright (C) 2010-2014 sl.cai(noirscsl@163.com). All rights reserved.
 *
 * This file is a part of the nsl toolkit.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * modification history
 * --------------------
 * 08/29/2010,      1.0,  sl.cai   Created
 * 09/02/2010,      1.1,  sl.cai   add detect processor, BSD series os, 
 *                                 endian byte order architecture and CPU bits.
 * 11/20/2010,      1.2,  sl.cai   modify CodeWarrior Macros.
 * 02/20/2014,      1.3,  sl.cai   add NSL_CPU_MIPS Macros.
 * 03/13/2014,      1.4,  sl.cai   use #include <endian.h> for NSL_BIG_ENDIAN and NSL_LITTLE_ENDIAN.
 * 
 */

#ifndef NSL_INCLUDE_INTERNAL_NSL_PORT_H_
#define NSL_INCLUDE_INTERNAL_NSL_PORT_H_

// #ifndef DTS_COMMON_H_INCLUDED
// # error "nsl_port.h should only be included from nsl.h"
// #endif /* DTS_COMMON_H_INCLUDED */

/*
Macros indicating the current platform (defined to 1 if compiled on
the given platform; otherwise undefined):
  NSL_OS_AIX      - IBM AIX
  NSL_OS_CYGWIN   - Cygwin
  NSL_OS_LINUX    - Linux
  NSL_OS_FREEBSD  - FreeBSD
  NSL_OS_NETBSD   - NetBSD
  NSL_OS_OPENBSD  - OpenBSD
  NSL_OS_MAC      - Mac OS X
  NSL_OS_SOLARIS  - Sun Solaris
  NSL_OS_SYMBIAN  - Symbian
  NSL_OS_WINDOWS  - Windows (Desktop, MinGW, or Mobile)
    NSL_OS_WINDOWS_DESKTOP  - Windows Desktop
    NSL_OS_WINDOWS_MINGW    - MinGW
    NSL_OS_WINDOWS_MOBILE   - Windows Mobile
  NSL_OS_ZOS      - z/OS
  NSL_OS_VXWORKS  - vxworks
*/

/* Determines the platform on which nsl toolkit is compiled. */
#if defined(__CYGWIN__) || defined(__CYGWIN32__)
#define NSL_OS_CYGWIN 1
#elif defined __SYMBIAN32__
#define NSL_OS_SYMBIAN 1
#elif defined (_WIN32) || defined(WIN32) || defined(__WIN32__) || defined (_WIN64)
#define NSL_OS_WINDOWS 1
#ifdef _WIN32_WCE
#define NSL_OS_WINDOWS_MOBILE 1
#elif defined (__MINGW__) || defined (__MINGW32__)
#define NSL_OS_WINDOWS_MINGW 1
#else
#define NSL_OS_WINDOWS_DESKTOP 1
#endif  /* _WIN32_WCE */
#elif defined (__APPLE__)  || defined (__MACH__) || defined(macintosh) || defined(__APPLE_CC__)
#define NSL_OS_MAC 1
#elif defined (__linux__) || defined (linux) || defined (__linux) || defined (__gnu_linux__)
#define NSL_OS_LINUX 1
#elif defined (__FreeBSD__)
#define NSL_OS_FREEBSD
#elif defined (__NetBSD__)
#define NSL_OS_NETBSD
#elif defined (__OpenBSD__)
#define NSL_OS_OPENBSD
#elif defined __MVS__
#define NSL_OS_ZOS 1
#elif defined(__sun) && defined(__SVR4)
#define NSL_OS_SOLARIS 1
#elif defined(_AIX)
#define NSL_OS_AIX 1
#elif (DEFAULT_RULE == vxWorks)
#define NSL_OS_VXWORKS 1
#else /* ? os */
#error "Unrecognised OS"
#endif  /* __CYGWIN__ */

/* 
    \note vxwork define in nsl_platform.h, because we compile vxwork source in 
          window platform.
*/
#ifndef NSL_OS_VXWORKS 

// #include <endian.h>
// #if __BYTE_ORDER == __LITTLE_ENDIAN
// #define NSL_LITTLE_ENDIAN
// #else
// #define NSL_BIG_ENDIAN
// #endif

/* Carbon defines this for us on Mac, apparently... */
#if defined (TARGET_CPU_PPC)
    #define NSL_CPU_PPC TARGET_CPU_PPC
/* ARM */
#elif defined (__arm__)
    #define NSL_CPU_ARM
/* DEC Alpha */
#elif defined (__alpha) || defined (__alpha__)
    #define NSL_CPU_ALPHA
/* Sun SPARC */
#elif defined (__sparc) || defined (__sparc__)
    #define NSL_CPU_SPARC
/* PowerPC */
#elif defined (_ARCH_PPC) || defined (__ppc__) || defined (__ppc64__) || defined (__PPC) || defined (powerpc) || defined (__PPC__) || defined (__powerpc64__) || defined (__powerpc64)
    #if defined (__ppc64__) || defined (__powerpc64__) || defined (__powerpc64)
        #define NSL_CPU_PPC 64
    #else
        #define NSL_CPU_PPC 32
    #endif
/* x86_64 or AMD64 or x64 */
#elif defined (__x86_64__) || defined (__x86_64) || defined (__amd64) || defined (__amd64__) || defined (_AMD64_) || defined (_M_X64)
    #define NSL_CPU_X64
/* Intel x86 */
#elif defined (__i386__) || defined (__i386) || defined (i386) || defined (_X86_) || defined (_M_IX86)
    #define NSL_CPU_X86
/* IA64 */
#elif defined (__ia64__) || defined (_IA64) || defined (__ia64) || defined (_M_IA64)
    #define NSL_CPU_IA64
#elif defined (__mips__)
    #define NSL_CPU_MIPS
#else /* ? processor */
    #error "Unrecognised processor"
#endif /* TARGET_CPU_PPC */

/*
Macros indicating the current CPU BITS
*/
#if defined (_LP64) || defined (__LP64__)
    #define NSL_CPU_BITS 64
#else
    #define NSL_CPU_BITS 32
#endif

#if defined (NSL_CPU_PPC)
    #if NSL_CPU_PPC == 32
        #define NSL_CPU_BITS 32
    #elif NSL_CPU_PPC == 64
        #define NSL_CPU_BITS 64
    #endif
#endif

#endif /* NSL_OS_VXWORKS */

/*
Macros indicating the current compiler:
  NSL_COMPILER_TORNADO        - WindRiver IDE with c/c++ Compiler 
  NSL_COMPILER_GCC            - gcc
  NSL_COMPILER_BORLANDC       - borland c++
  NSL_COMPILER_INTEL          - intel c++
  NSL_COMPILER_CODE_WARRIOR   - CodeWarrior
  NSL_COMPILER_MSVC           - Microsoft Visual C++
  NSL_COMPILER_MSVC$x$        - Microsoft Visual C++ x version
common:
  NSL_IMPL_LINK_COMPILER_NAME - string name of compiler/IDE
*/
/* Determines the compiler on which nsl toolkit is compiled. */
#ifdef NSL_OS_VXWORKS
    #define NSL_IMPL_LINK_COMPILER_NAME             "Tornado"
    #define NSL_COMPILER_TORNADO
#elif defined(__GNUC__)
    #define NSL_IMPL_LINK_COMPILER_NAME             "gcc"
    #define NSL_COMPILER_GCC
    /* Determines the version of gcc that is used to compile this. */
    /* 40302 means version 4.3.2. */
    #define NSL_GCC_VER_ \
    (__GNUC__*10000 + __GNUC_MINOR__*100 + __GNUC_PATCHLEVEL__)
#elif defined(__BORLANDC__)
    #define NSL_COMPILER_BORLANDC
    #if __BORLANDC__ == 0x0550
        #define NSL_IMPL_LINK_COMPILER_NAME         "bc55"
    #elif (__BORLANDC__ == 0x0551)
        #define NSL_IMPL_LINK_COMPILER_NAME         "bc551"
    #elif (__BORLANDC__ == 0x0560)
        #define NSL_IMPL_LINK_COMPILER_NAME         "bc56"
    #elif (__BORLANDC__ == 0x0564)
        #define NSL_IMPL_LINK_COMPILER_NAME         "bc564"
    #elif (__BORLANDC__ == 0x0582)
        #define NSL_IMPL_LINK_COMPILER_NAME         "bc582"
    #elif (0x0590 == (__BORLANDC__ & 0xfff0))
        #define NSL_IMPL_LINK_COMPILER_NAME         "bc59x"
    #elif (0x0610 == (__BORLANDC__ & 0xfff0))
        #define NSL_IMPL_LINK_COMPILER_NAME         "bc61x"
    #else /* ? __BORLANDC__ */
        #error "Unrecognised value of __BORLANDC__"
    #endif /* __BORLANDC__ */
/*
#elif defined(__DMC__)
    #define NSL_IMPL_LINK_COMPILER_NAME          "dm"
*/
#elif defined(__INTEL_COMPILER) || defined (__ICL)
    #define NSL_COMPILER_INTEL
    #if (__INTEL_COMPILER == 600)
        #define NSL_IMPL_LINK_COMPILER_NAME         "icl6"
    #elif (__INTEL_COMPILER == 700)
        #define NSL_IMPL_LINK_COMPILER_NAME         "icl7"
    #elif (__INTEL_COMPILER == 800)
        #define NSL_IMPL_LINK_COMPILER_NAME         "icl8"
    #elif (__INTEL_COMPILER == 900)
        #define NSL_IMPL_LINK_COMPILER_NAME         "icl9"
    #elif (__INTEL_COMPILER == 1000)
        #define NSL_IMPL_LINK_COMPILER_NAME         "icl10"
    #elif (__INTEL_COMPILER == 1100)
        #define NSL_IMPL_LINK_COMPILER_NAME         "icl11"
    #else /* ? __INTEL_COMPILER */
        #error "Unrecognised value of __INTEL_COMPILER"
    #endif /* __INTEL_COMPILER */
#elif defined(__MWERKS__)
    #define NSL_COMPILER_CODE_WARRIOR
    #if ((__MWERKS__ & 0xFF00) == 0x2400)
        #define NSL_IMPL_LINK_COMPILER_NAME         "cw7"
    #elif ((__MWERKS__ & 0xFF00) == 0x3000)
        #define NSL_IMPL_LINK_COMPILER_NAME         "cw8"
    #elif ((__MWERKS__ & 0xFF00) == 0x3200)
        #define NSL_IMPL_LINK_COMPILER_NAME         "cw9"
    #else /* ? __MWERKS__ */
        #error "CodeWarrior version not supported"
    #endif /* __MWERKS__ */
#elif defined(_MSC_VER)
    #define NSL_COMPILER_MSVC
    #if _MSC_VER == 1000
        #define NSL_IMPL_LINK_COMPILER_NAME         "vc4"
        #define NSL_COMPILER_MSVC4
    #elif _MSC_VER == 1020
        #define NSL_IMPL_LINK_COMPILER_NAME         "vc42"
        #define NSL_COMPILER_MSVC42
    #elif _MSC_VER == 1100
        #define NSL_IMPL_LINK_COMPILER_NAME         "vc5"
        #define NSL_COMPILER_MSVC5
    #elif _MSC_VER == 1200
        #define NSL_IMPL_LINK_COMPILER_NAME         "vc6"
        #define NSL_COMPILER_MSVC6
    #elif _MSC_VER == 1300
        #define NSL_IMPL_LINK_COMPILER_NAME         "vc7"
        #define NSL_COMPILER_MSVC7
    #elif _MSC_VER == 1310
        #define NSL_IMPL_LINK_COMPILER_NAME         "vc71"
        #define NSL_COMPILER_MSVC71
    #elif _MSC_VER == 1400
        #define NSL_IMPL_LINK_COMPILER_NAME         "vc8"
        #define NSL_COMPILER_MSVC8
    #elif _MSC_VER == 1500
        #define NSL_IMPL_LINK_COMPILER_NAME         "vc9"
        #define NSL_COMPILER_MSVC9
    #elif _MSC_VER == 1600
        #define NSL_IMPL_LINK_COMPILER_NAME         "vc10"
        #define NSL_COMPILER_MSVC10
    #else /* ? _MSC_VER */
        #error "Unrecognised value of _MSC_VER"
    #endif /* _MSC_VER */
#else /* ? compiler */
    #error "Unrecognised compiler"
#endif /* compiler */

#endif /* NSL_INCLUDE_INTERNAL_NSL_PORT_H_ */
