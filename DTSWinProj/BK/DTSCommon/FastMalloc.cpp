// FastMalloc.cpp: implementation of the CFastMalloc class.
//
///////////////////////////////////////////////////////////////
//	文 件 名 : FastMalloc.cpp
//	文件功能 : 
//	作    者 : 谭 北 华
//	创建时间 : 2005年7月8日
//	项目名称 : HNCEigrp
//	备    注 :
//	历史记录 : 2007.03.08 增加对分配内存的精细化尺寸管理。可以进一步避免内存的浪费
///////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "DTSCommon.h"
#include "FastMalloc.h"
//#include "BispDebug.h"
//#include "BISPCommonDebug.h"
#include "BISPModLink.h"
BISP_DECLARE_MODULE_LINK(BISP_FASTMALLOC)


#ifdef NSL_OS_WINDOWS

	#define FM_CRISEC CRITICAL_SECTION
	#define FMCrisec_Init(crisec) ::InitializeCriticalSection(&(crisec))
	#define FMCrisec_Deinit(crisec) ::DeleteCriticalSection(&(crisec))
	#define FMCrisec_Lock(crisec) ::EnterCriticalSection(&(crisec))
	#define FMCrisec_Unlock(crisec) ::LeaveCriticalSection(&(crisec))

	//Windows平台的intlock定义
	static FM_CRISEC g_csLockDrv;
	static int	g_RefCount = 0;

	//Windows 平台用一个临界区来模拟intLock

	#define FM_DrvMemInit() \
		if(0 == g_RefCount ++) \
			FMCrisec_Init(g_csLockDrv)

	#define FM_DrvMemDeinit() \
		if(0 == --g_RefCount) \
			FMCrisec_Deinit(g_csLockDrv)
	
	#define DrvLock() FMCrisec_Lock(g_csLockDrv)
	#define DrvUnlock() FMCrisec_Unlock(g_csLockDrv)

#elif defined(NSL_OS_LINUX)
	#include "BISPUtil.h"
	#define FM_CRISEC BISP_CRISEC
	#define FMCrisec_Init(crisec) BISPCrisec_Init(crisec) 
	#define FMCrisec_Deinit(crisec) BISPCrisec_Deinit(crisec)
	#define FMCrisec_Lock(crisec) BISPCrisec_Lock(crisec)
	#define FMCrisec_Unlock(crisec) BISPCrisec_Unlock(crisec)
	//Linux平台的intlock定义
	static FM_CRISEC g_csLockDrv;
	static int	g_RefCount = 0;

	//Linux 平台用一个临界区来模拟intLock

	#define FM_DrvMemInit() \
		if(0 == g_RefCount ++) \
		FMCrisec_Init(g_csLockDrv)

	#define FM_DrvMemDeinit() \
		if(0 == --g_RefCount) \
		FMCrisec_Deinit(g_csLockDrv)

	#define DrvLock() FMCrisec_Lock(g_csLockDrv)
	#define DrvUnlock() FMCrisec_Unlock(g_csLockDrv)

#elif defined(NSL_OS_VXWORKS)

	#include "semLib.h"
	#include "intLib.h"
	#include "taskLib.h"

	#define FM_CRISEC SEM_ID
	#define FMCrisec_Init(crisec) ((crisec) = semMCreate(SEM_Q_PRIORITY|SEM_INVERSION_SAFE))
	#define FMCrisec_Deinit(crisec) semDelete(crisec)
	#define FMCrisec_Lock(crisec) semTake(crisec,WAIT_FOREVER)
	#define FMCrisec_Unlock(crisec) semGive(crisec)

	//Vxworks 平台直接调用intLock和taskLock

	#define FM_DrvMemInit() (void)0

	#define FM_DrvMemDeinit() (void)0

	#define DrvLock() \
		int nOldLevel = intLock(); \
		if(!INT_CONTEXT())	taskLock()

	#define DrvUnlock() \
		intUnlock(nOldLevel); \
		if(!INT_CONTEXT())	taskUnlock()


#endif //WIN32


#ifndef LOCAL
#define LOCAL static
#endif //LOCAL

#define MALLOC_INLINE inline
#define MALLOC_REGISTER register

#define SYSTEM_MALLOC(size) malloc(size)
#define SYSTEM_FREE(p) free(p)

typedef unsigned int MALLOC_UINT;

#define ROUND(x,y) (((x)+(y-1))&~(y-1))
#define ROUND4(x) ROUND(x, 4)

#ifdef FASTMALLOC_OPTIMIZ_SIZE
//返回计算的索引号 (size + 31)/32 - 1
#define MAKE_INDEX(size) (((size_t)(size + 31)>>5) - 1)
#define MAX_INDEX 15L
#else
//返回计算的索引号 (size + 63)/64 - 1
#define MAKE_INDEX(size) (((size_t)(size + 63)>>6) - 1)
#define MAX_INDEX 7L
#endif //FASTMALLOC_OPTIMIZ_SIZE

