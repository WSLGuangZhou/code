#ifndef PACK_END_H_INCLUDED
#define PACK_END_H_INCLUDED

#ifdef NSL_OS_WINDOWS
#pragma pack(pop)
#elif defined (NSL_OS_VXWORKS)
#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif	/* CPU_FAMILY==I960 */
#endif /* NSL_OS_WINDOWS */

#undef PACK_BEGIN_H_INCLUDED

#endif	//PACK_END_H_INCLUDED
