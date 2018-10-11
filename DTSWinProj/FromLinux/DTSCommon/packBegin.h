#ifndef PACK_BEGIN_H_INCLUDED
#define PACK_BEGIN_H_INCLUDED


#undef PACK_STRUCT


#ifdef NSL_OS_WINDOWS

 #undef _LITTLE_ENDIAN
 #undef _BIG_ENDIAN
 #undef _PDP_ENDIAN
 #undef _BYTE_ORDER

 #pragma pack(push,1)
 #define _LITTLE_ENDIAN   1234    /* least-significant byte first (vax) */
 #define _BIG_ENDIAN      4321    /* most-significant byte first (IBM, net) */
 #define _PDP_ENDIAN      3412    /* LSB first in word, MSW first in long (pdp) */
 #define _BYTE_ORDER _LITTLE_ENDIAN
 #define PACK_STRUCT							//结构体1字节对其打包定义

#elif defined (NSL_OS_VXWORKS)

// vxworks 结构体1字节对其打包定义
 #if ((CPU_FAMILY==I960) && (defined __GNUC__))
  #pragma align 1                 /* tell gcc960 not to optimize alignments */
 #endif	/* CPU_FAMILY==I960 */

 #ifdef _WRS_VXWORKS_5_X
  #if ((defined __arm__) || (defined __sh__))
   #define PACK_STRUCT _WRS_PACK_ALIGN(1)
  #else
   #define PACK_STRUCT __attribute__((packed))
//   #define PACK_STRUCT
  #endif /* __arm__ */
 #else
  #if (((defined __arm__) || (defined __sh__)) && (defined __GNUC__))
   #define PACK_STRUCT __attribute__((packed))
  #else
   #define PACK_STRUCT __attribute__((packed))
  // #define PACK_STRUCT
  #endif /* __arm__ */
 #endif /* _WRS_VXWORKS_5_X */

#ifndef _BYTE_ORDER
/*
 * Definitions for byte order,
 * according to byte significance from low address to high.
 */
#define _LITTLE_ENDIAN   1234    /* least-significant byte first (vax) */
#define _BIG_ENDIAN      4321    /* most-significant byte first (IBM, net) */
#define _PDP_ENDIAN      3412    /* LSB first in word, MSW first in long (pdp) */
#ifdef vax
#define _BYTE_ORDER      _LITTLE_ENDIAN
#else
#define _BYTE_ORDER      _BIG_ENDIAN      /* mc68000, tahoe, most others */
#endif
#endif	/* _BYTE_ORDER */

#elif defined (NSL_OS_LINUX)

	#define PACK_STRUCT __attribute__((packed))
	
	#undef _LITTLE_ENDIAN
	#undef _BIG_ENDIAN
	#undef _PDP_ENDIAN
	#undef _BYTE_ORDER

	#define _LITTLE_ENDIAN   1234    /* least-significant byte first (vax) */
	#define _BIG_ENDIAN      4321    /* most-significant byte first (IBM, net) */
	#define _PDP_ENDIAN      3412    /* LSB first in word, MSW first in long (pdp) */
	#ifdef LITTLEEND
		#define _BYTE_ORDER _LITTLE_ENDIAN
		//#error "_LITTLE_ENDIAN"
	#else
		#define _BYTE_ORDER _BIG_ENDIAN
		//#error "_BIG_ENDIAN"
	#endif

#endif /* NSL_OS_WINDOWS */


#undef PACK_END_H_INCLUDED


#endif	//PACK_BEGIN_H_INCLUDED