#define FAST_PLEX_DATA(pBlock) ((char*)(pBlock) + sizeof(FASTPLEX))

//一个固定的内存块
typedef struct tagFastPlex
{
	tagFastPlex*	pNext;
}FASTPLEX,*LPFASTPLEX;

//一块小块内存
typedef struct tagFastNode
{
	tagFastNode*	pNext;   // only valid when in free list
}FASTNODE,*LPFASTNODE;

//固定内存结构
typedef struct tagFastAlloc
{
	MALLOC_UINT		nAllocSize;		// size of each block from Alloc
	MALLOC_UINT		nBlockCount;	// number of blocks to get at a time
	LPFASTPLEX		pBlocks;		// linked list of blocks (is nBlocks*nAllocSize)
	LPFASTNODE		pNodeFree;		// first free node (NULL if no free nodes)
	int				nAllocCount;	//分配的个数
}FASTALLOC,*LPFASTALLOC;

//快速分配内存结构
typedef struct tagFastMalloc
{
	FM_CRISEC		crisecLock;			//临界区
#ifdef FASTMALLOC_OPTIMIZ_SIZE
	FASTALLOC		memAlloc32;			//32字节分配优化
	FASTALLOC		memAlloc64;			//64字节分配优化
	FASTALLOC		memAlloc96;			//96字节分配优化
	FASTALLOC		memAlloc128;		//128字节分配优化
	FASTALLOC		memAlloc192;		//192字节分配优化
	FASTALLOC		memAlloc256;		//256字节分配优化
	FASTALLOC		memAlloc384;		//384字节分配优化
	FASTALLOC		memAlloc512;		//512字节分配优化
	LPFASTALLOC		pMemAllocs[16];		//Malloc Index table
#else
	FASTALLOC		memAlloc64;			//64字节分配优化
	FASTALLOC		memAlloc128;		//128字节分配优化
	FASTALLOC		memAlloc256;		//256字节分配优化
	FASTALLOC		memAlloc512;		//512字节分配优化
	LPFASTALLOC		pMemAllocs[8];		//Malloc Index table
#endif //FASTMALLOC_OPTIMIZ_SIZE
	size_t			nSysAllocSize;		//系统分配的内存大小	
}FASTMALLOC,*LPFASTMALLOC;

//固定内存分配结构
typedef struct tagFixedMalloc
{
	FASTALLOC		memAlloc;			//固定字节分配优化
	FM_CRISEC		crisecLock;			//临界区	
}FIXEDMALLOC,*LPFIXEDMALLOC;

//驱动内存分配结构
typedef struct tagDrvMalloc
{
	void*			pDrvMem;			//驱动内存(可以是外指定或自动分配)
	size_t			nMemSize;			//内存大小
	MALLOC_UINT		nAllocSize;			// size of each block from Alloc
	MALLOC_UINT		nBlockCount;		// 总共可分配的个数
	LPFASTNODE		pNodeFree;			// first free node (NULL if no free nodes)
	int				nAllocCount;		//分配的个数
	BOOL			bAutoMalloc;		//是否自动分配?
}DRVMALLOC,*LPDRVMALLOC;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LOCAL MALLOC_INLINE 
LPFASTPLEX _CreatePlex(MALLOC_REGISTER LPFASTPLEX* ppHead,
										  MALLOC_UINT nMax,
										  MALLOC_UINT cbElement)
{
	LPFASTPLEX pPlex;
	pPlex = (LPFASTPLEX)SYSTEM_MALLOC(sizeof(FASTPLEX) + nMax * cbElement);
	if(NULL != pPlex)
	{
		pPlex->pNext = *ppHead;
		*ppHead = pPlex;  // change head (adds in reverse order for simplicity)
	}
	return pPlex;
}

LOCAL MALLOC_INLINE 
void _FreePlexChain(MALLOC_REGISTER LPFASTPLEX pPlex)
{
	LPFASTPLEX pNext;
	while (pPlex != NULL)
	{
		pNext = pPlex->pNext;
		SYSTEM_FREE((void*)pPlex);
		pPlex = pNext;
	}	
}

LOCAL MALLOC_INLINE 
void _InitFastAlloc(MALLOC_REGISTER LPFASTALLOC pAlloc,
									   MALLOC_UINT nAllocSize, 
									   MALLOC_UINT nBlockCount)
{
	pAlloc->nAllocSize = nAllocSize;
	pAlloc->nBlockCount = nBlockCount;
	pAlloc->pNodeFree = NULL;
	pAlloc->pBlocks = NULL;
}

