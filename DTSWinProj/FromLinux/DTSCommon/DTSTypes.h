///////////////////////////////////////////////////////////////////////////////////////////
// 采用nsl_port.h对操作系统的定义，参照BISPTypes.h
#ifndef DTS_TYPES_H_INCLUDED
#define DTS_TYPES_H_INCLUDED

#ifdef NSL_INCLUDE_BISP_BISP_TYPE_ABANDON_H_
#error
#endif /* NSL_INCLUDE_BISP_BISP_TYPE_ABANDON_H_ */

#define DTS_DEFAULT_STR_LEN		16

//////////////////////////////////////////////////////////////////////////
//NSL_OS_LINUX
#define BISPASSERT(f) assert(f)


#ifdef NSL_OS_LINUX

	#include <stdint.h>
//#ifndef BOOL
	typedef unsigned char    BOOL;
//	typedef int32_t          BOOL;
//#endif
	typedef long         LPARAM;
	typedef int16_t      INT16;
	typedef int32_t      INT32;
	typedef uint16_t     UINT16;
	typedef uint16_t     WORD;
	typedef uint16_t     USHORT;
	typedef uint32_t     UINT;
	typedef uint32_t     UINT32;
	typedef uint32_t	 ULONG;
	typedef uint32_t     DWORD;
	typedef int32_t      INT;
	typedef uint64_t     UINT64;
	
	typedef WORD		*LPWORD;
	typedef DWORD		*LPDWORD;
	typedef uint8_t      UINT8;
	typedef uint8_t      BYTE;
	typedef uint8_t      *LPBYTE;
	typedef char         *LPSTR;
	typedef const char  *LPCSTR;
	typedef UINT		WPARAM;
	typedef char         *va_list;
	typedef UINT8		 UCHAR;                  /* unsigned 8 bit value */
	typedef char		 CHAR; 
	typedef char		 TCHAR; 

//#ifndef BISPASSERT
//	#define BISPASSERT(exp) ((void)0)
//#endif

	#ifndef STATUS
	#define STATUS int
	#endif /* STATUS */

	#ifndef LOCAL
	#define LOCAL static
	#endif /* LOCAL */

	#ifndef BISP_INLINE
	#define BISP_INLINE inline
	#endif /* BISP_INLINE */

	#ifndef BISP_REGISTER
	#define BISP_REGISTER volatile
	#endif /* BISP_REGISTER */

	#ifndef BISP_EXTERN
	#define BISP_EXTERN extern
	#endif /* BISP_EXTERN */

	#ifndef __max
	#define __max(a,b)  (((a) > (b)) ? (a) : (b))
	#endif /* __max */

	#ifndef __min
	#define __min(a,b)  (((a) < (b)) ? (a) : (b))
	#define min __min
	#endif /* __min */

	#ifndef offsetof
	#define offsetof(s,m)   ( (size_t)&((((s) *)0)->(m)) )
	#endif

	#ifndef WINAPI
	#define WINAPI
	#endif

	#ifdef TRUE
	#undef TRUE
	#endif

	#ifdef FALSE
	#undef FALSE
	#endif

	#ifndef TRUE
	#define TRUE                1
	#endif

	#ifndef FALSE
	#define FALSE               0
	#endif

#endif //NSL_OS_LINUX



//////////////////////////////////////////////////////////////////////////
//NSL_OS_WINDOWS
#ifdef NSL_OS_WINDOWS
	#ifndef LOCAL
	#define LOCAL static
	#endif /* LOCAL */
	typedef BYTE	uint8_t;
	typedef WORD	uint16_t;
	typedef DWORD	uint32_t;
	typedef UINT64	uint64_t;
	typedef const char  *LPCSTR;
	
#endif	//NSL_OS_WINDOWS



//////////////////////////////////////////////////////////////////////////
//NSL_OS_VXWORKS
#ifdef NSL_OS_VXWORKS
	#include "types/vxTypesOld.h"
	#include "vxWorks.h"
	#ifdef m_flags
	#undef m_flags
	#endif /* m_flags */
	/* md3 - added to override SENS macro, net/radix.h */
	#ifdef Free
	#undef Free
	#endif /* Free */
#endif	//NSL_OS_VXWORKS





#endif //DTS_TYPES_H_INCLUDED
