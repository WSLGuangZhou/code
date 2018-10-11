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
//����ģ�鶨��: BISP_MEM

//�ڴ����ģ�鶨��
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


// 	//������������
// 	#if (BISPOS_TYPE == BISPOS_VXWORKS)
// 		#define INLINE_MEM_FUNCTIONS
// 	#endif


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

//��ʼ��BISP�ڴ��
BOOL BISPMemInit(void);

//����ʼ��BISP�ڴ��
BOOL BISPMemDeinit(void);

//�Ƿ��Ѿ���ʼ�����?
BOOL BISPMemIsInit(void);

//�ڴ�ʹ�����ͳ�ƴ�ӡ
int BISPMallocPrint(char* pBuffer,int nBufferSize);

//�����������������
#ifndef INLINE_MEM_FUNCTIONS

	//����BISP�ڴ�
	void* BISPMemMalloc(size_t size);
	//�ͷ�BISP�ڴ�
	void BISPMemFree(void* pData);
	//�ڴ�����
	void* BISPMemZero(void* pData,size_t size);
	//�ڴ濽��
	void* BISPMemCpy(void* pDest,const void* pSrc,size_t size);
	//�ڴ��ƶ�
	void* BISPMemMove(void* pDest,const void* pSrc,size_t size);
	//�ڴ�Ƚ�
	int BISPMemCmp(const void* pComp1,const void* pComp2,size_t size);
	//�ڴ�����
	int BISPMemSet(void* pDest,int nValue,size_t size);


#endif //INLINE_MEM_FUNCTIONS

//�����ڴ������
extern HFASTMALLOC g_hFastMalloc;

#ifdef __cplusplus
};
#endif //__cplusplus


//�����������������
#ifdef INLINE_MEM_FUNCTIONS
//# error "INLINE_MEM_FUNCTIONS"

#define BISPMemMalloc(size)	FastMalloc(g_hFastMalloc,size)
#define BISPMemFree(pData)	FastFree(g_hFastMalloc,pData)
#define BISPMemMove(pDest,pSrc,size)	memmove((pDest),(pSrc),(size))


#ifdef NSL_OS_VXWORKS //Vxworks

//ֱ�����ú���
#include <string.h>
#ifdef _EXTENSION_WRS

//����bcopy��bzeroϵ�к���
#define USE_BYTE_FUNCTIONS

/********************************
extern void 	bcopy (const char *source, char *dest, int nbytes);
extern void 	bfill (char *buf, int nbytes, int ch);
extern void 	bzero (char *buffer, int nbytes);
extern int 	bcmp (char *buf1, char *buf2, int nbytes);
**************************************/

#endif //_EXTENSION_WRS

#endif //NSL_OS_VXWORKS


//����bcopyϵ�к���
#ifdef USE_BYTE_FUNCTIONS

#define BISPMemZero(pData,size)		bzero((char*)(pData),(int)(size))
#define BISPMemCpy(pDest,pSrc,size)	bcopy((const char*)(pSrc),(char*)(pDest),(int)(size))
#define BISPMemCmp(pComp1,pComp2,size) bcmp((char*)(pComp1),(char*)(pComp2),(int)(size))
#define BISPMemSet(pDest,c,size) bfill((char*)(pDest),(int)(size),(int)(c))

#else //����memmoveϵ�к���

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