LOCAL MALLOC_INLINE 
void _DeinitFastAlloc(MALLOC_REGISTER LPFASTALLOC pAlloc)
{

//在调试状态打印内存泄漏消息
#ifdef _DEBUG
	
	if(pAlloc->nAllocCount > 0 )
	{
		char szMsg[128];
		sprintf(szMsg,"---FastAlloc Mem leak!: AllocSize : %d ,AllocCount: %d --\n",
					pAlloc->nAllocSize,pAlloc->nAllocCount);
#ifdef WIN32
		OutputDebugString((LPCWSTR)szMsg);
#else //Vxworks
		printf(szMsg);
#endif //WIN32
	}

#endif //_DEBUG
	_FreePlexChain(pAlloc->pBlocks);
	pAlloc->pBlocks = NULL;
	pAlloc->pNodeFree = NULL;	
}

LOCAL MALLOC_INLINE 
void* _FastAllocData(MALLOC_REGISTER LPFASTALLOC pAlloc)
{
	MALLOC_REGISTER LPFASTNODE pTempNode;
	void* pData;
	LPFASTPLEX pNewBlock;
	LPFASTNODE pNode;
	int i;
	if (NULL == pAlloc->pNodeFree)
	{
		// add another block
		pNewBlock = _CreatePlex(&pAlloc->pBlocks,pAlloc->nAllocSize,pAlloc->nBlockCount);
		if(NULL != pNewBlock)
		{
			// chain them into free list
			pAlloc->pNodeFree = (LPFASTNODE)FAST_PLEX_DATA(pNewBlock);
			// free in reverse order to make it easier to debug
			pNode = (LPFASTNODE)( (char*)pAlloc->pNodeFree + (pAlloc->nAllocSize * (pAlloc->nBlockCount -1 )));
			pTempNode = NULL;
			for (i = (int)pAlloc->nBlockCount - 1; i >= 0; i-- )
			{
				pNode->pNext = pTempNode;
				pTempNode = pNode;
				pNode = (LPFASTNODE)( (char*)pNode - pAlloc->nAllocSize);
			}
		}
	}

	// remove the first available node from the free list
	pData = (void*)pAlloc->pNodeFree;

	//是否分配成功?
	if(NULL != pData)
	{
		pAlloc->pNodeFree = ((LPFASTNODE)pData)->pNext;

		//统计分配的个数
		pAlloc->nAllocCount ++;
	}

	return pData;	
}

LOCAL MALLOC_INLINE 
void _FastFreeData(MALLOC_REGISTER LPFASTALLOC pAlloc,
						void* pData)
{
	// free chunk of memory returned from _FastAlloc
	LPFASTNODE pNode;
	if (NULL != pData)
	{
		// simply return the node to the free list
		pNode = (LPFASTNODE)pData;
		pNode->pNext = pAlloc->pNodeFree;
		pAlloc->pNodeFree = pNode;
		//统计分配的个数
		pAlloc->nAllocCount --;
	}
}

/////////////////////////////////////////////////////////////////////////////
HFASTMALLOC FastMallocCreate(int nSizeOfAllocBlock)
{
	LPFASTMALLOC pMalloc;
	
	pMalloc = (LPFASTMALLOC)SYSTEM_MALLOC(sizeof(FASTMALLOC) + 10);

// 	BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_MEMERY)
// 		("[MEMERY] FastMallocCreate ,(%p)\r\n",pMalloc);

	if(nSizeOfAllocBlock < 1024)	//最小1K大小
	{
		nSizeOfAllocBlock = 1024L;
	}
	//整合为K的倍数
	nSizeOfAllocBlock = (nSizeOfAllocBlock + 1023)/1024;
	//转换为KB
	nSizeOfAllocBlock *= 1024;

	if(NULL != pMalloc)
	{
		//内存清0
		memset(pMalloc,0,sizeof(FASTMALLOC));

		//初始化临界区
		FMCrisec_Init(pMalloc->crisecLock);
// 		BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_FLOW)
// 			("FMCrisec_Init in FastMallocCreate (%p)\r\n",pMalloc->crisecLock);
		//初始化固定内存分配池

		//初始化某一个指针
#define _InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,size) \
		_InitFastAlloc(&(pMalloc)->memAlloc##size,\
		ROUND4((size)+sizeof(size_t)),\
		(size_t)(nSizeOfAllocBlock)/(size) )

#ifdef FASTMALLOC_OPTIMIZ_SIZE
		_InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,32);
		_InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,64);
		_InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,96);
		_InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,128);
		_InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,192);
		_InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,256);
		_InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,384);
		_InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,512);
		//初始化查找表
		pMalloc->pMemAllocs[0] = &pMalloc->memAlloc32;		// 0 - 32
		pMalloc->pMemAllocs[1] = &pMalloc->memAlloc64;		// 33 - 64
		pMalloc->pMemAllocs[2] = &pMalloc->memAlloc96;		// 65 - 96
		pMalloc->pMemAllocs[3] = &pMalloc->memAlloc128;		// 97 - 128
		pMalloc->pMemAllocs[4] = &pMalloc->memAlloc192;		// 129 - 160
		pMalloc->pMemAllocs[5] = &pMalloc->memAlloc192;		// 161 - 192
		pMalloc->pMemAllocs[6] = &pMalloc->memAlloc256;		// 193 - 224
		pMalloc->pMemAllocs[7] = &pMalloc->memAlloc256;		// 225 - 256
		pMalloc->pMemAllocs[8] = &pMalloc->memAlloc384;		// 257 - 288
		pMalloc->pMemAllocs[9] = &pMalloc->memAlloc384;		// 289 - 320
		pMalloc->pMemAllocs[10] = &pMalloc->memAlloc384;	// 321 - 352
		pMalloc->pMemAllocs[11] = &pMalloc->memAlloc384;	// 353 - 384
		pMalloc->pMemAllocs[12] = &pMalloc->memAlloc512;	// 385 - 416
		pMalloc->pMemAllocs[13] = &pMalloc->memAlloc512;	// 417 - 448
		pMalloc->pMemAllocs[14] = &pMalloc->memAlloc512;	// 449 - 480
		pMalloc->pMemAllocs[15] = &pMalloc->memAlloc512;	// 481 - 512
