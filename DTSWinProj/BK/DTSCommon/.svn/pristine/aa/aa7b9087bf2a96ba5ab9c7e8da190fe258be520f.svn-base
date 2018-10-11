/*
 * File: include/nsl/internal/nsl_platform.h
 *
 * Defines of the include file in platforms.
 *
 * Copyright (C) 2010 sl.cai(noirscsl@163.com). All rights reserved.
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
 * 04/17/2013,      1.0a, sl.cai   updata order of vxworks system include
 * 02/20/2014,      1.0b, sl.cai   add include for linux system
 * 
 */

#ifndef NSL_INCLUDE_INTERNAL_NSL_PLATFORM_H_
#define NSL_INCLUDE_INTERNAL_NSL_PLATFORM_H_

// #ifndef NSL_INCLUDE_NSL_H_
// # error "nsl_platform.h should only be included from nsl.h"
// #endif /* NSL_INCLUDE_NSL_H_ */

#ifdef NSL_OS_WINDOWS

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN  /* We always want minimal includes */
	#endif

	#include <windows.h>
	#include <io.h>              /* because we so often use open/close/etc */
	#include <process.h>         /* for _getpid */
	#include <winsock2.h>
	#include <assert.h>
	#include <WinSock2.h>
	#include <windows.h>
	#include <stddef.h>
	#include <stdio.h>
	#include <WS2TCPIP.H>
	#include <stdio.h>
	#include <stdlib.h>
	#include <Iphlpapi.h>
	#include <Iprtrmib.h>
	#include <atlbase.h>
	#include <time.h>
	#include <assert.h>

	#ifdef NSL_COMPILER_MSVC
	#pragma comment(lib,"WS2_32.lib")
	#pragma warning(disable:4100)//disable warning "unreferenced formal parameter"
	#pragma warning(disable:4819)
	#endif /* NSL_COMPILER_MSVC */

	#ifdef NSL_COMPILER_MSVC10
	#pragma warning(disable:4996)
	#endif /* NSL_COMPILER_MSVC10 */

#elif defined(NSL_OS_VXWORKS)

	#include <vxworks.h>
	#include <ioLib.h>
	#include <sysLib.h>
	#include <tickLib.h>
	#include <taskLib.h>

	#ifndef ROUTER_STACK
	#define ROUTER_STACK
	#endif /* ROUTER_STACK */

	#include <hostLib.h>
	#include <sockLib.h>
	#include <socket.h>
	#include <inetLib.h>
	#include <in.h>
	#include <net/inet.h>
	#include <selectLib.h>
	#include <time.h>

	#include <nsl/core/nsl_libc.h>

	#ifdef _WRS_VXWORKS_5_X
	#define NSL_PLATFORM_NO_VSPRINTF_FUNC
	#endif /* _WRS_VXWORKS_5_X */

	/* we are using VxWorks #define _BYTE_ORDER here */
	#ifndef _BYTE_ORDER
	#error "_BYTE_ORDER no defined"
	#else
	#if (_BYTE_ORDER == _BIG_ENDIAN)
	#define NSL_BIG_ENDIAN
	#elif defined (_BYTE_ORDER == _LITTLE_ENDIAN)
	#define NSL_LITTLE_ENDIAN
	#else
	#error "byte order is not defined"
	#endif /* (_BYTE_ORDER == _BIG_ENDIAN) */
	#endif /* _BYTE_ORDER */

#elif defined(NSL_OS_LINUX) || defined(NSL_OS_CYGWIN)

	#include <assert.h>
	#include <stddef.h>
	#include <errno.h>
	#include <unistd.h>
	#include <stdint.h>
	#include<string.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <pthread.h>
	#include <semaphore.h>

	#ifdef NSL_OS_LINUX
//	#include <linux/if.h>
	#endif /* NSL_OS_LINUX */

	#include <netinet/in.h>
	#include <netinet/ip.h>
	#include <arpa/inet.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <errno.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <sys/ioctl.h>
	#include <netdb.h>
	#include <net/if.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <stdarg.h>
	#include <assert.h>

#endif /* NSL_OS_WINDOWS */

#include <stdarg.h>          /* for va_list */
#include <stdio.h>           /* need this to override stdio's (v)snprintf */
#include <stdlib.h>
#include <string.h>

#ifdef NSL_POSIX_SUPPORT
#include <pthread.h>        /* posix threads */
#include <semaphore.h>      /* posix semaphores */
#include <mqueue.h>         /* posix message queue */
#endif /* NSL_POSIX_SUPPORT */

#endif /* NSL_INCLUDE_INTERNAL_NSL_PLATFORM_H_ */
