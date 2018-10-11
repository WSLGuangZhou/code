///////////////////////////////////////////////////////////////////////////////////////////
// Broadband Integrated Switching Platform(BISP) Basic Component
// Copyright(c) 2000-2008 Network Communications Company, 
//						  Haige Communications Group Incorporated Company.
// All rights reserved.
//
// This file is a part of the Broadband Integrated Switching Platform(BISP).
// The code and information is provided "as-is" without
// warranty of any kind, either expressed or implied.
///////////////////////////////////////////////////////////////////////////////////////////
//连接模块定义: BISP_MEM

//内存管理模块定义
#ifndef __BISPMEM_H_INCLUDED
#define __BISPMEM_H_INCLUDED

#define INLINE_MEM_FUNCTIONS

// #if (BISPOS_TYPE == BISPOS_LINUX)
// 	#define BISP_CONFIG_NO_BISP_MEM
// #endif

#ifdef BISP_CONFIG_NO_BISP_MEM

	#define BISPMemInit()                ((void)0)
	#define BISPMemDeinit()              ((void)0)
	#define BISPMemMalloc                malloc
	#define BISPMemFree                  free
	#define BISPMemCpy                   memcpy
	#define BISPMemZero(pData,size)      memset((pData),0,(size))
	#define BISPMemSet(pDest,c,size)     memset((pDest),(c),(size))
	#define BISPMemMove(pDest,pSrc,size) memmove((pDest),(pSrc),(size))
	#define BISPMemIsInit()				 TRUE				

#else

	#include "FastMalloc.h"
//	#include "BISPDebug.h"


// 	//定义内联函数
// 	#if (BISPOS_TYPE == BISPOS_VXWORKS)
// 		#define INLINE_MEM_FUNCTIONS
// 	#endif


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

//初始化BISP内存池
BOOL BISPMemInit(void);

//反初始化BISP内存池
BOOL BISPMemDeinit(void);

//是否已经初始化完毕?
BOOL BISPMemIsInit(void);

//内存使用情况统计打印
int BISPMallocPrint(char* pBuffer,int nBufferSize);

//如果定义了内联函数
#ifndef INLINE_MEM_FUNCTIONS

	//分配BISP内存
	void* BISPMemMalloc(size_t size);
	//释放BISP内存
	void BISPMemFree(void* pData);
	//内存清零
	void* BISPMemZero(void* pData,size_t size);
	//内存拷贝
	void* BISPMemCpy(void* pDest,const void* pSrc,size_t size);
	//内存移动
	void* BISPMemMove(void* pDest,const void* pSrc,size_t size);
	//内存比较
	int BISPMemCmp(const void* pComp1,const void* pComp2,size_t size);
	//内存设置
	int BISPMemSet(void* pDest,int nValue,size_t size);


#endif //INLINE_MEM_FUNCTIONS

//快速内存分配句柄
extern HFASTMALLOC g_hFastMalloc;

#ifdef __cplusplus
};
#endif //__cplusplus


//如果定义了内联函数
#ifdef INLINE_MEM_FUNCTIONS
//# error "INLINE_MEM_FUNCTIONS"

#define BISPMemMalloc(size)	FastMalloc(g_hFastMalloc,size)
#define BISPMemFree(pData)	FastFree(g_hFastMalloc,pData)
#define BISPMemMove(pDest,pSrc,size)	memmove((pDest),(pSrc),(size))


#ifdef NSL_OS_VXWORKS //Vxworks

//直接引用函数
#include <string.h>
#ifdef _EXTENSION_WRS

//采用bcopy，bzero系列函数
#define USE_BYTE_FUNCTIONS

/********************************
extern void 	bcopy (const char *source, char *dest, int nbytes);
extern void 	bfill (char *buf, int nbytes, int ch);
extern void 	bzero (char *buffer, int nbytes);
extern int 	bcmp (char *buf1, char *buf2, int nbytes);
**************************************/

#endif //_EXTENSION_WRS

#endif //NSL_OS_VXWORKS


//采用bcopy系列函数
#ifdef USE_BYTE_FUNCTIONS

#define BISPMemZero(pData,size)		bzero((char*)(pData),(int)(size))
#define BISPMemCpy(pDest,pSrc,size)	bcopy((const char*)(pSrc),(char*)(pDest),(int)(size))
#define BISPMemCmp(pComp1,pComp2,size) bcmp((char*)(pComp1),(char*)(pComp2),(int)(size))
#define BISPMemSet(pDest,c,size) bfill((char*)(pDest),(int)(size),(int)(c))

#else //采用memmove系列函数

#define BISPMemZero(pData,size)		memset((pData),0,(size))
#define BISPMemCpy(pDest,pSrc,size)	memcpy((pDest),(pSrc),(size))
#define BISPMemCmp(pComp1,pComp2,size) memcmp((pComp1),(pComp2),(size))
#define BISPMemSet(pDest,c,size) memset((pDest),(c),(size))

#endif //USE_BYTE_FUNCTIONS

#endif //INLINE_MEM_FUNCTIONS


// DECLARE_BISP_MALLOC -- used in class definition
#define DECLARE_BISP_MALLOC(class_name) \
public: \
	void* operator new(size_t size) \
	{ \
		return BISPMemMalloc(size); \
	} \
	void* operator new(size_t, void* p) \
		{ return p; } \
	void operator delete(void* p) { BISPMemFree(p); }


#endif /* BISP_CONFIG_NO_BISP_MEM */

#endif // __BISPMEM_H_INCLUDED