#else
		_InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,64);
		_InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,128);
		_InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,256);
		_InitFastAllocPointer(pMalloc,nSizeOfAllocBlock,512);
		//初始化查找表
		pMalloc->pMemAllocs[0] = &pMalloc->memAlloc64;		// 0 - 64
		pMalloc->pMemAllocs[1] = &pMalloc->memAlloc128;		// 65 - 128
		pMalloc->pMemAllocs[2] = &pMalloc->memAlloc256;		// 129 - 192
		pMalloc->pMemAllocs[3] = &pMalloc->memAlloc256;		// 193 - 256
		pMalloc->pMemAllocs[4] = &pMalloc->memAlloc512;		// 257 - 320
		pMalloc->pMemAllocs[5] = &pMalloc->memAlloc512;		// 321 - 384
		pMalloc->pMemAllocs[6] = &pMalloc->memAlloc512;		// 385 - 448
		pMalloc->pMemAllocs[7] = &pMalloc->memAlloc512;		// 449 - 512
#endif //FASTMALLOC_OPTIMIZ_SIZE
	}
	return (HFASTMALLOC)pMalloc;
}


BOOL FastMallocDestroy(HFASTMALLOC hFastMalloc)
{
	LPFASTMALLOC pMalloc;
	
	pMalloc = (LPFASTMALLOC)hFastMalloc;
	if(NULL == pMalloc)
	{
		return FALSE;
	}

#ifdef FASTMALLOC_OPTIMIZ_SIZE
	_DeinitFastAlloc(&pMalloc->memAlloc32);
	_DeinitFastAlloc(&pMalloc->memAlloc64);
	_DeinitFastAlloc(&pMalloc->memAlloc96);
	_DeinitFastAlloc(&pMalloc->memAlloc128);
	_DeinitFastAlloc(&pMalloc->memAlloc192);
	_DeinitFastAlloc(&pMalloc->memAlloc256);
	_DeinitFastAlloc(&pMalloc->memAlloc384);
	_DeinitFastAlloc(&pMalloc->memAlloc512);
#else
	_DeinitFastAlloc(&pMalloc->memAlloc64);
	_DeinitFastAlloc(&pMalloc->memAlloc128);
	_DeinitFastAlloc(&pMalloc->memAlloc256);
	_DeinitFastAlloc(&pMalloc->memAlloc512);
#endif //FASTMALLOC_OPTIMIZ_SIZE

	FMCrisec_Deinit(pMalloc->crisecLock);
	
	SYSTEM_FREE(pMalloc);
	return TRUE;
}


