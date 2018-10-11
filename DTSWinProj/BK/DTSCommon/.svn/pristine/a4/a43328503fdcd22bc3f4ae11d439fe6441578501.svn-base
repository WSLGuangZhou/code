// BISPMem.cpp: implementation of the BISPMem class.
//
//////////////////////////////////////////////////////////////////////
#include "DTSCommon.h"
//#include "stdafx.h"
#include "BISPMem.h"
#include "FastMalloc.h"

#include "BISPModLink.h"
BISP_DECLARE_MODULE_LINK(BISP_MEM)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//是否检查内存泄漏?
#if defined(NSL_OS_WINDOWS) && defined(_DEBUG)
	//注意：只有在Windows下调试内存泄漏的时候才打开以下开关!!!
	//#define WIN32_CHECK_MEM_LEAK
#endif //WIN32 && _DEBUG

#if defined(NSL_OS_WINDOWS) || defined(NSL_OS_LINUX)
#ifndef intLock
	#define intLock() 0L
	#define intUnlock(nOldLevel) ((nOldLevel) = 0)
#endif
#else //vxworks
	#include "intLib.h"
#endif //WIN32

//快速内存分配句柄
HFASTMALLOC g_hFastMalloc = NULL;
//# error "g_hFastMalloc"

//引用计数
static	int	g_nMallocRefCount = 0;

static inline 
int memAddRef(void)
{
	int nRet;

	int nOldLevel = intLock();
	nRet = ++g_nMallocRefCount;
	intUnlock(nOldLevel);

	return nRet;
}

static inline 
int memRelease(void)
{
	int nRet;

	int nOldLevel = intLock();
	nRet = --g_nMallocRefCount;
	intUnlock(nOldLevel);

	return nRet;
}

//初始化BISP内存池
BOOL BISPMemInit(void)
{
	memAddRef();
	
	if(NULL != g_hFastMalloc)
	{
		return TRUE;
	}

	//创建快速分配内存结构
	g_hFastMalloc = FastMallocCreate(DEFAULT_SIZEOF_ALLOCBLOCK);

	return (NULL != g_hFastMalloc);
}


//反初始化BISP内存池
BOOL BISPMemDeinit(void)
{
	if(NULL == g_hFastMalloc)
	{
		return FALSE;
	}
	//判断引用计数
	if(0 != memRelease() )
	{
		return TRUE;
	}
	
	FastMallocDestroy(g_hFastMalloc);

	g_hFastMalloc = NULL;

	return TRUE;
}

//是否已经初始化完毕?
BOOL BISPMemIsInit(void)
{
	return NULL != g_hFastMalloc;
}

//内存使用情况统计打印
int BISPMallocPrint(char* pBuffer,int nBufferSize)
{
	return FastMallocPrint(g_hFastMalloc,pBuffer,nBufferSize);
}

//如果没有定义内联函数,则实现一个版本
#ifndef INLINE_MEM_FUNCTIONS


//分配BISP内存
void* BISPMemMalloc(size_t size)
{

#ifdef WIN32_CHECK_MEM_LEAK

	//调用windows的malloc 可以用BoundsCheck检测内存泄漏
	return malloc(size);

#else 

	return FastMalloc(g_hFastMalloc,size);

#endif //WIN32_CHECK_MEM_LEAK

}

//释放BISP内存
void BISPMemFree(void* pData)
{
#ifdef WIN32_CHECK_MEM_LEAK

	//调用windows的free 可以BoundsCheck检测内存泄漏
	free(pData);

#else
	
	FastFree(g_hFastMalloc,pData);

#endif //WIN32_CHECK_MEM_LEAK

}

//内存清零
void* BISPMemZero(void* pData,size_t size)
{
	return memset(pData,0,size);
}

//内存拷贝
void* BISPMemCpy(void* pDest,const void* pSrc,size_t size)
{
	return memcpy(pDest,pSrc,size);
}

//内存移动
void* BISPMemMove(void* pDest,const void* pSrc,size_t size)
{
	return memmove(pDest,pSrc,size);
}


//内存比较
int BISPMemCmp(const void* pComp1,const void* pComp2,size_t size)
{
	return memcmp(pComp1,pComp2,size);
}

//内存设置
int BISPMemSet(void* pDest,int nValue,size_t size)
{
	return (int)memset(pDest,nValue,size);
}

#endif //INLINE_MEM_FUNCTIONS



