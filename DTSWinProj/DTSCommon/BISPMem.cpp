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

//�Ƿ����ڴ�й©?
#if defined(NSL_OS_WINDOWS) && defined(_DEBUG)
	//ע�⣺ֻ����Windows�µ����ڴ�й©��ʱ��Ŵ����¿���!!!
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

//�����ڴ������
HFASTMALLOC g_hFastMalloc = NULL;
//# error "g_hFastMalloc"

//���ü���
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

//��ʼ��BISP�ڴ��
BOOL BISPMemInit(void)
{
	memAddRef();
	
	if(NULL != g_hFastMalloc)
	{
		return TRUE;
	}

	//�������ٷ����ڴ�ṹ
	g_hFastMalloc = FastMallocCreate(DEFAULT_SIZEOF_ALLOCBLOCK);

	return (NULL != g_hFastMalloc);
}


//����ʼ��BISP�ڴ��
BOOL BISPMemDeinit(void)
{
	if(NULL == g_hFastMalloc)
	{
		return FALSE;
	}
	//�ж����ü���
	if(0 != memRelease() )
	{
		return TRUE;
	}
	
	FastMallocDestroy(g_hFastMalloc);

	g_hFastMalloc = NULL;

	return TRUE;
}

//�Ƿ��Ѿ���ʼ�����?
BOOL BISPMemIsInit(void)
{
	return NULL != g_hFastMalloc;
}

//�ڴ�ʹ�����ͳ�ƴ�ӡ
int BISPMallocPrint(char* pBuffer,int nBufferSize)
{
	return FastMallocPrint(g_hFastMalloc,pBuffer,nBufferSize);
}

//���û�ж�����������,��ʵ��һ���汾
#ifndef INLINE_MEM_FUNCTIONS


//����BISP�ڴ�
void* BISPMemMalloc(size_t size)
{

#ifdef WIN32_CHECK_MEM_LEAK

	//����windows��malloc ������BoundsCheck����ڴ�й©
	return malloc(size);

#else 

	return FastMalloc(g_hFastMalloc,size);

#endif //WIN32_CHECK_MEM_LEAK

}

//�ͷ�BISP�ڴ�
void BISPMemFree(void* pData)
{
#ifdef WIN32_CHECK_MEM_LEAK

	//����windows��free ����BoundsCheck����ڴ�й©
	free(pData);

#else
	
	FastFree(g_hFastMalloc,pData);

#endif //WIN32_CHECK_MEM_LEAK

}

//�ڴ�����
void* BISPMemZero(void* pData,size_t size)
{
	return memset(pData,0,size);
}

//�ڴ濽��
void* BISPMemCpy(void* pDest,const void* pSrc,size_t size)
{
	return memcpy(pDest,pSrc,size);
}

//�ڴ��ƶ�
void* BISPMemMove(void* pDest,const void* pSrc,size_t size)
{
	return memmove(pDest,pSrc,size);
}


//�ڴ�Ƚ�
int BISPMemCmp(const void* pComp1,const void* pComp2,size_t size)
{
	return memcmp(pComp1,pComp2,size);
}

//�ڴ�����
int BISPMemSet(void* pDest,int nValue,size_t size)
{
	return (int)memset(pDest,nValue,size);
}

#endif //INLINE_MEM_FUNCTIONS