void* FastMalloc(HFASTMALLOC hFastMalloc,size_t size)
{

	if(NULL == hFastMalloc)
	{
		return NULL;
	}
//	BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_MEMERY)
//		("[MEMERY] FastMalloc ,(%p) size(%d)\r\n",hFastMalloc,size);

	LPFASTMALLOC pMalloc;
	void* pData;
	int nAllocIndex;

	pMalloc = (LPFASTMALLOC)hFastMalloc;
	
	//malloc(0) 是允许的
	if(0 == size)	//Min size is 1
	{
		size ++;
	}
	//计算索引号
	nAllocIndex = MAKE_INDEX(size);

	if(nAllocIndex <= MAX_INDEX)
	{
		//临界区保护
// 		BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_FLOW)
// 							("FMCrisec_Lock in FastMalloc (%p)\r\n",
// 							pMalloc->crisecLock);
		FMCrisec_Lock(pMalloc->crisecLock);
// 		BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_FLOW)
// 							("FMCrisec_Lock in FastMalloc Succeed, (%p)\r\n",
// 							pMalloc->crisecLock);
		//分配内存
		pData = _FastAllocData(pMalloc->pMemAllocs[nAllocIndex]);
		//临界区解除保护
// 		BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_FLOW)
// 							("FMCrisec_Unlock in FastMalloc (%p)\r\n",
// 							pMalloc->crisecLock);
		FMCrisec_Unlock(pMalloc->crisecLock);	
// 		BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_FLOW)
// 							("FMCrisec_Unlock in FastMalloc Succeed,(%p)\r\n",
// 							pMalloc->crisecLock);
	}
	else
	{
		//统计系统内存分配数量
		pMalloc->nSysAllocSize += size;
		pData = SYSTEM_MALLOC( size + sizeof(size_t) );
	}
	//记录malloc的大小
	if(NULL != pData)
	{
		*(size_t*)pData = size;

		//Warning 672: Possible memory leak in assignment to pointer 'pData'
		/*lint -save -e672 */
		pData = (void*) (((char*)pData) + sizeof(size_t));
		/*lint -restore */

		//调试版本时把内存内容改变为无效内容
#ifdef _DEBUG
		memset(pData,0xcd,size);
#endif //_DEBUG
		
	}
	return  pData;
}


void FastFree(HFASTMALLOC hFastMalloc,void* pData)
{
	LPFASTMALLOC pMalloc;
	size_t size;
	int nAllocIndex;
	
	//Free一个空指针是允许的
	if(NULL == hFastMalloc || NULL == pData)
	{
		return ;
	}
	pMalloc = (LPFASTMALLOC)hFastMalloc;
	pData = (void*) (((char*)pData) - sizeof(size_t));
	size = *(size_t*)pData;

	//调试版本时把内存释放为野指针
#ifdef _DEBUG
	memset((char*)pData + sizeof(size_t),0xcd,size);
#endif //_DEBUG

	//计算索引号
	nAllocIndex = MAKE_INDEX(size);

	if(nAllocIndex <= MAX_INDEX)
	{
		//临界区保护
		FMCrisec_Lock(pMalloc->crisecLock);
		//释放内存
		_FastFreeData(pMalloc->pMemAllocs[nAllocIndex],pData);
		//临界区解除保护
		FMCrisec_Unlock(pMalloc->crisecLock);	
	}
	else
	{
		//统计系统内存分配数量
		pMalloc->nSysAllocSize -= size;
		//采用系统的Free函数
		SYSTEM_FREE(pData);
	}
}

//内存使用情况统计打印
int FastMallocPrint(HFASTMALLOC hFastMalloc,char* pBuffer,int nBufferSize)
{
	if(NULL == hFastMalloc || nBufferSize < 800)
	{
		return 0;
	}
	size_t nMemSize = 0;
	char* pPrintBuf = pBuffer;
	LPFASTMALLOC pMalloc = (LPFASTMALLOC)hFastMalloc;
#ifdef FASTMALLOC_OPTIMIZ_SIZE

	pPrintBuf += sprintf(pPrintBuf,"  Block size 32  Bytes Alloc Count is%5d, Alloc size is%8d Bytes\r\n",
				pMalloc->memAlloc32.nAllocCount,pMalloc->memAlloc32.nAllocCount*32);
	nMemSize += (size_t)(pMalloc->memAlloc32.nAllocCount*32);

	pPrintBuf += sprintf(pPrintBuf,"  Block size 64  Bytes Alloc Count is%5d, Alloc size is%8d Bytes\r\n",
				pMalloc->memAlloc64.nAllocCount,pMalloc->memAlloc64.nAllocCount*64);
	nMemSize += (size_t)(pMalloc->memAlloc64.nAllocCount*64);

	pPrintBuf += sprintf(pPrintBuf,"  Block size 96  Bytes Alloc Count is%5d, Alloc size is%8d Bytes\r\n",
				pMalloc->memAlloc96.nAllocCount,pMalloc->memAlloc96.nAllocCount*96);
	nMemSize += (size_t)(pMalloc->memAlloc96.nAllocCount*96);

	pPrintBuf += sprintf(pPrintBuf,"  Block size 128 Bytes Alloc Count is%5d, Alloc size is%8d Bytes\r\n",
				pMalloc->memAlloc128.nAllocCount,pMalloc->memAlloc128.nAllocCount*128);
	nMemSize += (size_t)(pMalloc->memAlloc128.nAllocCount*128);

	pPrintBuf += sprintf(pPrintBuf,"  Block size 192 Bytes Alloc Count is%5d, Alloc size is%8d Bytes\r\n",
				pMalloc->memAlloc192.nAllocCount,pMalloc->memAlloc192.nAllocCount*192);
	nMemSize += (size_t)(pMalloc->memAlloc192.nAllocCount*192);

	pPrintBuf += sprintf(pPrintBuf,"  Block size 256 Bytes Alloc Count is%5d, Alloc size is%8d Bytes\r\n",
				pMalloc->memAlloc256.nAllocCount,pMalloc->memAlloc256.nAllocCount*256);
	nMemSize += (size_t)(pMalloc->memAlloc256.nAllocCount*256);

	pPrintBuf += sprintf(pPrintBuf,"  Block size 384 Bytes Alloc Count is%5d, Alloc size is%8d Bytes\r\n",
				pMalloc->memAlloc384.nAllocCount,pMalloc->memAlloc384.nAllocCount*384);
	nMemSize += (size_t)(pMalloc->memAlloc384.nAllocCount*384);

	pPrintBuf += sprintf(pPrintBuf,"  Block size 512 Bytes Alloc Count is%5d, Alloc size is%8d Bytes\r\n",
				pMalloc->memAlloc512.nAllocCount,pMalloc->memAlloc512.nAllocCount*512);
	nMemSize += (size_t)(pMalloc->memAlloc512.nAllocCount*512);

#else

	pPrintBuf += sprintf(pPrintBuf,"  Block size 64 Bytes Alloc Count is%5d, Alloc size is%8d Bytes\r\n",
				pMalloc->memAlloc64.nAllocCount,pMalloc->memAlloc64.nAllocCount*64);
	nMemSize += (size_t)(pMalloc->memAlloc64.nAllocCount*64);

	pPrintBuf += sprintf(pPrintBuf,"  Block size 128 Bytes Alloc Count is%5d, Alloc size is%8d Bytes\r\n",
				pMalloc->memAlloc128.nAllocCount,pMalloc->memAlloc128.nAllocCount*128);
	nMemSize += (size_t)(pMalloc->memAlloc128.nAllocCount*128);

	pPrintBuf += sprintf(pPrintBuf,"  Block size 256 Bytes Alloc Count is%5d, Alloc size is%8d Bytes\r\n",
				pMalloc->memAlloc256.nAllocCount,pMalloc->memAlloc256.nAllocCount*256);
	nMemSize += (size_t)(pMalloc->memAlloc256.nAllocCount*256);

	pPrintBuf += sprintf(pPrintBuf,"  Block size 512 Bytes Alloc Count is%5d, Alloc size is%8d Bytes\r\n",
				pMalloc->memAlloc512.nAllocCount,pMalloc->memAlloc512.nAllocCount*512);
	nMemSize += (size_t)(pMalloc->memAlloc512.nAllocCount*512);

#endif //FASTMALLOC_OPTIMIZ_SIZE	

	pPrintBuf += sprintf(pPrintBuf,"  Block size more than 512 Bytes Alloc by system is %8d Bytes\r\n",
				pMalloc->nSysAllocSize);

	nMemSize += (size_t)pMalloc->nSysAllocSize;
	
	pPrintBuf += sprintf(pPrintBuf,"Fast Malloc Total Memory used %d bytes (%d K bytes)\r\n",
					nMemSize,nMemSize/1024);
	
	return (int)(pPrintBuf - pBuffer);
}

/////////////////////////////////////////////////////////////////
//固定内存块分配，比FastMalloc更快

//创建固定内存分配结构
HFIXEDMALLOC FixedMallocCreate(int nFixedSize,int nSizeOfAllocBlock)
{
	LPFIXEDMALLOC pMalloc;
	
	pMalloc = (LPFIXEDMALLOC)SYSTEM_MALLOC(sizeof(FIXEDMALLOC) + 10);

// 	BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_MEMERY)
// 		("[MEMERY] FixedMallocCreate ,(%p)\r\n",pMalloc);

	if(nFixedSize < 4)
	{
		nFixedSize = 4;
	}
	if(nSizeOfAllocBlock < nFixedSize)
	{
		nSizeOfAllocBlock = nFixedSize;
	}
	if(nSizeOfAllocBlock < 1024)	//最小1K大小
	{
		nSizeOfAllocBlock = 1024L;
	}
	//整合为K的倍数
	nSizeOfAllocBlock = (nSizeOfAllocBlock + 1023)/1024;
	//转换为KB
	nSizeOfAllocBlock *= 1024;

	UINT nRoundSize = ROUND4(nFixedSize);
	if(NULL != pMalloc)
	{
		//内存清0
		memset(pMalloc,0,sizeof(FIXEDMALLOC));

		//初始化临界区
		FMCrisec_Init(pMalloc->crisecLock);
// 		BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_FLOW)
// 			("FMCrisec_Init in FixedMallocCreate (%p)\r\n",pMalloc->crisecLock);
		//初始化固定内存分配池
		_InitFastAlloc(&pMalloc->memAlloc,nRoundSize,(UINT)nSizeOfAllocBlock/nRoundSize);
	}
	return (HFIXEDMALLOC)pMalloc;
	
}

//销毁固定内存分配结构
BOOL FixedMallocDestroy(HFIXEDMALLOC hFixedMalloc)
{
	LPFIXEDMALLOC pMalloc = (LPFIXEDMALLOC)hFixedMalloc;
	if(NULL == pMalloc)
	{
		return FALSE;
	}
	_DeinitFastAlloc(&pMalloc->memAlloc);
	FMCrisec_Deinit(pMalloc->crisecLock);
	
	SYSTEM_FREE(pMalloc);
	return TRUE;

}

//分配固定内存
void* FixedMalloc(HFIXEDMALLOC hFixedMalloc)
{
	if(NULL == hFixedMalloc)
	{
		return NULL;
	}
// 	BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_MEMERY)
// 		("[MEMERY] FixedMalloc ,(%p)\r\n",hFixedMalloc);

	void* pData;
	LPFIXEDMALLOC pMalloc = (LPFIXEDMALLOC)hFixedMalloc;
	//临界区保护
	FMCrisec_Lock(pMalloc->crisecLock);
// 	BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_FLOW)
// 		("FMCrisec_Lock in FixedMalloc (%p)\r\n",pMalloc->crisecLock);
	//分配内存
	pData = _FastAllocData(&pMalloc->memAlloc);
	//临界区解除保护
// 	BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_FLOW)
// 		("FMCrisec_Unlock in FixedMallocCreate (%p)\r\n",pMalloc->crisecLock);
	FMCrisec_Unlock(pMalloc->crisecLock);	
// 	BISP_COMMON_DEBUG_PRINT(BISP_COMMON_DEBUG_FLOW)
// 		("FMCrisec_Unlock in FixedMallocCreate Succeed ,(%p)\r\n",pMalloc->crisecLock);
	return pData;
}

//释放固定内存
void FixedFree(HFIXEDMALLOC hFixedMalloc,void* pData)
{
	//Free一个空指针是允许的
	if(NULL == hFixedMalloc || NULL == pData)
	{
		return ;
	}

	LPFIXEDMALLOC pMalloc = (LPFIXEDMALLOC)hFixedMalloc;
	//临界区保护
	FMCrisec_Lock(pMalloc->crisecLock);
	//分配内存
	_FastFreeData(&pMalloc->memAlloc,pData);
	//临界区解除保护
	FMCrisec_Unlock(pMalloc->crisecLock);	
}


//内存使用情况统计打印
int FixedMallocPrint(HFIXEDMALLOC hFixedMalloc,char* pBuffer,int nBufferSize)
{
	if(NULL == hFixedMalloc || nBufferSize < 256)
	{
		return 0;
	}
	char* pPrintBuf = pBuffer;
	LPFIXEDMALLOC pMalloc = (LPFIXEDMALLOC)hFixedMalloc;

	pPrintBuf += sprintf(pPrintBuf,"  Fiexd Block size %d Alloc Count is%8d,Alloc size is%8d\r\n",
				pMalloc->memAlloc.nAllocCount,pMalloc->memAlloc.nAllocCount,pMalloc->memAlloc.nAllocSize);

	return (int)(pPrintBuf - pBuffer);	
}


///////////////////////////////////////////////////////////////
//驱动层内存分配，适用于中断缓冲


//创建驱动内存分配结构
HDRVMALLOC	DrvMemMallocCreate(int nAllocSize,void* pDrvMem,int nMemSize)
{
	int i;
	LPDRVMALLOC pMalloc;
	LPFASTNODE pNode;
	LPFASTNODE pTempNode;
	UINT	nMemAddr;

	if(nAllocSize < (int)sizeof(LPFASTNODE) )
	{
		nAllocSize  =  DEFAULT_DRVMEM_ALLOC_SIZE;
	}

	//nAllocSize 4字节对齐	
	if(nAllocSize & 0x03)
	{
		nAllocSize &= 0x00FFFFFC;
		nAllocSize += 4;
	}

	if(nMemSize <= nAllocSize)
	{
		return NULL;
	}

	BOOL bAutoMalloc = FALSE;

	//是否要求自动分配内存
	if(NULL == pDrvMem)
	{
		pDrvMem = (void*)SYSTEM_MALLOC((size_t)nMemSize);
		bAutoMalloc = TRUE;
		
	}

	if(NULL == pDrvMem )
	{
		return NULL;
	}

	//内存 4字节对齐	
	nMemAddr = (UINT)pDrvMem;
	if( nMemAddr & 0x03)
	{
		nMemAddr &= 0xFFFFFFFC;
		nMemAddr += 4;
		nMemSize -= 4;
		pDrvMem = (void*)nMemAddr;
	}


	pMalloc = (LPDRVMALLOC)SYSTEM_MALLOC(sizeof(DRVMALLOC) + 10);
	if(NULL == pMalloc)
	{
		return NULL;
	}	
		
	//填充信息
	pMalloc->pDrvMem = pDrvMem;
	pMalloc->nMemSize = (size_t)nMemSize;		//内存大小
	pMalloc->nAllocSize = (MALLOC_UINT)nAllocSize;		// size of each block from Alloc
	pMalloc->bAutoMalloc = bAutoMalloc;		//是否自动分配

	//计算nBlockCount个数
	pMalloc->nBlockCount = (MALLOC_UINT)(nMemSize/nAllocSize);

	// chain them into free list
	pMalloc->pNodeFree = (LPFASTNODE)pDrvMem;

	// free in reverse order to make it easier to debug
	pNode = (LPFASTNODE)( (char*)pMalloc->pNodeFree + ((UINT)nAllocSize * (pMalloc->nBlockCount -1 )));
	pTempNode = NULL;
	for (i = (int)pMalloc->nBlockCount - 1; i >= 0; i-- )
	{
		pNode->pNext = pTempNode;
		pTempNode = pNode;
		pNode = (LPFASTNODE)( (char*)pNode - nAllocSize);
	}

	pMalloc->nAllocCount = 0;		//分配的个数
	
	//初始化驱动内存临界区
	FM_DrvMemInit();

	return (HDRVMALLOC)pMalloc;
}

//销毁驱动内存分配结构
BOOL DrvMemMallocDestroy(HDRVMALLOC hDrvMalloc)
{
	LPDRVMALLOC pMalloc = (LPDRVMALLOC)hDrvMalloc;
	if(NULL == pMalloc)
	{
		return FALSE;
	}

	//释放自动分配的内存
	if(pMalloc->bAutoMalloc)
	{
		SYSTEM_FREE(pMalloc->pDrvMem);
	}
	
	SYSTEM_FREE(pMalloc);
	
	//反初始化驱动内存临界区
	FM_DrvMemDeinit();

	return TRUE;
}

//分配驱动内存
void* DrvMemMalloc(HDRVMALLOC hDrvMalloc)
{
	void* pData;

	LPDRVMALLOC pMalloc = (LPDRVMALLOC)hDrvMalloc;

	if(NULL == pMalloc)
	{
		return NULL;
	}

	//进入驱动临界区
	DrvLock();

	// remove the first available node from the free list
	pData = (void*)pMalloc->pNodeFree;

	//是否分配成功?
	if(NULL != pData)
	{
		pMalloc->pNodeFree = ((LPFASTNODE)pData)->pNext;

		//统计分配的个数
		pMalloc->nAllocCount ++;
	}
	
	//退出驱动临界区
	DrvUnlock();

	return pData;
}

//释放驱动内存
void DrvMemFree(HDRVMALLOC hDrvMalloc,void* pData)
{
	LPFASTNODE pNode;
	LPDRVMALLOC pMalloc = (LPDRVMALLOC)hDrvMalloc;

	if(NULL == pMalloc)
	{
		return;
	}

	assert((UINT)pData >= (UINT)pMalloc->pDrvMem &&
			(UINT)pData < (UINT)pMalloc->pDrvMem + pMalloc->nMemSize);

	//进行内存合法性检查
	if((UINT)pData < (UINT)pMalloc->pDrvMem ||
				(UINT)pData >= (UINT)pMalloc->pDrvMem + pMalloc->nMemSize)
	{
		return;
	}

	//进入驱动临界区
	DrvLock();
	
	// free chunk of memory returned from LPDRVMALLOC
	if (NULL != pData)
	{
		// simply return the node to the free list
		pNode = (LPFASTNODE)pData;
		pNode->pNext = pMalloc->pNodeFree;
		pMalloc->pNodeFree = pNode;
		//统计分配的个数
		pMalloc->nAllocCount --;
	}

	//退出驱动临界区
	DrvUnlock();

}

//内存使用情况统计打印
int DrvMemMallocPrint(HDRVMALLOC hDrvMalloc,char* pBuffer,int nBufferSize)
{
	if(NULL == hDrvMalloc || nBufferSize < 256)
	{
		return 0;
	}

	char* pPrintBuf = pBuffer;
	LPDRVMALLOC pMalloc = (LPDRVMALLOC)hDrvMalloc;

	pPrintBuf += sprintf(pPrintBuf,"  Drv Mem(%p) size %d ,Block size is %d ,Alloc Count is%8d,Alloc size is%8d\r\n",
				pMalloc->pDrvMem,pMalloc->nMemSize,
				pMalloc->nAllocSize,pMalloc->nAllocCount,
				pMalloc->nAllocSize*(UINT)pMalloc->nAllocCount);

	return (int)(pPrintBuf - pBuffer);	

}
