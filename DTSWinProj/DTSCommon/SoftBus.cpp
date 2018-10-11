///////////////////////////////////////////////////////////////
//	�� �� �� : SoftBus.cpp
//	�ļ����� : BISP�����ܺ����������ʵ��(V3)
//	��    �� : tbh
//	����ʱ�� : 2005��4��
//	��Ŀ���� : 
//	��    ע : ��ǰ�汾 V3.2.20080707
//	��ʷ��¼ : 
//			V2�汾����V1�汾�ĸĽ�:
//			   2005.04.15 ����ԭ�еĻ���STL��Ϊ����ṹ��������߼��붨ʱ�������¸�д
//			   2005.04.20 ��������ͨ��
//			   2005.07.28 ��һ�������Ż�����GSBFast_Lock����GSBCrisec_Lock
//			   2005.08.24 �����˶�ʱ���ж���ظ�Ͷ��ͬһ��������Ϣ��Bug,
//							��GSB_KillTimer�����л������߽ڵ��ڴ�
//			   2006.01.05 ����GSB_KillAllModuleTimer����������ɾ��ģ��ʱ����
//							GSB_KillAllModuleTimer����������ģ��ɾ����ʱ���������õ�Bug
//			   2006.01.06 ����GSB_GetSystemMsgEvent�������ṩ��Windowsϵͳ����MsgWaitForMutipleObjects��֧��
//			   2006.01.18 ����ͨ��ģ��Ŀ��IDΪ0xFFFFFFFF��Bug
//			   2006.04.03 ��SendMessage���û�WaitJobAdd������,��GSBMSG�еĳ�Ա����nTimerId(��ʱ�����)
//							��Ϊ����ֵָ���ָ�봫�ݷ���ֵ,����SendMessage���û�WaitJobAdd��������ʱ�򷵻�ֵ���0��Bug
//			   2006.04.24 ���Ӷ�0��ʱ���Ĵ���������GSB_SetTimer��GSB_SetTimerByID�����еĲ���UINT uElapse,
//							��uElapse == 0ʱ�������ò���ʧ��,�����൱������һ����������ɾ����JobAdd�Ĺ���
//				2006.10.26 �޸�#include "HNCDebug.h"Ϊ#include "BISPDebug.h",ʹ֮����BISPƽ̨��Ŀ
//			V3�汾��ʷ��¼:
//				2007.01.31 ������gsbWaitForMessage�����п��ܻᶪ��һ����Ϣ�Ĵ���
//				2007.04.14 ����vxworks ringLib.c������gsbRngBufGet��gsbRngBufPut����
//							ȥ���ṹ��1�ֽڶ����PACKET_STRUCT���壬��CPUȥ�Ż��ڴ����
//							�Ľ���gshGetModuleHashIndex������HASH�㷨
//							����GSB_PostQuitMessage������ʹ���ܰ�ȫ�������˳�
//				2007.04.27 ������GSB_PostQuitMessage������Release�汾�����BUG������һ��BUILD_NULL_MSG��
//							���ظ����õ��¼��ӿ���WaitJobAdd��������Ӧ�ٶ�,�μ�USE_FAST_WAIT_EVENT
//				2007.07.28 �������ڴ����Ϊ61-63�ֽ�ʱ����Խ������Ƿ�ָ�������BUG
//				2007.08.23 ��������GSB_Create�����а�MALLOC_BLOCK_SIZE����Ԫ�ظ�����BUG����BUG������ָ��
//							MaxBufferSizeΪ2Kʱ����32M���ڴ棬�Ӷ������ڴ治�������
//							GSB_BuildMessage������ɾ���������Ϣ���ȵļ�飬֧�ֲ����Ƴ��ȴ�С����Ϣ����
//							GSBInfo_AllocMsgPack���������Ӷ��ڴ����ʧ��ʱ���ݴ���
//							��GSB_IntPostMessage��GSB_IntJobAdd������,������ƵĻ����С�����Ż���С����������
//				2007.11.03  ������gsbWaitForMessage�����д����ж���ϢintLockʱ�����������
//				2007.11.08  ����GSB_IntJobAdd��BUG����,��BUG��GSB_IntJobAdd�޷���Ԥ��һ������
//				2007.11.14  ����m_dwWaitTime�����Ժ����vxworks tickΪ��λʱ��һ�µ����⣬�����⵼�¶�ʱ����׼��
//				2007.12.11  ���Ӷ������ȼ�ϵ�к�����֧��������߶������ȼ�
//							����GSB_JobAddFast��GSB_IntJobAddFast����Ͷ�ݺ���֧��IntJobAdd��JobAdd�Ŀ���Ͷ��
//				2007.12.12	����GSB_WaitJobAddFast��GSB_SetTimerFast��ϵ�п���Ͷ�ݺ�����֧�ֿ���Ͷ�ݹ���
//							������gsbWaitForMessage�����д����ж���Ϣ��BUG����BUG�ᵼ���ж���Ϣ������ʱ
//				2007.12.13  ���Ӷ�m_nCurrentTimerID�Ĵ�����֤��ʱ��ID��Ϊ0
//				2008.01.11  ������SetJitterTimerϵ�к�����֧��������ʱ������ʱ��
//				2008.03.17  ����GSB_GetTimerLeftSleep����������ͳ�ƶ�ʱ�����´δ���ʱ��
//				2008.03.26  �޸�GSB_LoopMessage��GSB_PeekMessage�ķ���ֵ��������Timeout��ָʾ
//                           ����GSB_Destroy�˳�ʱ�ͷŽڵ㻺��Ĵ���
//				2008.05.20 ����gsbWaitForMessage��������vxworksƽ̨����ʱ,
//							��û����Ϣ������²��������Զ�ȴ�״̬�����⣬�����˿���ʱ��CPU��Դռ����
//				2008.07.07 �޸�GSB_SetJitterTimerXXϵ�к���,���Ӷ�nJitterTime�Ŀ����жϣ��Ż��˴���
//				2009.04.10 ����semMCreate��SEM_DELETE_SAFE��־λ����������ɾ������
//			    2010.01.15 �޸�GET_TICK_COUNT��صĴ��룬��ʱ����64λ���������ʱ���
//						   TickCount���������Ķ�ʱ����ʱ�䲻����������
//				2010.04.22 �޸�GETTASKID()�궨�壬��Ϊ����ĵ���
//	  SoftBug V2Ϊ���õİ汾
//	  SoftBus V3 �Ż��Ľ�Ҫ��(2006.05.25):
//              1.��ɢ��Ͱ����������е��������Ż�ģ�����
//				2.��Timer�̲߳��е������̣߳����ٶ�ʱ���ĵ��ÿ���
//				3.�������ж��д�����ʱ���ͷ�����Ϣ�����
//				4.Ϊ��Ϣ�Ľڵ�����ڴ������Ż�(��ΪBuffer����Ϊ0,64,��ָ����󳤶ȵ����)
//			   *5.�ֲ�ʽ���������(��Ͽɿ��鲥/�����Ͷ�忨ͨ��),����һ���ٽ���������ڴ���䡢
//					��Ϣ��ӺͶ�ʱ�����еĴ���
//			   *6.�ڴ����¼�ʱʹ��˫��⼼��(����ǰ���һ�ζ��д�С��֮����һ�μ����д�С)
//			    7.֧����ʱ������ʱ��
///////////////////////////////////////////////////////////////
// SoftBus.cpp: implementation of Soft Bus.
//	
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"

#include "DTSCommon.h"
#include "BISPUtil.h"
#include "SoftBus.h"
//#include "BISPDebug.h"
#include <stdlib.h>

//Boolean within 'if' always evaluates to False
/*lint -e774 */

#ifdef NSL_OS_WINDOWS
//#include <process.h>
//���Ծ��棺local variable 'XXX' may be used without having been initialized
#pragma warning(disable:4701)
#elif defined(NSL_OS_VXWORKS)
#include <sysLib.h>
#include <tickLib.h>
#endif //WIN32


#include "BISPModLink.h"
BISP_DECLARE_MODULE_LINK(BISP_SOFTBUS)

/*****************************
#define GSB_DEBUG

#ifdef GSB_DEBUG
#define GSB_DBGLOG(msg,p1,p2,p3,p4,p5,p6) printf(msg,p1,p2,p3,p4,p5,p6)
#else
#define GSB_DBGLOG(msg,p1,p2,p3,p4,p5,p6) 
#endif //GSB_DEBUG
**************************************/

#ifdef NSL_OS_WINDOWS
  //Windows�汾ǿ������
 #define INLINE __forceinline
#else
 #define INLINE inline
#endif

#ifndef LOCAL
 #define LOCAL static
#endif //LOCAL

#define REGISTER register

#define GSB_MALLOC malloc
#define GSB_FREE free


//�Ƿ�ʹ��64λʱ��?
#define GSB_USE_TIME64

#ifdef GSB_USE_TIME64
//64λʱ�����͵Ķ���
typedef UINT64 TICKTYPE;
#else
//32λʱ�����͵Ķ���
typedef DWORD TICKTYPE;
#endif //GSB_USE_TIME64


#define MAX_MODULE (64L)								//���64��ģ��
#define MAX_BUFFER_SIZE (2*1024L)						//���ݵ��������2K
#define GSB_FLAG (DWORD)('G'<<24|'S'<<16|'B'<<8|'F')	//�Ϸ���־

//#define MAX_FREE_GSBMSG_COUNT 200L	//�������200������GSB��Ϣ

//#define MAX_FREE_TIMERMSG_COUNT 400L	//�������400������Timer��Ϣ

//�ȴ����ʱ��
#define LOOP_FOREVER ((UINT)-1)

//�ȴ��Ĵ���
#define MAX_WAIT_TIME LOOP_FOREVER

//���ٵȴ�Event����
#define USE_FAST_WAIT_EVENT

//�������ʹ�õ�Event�ظ���
#define MAX_EVENT_POOL_COUNT 8L

enum BISP_CALLBACK_TYPEMASK
{
	CT_NORMAL			= 0,		//��ͨ�ص�����
	CT_TIMERNORMAL		= 1,		//Timer ��ͨ�ص�����
	CT_BROADCAST		= 2,		//�㲥����
	CT_TIMERBROADCAST	= 3,		//Timer �㲥����
	CT_JOBADD			= 4,		//Add Job����
	CT_FASTJOBADD		= 5,		//Fast Int Job����
	CT_TIMERJOBADD		= 6,		//Timer Add Job����
	CT_FASTTIMERJOBADD	= 7,		//Fast Timer Add Job����
	CT_EXIT				= 8			//�˳���Ϣ
};

//����Ϊƽ̨��ص��ٽ������¼��Ķ���


#ifdef NSL_OS_WINDOWS

	#define GSB_CRISEC CRITICAL_SECTION
	#define GSBCrisec_Init(crisec) ::InitializeCriticalSection(&(crisec))
	#define GSBCrisec_Deinit(crisec) ::DeleteCriticalSection(&(crisec))
	#define GSBCrisec_Lock(crisec) ::EnterCriticalSection(&(crisec))
	#define GSBCrisec_Unlock(crisec) ::LeaveCriticalSection(&(crisec))

	#define GSB_EVENT HANDLE
	#define GSBEvent_Init(hEvent) ((hEvent) = ::CreateEvent(NULL,FALSE,FALSE,NULL))
	#define GSBEvent_Deinit(hEvent) ::CloseHandle(hEvent)
	#define GSBEvent_WaitEvent(hEvent,dwWaitTime) ::WaitForSingleObject(hEvent, dwWaitTime)
	#define GSBEvent_SetEvent(hEvent) ::SetEvent(hEvent)

	#define GETTASKID() ((UINT)GetCurrentThreadId())
	//#define SYS_GETTICKCOUNT() ((UINT)GetTickCount())

	#define GSBFast_Lock(crisec) GSBCrisec_Lock(crisec)
	#define GSBFast_Unlock(crisec) GSBCrisec_Unlock(crisec)

	#define bcopy(src,dest,size) memmove(dest,src,(size_t)(size))
	//#define bzero(buf,size) memset(buf,0,size)

	#define intLock() 0L
	#define intUnlock(nOldLevel) ((nOldLevel) = 0)

#endif //NSL_OS_WINDOWS

#ifdef NSL_OS_LINUX
	#include <pthread.h>
	#include <semaphore.h>

	#define GSB_CRISEC               pthread_mutex_t
	#define GSBCrisec_Init(crisec)   pthread_mutex_init(&(crisec), NULL)
	#define GSBCrisec_Deinit(crisec) pthread_mutex_destroy(&(crisec))
	#define GSBCrisec_Lock(crisec)   pthread_mutex_lock(&(crisec))
	#define GSBCrisec_Unlock(crisec) pthread_mutex_unlock(&(crisec))

	#define WAIT_FOREVER							(UINT(-1))
	#define GSB_EVENT                              sem_t*
	#define GSBEvent_Init(hEvent)                  hEvent=(sem_t*)malloc(sizeof(sem_t)); if(NULL!=hEvent){/*BISPTRACE("sem_t malloc succeed\r\n");*/ sem_init((hEvent), 0, 0);}
	#define GSBEvent_Deinit(hEvent)                sem_destroy(hEvent); if(NULL!=hEvent){/*BISPTRACE("sem_t free \r\n");*/free(hEvent);}
	#define GSBEvent_WaitEvent(hEvent,dwWaitTime)  if(NULL!=hEvent){/*BISPTRACE("sem_t WaitEvent \r\n");*/BISPEventWaitEvent(hEvent,dwWaitTime);}
	#define GSBEvent_SetEvent(hEvent)              if(NULL!=hEvent){/*BISPTRACE("sem_t post \r\n");*/sem_post(hEvent);}

	#define GSBGETTASKID()           (pthread_self())
	#define GETTASKID()				(pthread_self())
//	#define GSBSYS_GETTICKCOUNT()    (BISPGetTickCount())

	#define GSBFast_Lock(crisec) GSBCrisec_Lock(crisec)
	#define GSBFast_Unlock(crisec) GSBCrisec_Unlock(crisec)

	//Linuxƽ̨�ݶ�û��intLock
#ifndef intLock
	#define intLock() 0L
	#define intUnlock(oldlevel) ((oldlevel) = 0)
#endif

#endif //NSL_OS_LINUX



#ifdef NSL_OS_VXWORKS

	#include "semLib.h"
	#include "taskLib.h"
	#include "intLib.h"
	#include "logLib.h"

	//�����̨�����ջ��С
	#define TIMERTASK_STACKSIZE 2048L

	#define GSB_CRISEC SEM_ID
	//���¶���semMCreate��Ϊ�˷�ֹ�����ȼ���ת�����ⷢ��
	#define GSBCrisec_Init(crisec) ((crisec) = semMCreate(SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_DELETE_SAFE))
	//#define GSBCrisec_Init(crisec) ((crisec) = semBCreate(SEM_Q_PRIORITY,SEM_FULL))
	#define GSBCrisec_Deinit(crisec) semDelete(crisec)
	#define GSBCrisec_Lock(crisec) semTake(crisec,WAIT_FOREVER)
	#define GSBCrisec_Unlock(crisec) semGive(crisec)

	#define GSB_EVENT SEM_ID
	#define GSBEvent_Init(hEvent) ((hEvent) = semBCreate(SEM_Q_PRIORITY,SEM_EMPTY))
	#define GSBEvent_Deinit(hEvent) semDelete(hEvent)
	#define GSBEvent_WaitEvent(hEvent,dwWaitTime) semTake(hEvent, (int)(dwWaitTime))
	#define GSBEvent_SetEvent(hEvent) semGive(hEvent)

	//#define GETTASKID() ((UINT)taskIdSelf())
	//��Ϊ����ĵ���2010.04.22
	#define GETTASKID() ((UINT)taskIdCurrent)

	//#define SYS_GETTICKCOUNT() ((UINT)tickGet())

	//������intLock��TASK_LOCK�����������ٶ�
	//#define GSBFast_Lock(crisec) TASK_LOCK()
	//#define GSBFast_Unlock(crisec) TASK_UNLOCK()

	#define GSBFast_Lock(crisec) GSBCrisec_Lock(crisec)
	#define GSBFast_Unlock(crisec) GSBCrisec_Unlock(crisec)

#endif //NSL_OS_VXWORKS




#define GSBMemCpy(src,dest,size) memmove(dest,src,size)
#define GSBMemZero(buf,size) memset(buf,0,size)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//ģ��Hash����
#define MODULE_HASH_LEN 8L

//�Ż��Ļ����С
#define OPTIMIZ_BUFFER_SIZE 64L

//4K Ring Buffer Size
#define RING_BUFFER_SIZE (512*sizeof(GSBMSGINT))

//4K Fixed Malloc MSG Block Size
#define MALLOC_BLOCK_SIZE_MSG (1024*4)

//4K Fixed Malloc 64 Block Size
#define MALLOC_BLOCK_SIZE_64 (1024*4)

//12K Fixed Malloc MaxBuffer Block Size
#define MALLOC_BLOCK_SIZE_MAX (1024*12)

//Բ���ֽ�
#define ROUND(x,y) (((x)+(y-1))&~(y-1))
#define ROUND4(x) ROUND(x, 4)

//һ���̶����ڴ��
typedef struct tagGSBPlex
{
	tagGSBPlex*		pNext;
}GSBPLEX,*LPGSBPLEX;

//һ��С���ڴ�
typedef struct tagGSBMemNode
{
	tagGSBMemNode*	pNext;				 // only valid when in free list
}GSBMEMNODE,*LPGSBMEMNODE;


//�̶��ڴ�ṹ
typedef struct tagGSBAlloc
{
	UINT			nAllocSize;			// size of each block from Alloc
	UINT			nBlockCount;			// number of blocks to get at a time
	LPGSBPLEX		pBlocks;			// linked list of blocks (is nBlocks*nAllocSize)
	LPGSBMEMNODE	pNodeFree;			// first free node (NULL if no free nodes)
}GSBALLOC,*LPGSBALLOC;


#define GSB_PLEX_DATA(pBlock) ((char*)(pBlock) + sizeof(GSBPLEX))


typedef struct tagGSBRingBuf		// RING - ring buffer 
{
	int		pToBuf;		// offset from start of buffer where to write next 
	int		pFromBuf;	// offset from start of buffer where to read next 
	int		bufSize;	// size of ring in bytes 
	char	*buf;		// pointer to start of buffer 
}GSBRINGBUF,*LPGSBRINGBUF;

//////////////////////////////////////////////////////////

//Buffer�����ֽ�
#define GSBBUF_ALIGN_SIZE 4L
//�������Ĵ�С
#define GSBBUF_EXTACT_SIZE (sizeof(GSBBUF) - GSBBUF_ALIGN_SIZE)

//Я���Ļ��嶨��
typedef struct tagGSBBuf
{
	int				nRefCount;						//�������ü���
	BYTE			lpBuffer[GSBBUF_ALIGN_SIZE];	//�������ʵ�ʵĻ���
}GSBBUF,*LPGSBBUF;

//��ǿ�͵���Ϣ�ڵ�����
typedef struct tagGSBMsgEx
{
	tagGSBMsgEx*	pNextMsg;					//��һ������ڵ�(�ڲ�ʹ��)
	tagGSBMsgEx*	pPrevMsg;					//��һ������ڵ�(�ڲ�ʹ��)
	UINT			nModuleSrcID;				//������������ģ��ID,nParam1
	UINT			nModuleDestID;				//Ŀ���������ģ��ID,nParam2
	UINT			nMsg;						//��Ϣ,nParam3
	WPARAM			wParam;						//����1,nParam4
	LPARAM			lParam;						//����2,nParam5
//	DWORD			dwPrivate;					//˽������
	//����Ϊ��ǿ������
	UINT16			nCallbackType;				//�ص�����
	UINT16			nPriority;					//��Ϣ�����ȼ�
	LPONBUSMESSAGE	lpfnCallback;				//�����¼��ص�
	UINT			nIDEvent;					//��ʱ��ID
	TICKTYPE		dwDelayTime;				//��һ�ε���ʱʱ��(��������)
	TICKTYPE		dwNextTime;					//��һ�ε���ʱʱ��(������)
	UINT			nElapse;					//��ʱ���
	UINT			nJitterTime;				//����ʱ��(��������ʱ��)
	UINT			nLoopTimes;					//ѭ������	
	DWORD			dwWaitEvent;				//��Ϣ��ɵĵȴ��¼�,������Ϊ����ֵ����
	//���嶨��:
	int				nBufLen;					//���峤��
	LPGSBBUF		pAttachBuffer;				//����
}GSBMSGEX,*LPGSBMSGEX;

//�ж��͵���Ϣ����
typedef struct tagGSBMsgInt
{
	UINT			nModuleSrcID;				//������������ģ��ID,nParam1
	UINT			nModuleDestID;				//Ŀ���������ģ��ID,nParam2
	UINT			nMsg;						//��Ϣ,nParam3
	WPARAM			wParam;						//����1,nParam4
	LPARAM			lParam;						//����2,nParam5
	//����Ϊ��ǿ������
	UINT16			nCallbackType;				//�ص�����
	UINT16			nBufLen;					//���峤��
	LPONBUSMESSAGE	lpfnCallback;				//�����¼��ص�
}GSBMSGINT,*LPGSBMSGINT;


//ȡ�ý����Ļ���
#define GET_BUFFER(pMsgPack) (void*)((pMsgPack)->pAttachBuffer->lpBuffer)


//�����ȳ����� FIFO List
typedef struct tagGSBList	
{
	int				nCount;						//����
	LPGSBMSGEX		pHeadNode;					//����ͷ
	LPGSBMSGEX		pTailNode;					//����β
}GSBLIST,*LPGSBLIST;

typedef GSBLIST TIMERLIST,*LPTIMERLIST;

/*
//�����ȳ����� FIFO List
typedef struct tagTimerList	
{
	int				nCount;						//����
	LPGSBMSGEX		pHeadNode;					//����ͷ
	LPGSBMSGEX		pTailNode;					//����β
}TIMERLIST,*LPTIMERLIST;
*/

//ע���ģ�鶨��
typedef struct tagModuleInfo
{
	UINT			nModuleID;					//ģ��ID
	LPONBUSMESSAGE	lpfnCallback;				//�ص�����
	DWORD			dwUserData;					//�Զ�������
}MODULEINFO,*LPMODULEINFO;

//����������ݽṹ����
typedef struct tagGSBINFO
{
	DWORD			m_dwFlag;						//�Ϸ���־
	UINT			m_nThreadID;					//�������߳�ID
	DWORD			m_dwMaxModuleCount;				//���ģ�����
	DWORD			m_dwMaxBufferSize;				//������ݻ���(�Ż����ж�ʱʹ��)
	GSBLIST			m_listQueue;					//δ������Ϣ����(�ŶӶ���)
	GSB_CRISEC		m_csLock;						//�����ٽ���
	GSB_EVENT		m_objEvent;						//���յȴ��¼�
	//��ʱ�������ݽṹ
	UINT			m_nCurrentTimerID;				//��ǰ��ʱ��ID
	TIMERLIST		m_listTimer;					//��ʱ������
	TICKTYPE		m_dwLastTickCount;				//�ڵȴ�ms
	TICKTYPE		m_dwWaitTime;					//�ȴ��ĳ�ʱ����ms
#ifdef GSB_USE_TIME64
	TICKTYPE		m_nBaseSysTick;					//����ϵͳʱ��
	UINT			m_nLastSysTick;					//�ϴ�ϵͳʱ��
#endif //GSB_USE_TIME64	  
	//�ж�Ҫ��������ݽṹ:
	int				m_nIntMsgCount;					//�жϴ�����Ϣ����
	GSBRINGBUF		m_RingBuf;						//ѭ������
	//Event��������(����WaitEvent����)
#ifdef USE_FAST_WAIT_EVENT
	UINT			m_nEventCount;					//���õ�Event����
	GSB_EVENT		m_eventWait[MAX_EVENT_POOL_COUNT];	//Event�صĸ���
#endif //USE_FAST_WAIT_EVENT
	//�ڴ��������
	GSBALLOC		m_MemAllocMsg;					//�ڵ�����ڴ����(�̶������С)	
	GSBALLOC		m_MemAlloc64;					//64�ֽڻ����ڴ����(sizeof(GSBBUF) + 64)	
	GSBALLOC		m_MemAllocMax;					//��󻺳�����ڴ�(sizeof(GSBBUF) + MAXBUFSIZE)
	//���ɸ��ҽӵ�ģ����Ϣ
	DWORD			m_dwModuleCount[MODULE_HASH_LEN];//ģ�����
	LPMODULEINFO	m_lpModuleInfo[MODULE_HASH_LEN];//ģ����Ϣ
}GSBINFO,*LPGSBINFO;

#ifdef USE_FAST_WAIT_EVENT

//����һ���ȴ����ź���
#define GSB_EVENT_ALLOC(_theEvent,_pGSBInfo) \
	if((_pGSBInfo)->m_nEventCount > 0 ) { \
			BISPASSERT((_pGSBInfo)->m_nEventCount <= MAX_EVENT_POOL_COUNT); \
			(_pGSBInfo)->m_nEventCount --; \
			(_theEvent) = (_pGSBInfo)->m_eventWait[(_pGSBInfo)->m_nEventCount]; \
	} else 	\
		GSBEvent_Init(_theEvent)

//�ͷ�һ���ȴ����ź���
#define GSB_EVENT_FREE(_theEvent,_pGSBInfo) \
	if((_pGSBInfo)->m_nEventCount < MAX_EVENT_POOL_COUNT) { \
		(_pGSBInfo)->m_eventWait[(_pGSBInfo)->m_nEventCount] = (_theEvent); \
		(_pGSBInfo)->m_nEventCount ++; \
	} else  \
		GSBEvent_Deinit(_theEvent)

#endif //USE_FAST_WAIT_EVENT

//ȫ�ֱ���,�յĻ���
LOCAL GSBBUF g_NullBuffer = {1,0,0,0,0};

//GET_TICK_COUNT�Ķ���

#ifdef NSL_OS_WINDOWS
  #ifdef GSB_USE_TIME64

  LOCAL TICKTYPE GSB_GetTickCount(LPGSBINFO pGSBInfo)
  {
	  ULONG nCurTick = GetTickCount();
	  if(nCurTick < pGSBInfo->m_nLastSysTick)	//���������������
	  {
			pGSBInfo->m_nBaseSysTick += (TICKTYPE)0xFFFFFFFF;	
	  }
	  pGSBInfo->m_nLastSysTick = nCurTick;

	  return (pGSBInfo->m_nBaseSysTick + nCurTick);
  }
  #define GET_TICK_COUNT(pGSBInfo) GSB_GetTickCount(pGSBInfo)

  #else

       #define GET_TICK_COUNT(pGSBInfo) (TICKTYPE)GetTickCount()

  #endif //GSB_USE_TIME64

#elif defined(NSL_OS_VXWORKS)
	LOCAL ULONG g_nSysRate = (ULONG)sysClkRateGet();
	#ifdef GSB_USE_TIME64

	LOCAL TICKTYPE GSB_GetTickCount(LPGSBINFO pGSBInfo)
	{
		ULONG nCurTick = tickGet();
		if(nCurTick < pGSBInfo->m_nLastSysTick)	//���������������
		{
			pGSBInfo->m_nBaseSysTick += (TICKTYPE)0xFFFFFFFF;	
		}
		pGSBInfo->m_nLastSysTick = nCurTick;

		return (pGSBInfo->m_nBaseSysTick + nCurTick)*1000/g_nSysRate;
	}
	#define GET_TICK_COUNT(pGSBInfo) GSB_GetTickCount(pGSBInfo)

	#else

		#define GET_TICK_COUNT(pGSBInfo) ((UINT)tickGet()*1000/g_nSysRate)

	#endif //GSB_USE_TIME64

#elif defined(NSL_OS_LINUX)
	LOCAL ULONG DTSLinuxGetTickCount()
	{
		struct timespec ts={0};
		clock_gettime(CLOCK_MONOTONIC,&ts);
		return (ts.tv_sec*1000 + ts.tv_nsec/1000000);
	}
	#ifdef GSB_USE_TIME64
		LOCAL TICKTYPE GSB_GetTickCount(LPGSBINFO pGSBInfo)
		{
			ULONG nCurTick = DTSLinuxGetTickCount();
			if(nCurTick < pGSBInfo->m_nLastSysTick)	//���������������
			{
				pGSBInfo->m_nBaseSysTick += (TICKTYPE)0xFFFFFFFF;	
			}
			pGSBInfo->m_nLastSysTick = nCurTick;

			return (pGSBInfo->m_nBaseSysTick + nCurTick);
		}
		#define GET_TICK_COUNT(pGSBInfo) GSB_GetTickCount(pGSBInfo)

	#else

		#define GET_TICK_COUNT(pGSBInfo) (TICKTYPE)DTSLinuxGetTickCount()

	#endif //GSB_USE_TIME64



#endif //WIN32

////////////////////////////////////////////////////////////////////////////////
//Mem malloc functions declares:

LOCAL INLINE 
LPGSBPLEX gsbCreatePlex(REGISTER LPGSBPLEX* ppHead,
										  UINT nMax,
										  UINT cbElement);

LOCAL INLINE 
void gsbFreePlexChain(REGISTER LPGSBPLEX pPlex);

LOCAL INLINE 
void gsbInitFastAlloc(REGISTER LPGSBALLOC pAlloc,
									   UINT nAllocSize, 
									   UINT nBlockCount);

LOCAL INLINE 
void gsbDeinitFastAlloc(REGISTER LPGSBALLOC pAlloc);


LOCAL 
void* gsbFastAllocData(REGISTER LPGSBALLOC pAlloc);

LOCAL INLINE 
void gsbFastFreeData(REGISTER LPGSBALLOC pAlloc,
								void* pData);

LOCAL INLINE
BOOL gsbRngCreate(LPGSBRINGBUF ringId,int nbytes);

LOCAL INLINE
void gsbRngDelete(LPGSBRINGBUF ringId);

LOCAL INLINE
void gsbRngFlush(LPGSBRINGBUF ringId);

/*********
LOCAL INLINE
BOOL gsbRngIsEmpty(LPGSBRINGBUF ringId);

LOCAL INLINE
BOOL gsbRngIsFull(LPGSBRINGBUF ringId);
*************/

LOCAL INLINE
int gsbRngFreeBytes(LPGSBRINGBUF ringId);

LOCAL
int gsbRngBufGet(LPGSBRINGBUF ringId,char* buffer,int maxbytes);

LOCAL
int gsbRngBufPut(LPGSBRINGBUF ringId,char*  buffer,int nbytes);

//////////////////////////////////////////////////////////////////////////
//functions:

LOCAL INLINE 
LPGSBPLEX gsbCreatePlex(REGISTER LPGSBPLEX* ppHead,
										  UINT nMax,
										  UINT cbElement)
{
	LPGSBPLEX pPlex;
	pPlex = (LPGSBPLEX)GSB_MALLOC(sizeof(GSBPLEX) + nMax * cbElement);
	if(NULL != pPlex)
	{
		pPlex->pNext = *ppHead;
		*ppHead = pPlex;  // change head (adds in reverse order for simplicity)
	}
	return pPlex;
}

LOCAL INLINE 
void gsbFreePlexChain(REGISTER LPGSBPLEX pPlex)
{
	LPGSBPLEX pNext;
	while (pPlex != NULL)
	{
		pNext = pPlex->pNext;
		GSB_FREE((void*)pPlex);
		pPlex = pNext;
	}	
}

LOCAL INLINE 
void gsbInitFastAlloc(REGISTER LPGSBALLOC pAlloc,
									   UINT nAllocSize, 
									   UINT nBlockCount)
{
	pAlloc->nAllocSize = nAllocSize;
	pAlloc->nBlockCount = nBlockCount;
	pAlloc->pNodeFree = NULL;
	pAlloc->pBlocks = NULL;
}

LOCAL INLINE 
void gsbDeinitFastAlloc(REGISTER LPGSBALLOC pAlloc)
{
	gsbFreePlexChain(pAlloc->pBlocks);
	pAlloc->pBlocks = NULL;
	pAlloc->pNodeFree = NULL;	
}

LOCAL 
void* gsbFastAllocData(REGISTER LPGSBALLOC pAlloc)
{
	REGISTER LPGSBMEMNODE pTempNode;
	void* pData;
	LPGSBPLEX pNewBlock;
	LPGSBMEMNODE pNode;
	int i;
	if (NULL == pAlloc->pNodeFree)
	{
		// add another block
		pNewBlock = gsbCreatePlex(&pAlloc->pBlocks,pAlloc->nAllocSize,pAlloc->nBlockCount);
		if(NULL != pNewBlock)
		{
			// chain them into free list
			pAlloc->pNodeFree = (LPGSBMEMNODE)GSB_PLEX_DATA(pNewBlock);
			// free in reverse order to make it easier to debug
			pNode = (LPGSBMEMNODE)( (char*)pAlloc->pNodeFree + (pAlloc->nAllocSize * (pAlloc->nBlockCount -1 )));
			pTempNode = NULL;
			for (i = (int)pAlloc->nBlockCount - 1; i >= 0; i-- )
			{
				pNode->pNext = pTempNode;
				pTempNode = pNode;
				pNode = (LPGSBMEMNODE)( (char*)pNode - pAlloc->nAllocSize);
			}
		}
	}

	// remove the first available node from the free list
	pData = (void*)pAlloc->pNodeFree;
	if(NULL != pData)
	{
		pAlloc->pNodeFree = ((LPGSBMEMNODE)pData)->pNext;
	}

	return pData;	
}

LOCAL INLINE 
void gsbFastFreeData(REGISTER LPGSBALLOC pAlloc,
								void* pData)
{
	// free chunk of memory returned from _FastAlloc
	LPGSBMEMNODE pNode;
	if (NULL != pData)
	{
		// simply return the node to the free list
		pNode = (LPGSBMEMNODE)pData;
		pNode->pNext = pAlloc->pNodeFree;
		pAlloc->pNodeFree = pNode;
	}
}
///////////////////////////////////////

//����һ�λ���
LOCAL INLINE
LPGSBBUF gsbMallocBuffer(REGISTER LPGSBINFO pGSBInfo,int nBufLen)
{
	LPGSBBUF pBuf;
	if(0 == nBufLen)
	{
		return &g_NullBuffer;
	}
	else if(nBufLen <= OPTIMIZ_BUFFER_SIZE)
	{
		//һ�η���64�ֽ�
		pBuf = (LPGSBBUF)gsbFastAllocData(&pGSBInfo->m_MemAlloc64);
	}
	else if(nBufLen <= (int)(pGSBInfo->m_dwMaxBufferSize) )
	{
		//һ�η�������ֽ�
		pBuf = (LPGSBBUF)gsbFastAllocData(&pGSBInfo->m_MemAllocMax);
	}
	else
	{
		pBuf = (LPGSBBUF)GSB_MALLOC( (sizeof(GSBBUF) - GSBBUF_ALIGN_SIZE) + (size_t)nBufLen);
	}

	//�������ü���
	if(NULL != pBuf)
	{
		pBuf->nRefCount = 1;
	}

	return pBuf;
}

LOCAL INLINE
void gsbFreeBuffer(REGISTER LPGSBINFO pGSBInfo,int nBufLen,LPGSBBUF pBuf)
{
	//ע��g_NullBuffer�ĳ���Ϊ0
	if(0 != nBufLen && NULL != pBuf)
	{
		pBuf->nRefCount --;

		if(0 == pBuf->nRefCount)
		{
			if(nBufLen <= OPTIMIZ_BUFFER_SIZE)
			{
				gsbFastFreeData(&pGSBInfo->m_MemAlloc64,pBuf);
			}
			else if(nBufLen <= (int)(pGSBInfo->m_dwMaxBufferSize) )
			{
				gsbFastFreeData(&pGSBInfo->m_MemAllocMax,pBuf);
			}
			else
			{
				GSB_FREE(pBuf);
			}

		} //if(0 == pBuf->nRefCount)

	} //end if(GBT_NULL != pBuf->nBufType)
}

//����һ�λ���
LOCAL INLINE
LPGSBBUF gsbDuplicateBuffer(REGISTER LPGSBBUF pBuf,int nBufLen)
{
	if(0 != nBufLen)
	{
		pBuf->nRefCount ++;
	}
	return pBuf;
}

//����һ����Ϣ
LOCAL INLINE
LPGSBMSGEX gsbMallocMsg(REGISTER LPGSBINFO pGSBInfo)
{
	return (LPGSBMSGEX)gsbFastAllocData(&pGSBInfo->m_MemAllocMsg);
}

//�ͷ�һ����Ϣ
LOCAL INLINE 
void gsbFreeMsg(REGISTER LPGSBINFO pGSBInfo,LPGSBMSGEX pMsg)
{
	gsbFastFreeData(&pGSBInfo->m_MemAllocMsg,pMsg);
}

////////////////////////////////////////////////////////////
//rng functions:

//gsbRngCreate( ) - create an empty ring buffer
LOCAL INLINE
BOOL gsbRngCreate(LPGSBRINGBUF ringId,int nbytes)
{
	REGISTER char* pBuffer;

	pBuffer = (char*)GSB_MALLOC(size_t(++nbytes));
	if(NULL == pBuffer)
	{
		return FALSE;
	}
	ringId->buf = pBuffer;
	ringId->bufSize = nbytes;

	gsbRngFlush(ringId);

	return TRUE;
}

//gsbRngDelete( ) - delete a ring buffer
LOCAL INLINE
void gsbRngDelete(LPGSBRINGBUF ringId)
{
	GSB_FREE(ringId->buf);
}

//gsbRngFlush( ) - make a ring buffer empty
LOCAL INLINE
void gsbRngFlush(LPGSBRINGBUF ringId)
{
	ringId->pToBuf = ringId->pFromBuf = 0;
}

/***************
//gsbRngIsEmpty( ) - test if a ring buffer is empty
LOCAL INLINE
BOOL gsbRngIsEmpty(LPGSBRINGBUF ringId)
{
	return (ringId->pToBuf == ringId->pFromBuf);
}


//gsbRngIsFull( ) - test if a ring buffer is full (no more room)
LOCAL INLINE
BOOL gsbRngIsFull(LPGSBRINGBUF ringId)
{
	 //rngFull condiction:
     // 1.  pToBuf + 1 == pFromBuf
     // 2.  pToBuf = bufSize,  pFromBuf = 0
	
	int nOffset = ringId->pToBuf - ringId->pFromBuf + 1;
	return (0 == nOffset) || (nOffset == ringId->bufSize);
}
***************/

LOCAL INLINE
int gsbRngFreeBytes(LPGSBRINGBUF ringId)
{
	int n = (ringId->pFromBuf - ringId->pToBuf) - 1;

	if (n < 0)
	{
		n += ringId->bufSize;
	}
	return (n);
}

//gsbRngBufGet( ) - get characters from a ring buffer
LOCAL int gsbRngBufGet
    (
    LPGSBRINGBUF ringId,      // ring buffer to get data from
    char *  buffer,           // pointer to buffer to receive data
    int     maxbytes          // maximum number of bytes to get
    )
{
	REGISTER int nCopyBytes;
	int nSize;
	
	int nToBuf = ringId->pToBuf;

	if(nToBuf >= ringId->pFromBuf)
	{		
		//nToBuf has not wrapped around
		nCopyBytes = nToBuf - ringId->pFromBuf;
		if(nCopyBytes > maxbytes)
		{
			nCopyBytes = maxbytes;
		}
		bcopy(ringId->buf + ringId->pFromBuf,buffer,nCopyBytes);
		ringId->pFromBuf += nCopyBytes;
	}
	else	
	{
		//nToBuf has wrapped around.  Grab chars up to the end of the
		// buffer, then wrap around if we need to. 

		nCopyBytes = ringId->bufSize - ringId->pFromBuf;
		if(nCopyBytes > maxbytes)
		{
			nCopyBytes = maxbytes;
		}

		bcopy(ringId->buf + ringId->pFromBuf,buffer,nCopyBytes);
		nSize = nCopyBytes + ringId->pFromBuf;

		// If nFromBuf is equal to bufSize, we've read the entire buffer,
		// and need to wrap now.  If bytesgot < maxbytes, copy some more chars in now.
		if(nSize == ringId->bufSize)
		{
			nSize = nToBuf;
			if(nSize > maxbytes - nCopyBytes)
			{
				nSize = maxbytes - nCopyBytes;	
			}
			bcopy(ringId->buf,buffer + nCopyBytes,nSize);
			ringId->pFromBuf = nSize;		
			nCopyBytes += nSize;
		}
		else
		{
			ringId->pFromBuf = nSize;
		}
	}

	return nCopyBytes;
}

//gsbRngBufPut( ) - put bytes into a ring buffer
LOCAL int gsbRngBufPut
    (
    LPGSBRINGBUF ringId,      // ring buffer to put data into 
    char *  buffer,           // buffer to get data from 
    int     nbytes            // number of bytes to try to put
    )
{
	REGISTER int nCopyBytes;
	int nSize;
	int nFromBuf = ringId->pFromBuf;

	if(nFromBuf > ringId->pToBuf) 
	{
		// nFromBuf is ahead of pToBuf.  We can fill up to two bytes before it 

		nCopyBytes = (nFromBuf - ringId->pToBuf) - 1;
		if(nCopyBytes > nbytes)
		{
			nCopyBytes = nbytes;
		}
		bcopy(buffer,ringId->buf + ringId->pToBuf,nCopyBytes);
		ringId->pToBuf += nCopyBytes;
	}
	else if(0 == nFromBuf)
	{
		// nFromBuf is at the beginning of the buffer.  We can fill till
		// the next-to-last element */

		nCopyBytes = (ringId->bufSize - ringId->pToBuf) - 1;
		if(nCopyBytes > nbytes)
		{
			nCopyBytes = nbytes;
		}
		bcopy(buffer,ringId->buf + ringId->pToBuf,nCopyBytes);
		ringId->pToBuf += nCopyBytes;
	}
	else
	{
		// nFromBuf has wrapped around, and its not 0, so we can fill
		// at least to the end of the ring buffer.  Do so, then see if
		// we need to wrap and put more at the beginning of the buffer.
		
		nCopyBytes = ringId->bufSize - ringId->pToBuf;
		if(nCopyBytes > nbytes)
		{
			nCopyBytes = nbytes;
		}
		bcopy(buffer,ringId->buf + ringId->pToBuf,nCopyBytes);		
		nSize = nCopyBytes + ringId->pToBuf;
		if(nSize == ringId->bufSize)
		{
			//We need to wrap, and perhaps put some more chars
			nSize = nFromBuf - 1;
			if(nSize > nbytes - nCopyBytes)
			{
				nSize = nbytes - nCopyBytes;	
			}
			bcopy(buffer + nCopyBytes,ringId->buf,nSize);
			ringId->pToBuf = nSize;	
			nCopyBytes += nSize;
		}
		else
		{
			ringId->pToBuf = nSize;	
		}
	}

	//���ؿ������ֽڸ���
	return nCopyBytes;
}


//////////////////////////////////////////////////////////////////////
//list functions:

//�Ƿ����������ṹ
//#define _GSBDEBUG

#ifdef _GSBDEBUG
LOCAL INLINE 
void GSBList_CheckList(REGISTER LPGSBLIST pList)
{
	int i=0;
	LPGSBMSG pMsgNode;
	HNCTRACE("pList Count = %d\n",pList->nCount);
	for(pMsgNode = pList->pHeadNode;i<pList->nCount;i++)
	{
		BISPASSERT(pMsgNode);
		pMsgNode = pMsgNode->pNextMsg;
	}
	if(0 == pList->nCount)
	{
		BISPASSERT(NULL == pList->pHeadNode && NULL == pList->pTailNode);
	}
}
#define CHECK_LIST(pList) GSBList_CheckList(pList)
#else
#define CHECK_LIST(pList)
#endif //_DEBUG


//��ʼ��
LOCAL INLINE 
void GSBList_Init(REGISTER LPGSBLIST pList)
{
	BISPASSERT(NULL != pList);
	GSBMemZero(pList,sizeof(GSBLIST));
}

//��С
LOCAL INLINE 
int GSBList_Size(REGISTER LPGSBLIST pList)
{
	BISPASSERT(NULL != pList);
//	CHECK_LIST(pList);
	return pList->nCount;
}

//ɾ��
LOCAL INLINE 
void GSBList_Deinit(REGISTER LPGSBINFO pGSBInfo,LPGSBLIST pList,BOOL bFreeList)
{
	BISPASSERT(NULL != pList);
	CHECK_LIST(pList);
	if(!bFreeList)
	{
		return ;
	}
	LPGSBMSGEX pMsg;
	LPGSBMSGEX pTempMsg;
	for(pMsg = pList->pHeadNode; NULL != pMsg; )
	{
		pTempMsg = pMsg;
		pMsg = pMsg->pNextMsg;
		gsbFreeMsg(pGSBInfo,pTempMsg);
	}
	GSBMemZero(pList,sizeof(GSBLIST));	
}

//�ӵ�����β��
LOCAL INLINE 
void GSBList_PushBack(REGISTER LPGSBLIST pList,LPGSBMSGEX pMsgNode)
{
	BISPASSERT(NULL != pList && NULL != pMsgNode );
	CHECK_LIST(pList);
	pMsgNode->pNextMsg = NULL;
	if(0 == pList->nCount)	//first time insert
	{
		pList->pHeadNode = pList->pTailNode = pMsgNode;
	}
	else	//
	{
		BISPASSERT(NULL != pList->pTailNode);
		pList->pTailNode->pNextMsg = pMsgNode;
		pList->pTailNode = pMsgNode;
	}
	pList->nCount ++;
//	InterlockedIncrement((LPLONG)&pList->nCount);
	BISPASSERT(pList->nCount >= 0);
	CHECK_LIST(pList);
}

//��ͷ������
LOCAL INLINE 
LPGSBMSGEX GSBList_PopFront(REGISTER LPGSBLIST pList)
{
	BISPASSERT(NULL != pList);
	CHECK_LIST(pList);
	LPGSBMSGEX pMsg;
	if(pList->nCount <=0 )
	{
		return NULL;
	}
	BISPASSERT(pList->pHeadNode && pList->pTailNode);
	pMsg = pList->pHeadNode;
	pList->pHeadNode = pMsg->pNextMsg;
	pList->nCount --;
//	InterlockedDecrement((LPLONG)&pList->nCount);
	if(0 == pList->nCount)
	{
//		pList->pHeadNode = pList->pTailNode = NULL;
		pList->pTailNode = NULL;
	}
	CHECK_LIST(pList);
	//����ͷ�ڵ�
	return pMsg;
}


//��ָ���ڵ�ǰ����һ���ڵ�
LOCAL INLINE 
void GSBList_InsertBeforeNode(REGISTER LPGSBLIST pList,LPGSBMSGEX pMsgNode,LPGSBMSGEX pInsertBeforeNode)
{
	BISPASSERT(NULL != pList && NULL != pMsgNode && NULL != pInsertBeforeNode);
	if(NULL != pInsertBeforeNode->pPrevMsg)
	{
		pInsertBeforeNode->pPrevMsg->pNextMsg = pMsgNode;
	}
	pMsgNode->pPrevMsg = pInsertBeforeNode->pPrevMsg;
	pInsertBeforeNode->pPrevMsg = pMsgNode;
	pMsgNode->pNextMsg = pInsertBeforeNode;
	if(pList->pHeadNode == pInsertBeforeNode)
	{
		pList->pHeadNode = pMsgNode;
	}
	pList->nCount ++;
}

//�ӵ�����β��
LOCAL INLINE 
void TimerList_PushBack(REGISTER LPTIMERLIST pList,LPGSBMSGEX pMsgNode)
{
	BISPASSERT(NULL != pList && NULL != pMsgNode);
	pMsgNode->pNextMsg = NULL;
	if(0 == pList->nCount)	//first time insert
	{
		pMsgNode->pPrevMsg = NULL;
		pList->pHeadNode = pList->pTailNode = pMsgNode;
	}
	else	//
	{
		pList->pTailNode->pNextMsg = pMsgNode;
		pMsgNode->pPrevMsg = pList->pTailNode;
		pList->pTailNode = pMsgNode;
	}
	pList->nCount ++;
}

//��ͷ������
LOCAL INLINE 
LPGSBMSGEX TimerList_PopFront(REGISTER LPTIMERLIST pList)
{
	BISPASSERT(NULL != pList);
	LPGSBMSGEX pMsg;
	if(pList->nCount <=0 )
	{
		return NULL;
	}
	pMsg = pList->pHeadNode;
	pList->pHeadNode = pMsg->pNextMsg;
	if(NULL != pList->pHeadNode)
	{
		pList->pHeadNode->pPrevMsg = NULL;
	}
	else
	{
		BISPASSERT(1 == pList->nCount);
		pList->pTailNode = NULL;
	}
	pList->nCount --;

	//����ͷ�ڵ�
	return pMsg;
}
//ɾ��һ���ڵ�
LOCAL INLINE 
void TimerList_DeleteNode(REGISTER LPTIMERLIST pList,LPGSBMSGEX pMsgNode)
{
	BISPASSERT(NULL != pList && NULL != pMsgNode);
	BISPASSERT(pList->nCount > 0);
	if(NULL != pMsgNode->pNextMsg)
	{
		pMsgNode->pNextMsg->pPrevMsg = pMsgNode->pPrevMsg;	

	}
	if(NULL != pMsgNode->pPrevMsg)
	{
		pMsgNode->pPrevMsg->pNextMsg = pMsgNode->pNextMsg;
	}
	if(pList->pTailNode == pMsgNode)
	{
		pList->pTailNode = pMsgNode->pPrevMsg;
	}
	if(pList->pHeadNode == pMsgNode)
	{
		pList->pHeadNode = pMsgNode->pNextMsg;
	}
	pList->nCount --;
}

 
/****************************************************************
//��ָ���ڵ�����һ���ڵ�
LOCAL INLINE 
void TimerList_InsertAfterNode(REGISTER LPTIMERLIST pList,LPGSBMSGEX pMsgNode,LPGSBMSGEX pInsertAfterNode)
{
	BISPASSERT(NULL != pList && NULL != pMsgNode && NULL != pInsertAfterNode);
	if(NULL != pInsertAfterNode->pNextMsg)
	{
		pInsertAfterNode->pNextMsg->pPrevMsg = pMsgNode;
	}
	pMsgNode->pNextMsg = pInsertAfterNode->pNextMsg;
	pInsertAfterNode->pNextMsg = pMsgNode;
	pMsgNode->pPrevMsg = pInsertAfterNode;
	if(pList->pTailNode == pInsertAfterNode)
	{
		pList->pTailNode = pMsgNode;
	}
	pList->nCount ++;
}
******************************************************************/

//��ָ���ڵ�ǰ����һ���ڵ�
LOCAL INLINE 
void TimerList_InsertBeforeNode(REGISTER LPTIMERLIST pList,LPGSBMSGEX pMsgNode,LPGSBMSGEX pInsertBeforeNode)
{
	BISPASSERT(NULL != pList && NULL != pMsgNode && NULL != pInsertBeforeNode);
	if(NULL != pInsertBeforeNode->pPrevMsg)
	{
		pInsertBeforeNode->pPrevMsg->pNextMsg = pMsgNode;
	}
	pMsgNode->pPrevMsg = pInsertBeforeNode->pPrevMsg;
	pInsertBeforeNode->pPrevMsg = pMsgNode;
	pMsgNode->pNextMsg = pInsertBeforeNode;
	if(pList->pHeadNode == pInsertBeforeNode)
	{
		pList->pHeadNode = pMsgNode;
	}
	pList->nCount ++;
}


typedef struct tagHashDword 
{
    union 
	{
		BYTE	byte[sizeof(DWORD)/sizeof(BYTE)];
//		WORD	word[sizeof(DWORD)/sizeof(WORD)];
		DWORD	dword;
    } data;
}HASHDWORD;


//ȡ��ģ��ID��Hashֵ
LOCAL INLINE 
UINT gshGetModuleHashIndex(UINT nModuleID)
{
//	UINT nIndex = (nModuleID>>24) + ((nModuleID>>16) &0xFF) + ((nModuleID>>8) &0xFF) + ((nModuleID) &0xFF);

	REGISTER HASHDWORD hashDW;
	REGISTER UINT nIndex;

	hashDW.data.dword = nModuleID;
	nIndex = hashDW.data.byte[0] + hashDW.data.byte[1] + hashDW.data.byte[2] + hashDW.data.byte[3];
	 
	return (nIndex & 0x07);
}


//����ָ��ID��ģ��
LOCAL INLINE 
LPMODULEINFO GSBInfo_FindModuleInfo(REGISTER LPGSBINFO pGSBInfo,UINT nModuleID)
{
	UINT nIndex = gshGetModuleHashIndex(nModuleID);

	LPMODULEINFO lpModuleInfo = pGSBInfo->m_lpModuleInfo[nIndex];

	LPMODULEINFO lpMaxModuleInfo = lpModuleInfo + pGSBInfo->m_dwModuleCount[nIndex];
	
	for(; (DWORD)lpModuleInfo < (DWORD)lpMaxModuleInfo;lpModuleInfo++)
	{
		if(nModuleID == lpModuleInfo->nModuleID)
		{
			return lpModuleInfo;
		}		
	}
	return NULL;
}


LOCAL INLINE 
LPGSBMSGEX GSBInfo_AllocMsgPack(REGISTER LPGSBINFO pGSBInfo,int nBufLen)
{
	LPGSBMSGEX pMsgPack = gsbMallocMsg(pGSBInfo);
	if(NULL != pMsgPack )
	{
		pMsgPack->nBufLen = nBufLen;
		pMsgPack->pAttachBuffer = gsbMallocBuffer(pGSBInfo,nBufLen);
#ifdef _DEBUG
		if(0 != nBufLen && NULL != pMsgPack->pAttachBuffer)
		{
			GSBMemZero(pMsgPack->pAttachBuffer->lpBuffer,(size_t)nBufLen);
		}
#endif //_DEBUG

		//2007.08.23����ڴ����ʧ�ܣ����ͷ��ڴ棬��Ϣ����ʧ��!
		if(NULL == pMsgPack->pAttachBuffer)
		{
			gsbFreeMsg(pGSBInfo,pMsgPack);
			//��Ϣ����ʧ��!
			pMsgPack = NULL;
		}
	}
		
	BISPASSERT(pMsgPack);
	return pMsgPack;
}

//�����ݼ��뵽��β
LOCAL INLINE 
BOOL GSBInfo_AddMsgPack(REGISTER LPGSBINFO pGSBInfo,LPGSBMSGEX pMsgPack)
{
	BISPASSERT(pMsgPack);
	GSBList_PushBack(&pGSBInfo->m_listQueue,pMsgPack);
	return (1 == GSBList_Size(&pGSBInfo->m_listQueue));
	//�����¼�
//	GSBEvent_SetEvent(pGSBInfo->m_objEvent);		
}

//�����ݰ����ȼ�����뵽���е���
LOCAL INLINE 
BOOL GSBInfo_InsertMsgPack(REGISTER LPGSBINFO pGSBInfo,LPGSBMSGEX pMsgPack)
{
	BISPASSERT(pMsgPack);
	REGISTER LPGSBMSGEX pInsertMsgPack = pGSBInfo->m_listQueue.pHeadNode;
	REGISTER UINT16 nPriority = pMsgPack->nPriority;
	for(;NULL != pInsertMsgPack; pInsertMsgPack = pInsertMsgPack->pNextMsg)
	{
		//�ҵ������
		if(pInsertMsgPack->nPriority > nPriority)
		{
			GSBList_InsertBeforeNode(&pGSBInfo->m_listQueue,pMsgPack,pInsertMsgPack);
			//��ʱ�϶���������������FALSE
			return FALSE;
		}
	}

	//û���¼��Ͳ���ĩβ
	GSBList_PushBack(&pGSBInfo->m_listQueue,pMsgPack);
	return (1 == GSBList_Size(&pGSBInfo->m_listQueue));
}

LOCAL INLINE 
void GSBInfo_FreeMsgPack(REGISTER LPGSBINFO pGSBInfo,LPGSBMSGEX pMsgPack)
{
	BISPASSERT(NULL != pMsgPack);

	gsbFreeBuffer(pGSBInfo,pMsgPack->nBufLen,pMsgPack->pAttachBuffer);

	gsbFreeMsg(pGSBInfo,pMsgPack);
}

//������Ϣ��
LOCAL INLINE 
LPGSBMSGEX GSBInfo_DuplicateMsgPack(REGISTER LPGSBINFO pGSBInfo,LPGSBMSGEX pMsgPack)
{
	BISPASSERT(NULL != pMsgPack);

	LPGSBMSGEX pCopyMsgPack = gsbMallocMsg(pGSBInfo);
//	if(NULL != pMsgPack )
	{
		*pCopyMsgPack = *pMsgPack;
		//���ƻ���
		pCopyMsgPack->pAttachBuffer = gsbDuplicateBuffer(pMsgPack->pAttachBuffer,pMsgPack->nBufLen);
	}
	return pCopyMsgPack;
}

LOCAL INLINE 
void GSBInfo_AddTimerMsgPack(REGISTER LPGSBINFO pGSBInfo,LPGSBMSGEX pMsgPack)
{
	BISPASSERT(NULL != pMsgPack);
	REGISTER LPGSBMSGEX pTempNode;
	REGISTER TICKTYPE dwTickCount = pMsgPack->dwNextTime;
	for(pTempNode = pGSBInfo->m_listTimer.pHeadNode; NULL != pTempNode; pTempNode = pTempNode->pNextMsg)
	{
		if( pTempNode->dwNextTime > dwTickCount)
		{
			//��ʼ��һ�ζ�ʱ
			TimerList_InsertBeforeNode(&pGSBInfo->m_listTimer,pMsgPack,pTempNode);
			break;
		}
	}
	if(NULL == pTempNode)	//�ӵ��������
	{
		TimerList_PushBack(&pGSBInfo->m_listTimer,pMsgPack);
	}	
}

//////////////////////////////////////////////////////////////////////////////////
//GSB API Function

	
SBHANDLE WINAPI GSB_Create(DWORD dwMaxModule,
						   DWORD dwMaxBufferSize)
{
	DWORD dwSize;

	if(dwMaxModule > MAX_MODULE)
	{
		return NULL;
	}
	
	//����Ż���С���ܴ���2048
	if(dwMaxBufferSize > MAX_BUFFER_SIZE)
	{
		dwMaxBufferSize = MAX_BUFFER_SIZE;
	}

	//����Ż���С��СΪ128
	if(dwMaxBufferSize <= OPTIMIZ_BUFFER_SIZE)
	{
		dwMaxBufferSize = OPTIMIZ_BUFFER_SIZE*2;
	}
	
	//��С��Ҫ1��ģ��,����һЩû��Ҫ������2007.04.27
	if(0 == dwMaxModule)
	{
		dwMaxModule = 1;
	}

	//�����Ҫ������ڴ��С
	LPGSBINFO pGSBInfo = (LPGSBINFO)GSB_MALLOC(sizeof(GSBINFO));
	if(NULL == pGSBInfo)
	{
		return NULL;
	}
	memset(pGSBInfo,0,sizeof(GSBINFO));
	pGSBInfo->m_dwFlag = GSB_FLAG;
	pGSBInfo->m_dwMaxBufferSize = dwMaxBufferSize;
	pGSBInfo->m_dwMaxModuleCount = dwMaxModule;
	pGSBInfo->m_nCurrentTimerID = 1;			//������ʼ��ʱ��ID
	//��ʼ������
	GSBList_Init(&pGSBInfo->m_listQueue);
	GSBList_Init(&pGSBInfo->m_listTimer);
	//��ʼ���ٽ���
	GSBCrisec_Init(pGSBInfo->m_csLock);
	//��ʼ���¼�
	GSBEvent_Init(pGSBInfo->m_objEvent);
	//ȡ�ñ��߳�ID
	pGSBInfo->m_nThreadID = GETTASKID();

	pGSBInfo->m_dwLastTickCount = GET_TICK_COUNT(pGSBInfo);
	//���õȴ�ʱ��Ϊ���޳�
	pGSBInfo->m_dwWaitTime = LOOP_FOREVER;
	
	//��ʼ�����λ���
	gsbRngCreate(&pGSBInfo->m_RingBuf,RING_BUFFER_SIZE);						//��ʼ��ѭ������
	
	//��ʼ�������ڴ��������
	gsbInitFastAlloc(&pGSBInfo->m_MemAllocMsg,ROUND4(sizeof(GSBMSGEX)),
						MALLOC_BLOCK_SIZE_MSG / ROUND4(sizeof(GSBMSGEX)) );
	
	//2007.07.28 �������ڴ����Ϊ61-63�ֽ�ʱ����Խ������Ƿ�ָ�������BUG
	// ע��m_MemAlloc64��m_MemAllocMaxʵ�ʷ�����ֽ�Ҫ��ָ���Ķ�GSBBUF_EXTACT_SIZE���ֽ�
	gsbInitFastAlloc(&pGSBInfo->m_MemAlloc64,ROUND4(GSBBUF_EXTACT_SIZE + 64),
						MALLOC_BLOCK_SIZE_64 / ROUND4(GSBBUF_EXTACT_SIZE + 64));
	
	//ȡ��Բ�����ڴ��С
	dwSize = GSBBUF_EXTACT_SIZE + pGSBInfo->m_dwMaxBufferSize;
	dwSize = ROUND4(dwSize);
	//�������
	gsbInitFastAlloc(&pGSBInfo->m_MemAllocMax,dwSize,
					(dwSize/2 >= MALLOC_BLOCK_SIZE_MAX)? 2:(MALLOC_BLOCK_SIZE_MAX/dwSize) );

	//��ʼ��ģ��Hash����
	for(int i = 0; i< MODULE_HASH_LEN; i++)
	{
		pGSBInfo->m_lpModuleInfo[i] = (LPMODULEINFO)GSB_MALLOC(dwMaxModule*sizeof(MODULEINFO));
		GSBMemZero(pGSBInfo->m_lpModuleInfo[i],dwMaxModule*sizeof(MODULEINFO));
	}

#ifdef NSL_OS_VXWORKS	//VxWorks ��ʱ����������
 	g_nSysRate = (ULONG)sysClkRateGet();
#endif //WIN32


	return (SBHANDLE)pGSBInfo;
}

BOOL WINAPI GSB_Destroy(SBHANDLE hBus)
{
	UINT i;
	LPGSBMSGEX lpMsgPack;

	if(NULL == hBus)
	{
		return FALSE;
	}
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	//�����˳���־λ

	//ɾ�������ڵ�
	GSBCrisec_Lock(pGSBInfo->m_csLock);

	//�ͷ�������Ϣ�ڵ���ڴ滺��
	while(NULL != (lpMsgPack = GSBList_PopFront(&pGSBInfo->m_listQueue) ))
	{
		GSBInfo_FreeMsgPack(pGSBInfo,lpMsgPack);
	}

	//�ͷ����ж�ʱ���ڵ���ڴ滺��
	while(NULL != (lpMsgPack = GSBList_PopFront(&pGSBInfo->m_listTimer) ))
	{
		GSBInfo_FreeMsgPack(pGSBInfo,lpMsgPack);
	}

	GSBList_Deinit(pGSBInfo,&pGSBInfo->m_listQueue,TRUE);
	GSBList_Deinit(pGSBInfo,&pGSBInfo->m_listTimer,TRUE);

	GSBCrisec_Unlock(pGSBInfo->m_csLock);

	//����ʼ���ٽ���
	GSBCrisec_Deinit(pGSBInfo->m_csLock);
	//����ʼ���¼�
	GSBEvent_Deinit(pGSBInfo->m_objEvent);

	//�ͷŻ��λ���
	gsbRngDelete(&pGSBInfo->m_RingBuf);

	//�ͷŹ��õ��¼�
#ifdef USE_FAST_WAIT_EVENT
	for(i = 0; i< pGSBInfo->m_nEventCount; i++)
	{
		//����ʼ��wait�¼���
		GSBEvent_Deinit(pGSBInfo->m_eventWait[i]);
	}
#endif //USE_FAST_WAIT_EVENT

	//�ͷ��Ѿ�����Ŀ����ڴ�
	gsbDeinitFastAlloc(&pGSBInfo->m_MemAllocMsg);
	gsbDeinitFastAlloc(&pGSBInfo->m_MemAlloc64);
	gsbDeinitFastAlloc(&pGSBInfo->m_MemAllocMax);

	//�ͷ�ģ��Hash����
	for(i = 0; i< MODULE_HASH_LEN; i++)
	{
		GSB_FREE(pGSBInfo->m_lpModuleInfo[i]);
	}

	//�ͷ��ڴ�
	GSB_FREE(pGSBInfo);
	return TRUE;
}

BOOL WINAPI GSB_SetOwnerTaskId(SBHANDLE hBus,
								sb_task_id_t nTaskID)
{
	if(NULL == hBus)
	{
		return FALSE;
	}
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	if(0 == nTaskID)
	{
		nTaskID = GETTASKID();
	}
	GSBFast_Lock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
	pGSBInfo->m_nThreadID = nTaskID;
	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����

	return TRUE;
}

DWORD WINAPI GSB_GetMaxBufferSize(SBHANDLE hBus)
{
	if(NULL == hBus)
	{
		return 0;
	}
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	return 	pGSBInfo->m_dwMaxBufferSize;
}

BOOL WINAPI GSB_RegisterModule(SBHANDLE hBus,
							   UINT nModuleID,
							   LPONBUSMESSAGE lpfnCallback,
							   DWORD dwUserData)
{
	if(NULL == hBus || NULL == lpfnCallback)
	{
		return FALSE;
	}
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);

	UINT nIndex = gshGetModuleHashIndex(nModuleID);
	
	GSBFast_Lock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����

	//�Ƿ�ﵽ���ģ�����
	if(pGSBInfo->m_dwModuleCount[nIndex] >= pGSBInfo->m_dwMaxModuleCount)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}

	LPMODULEINFO lpModuleInfo = GSBInfo_FindModuleInfo(pGSBInfo,nModuleID);
	if(NULL == lpModuleInfo)	//New One
	{
		lpModuleInfo = &pGSBInfo->m_lpModuleInfo[nIndex][pGSBInfo->m_dwModuleCount[nIndex] ];
		pGSBInfo->m_dwModuleCount[nIndex]++;		
	}
	//����Module��Ϣ
	lpModuleInfo->nModuleID = nModuleID;
	lpModuleInfo->lpfnCallback = lpfnCallback;
	lpModuleInfo->dwUserData = dwUserData;
	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����

	return TRUE;
}

BOOL WINAPI GSB_UnregisterModule(SBHANDLE hBus,
								 UINT nModuleID)
{
	if(NULL == hBus)
	{
		return FALSE;
	}
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	
	//����ģ��ɾ����ʱ���������õ�Bug,add by tbh 2006.01.06
	GSB_KillAllModuleTimer(hBus,nModuleID);

	UINT nIndex = gshGetModuleHashIndex(nModuleID);

	GSBFast_Lock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
	
	LPMODULEINFO lpModuleInfo = GSBInfo_FindModuleInfo(pGSBInfo,nModuleID);
	if(NULL == lpModuleInfo)	//ԭ��û�е�Module	
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	pGSBInfo->m_dwModuleCount[nIndex] --;	//Sub Module Count;
	
	LPMODULEINFO lpMoveModuleInfo = &pGSBInfo->m_lpModuleInfo[nIndex][pGSBInfo->m_dwModuleCount[nIndex] ];
	if(lpModuleInfo != lpMoveModuleInfo)	//Move forward
	{
		GSBMemCpy(lpModuleInfo,lpMoveModuleInfo,sizeof(MODULEINFO));
	}
	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
	
	return TRUE;
}

//��Ϣ�����������ûص���
LOCAL INLINE 
int GSB_ProcessMessage(REGISTER LPGSBINFO pGSBInfo,
								LPGSBMSGEX lpMsgPack,
								LPMODULEINFO pFindModuleInfo)
{
	BISPASSERT(pGSBInfo && lpMsgPack);
	LPMODULEINFO lpModuleInfo;
	UINT nModuleID;
	LPONBUSMESSAGE lpfnCallback = NULL;
	DWORD dwUserData = 0;

	int nRet = 0;
	
	switch(lpMsgPack->nCallbackType)	//ֻ�е�λ������
	{
	case CT_NORMAL:			//��ͨ�ص�����
	case CT_TIMERNORMAL:			//Timer ��ͨ�ص�����
		{
			if(NULL == pFindModuleInfo)
			{
				GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
				//Ѱ��Ŀ��ģ��			
				lpModuleInfo = GSBInfo_FindModuleInfo(pGSBInfo,lpMsgPack->nModuleDestID);
				if(NULL != lpModuleInfo)
				{
					lpfnCallback = lpModuleInfo->lpfnCallback;
					dwUserData = lpModuleInfo->dwUserData;
				}
				GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
			}
			else
			{
				lpModuleInfo = pFindModuleInfo;
				lpfnCallback = lpModuleInfo->lpfnCallback;
				dwUserData = lpModuleInfo->dwUserData;
			}
			if(NULL != lpfnCallback) //�ȼ���NULL != lpModuleInfo	
			{
				//���ûص�����
				nRet = (*lpfnCallback)(lpMsgPack->nModuleSrcID,			//ԴID
					lpMsgPack->nModuleDestID,					//Ŀ��ID
					lpMsgPack->nMsg,				//��ϢID
					lpMsgPack->wParam,				//Add wParam
					lpMsgPack->lParam,				//Add lParam								
					(LPCSTR)GET_BUFFER(lpMsgPack),	//���ݻ���
					lpMsgPack->nBufLen,				//���ݳ���
					dwUserData);					//�Զ�������
				
			}
		}
		break;
	case CT_BROADCAST:			//�㲥����
	case CT_TIMERBROADCAST:			//Timer�㲥����
		{
			GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
			UINT nModuleSrcID = lpMsgPack->nModuleSrcID;
			for(int i = 0; i< MODULE_HASH_LEN; i++)
			{
				lpModuleInfo = pGSBInfo->m_lpModuleInfo[i];
				LPMODULEINFO lpMaxModuleInfo = lpModuleInfo + pGSBInfo->m_dwModuleCount[i];
				//�о����е�ģ��
				for(; (DWORD)lpModuleInfo < (DWORD)lpMaxModuleInfo;lpModuleInfo++)
				{
					//�����������ģ��,����
					if(nModuleSrcID == lpModuleInfo->nModuleID)
					{
						continue;
					}
					lpfnCallback = lpModuleInfo->lpfnCallback;
					dwUserData = lpModuleInfo->dwUserData;
					nModuleID = lpModuleInfo->nModuleID;	//add by tbh 2006.01.18

					GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
					//���ûص�����
					(void)((*lpfnCallback)(nModuleSrcID,		//ԴID
						nModuleID,						//Ŀ��ID
						lpMsgPack->nMsg,				//��ϢID
						lpMsgPack->wParam,				//Add wParam
						lpMsgPack->lParam,				//Add lParam
						(LPCSTR)GET_BUFFER(lpMsgPack),	//���ݻ���
						lpMsgPack->nBufLen,				//���ݳ���
						dwUserData));					//�Զ������� 
					
					GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
					
				}
			}//end for(int i = 0; i< MODULE_HASH_LEN; i++)
			GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����			
		}
		break;
	case CT_JOBADD:				//Add Job����
		{			
			BISPASSERT(lpMsgPack->lpfnCallback != NULL);
			nRet = (*((GSBFUNCPTR)lpMsgPack->lpfnCallback))(
											(LPCSTR)GET_BUFFER(lpMsgPack),	//���ݻ���
											lpMsgPack->nBufLen,				//���ݳ���
											(int)lpMsgPack->nModuleSrcID,
											(int)lpMsgPack->nModuleDestID,
											(int)lpMsgPack->nMsg,
											(int)lpMsgPack->wParam,
											(int)lpMsgPack->lParam);
			
		}
		break;
	case CT_FASTJOBADD:			//Fast Int Job����	
		{
			BISPASSERT(lpMsgPack->lpfnCallback != NULL);
			nRet = (*((GSBFASTFUNCPTR)lpMsgPack->lpfnCallback))(
											(int)lpMsgPack->nModuleSrcID,
											(int)lpMsgPack->nModuleDestID,
											(int)lpMsgPack->nMsg,
											(int)lpMsgPack->wParam,
											(int)lpMsgPack->lParam);
		}
		break;
	case CT_TIMERJOBADD:			//Timer Add Job����
		{
			BISPASSERT(lpMsgPack->lpfnCallback != NULL);
			nRet = (*((GSBTIMERFUNCPTR)lpMsgPack->lpfnCallback))(lpMsgPack->nIDEvent,
											(LPCSTR)GET_BUFFER(lpMsgPack),	//���ݻ���
											lpMsgPack->nBufLen,				//���ݳ���
											(int)lpMsgPack->nModuleSrcID,
											(int)lpMsgPack->nModuleDestID,
											(int)lpMsgPack->nMsg,
											(int)lpMsgPack->wParam,
											(int)lpMsgPack->lParam);
		}
		break;
	case CT_FASTTIMERJOBADD:			//Timer Add Job����
		{
			BISPASSERT(lpMsgPack->lpfnCallback != NULL);
			nRet = (*((GSBFASTTIMERFUNCPTR)lpMsgPack->lpfnCallback))(lpMsgPack->nIDEvent,
											(int)lpMsgPack->nModuleSrcID,
											(int)lpMsgPack->nModuleDestID,
											(int)lpMsgPack->nMsg,
											(int)lpMsgPack->wParam,
											(int)lpMsgPack->lParam);
		}
		break;
	default:
		BISPASSERT(FALSE);
	//	break;
	}
	//֪ͨ��Ϣ�������
	if(0 != lpMsgPack->dwWaitEvent)
	{
		GSB_EVENT event = (GSB_EVENT)lpMsgPack->dwWaitEvent;

		//lpMsgPack->dwWaitEvent = nRet;		//delete by tbh 2006.04.03 ��������ֵ��Bug
		//add by tbh 2006.04.03 ��������ֵ��Bug
		if(0 != lpMsgPack->nIDEvent)
		{
			*(int*)lpMsgPack->nIDEvent = nRet;
		}
		//end add by tbh 2006.04.03
		GSBEvent_SetEvent(event);	//֪ͨ��Ϣ�������
	}
	return nRet;
}




//���´���ר��Ϊ���Ż�����:
#ifdef _DEBUG	//Debug �汾������
								
LOCAL INLINE 
LPGSBMSGEX GSB_BuildMessage(REGISTER LPGSBINFO pGSBInfo,
										UINT nModuleSrcID,
										UINT nModuleDestID,
										UINT nMsg,
										WPARAM wParam,
										LPARAM lParam,
										LPCSTR lpBuffer,
										int nBufLen,
										UINT16 nPriority)
{
	BISPASSERT(NULL != pGSBInfo);
	//Fix NULL pointer Bug 2005.07.21
	if(NULL == lpBuffer)
	{
		nBufLen = 0;
	}
	else if(nBufLen < 0)
	{
		nBufLen = (int)strlen(lpBuffer) + 1;
	}

	/*****2007.08.23 ɾ�����´��벻������BUFFER��С���������г��ȵ���Ϣ����
	//���Buffer����	
	if((DWORD)nBufLen > pGSBInfo->m_dwMaxBufferSize)	//���ܳ�����󳤶�
	{
		BISPASSERT((DWORD)nBufLen <= pGSBInfo->m_dwMaxBufferSize);
		nBufLen = (int)pGSBInfo->m_dwMaxBufferSize;
	}
	***********************************/

	LPGSBMSGEX lpMsg = GSBInfo_AllocMsgPack(pGSBInfo,nBufLen);
	if(NULL != lpMsg)	//����ʧ��!!!!
	{
		lpMsg->pNextMsg = NULL;
		lpMsg->pPrevMsg = NULL;
		lpMsg->nModuleSrcID = nModuleSrcID;
		lpMsg->nModuleDestID = nModuleDestID;
		lpMsg->nMsg = nMsg;
		lpMsg->wParam = wParam;
		lpMsg->lParam = lParam;
		lpMsg->nIDEvent = 0;	//add by tbh 2006.04.03
		lpMsg->dwWaitEvent = 0; //û�еȴ��¼�
		lpMsg->nBufLen = nBufLen;
		lpMsg->nPriority = (UINT16)nPriority;	//add by tbh 2007.12.11
		//Fix NULL pointer Bug 2005.07.21
		if(NULL != lpBuffer) \
		{
			memcpy(GET_BUFFER(lpMsg),lpBuffer,(size_t)nBufLen); 
		}
	}	
	return lpMsg;
}

#define BUILD_MSG(_lpMsg,_pGSBInfo,_nModuleSrcID,_nModuleDestID,_nMsg,_wParam,_lParam,_lpBuffer,_nBufLen,_nPriority) \
	( (_lpMsg) = GSB_BuildMessage((_pGSBInfo),(UINT)(_nModuleSrcID),(UINT)(_nModuleDestID), \
			(UINT)(_nMsg),(WPARAM)(_wParam),(LPARAM)(_lParam),(_lpBuffer),(_nBufLen),(UINT16)(_nPriority)) )

#define BUILD_FAST_MSG(_lpMsg,_pGSBInfo,_nModuleSrcID,_nModuleDestID,_nMsg,_wParam,_lParam,_nPriority) \
	( (_lpMsg) = GSB_BuildMessage((_pGSBInfo),(UINT)(_nModuleSrcID),(UINT)(_nModuleDestID), \
			(UINT)(_nMsg),(WPARAM)(_wParam),(LPARAM)(_lParam),NULL,0,(UINT16)(_nPriority)) )

#define BULID_NULL_MSG(_lpMsg,_pGSBInfo,_nPriority) \
	( (_lpMsg) = GSB_BuildMessage((_pGSBInfo),(UINT)0,(UINT)0,(UINT)0,(WPARAM)0,(LPARAM)0,\
	NULL,0,(UINT16)(_nPriority)) )

#else	//�Ż�ʵ��

#define BUILD_MSG(_lpMsg,_pGSBInfo,_nModuleSrcID,_nModuleDestID,_nMsg,_wParam,_lParam,_lpBuffer,_nBufLen,_nPriority) \
	BISPASSERT(NULL != (_pGSBInfo)); \
	if(NULL == (_lpBuffer) ) \
	{	(_nBufLen) = 0; } \
	else if((_nBufLen) < 0) \
	{	(_nBufLen) = (int)strlen(_lpBuffer) + 1; } \
	(_lpMsg) = GSBInfo_AllocMsgPack((_pGSBInfo),(int)(_nBufLen)); \
	if(NULL != (_lpMsg))	\
	{ \
		(_lpMsg)->pNextMsg = NULL; \
		(_lpMsg)->pPrevMsg = NULL; \
		(_lpMsg)->nModuleSrcID = (UINT)(_nModuleSrcID); \
		(_lpMsg)->nModuleDestID = (UINT)(_nModuleDestID); \
		(_lpMsg)->nMsg = (UINT)(_nMsg); \
		(_lpMsg)->wParam = (WPARAM)(_wParam); \
		(_lpMsg)->lParam = (LPARAM)(_lParam); \
		(_lpMsg)->nIDEvent = 0; \
		(_lpMsg)->dwWaitEvent = 0; \
		(_lpMsg)->nBufLen = (int)(_nBufLen); \
		(_lpMsg)->nPriority = (UINT16)(_nPriority); \
		if(NULL != (_lpBuffer)) \
		{	memcpy(GET_BUFFER(_lpMsg),(_lpBuffer),(size_t)(_nBufLen)); } \
	}

#define BUILD_FAST_MSG(_lpMsg,_pGSBInfo,_nModuleSrcID,_nModuleDestID,_nMsg,_wParam,_lParam,_nPriority) \
	BISPASSERT(NULL != (_pGSBInfo)); \
	(_lpMsg) = GSBInfo_AllocMsgPack((_pGSBInfo),0); \
	if(NULL != (_lpMsg))	\
	{ \
		(_lpMsg)->pNextMsg = NULL; \
		(_lpMsg)->pPrevMsg = NULL; \
		(_lpMsg)->nModuleSrcID = (UINT)(_nModuleSrcID); \
		(_lpMsg)->nModuleDestID = (UINT)(_nModuleDestID); \
		(_lpMsg)->nMsg = (UINT)(_nMsg); \
		(_lpMsg)->wParam = (WPARAM)(_wParam); \
		(_lpMsg)->lParam = (LPARAM)(_lParam); \
		(_lpMsg)->nIDEvent = 0; \
		(_lpMsg)->dwWaitEvent = 0; \
		(_lpMsg)->nBufLen = 0; \
		(_lpMsg)->nPriority = (UINT16)(_nPriority); \
	}

#define BULID_NULL_MSG(_lpMsg,_pGSBInfo,_nPriority) \
	(_lpMsg) = GSBInfo_AllocMsgPack((_pGSBInfo),0); \
	if(NULL != (_lpMsg))	\
	{ \
		(_lpMsg)->pNextMsg = NULL; \
		(_lpMsg)->pPrevMsg = NULL; \
		(_lpMsg)->nModuleSrcID = (UINT)(0); \
		(_lpMsg)->nModuleDestID = (UINT)(0); \
		(_lpMsg)->nMsg = (UINT)(0); \
		(_lpMsg)->wParam = (WPARAM)(0); \
		(_lpMsg)->lParam = (LPARAM)(0); \
		(_lpMsg)->nIDEvent = 0; \
		(_lpMsg)->dwWaitEvent = 0; \
		(_lpMsg)->nBufLen = (int)(0); \
		(_lpMsg)->nPriority = (UINT16)(_nPriority); \
	}	

#endif //_DEBUG


BOOL WINAPI GSB_PostMessage(SBHANDLE hBus,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,
							LPCSTR lpBuffer,
							int nBufLen)
{
	if(NULL == hBus)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	//���º��GSB_BuildMessage�ȼ�
	BUILD_MSG(lpMsg,
			pGSBInfo,
			nModuleSrcID,
			nModuleDestID,
			nMsg,
			wParam,
			lParam,
			lpBuffer,
			nBufLen,
			GSB_PRIORITY_DEFAULT);
	
	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	lpMsg->nCallbackType = (UINT16)
		((BROASTCAST_MODULE_ID == nModuleDestID)? CT_BROADCAST:CT_NORMAL);//��ͨ�ص�����
	lpMsg->dwWaitEvent = 0;												//û�еȴ��¼�

	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//���ӵ�������ȥ

	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	
	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;
}

BOOL WINAPI GSB_PostMessageEx(SBHANDLE hBus,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,
							LPCSTR lpBuffer,
							int nBufLen,
							UINT nPriority)
{
	if(NULL == hBus)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	//���º��GSB_BuildMessage�ȼ�
	BUILD_MSG(lpMsg,
			pGSBInfo,
			nModuleSrcID,
			nModuleDestID,
			nMsg,
			wParam,
			lParam,
			lpBuffer,
			nBufLen,
			(nPriority & 0xFF));
	
	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	lpMsg->nCallbackType = (UINT16)
		((BROASTCAST_MODULE_ID == nModuleDestID)? CT_BROADCAST:CT_NORMAL);//��ͨ�ص�����
	lpMsg->dwWaitEvent = 0;												//û�еȴ��¼�

	//�������ȼ����ӵ�������ȥ
	BOOL bSetEventImmediately = GSBInfo_InsertMsgPack(pGSBInfo,lpMsg);		

	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	
	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_PostQuitMessage
//	�������� : ����һ���������ѭ�����˳�����
//	��    �� : ̷ �� ��
//	ʱ    �� : 2007��4��14��
//	�� �� ֵ : BOOL
//	����˵�� : SBHANDLE hBus
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_PostQuitMessage(SBHANDLE hBus)
{
	if(NULL == hBus)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	//���º��GSB_BuildMessage�ȼ�
	BULID_NULL_MSG(lpMsg,pGSBInfo,GSB_PRIORITY_DEFAULT);
	
	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	lpMsg->nCallbackType = CT_EXIT;				//�˳���Ϣ

	lpMsg->dwWaitEvent = 0;						//û�еȴ��¼�

	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//���ӵ�������ȥ

	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	
	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;	
}

//PostͶ����ʱ��Ϣ
UINT WINAPI GSB_PostDelayMessage(SBHANDLE hBus,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,
							LPCSTR lpBuffer,
							int nBufLen,
							DWORD dwDelayTime)
{
	if(0 == dwDelayTime)
	{
		GSB_PostMessage(hBus,nModuleSrcID,nModuleDestID,nMsg,wParam,lParam,lpBuffer,nBufLen);
		return 0;
	}
	if(NULL == hBus)
	{
		return 0;
	}
	UINT nIDEvent;
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	//���º��GSB_BuildMessage�ȼ�
	BUILD_MSG(lpMsg,
				pGSBInfo,
				nModuleSrcID,
				nModuleDestID,
				nMsg,
				wParam,
				lParam,
				lpBuffer,
				nBufLen,
				GSB_PRIORITY_DEFAULT);

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return 0;
	}

	lpMsg->nCallbackType = (UINT16)
		((BROASTCAST_MODULE_ID == nModuleDestID)? CT_TIMERBROADCAST:CT_TIMERNORMAL);//Timer ��ͨ�ص�����
	lpMsg->nMsg = nMsg;
	lpMsg->dwWaitEvent = 0; //û�еȴ��¼�

	//���϶�ʱ��ID��Ϊ0
	if( 0 == pGSBInfo->m_nCurrentTimerID)
	{
		pGSBInfo->m_nCurrentTimerID = 1;
	}

	//���ö�ʱ��ID
	nIDEvent = lpMsg->nIDEvent = pGSBInfo->m_nCurrentTimerID++;

	//���ö�ʱ���
	lpMsg->nElapse = dwDelayTime;
	lpMsg->nJitterTime = 0;
	lpMsg->nLoopTimes = 1;						//ֻѭ��һ��

	//ȡ�õ�ǰʱ��
	TICKTYPE dwTickCount = GET_TICK_COUNT(pGSBInfo);

	//�����´ε���ʱʱ��
	dwTickCount += dwDelayTime;
	lpMsg->dwNextTime = lpMsg->dwDelayTime = dwTickCount;
	
	GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsg);
	//�������Ķ�ʱ����Ϣ��Ҫ���ϴ���,�򴥷�,�����ʾԭ���и����ȵĶ�ʱ����ǰ��Ⱥ�,
	//���ʱ��Ͳ���Ҫ������̨�߳���
	BOOL bSetEventImmediately = FALSE;

	//���Ҫ�����ϴ���,�������´δ�����ʱ��
	if(pGSBInfo->m_listTimer.pHeadNode == lpMsg &&
		( LOOP_FOREVER == pGSBInfo->m_dwWaitTime ||
		 pGSBInfo->m_dwLastTickCount + pGSBInfo->m_dwWaitTime > lpMsg->dwNextTime) )
	{
		bSetEventImmediately = TRUE;
	}

	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����

	//��Զ�ʱ�̵���Ϣ���ϴ�����̨��ʱ�߳�
	if(bSetEventImmediately)
	{
		//������ʱ���¼�
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return nIDEvent;
}

LOCAL INLINE 
int GSB_SendMessageSynch(REGISTER LPGSBINFO pGSBInfo,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,							
							LPCSTR lpBuffer,
							int nBufLen)
{
	LPONBUSMESSAGE lpfnCallback;
	UINT nModuleID;
	DWORD dwUserData;
				
	if(nBufLen <0 )
	{
		nBufLen = (int)strlen(lpBuffer) + 1;
	}
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	LPMODULEINFO lpModuleInfo;
	if(BROASTCAST_MODULE_ID == nModuleDestID)	//ͨ����ʽ
	{
		GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
		
		for(int i = 0; i< MODULE_HASH_LEN; i++)
		{
			lpModuleInfo = pGSBInfo->m_lpModuleInfo[i];
			LPMODULEINFO lpMaxModuleInfo = lpModuleInfo + pGSBInfo->m_dwModuleCount[i];
			
			for(; (DWORD)lpModuleInfo < (DWORD)lpMaxModuleInfo;lpModuleInfo++)
			{
				//���ڲ��������ģ��
				if(nModuleSrcID != lpModuleInfo->nModuleID)
				{
					lpfnCallback = lpModuleInfo->lpfnCallback;
					nModuleID = lpModuleInfo->nModuleID;
					dwUserData = lpModuleInfo->dwUserData;

					GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
					
					//���ûص�����
					(void)((*lpfnCallback)(nModuleSrcID,			//ԴID
						nModuleID,			//Ŀ��ID
						nMsg,				//��ϢID
						wParam,				//Add wParam
						lParam,				//Add lParam
						(LPCSTR)lpBuffer,	//���ݻ���
						nBufLen,				//���ݳ���
						dwUserData));		//�Զ�������

					GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
					
				}
			}	

		} //end for(int i = 0; i< MODULE_HASH_LEN; i++)

		GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
		return 0;		//ͨ������,����
	}
	
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	lpModuleInfo = GSBInfo_FindModuleInfo(pGSBInfo,nModuleDestID);
	if(NULL == lpModuleInfo)	
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
		return FALSE;
	}
	
	lpfnCallback = lpModuleInfo->lpfnCallback;
	dwUserData = lpModuleInfo->dwUserData;
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
				
	//���ûص�����
	int nRet = (*lpfnCallback)(nModuleSrcID,			//ԴID
							nModuleDestID,				//Ŀ��ID
							nMsg,						//��ϢID
							wParam,						//Add wParam
							lParam,						//Add lParam
							lpBuffer,					//���ݻ���
							nBufLen,					//���ݳ���
							dwUserData);				//�Զ�������
	
	
	return nRet;
}

int  WINAPI GSB_SendMessage(SBHANDLE hBus,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,							
							LPCSTR lpBuffer,
							int nBufLen)
{
	

	if(NULL == hBus)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	if(GETTASKID() == pGSBInfo->m_nThreadID)	//����Ǳ��̣߳�ֱ�ӵ���
	{
		return GSB_SendMessageSynch(pGSBInfo,nModuleSrcID,nModuleDestID,nMsg,wParam,lParam,lpBuffer,nBufLen);
	}
	////////////////////////////////////////////
	//�����ߵȴ�����.....
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	//���º��GSB_BuildMessage�ȼ�
	BUILD_MSG(lpMsg,
				pGSBInfo,
				nModuleSrcID,
				nModuleDestID,
				nMsg,
				wParam,
				lParam,
				lpBuffer,
				nBufLen,
				GSB_PRIORITY_DEFAULT);

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}

	GSB_EVENT eventWait;		//����һ���ȴ��¼�
#ifdef USE_FAST_WAIT_EVENT //����Event
	GSB_EVENT_ALLOC(eventWait,pGSBInfo);
#else
	GSBEvent_Init(eventWait);
#endif //USE_FAST_WAIT_EVENT
	lpMsg->dwWaitEvent = (DWORD)eventWait; //����Ϣ֪ͨ�������

	int nRet = 0;							//add by tbh 2006.04.03 ��������ֵ��Bug
	//nTimerIdָ�򱣴淵��ֵ��intָ��
	lpMsg->nIDEvent = (UINT)&nRet;			//add by tbh 2006.04.03 ��������ֵ��Bug

	lpMsg->nCallbackType = (UINT16)
		((BROASTCAST_MODULE_ID == nModuleDestID)? CT_BROADCAST:CT_NORMAL);//��ͨ�ص�����
	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//���ӵ�������ȥ
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}

	//�ȴ��¼�
	GSBEvent_WaitEvent(eventWait,(UINT)-1);
	//ȡ�÷���ֵ,ע�⣬��ʱ�ķ���ֵ�Ѿ���GSB_ProcessMessage����������....
//	int nRet = (int)lpMsg->dwWaitEvent;	//delete by tbh 2006.04.03 ��������ֵ��Bug

#ifdef USE_FAST_WAIT_EVENT //����Event
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	GSB_EVENT_FREE(eventWait,pGSBInfo);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
#else
	//ɾ���¼�����
	GSBEvent_Deinit(eventWait);
#endif //USE_FAST_WAIT_EVENT


	return nRet;
}

int  WINAPI GSB_SendMessageEx(SBHANDLE hBus,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,							
							LPCSTR lpBuffer,
							int nBufLen,
							UINT nPriority)
{
	

	if(NULL == hBus)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	if(GETTASKID() == pGSBInfo->m_nThreadID)	//����Ǳ��̣߳�ֱ�ӵ���
	{
		return GSB_SendMessageSynch(pGSBInfo,nModuleSrcID,nModuleDestID,nMsg,wParam,lParam,lpBuffer,nBufLen);
	}
	////////////////////////////////////////////
	//�����ߵȴ�����.....
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	//���º��GSB_BuildMessage�ȼ�
	BUILD_MSG(lpMsg,
				pGSBInfo,
				nModuleSrcID,
				nModuleDestID,
				nMsg,
				wParam,
				lParam,
				lpBuffer,
				nBufLen,
				(nPriority & 0xFF) );

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}

	GSB_EVENT eventWait;		//����һ���ȴ��¼�
#ifdef USE_FAST_WAIT_EVENT //����Event
	GSB_EVENT_ALLOC(eventWait,pGSBInfo);
#else
	GSBEvent_Init(eventWait);
#endif //USE_FAST_WAIT_EVENT
	lpMsg->dwWaitEvent = (DWORD)eventWait; //����Ϣ֪ͨ�������

	int nRet = 0;							//add by tbh 2006.04.03 ��������ֵ��Bug
	//nTimerIdָ�򱣴淵��ֵ��intָ��
	lpMsg->nIDEvent = (UINT)&nRet;			//add by tbh 2006.04.03 ��������ֵ��Bug

	lpMsg->nCallbackType = (UINT16)
		((BROASTCAST_MODULE_ID == nModuleDestID)? CT_BROADCAST:CT_NORMAL);//��ͨ�ص�����

	//�������ȼ����ӵ�������ȥ
	BOOL bSetEventImmediately = GSBInfo_InsertMsgPack(pGSBInfo,lpMsg);		

	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}

	//�ȴ��¼�
	GSBEvent_WaitEvent(eventWait,(UINT)-1);
	//ȡ�÷���ֵ,ע�⣬��ʱ�ķ���ֵ�Ѿ���GSB_ProcessMessage����������....
//	int nRet = (int)lpMsg->dwWaitEvent;	//delete by tbh 2006.04.03 ��������ֵ��Bug

#ifdef USE_FAST_WAIT_EVENT //����Event
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	GSB_EVENT_FREE(eventWait,pGSBInfo);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
#else
	//ɾ���¼�����
	GSBEvent_Deinit(eventWait);
#endif //USE_FAST_WAIT_EVENT


	return nRet;
}


LOCAL INLINE
void gsbFillModuleInfo(REGISTER LPGSBINFO pGSBInfo,LPGSBMSGEX lpMsgPack,LPMODULEINFO& pModuleInfo,MODULEINFO& moduleInfo)
{
	//�Ż�FindModule�Ĳ���
	if(NULL != lpMsgPack)
	{
		int CallbackType = lpMsgPack->nCallbackType & 0xFF;
		if(CT_NORMAL == CallbackType || CT_TIMERNORMAL == CallbackType )
		{
			LPMODULEINFO pSrcModuleInfo = GSBInfo_FindModuleInfo(pGSBInfo,lpMsgPack->nModuleDestID);
			if(NULL != pSrcModuleInfo)
			{
				moduleInfo = *pSrcModuleInfo;
				pModuleInfo = &moduleInfo;
			}
		}
	}// end if(NULL != pMsgPack)
}

LOCAL INLINE
void gsbCalcNextWaitTime(REGISTER LPGSBINFO pGSBInfo,TICKTYPE dwTickCount)
{
	//������һ�ζ�ʱʱ��
	LPGSBMSGEX lpTempMsgPack = pGSBInfo->m_listTimer.pHeadNode;
	if(NULL != lpTempMsgPack)
	{
		//�ȴ�һ����ʱ�䴥��
		if(lpTempMsgPack->dwNextTime > dwTickCount )
		{
			pGSBInfo->m_dwWaitTime = (lpTempMsgPack->dwNextTime - dwTickCount);
		}
		else	//���õȴ�,��һ�ζ�ʱ�����ϳ�ʱ
		{
			pGSBInfo->m_dwWaitTime = 0;
		}
	}
	else
	{
		//�����´εĵȴ�ʱ��
		pGSBInfo->m_dwWaitTime = LOOP_FOREVER;
	}
}

//�ɱ���ʱ�������һ�ζ���ʱ��
LOCAL INLINE
TICKTYPE gsbCalcJitterTime(REGISTER TICKTYPE nDelayTime,
						 REGISTER UINT uElase,
						 REGISTER UINT nJitterTime)
{
	BISPASSERT(uElase >= nJitterTime*2);

	if(0 != nJitterTime)
	{
		//���õ����λ��
		uElase -= nJitterTime;

		//�����µĶ���ʱ��
		uElase += nJitterTime*(UINT)rand()*2/RAND_MAX;
	}
	
	//���ض������ʱ��
	return (nDelayTime + uElase);
}


LOCAL INLINE
LPGSBMSGEX gsbWaitForMessage(REGISTER LPGSBINFO pGSBInfo, 
							 LPMODULEINFO& pModuleInfo,
							 MODULEINFO& moduleInfo,
							 DWORD dwWaitTime)
{
	int nCount;
	REGISTER TICKTYPE dwTickCount;
	REGISTER TICKTYPE dwDelayTime;
	TICKTYPE qWaitTime = (TICKTYPE)dwWaitTime;

	LPGSBMSGEX lpMsgPack = NULL;

	//ȡ�õ�ǰ��ʱ��TickCount
	dwTickCount = GET_TICK_COUNT(pGSBInfo);

	///////////////////////////////////////////////////////
	//1.���ȴ����ж���Ϣ
	if(pGSBInfo->m_nIntMsgCount > 0  )
	{
		//�����ж��͵���Ϣ���Ͷ����Ա�����ж϶�ջ�Ŀ���
		GSBMSGINT msg;
		msg.nBufLen = 0;

		//���ж�
		int nOldLevel = intLock();
		//˫���ж�
		if(pGSBInfo->m_nIntMsgCount > 0 )
		{
			nCount = gsbRngBufGet(&pGSBInfo->m_RingBuf,(char*)&msg,sizeof(GSBMSGINT));
			//�����жϺ����������
			pGSBInfo->m_nIntMsgCount --;
		}
		else
		{
			nCount = 0;
		}
		intUnlock(nOldLevel);

		//ȡ������
		if(sizeof(GSBMSGINT) ==  (size_t)nCount)
		{
			//bRet = TRUE;
			GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

			//���º��GSB_BuildMessage�ȼ�
			lpMsgPack = GSBInfo_AllocMsgPack(pGSBInfo,msg.nBufLen);
	
			if(NULL != lpMsgPack)
			{
				//��ֵ
				lpMsgPack->pNextMsg = NULL;
				lpMsgPack->pPrevMsg = NULL;
				lpMsgPack->nModuleSrcID = msg.nModuleSrcID;
				lpMsgPack->nModuleDestID = msg.nModuleDestID;
				lpMsgPack->nMsg = msg.nMsg;
				lpMsgPack->wParam = msg.wParam;
				lpMsgPack->lParam = msg.lParam;
				//����Ϊ��ǿ������
				lpMsgPack->nCallbackType = msg.nCallbackType;	//�ص�����
				lpMsgPack->lpfnCallback = msg.lpfnCallback;		//�����¼��ص�
			#if 0	//����ֻ��JobAdd������Ҫ���ö�ʱ����صĲ���
				lpMsgPack->nIDEvent = 0;						//��ʱ��ID
				lpMsgPack->dwDelayTime = 0;						//��һ�ε���ʱʱ��(��������)
				lpMsgPack->dwNextTime = 0;						//��һ�ε���ʱʱ��(������)
				lpMsgPack->nElapse = 0;							//��ʱ���
				lpMsgPack->nJitterTime = 0;						//����ʱ��(��������ʱ��)
				lpMsgPack->nLoopTimes = 0;						//ѭ������	
			#endif
				lpMsgPack->dwWaitEvent = 0;						//��Ϣ��ɵĵȴ��¼�,������Ϊ����ֵ����

				lpMsgPack->nBufLen = msg.nBufLen;

				if(msg.nBufLen > 0 )
				{		
					nOldLevel = intLock();
					//�ӻ��λ����и�������
					gsbRngBufGet(&pGSBInfo->m_RingBuf,(char*)GET_BUFFER(lpMsgPack),msg.nBufLen);
					intUnlock(nOldLevel);
				}
			}
			//���Module
			gsbFillModuleInfo(pGSBInfo,lpMsgPack,pModuleInfo,moduleInfo);

			//ȡ���´εȴ�ʱ��
			gsbCalcNextWaitTime(pGSBInfo,dwTickCount);

			GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		}	
		
	} //end if(pGSBInfo->m_nIntMsgCount > 0  )

	//////////////////////////////////////////////////////////////////////
	//2.�����ȴ�����ͨ��Ϣ
	if(NULL == lpMsgPack&&
		GSBList_Size(&pGSBInfo->m_listQueue) > 0  )
	{
		GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
		lpMsgPack = GSBList_PopFront(&pGSBInfo->m_listQueue);
		
		//���Module
		gsbFillModuleInfo(pGSBInfo,lpMsgPack,pModuleInfo,moduleInfo);
		
		//ȡ���´εȴ�ʱ��
		gsbCalcNextWaitTime(pGSBInfo,dwTickCount);

		GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	}

	//���û����Ϣ��Ҫ����Ž���ȴ�����
	if(NULL == lpMsgPack &&
		0 == pGSBInfo->m_nIntMsgCount)		//Fixed bug 2007.12.12
	{
		if(qWaitTime > pGSBInfo->m_dwWaitTime)
		{
			qWaitTime = pGSBInfo->m_dwWaitTime;
		}

		if(qWaitTime > 0)
		{
#ifdef NSL_OS_VXWORKS
			//ת���ɲ���ϵͳTickʱ�� 2007.11.14
			// �Եȴ�ʱ������жϣ��Ƿ����޵ȴ�? 2008.05.20
			if(LOOP_FOREVER != qWaitTime)
			{
				qWaitTime = qWaitTime*g_nSysRate/1000;
			}
#endif //WIN32

			//�����ȴ�.
			GSBEvent_WaitEvent(pGSBInfo->m_objEvent,(UINT)qWaitTime);
			
			//ȡ�õ�ǰ��ʱ��TickCount
			dwTickCount = GET_TICK_COUNT(pGSBInfo);
		}
	}
	
	//������һ�ε�Tick
	pGSBInfo->m_dwLastTickCount = dwTickCount;

	//////////////////////////////////////////////////////////////
	//3.��ʱ���Ĵ���
	if(NULL == lpMsgPack  && 
		GSBList_Size(&pGSBInfo->m_listTimer) > 0)
	{
		GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
		if(GSBList_Size(&pGSBInfo->m_listTimer) > 0 )
		{
			//ȡ�ö�ʱ�������ͷ���
			lpMsgPack = pGSBInfo->m_listTimer.pHeadNode;

			if(NULL != lpMsgPack &&
				lpMsgPack->dwNextTime <= dwTickCount) //�涨��ʱ�䵽��,���¼���Ҫ����
			{	

				if(LOOP_FOREVER != lpMsgPack->nLoopTimes)	//������ѭ����������1
				{
					lpMsgPack->nLoopTimes --;
				}

				//ɾ��ͷ�ڵ�
				TimerList_PopFront(&pGSBInfo->m_listTimer);

				//����Ҫ���ƶ�ʱ��Ϣ�ڵ�
				if(0 != lpMsgPack->nLoopTimes)
				{			
					//�ϴε��õĺ���ռ��ʱ����࣬��λ����
					if(lpMsgPack->dwDelayTime + lpMsgPack->nElapse <= dwTickCount)
					{
						dwDelayTime = dwTickCount;
					}
					else	//�����ϴε���ʱʱ��
					{
						//�������ƫ�Ʋ��������ǰһ�εķ�ʽ�������˶���ۻ����
						dwDelayTime = lpMsgPack->dwDelayTime; 
					}

					//������һ�εĶ���ʱ��
					lpMsgPack->dwNextTime = gsbCalcJitterTime(dwDelayTime,
																lpMsgPack->nElapse,
																lpMsgPack->nJitterTime);					

					//������һ�εĲ�������ʱ��
					lpMsgPack->dwDelayTime = dwDelayTime + lpMsgPack->nElapse;
					
					//������뵽����
					GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsgPack);				

					//���ƽڵ�
					lpMsgPack = GSBInfo_DuplicateMsgPack(pGSBInfo,lpMsgPack);

				}
			}
			else //û�ж�ʱ��Ϣ��Ҫ����
			{
				lpMsgPack = NULL;
			}
		}

		//���Module
		gsbFillModuleInfo(pGSBInfo,lpMsgPack,pModuleInfo,moduleInfo);

		//ȡ���´εȴ�ʱ��
		gsbCalcNextWaitTime(pGSBInfo,dwTickCount);

		GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	} //end if(NULL == lpMsgPack  )
	
	return lpMsgPack;
}

int WINAPI GSB_LoopMessage(SBHANDLE hBus,
							DWORD dwWaitTime)
{

	//��������:
	REGISTER LPGSBINFO pGSBInfo;
	LPGSBMSGEX lpMsgPack;
	MODULEINFO moduleInfo;
	LPMODULEINFO pModuleInfo = NULL;

	if(NULL == hBus)
	{
		return GSB_ERROR;
	}

	pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);

	//���޵ȴ�?
	if(dwWaitTime == MAX_WAIT_TIME)
	{
		//���޵ȴ�ģʽ
		do 
		{
			lpMsgPack = gsbWaitForMessage(pGSBInfo,pModuleInfo,moduleInfo,dwWaitTime);

		} while(NULL == lpMsgPack);
	}
	else	//��ʱ�ȴ�
	{
		TICKTYPE dwCurTick = GET_TICK_COUNT(pGSBInfo);
		TICKTYPE dwLastTick = dwCurTick + dwWaitTime;

		//��ʱ�ȴ�ģʽ
		for(;;) 
		{
			lpMsgPack = gsbWaitForMessage(pGSBInfo,pModuleInfo,moduleInfo,dwWaitTime);
			
			if(NULL != lpMsgPack )
			{
				break;
			}
			
			//�õ��µ�ʱ��
			dwCurTick = GET_TICK_COUNT(pGSBInfo);

			//�Ƿ��Ѿ���ʱ��?
			if(dwCurTick >= dwLastTick)
			{
				break;
			}
			
			//�����µĵȴ�ʱ��
			dwWaitTime = (DWORD)(dwLastTick - dwCurTick); 
		}

		//��Ϣ������û����Ϣ
		if(NULL == lpMsgPack)
		{
			//��ʱ����
			return GSB_TIMEOUT;
		}
	}

	//�յ��˳���Ϣ
	if(CT_EXIT == lpMsgPack->nCallbackType)	
	{
		//�����߳��˳���Ϣ
		return GSB_EXIT;
	}

	//������Ϣ
	GSB_ProcessMessage(pGSBInfo,lpMsgPack,pModuleInfo);

	//�ͷŴ�����ϵ���Դ
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	//�ͷ���Դ
	GSBInfo_FreeMsgPack(pGSBInfo,lpMsgPack);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���سɹ�
	return GSB_OK;

}

int WINAPI GSB_PeekMessage(SBHANDLE hBus,
							LPGSBMSG lpMsg,
							DWORD dwWaitTime)
{
	//��������:
	REGISTER LPGSBINFO pGSBInfo;
	LPGSBMSGEX lpMsgPack;
	MODULEINFO moduleInfo;
	LPMODULEINFO pModuleInfo = NULL;

	if(NULL == hBus || NULL == lpMsg)
	{
		return GSB_ERROR;
	}

	pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);

	//���޵ȴ�?
	if(dwWaitTime == MAX_WAIT_TIME)
	{
		//���޵ȴ�ģʽ
		do 
		{
			lpMsgPack = gsbWaitForMessage(pGSBInfo,pModuleInfo,moduleInfo,dwWaitTime);

		} while(NULL == lpMsgPack);
	}
	else	//��ʱ�ȴ�
	{
		TICKTYPE dwCurTick = GET_TICK_COUNT(pGSBInfo);
		TICKTYPE dwLastTick = dwCurTick + dwWaitTime;

		//��ʱ�ȴ�ģʽ
		for(;;) 
		{
			lpMsgPack = gsbWaitForMessage(pGSBInfo,pModuleInfo,moduleInfo,dwWaitTime);
			
			if(NULL != lpMsgPack )
			{
				break;
			}
			
			//�õ��µ�ʱ��
			dwCurTick = GET_TICK_COUNT(pGSBInfo);

			//�Ƿ��Ѿ���ʱ��?
			if(dwCurTick >= dwLastTick)
			{
				break;
			}
			
			//�����µĵȴ�ʱ��
			dwWaitTime = (DWORD)(dwLastTick - dwCurTick); 
		}

		//��Ϣ������û����Ϣ
		if(NULL == lpMsgPack)
		{
			//û����Ϣ��Ҫ����
			lpMsg->dwPrivate = 0;
			//��ʱ����
			return GSB_TIMEOUT;
		}
	}

	//�յ��˳���Ϣ
	if(CT_EXIT == lpMsgPack->nCallbackType)	
	{
		//�����߳��˳���Ϣ
		return GSB_EXIT;
	}	

	lpMsg->nModuleSrcID = lpMsgPack->nModuleSrcID;		//������������ģ��ID,nParam1
	lpMsg->nModuleDestID = lpMsgPack->nModuleDestID;	//Ŀ���������ģ��ID,nParam2
	lpMsg->nMsg = lpMsgPack->nMsg;						//��Ϣ,nParam3
	lpMsg->wParam = lpMsgPack->wParam;					//����1,nParam4
	lpMsg->lParam = lpMsgPack->lParam;					//����2,nParam5
	lpMsg->dwPrivate = (DWORD)lpMsgPack;

	//���سɹ�
	return GSB_OK;
}

int WINAPI GSB_DispatchMessage(SBHANDLE hBus,
							   LPGSBMSG lpMsg)
{
	if(NULL == hBus || NULL == lpMsg)
	{
		return -1;
	}
	int nRet = 0;

	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	
	//ȡ��ԭʼ������
	LPGSBMSGEX lpMsgPack = (LPGSBMSGEX)(lpMsg->dwPrivate);

	if(NULL != lpMsgPack)
	{
		//ȷ�����ݲ���
		BISPASSERT(lpMsgPack->nModuleDestID == lpMsg->nModuleDestID &&
					lpMsgPack->nModuleSrcID == lpMsg->nModuleSrcID &&
					lpMsgPack->nMsg == lpMsg->nMsg);

		//������Ϣ
		nRet = GSB_ProcessMessage(pGSBInfo,lpMsgPack,NULL);

		//�ͷŴ�����ϵ���Դ
		GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
		//�ͷ���Դ
		GSBInfo_FreeMsgPack(pGSBInfo,lpMsgPack);
		GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	}
		
	return nRet;
}

//�趨һ����ʱ������ʱ��
UINT WINAPI GSB_SetJitterTimer(SBHANDLE hBus,
						 UINT uElapse,
						 UINT nJitterTime,
						 UINT nLoopTimes,
						 GSBTIMERFUNCPTR lpfnRoutine,
						 LPCSTR lpBuffer,
						 int nBufLen,
						 int nParam1,
						 int nParam2,
						 int nParam3,
						 int nParam4,
						 int nParam5)
{

	//�������(ע������uElapse����Ϊ0�������) fixed by tbh 2006.04.24
	if(NULL == hBus || (0 == uElapse && nLoopTimes == LOOP_FOREVER) || 
				0 == nLoopTimes || NULL == lpfnRoutine)
	{
		return 0;
	}
	
	//��ʱ�������ܳ���50%
	if( (nJitterTime > 0) && (nJitterTime > (uElapse>>1)) )
	{
		nJitterTime = (uElapse>>1);
	}

	UINT nIDEvent;
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);

	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					lpBuffer,
					nBufLen,
					GSB_PRIORITY_DEFAULT);

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	lpMsg->nCallbackType = CT_TIMERJOBADD;		//JobAdd����
	lpMsg->dwWaitEvent = 0; //û�еȴ��¼�
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;
	
	//���϶�ʱ��ID��Ϊ0
	if( 0 == pGSBInfo->m_nCurrentTimerID)
	{
		pGSBInfo->m_nCurrentTimerID = 1;
	}
	//���ö�ʱ��ID
	nIDEvent = lpMsg->nIDEvent = pGSBInfo->m_nCurrentTimerID++;
				
	//���ö�ʱ���
	lpMsg->nElapse = uElapse;
	lpMsg->nJitterTime = nJitterTime;
	lpMsg->nLoopTimes = nLoopTimes;	
	
	//ȡ�õ�ǰʱ��
	TICKTYPE dwTickCount = GET_TICK_COUNT(pGSBInfo);

	//�����´β���������ʱ��
	lpMsg->dwDelayTime = dwTickCount + uElapse;
	//�����´κ�������ʱ��
	lpMsg->dwNextTime = gsbCalcJitterTime(dwTickCount,uElapse,nJitterTime);

	
	GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsg);
	//�������Ķ�ʱ����Ϣ��Ҫ���ϴ���,�򴥷�,�����ʾԭ���и����ȵĶ�ʱ����ǰ��Ⱥ�,
	//���ʱ��Ͳ���Ҫ������̨�߳���
	BOOL bSetEventImmediately = FALSE;

	//���Ҫ�����ϴ���,�������´δ�����ʱ��
	if(pGSBInfo->m_listTimer.pHeadNode == lpMsg &&
		( LOOP_FOREVER == pGSBInfo->m_dwWaitTime ||
		 pGSBInfo->m_dwLastTickCount + pGSBInfo->m_dwWaitTime > lpMsg->dwNextTime) )
	{
		bSetEventImmediately = TRUE;
	}

	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����

	//��Զ�ʱ�̵���Ϣ���ϴ�����̨��ʱ�߳�
	if(bSetEventImmediately)
	{
		//������ʱ���¼�
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return nIDEvent;

}



//��ʱ��֧��
UINT WINAPI GSB_SetTimer(SBHANDLE hBus,
						 UINT uElapse,
						 UINT nLoopTimes,
						 GSBTIMERFUNCPTR lpfnRoutine,
						 LPCSTR lpBuffer,
						 int nBufLen,
						 int nParam1,
						 int nParam2,
						 int nParam3,
						 int nParam4,
						 int nParam5)
{
	//ֱ�ӵ�����ʱ������ʱ��
	return GSB_SetJitterTimer(hBus,uElapse,0,nLoopTimes,lpfnRoutine,
								lpBuffer,nBufLen,nParam1,nParam2,nParam3,nParam4,nParam5);

}


//�趨һ����ʱ������ʱ��,���ٺ����ص��汾
UINT WINAPI GSB_SetJitterTimerFast(SBHANDLE hBus,
						 UINT uElapse,
						 UINT nJitterTime,
						 UINT nLoopTimes,
						 GSBFASTTIMERFUNCPTR lpfnRoutine,
						 int nParam1,
						 int nParam2,
						 int nParam3,
						 int nParam4,
						 int nParam5)
{
	//�������(ע������uElapse����Ϊ0�������) fixed by tbh 2006.04.24
	if(NULL == hBus || (0 == uElapse && nLoopTimes == LOOP_FOREVER) || 
				0 == nLoopTimes || NULL == lpfnRoutine)
	{
		return 0;
	}

	//��ʱ�������ܳ���50%
	if( (nJitterTime > 0) && (nJitterTime > (uElapse>>1)) )
	{
		nJitterTime = (uElapse>>1);
	}

	UINT nIDEvent;
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);

	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_FAST_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					GSB_PRIORITY_DEFAULT);

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	lpMsg->nCallbackType = CT_FASTTIMERJOBADD;		//FastTimerJobAdd����
	lpMsg->dwWaitEvent = 0; //û�еȴ��¼�
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;
	
	//���϶�ʱ��ID��Ϊ0
	if( 0 == pGSBInfo->m_nCurrentTimerID)
	{
		pGSBInfo->m_nCurrentTimerID = 1;
	}
	//���ö�ʱ��ID
	nIDEvent = lpMsg->nIDEvent = pGSBInfo->m_nCurrentTimerID++;

	//���ö�ʱ���
	lpMsg->nElapse = uElapse;
	lpMsg->nJitterTime = nJitterTime;
	lpMsg->nLoopTimes = nLoopTimes;	

	//ȡ�õ�ǰʱ��
	TICKTYPE dwTickCount = GET_TICK_COUNT(pGSBInfo);

	//�����´β���������ʱ��
	lpMsg->dwDelayTime = dwTickCount + uElapse;
	//�����´κ�������ʱ��
	lpMsg->dwNextTime = gsbCalcJitterTime(dwTickCount,uElapse,nJitterTime);
	
	GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsg);
	//�������Ķ�ʱ����Ϣ��Ҫ���ϴ���,�򴥷�,�����ʾԭ���и����ȵĶ�ʱ����ǰ��Ⱥ�,
	//���ʱ��Ͳ���Ҫ������̨�߳���
	BOOL bSetEventImmediately = FALSE;

	//���Ҫ�����ϴ���,�������´δ�����ʱ��
	if(pGSBInfo->m_listTimer.pHeadNode == lpMsg &&
		( LOOP_FOREVER == pGSBInfo->m_dwWaitTime ||
		 pGSBInfo->m_dwLastTickCount + pGSBInfo->m_dwWaitTime > lpMsg->dwNextTime) )
	{
		bSetEventImmediately = TRUE;
	}

	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����

	//��Զ�ʱ�̵���Ϣ���ϴ�����̨��ʱ�߳�
	if(bSetEventImmediately)
	{
		//������ʱ���¼�
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return nIDEvent;
}


//�趨һ����ʱ��,���ٺ����ص��汾
UINT WINAPI GSB_SetTimerFast(SBHANDLE hBus,
						 UINT uElapse,
						 UINT nLoopTimes,
						 GSBFASTTIMERFUNCPTR lpfnRoutine,
						 int nParam1,
						 int nParam2,
						 int nParam3,
						 int nParam4,
						 int nParam5)
{
	//ֱ�ӵ��ÿ�����ʱ������ʱ��
	return GSB_SetJitterTimerFast(hBus,uElapse,0,nLoopTimes,lpfnRoutine,
									nParam1,nParam2,nParam3,nParam4,nParam5);
}

//��ģ��IDΪ�������ö�����ʱ��
UINT WINAPI GSB_SetJitterTimerByID(SBHANDLE hBus,
						UINT uElapse,
						UINT nJitterTime,
						UINT nLoopTimes,
						UINT nModuleDestID,
						UINT nMsg,
						WPARAM wParam,
						LPARAM lParam,
						LPCSTR lpBuffer,
						int nLen)
{
	//�������(ע������uElapse����Ϊ0�������) fixed by tbh 2006.04.24
	if(NULL == hBus || (0 == uElapse && nLoopTimes == LOOP_FOREVER) || 0 == nLoopTimes)
	{
		return 0;
	}

	//��ʱ�������ܳ���50%
	if( (nJitterTime > 0) && (nJitterTime > (uElapse>>1)) )
	{
		nJitterTime = (uElapse>>1);
	}

	UINT nIDEvent;
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_MSG(lpMsg,pGSBInfo,
					(UINT)0,
					nModuleDestID,
					nMsg,
					wParam,
					lParam,
					lpBuffer,
					nLen,
					GSB_PRIORITY_DEFAULT);

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	lpMsg->nCallbackType = CT_TIMERNORMAL;		//JobAdd����
	lpMsg->dwWaitEvent = 0; //û�еȴ��¼�

	//���϶�ʱ��ID��Ϊ0
	if( 0 == pGSBInfo->m_nCurrentTimerID)
	{
		pGSBInfo->m_nCurrentTimerID = 1;
	}	
	//���ö�ʱ��ID
	nIDEvent = lpMsg->nIDEvent = pGSBInfo->m_nCurrentTimerID++;

	//���ö�ʱ���
	lpMsg->nElapse = uElapse;
	lpMsg->nJitterTime = nJitterTime;
	lpMsg->nLoopTimes = nLoopTimes;	

	//ȡ�õ�ǰʱ��
	TICKTYPE dwTickCount = GET_TICK_COUNT(pGSBInfo);

	//�����´β���������ʱ��
	lpMsg->dwDelayTime = dwTickCount + uElapse;
	//�����´κ�������ʱ��
	lpMsg->dwNextTime = gsbCalcJitterTime(dwTickCount,uElapse,nJitterTime);

	
	GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsg);
	//�������Ķ�ʱ����Ϣ��Ҫ���ϴ���,�򴥷�,�����ʾԭ���и����ȵĶ�ʱ����ǰ��Ⱥ�,
	//���ʱ��Ͳ���Ҫ������̨�߳���
	BOOL bSetEventImmediately = FALSE;

	//���Ҫ�����ϴ���,�������´δ�����ʱ��
	if(pGSBInfo->m_listTimer.pHeadNode == lpMsg &&
		( LOOP_FOREVER == pGSBInfo->m_dwWaitTime ||
		 pGSBInfo->m_dwLastTickCount + pGSBInfo->m_dwWaitTime > lpMsg->dwNextTime) )
	{
		bSetEventImmediately = TRUE;
	}

	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����

	//��Զ�ʱ�̵���Ϣ���ϴ�����̨��ʱ�߳�
	if(bSetEventImmediately)
	{
		//������ʱ���¼�
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	
	return nIDEvent;

}

//��ģ��IDΪ�������ö�ʱ��
UINT WINAPI GSB_SetTimerByID(SBHANDLE hBus,
						UINT uElapse,
						UINT nLoopTimes,
						UINT nModuleDestID,
						UINT nMsg,
						WPARAM wParam,
						LPARAM lParam,
						LPCSTR lpBuffer,
						int nLen)
{
	//ֱ�ӵ�����ʱ������ʱ��
	return GSB_SetJitterTimerByID(hBus,uElapse,0,nLoopTimes,nModuleDestID,nMsg,
								wParam,lParam,lpBuffer,nLen);
}

//ȡ����ʱ��
BOOL WINAPI GSB_KillTimer(SBHANDLE hBus,
							UINT nTimer)
{
	if(NULL == hBus || 0 == nTimer)
	{
		return FALSE;
	}

	BOOL fOK = FALSE;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	LPGSBMSGEX lpTimerMsg;
	GSBCrisec_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	for(lpTimerMsg = pGSBInfo->m_listTimer.pHeadNode;lpTimerMsg;lpTimerMsg=lpTimerMsg->pNextMsg)
	{
		if(nTimer == lpTimerMsg->nIDEvent)
		{

			TimerList_DeleteNode(&pGSBInfo->m_listTimer,lpTimerMsg);
			//�Ѷ�ʱ��Ϣ�ӵ����������β�� modify by tbh 2005.06.25
			
			GSBInfo_FreeMsgPack(pGSBInfo,lpTimerMsg);
		
			fOK = TRUE;
			break;
		}
	}
	GSBCrisec_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	
	return fOK;
}

//ȡ��ʱ�䴥��ʣ��ʱ��
UINT WINAPI GSB_GetTimerLeftSleep(SBHANDLE hBus,
						UINT nTimer)
{
	if(NULL == hBus || 0 == nTimer)
	{
		return 0;
	}

	TICKTYPE nTimeLeft = 0;

	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	LPGSBMSGEX lpTimerMsg;
	GSBCrisec_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	for(lpTimerMsg = pGSBInfo->m_listTimer.pHeadNode;lpTimerMsg;lpTimerMsg=lpTimerMsg->pNextMsg)
	{
		if(nTimer == lpTimerMsg->nIDEvent)
		{
			//ȡ����һ�δ���ʣ��ʱ�� = ��һ��ʱ�� - ��ǰʱ��		
			nTimeLeft = lpTimerMsg->dwNextTime - GET_TICK_COUNT(pGSBInfo);
			break;
		}
	}
	GSBCrisec_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	
	return (UINT)nTimeLeft;
}

//�¼�����ֱ�ӻص�
BOOL WINAPI GSB_JobAdd(SBHANDLE hBus,
					   GSBFUNCPTR lpfnRoutine,
					   LPCSTR lpBuffer,
					   int nBufLen,
					   int nParam1, 
					   int nParam2, 
					   int nParam3,
					   int nParam4, 
					   int nParam5)
{
	if(NULL == hBus || NULL == lpfnRoutine)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					lpBuffer,
					nBufLen,
					GSB_PRIORITY_DEFAULT);

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	

	lpMsg->nCallbackType = CT_JOBADD;		//JobAdd����
	lpMsg->dwWaitEvent = 0; //û�еȴ��¼�
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//���ӵ�������ȥ
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;
}


//�¼�����ֱ�ӻص�
BOOL WINAPI GSB_JobAddEx(SBHANDLE hBus,
					   GSBFUNCPTR lpfnRoutine,
					   LPCSTR lpBuffer,
					   int nBufLen,
					   int nParam1, 
					   int nParam2, 
					   int nParam3,
					   int nParam4, 
					   int nParam5,
					   UINT nPriority)
{
	if(NULL == hBus || NULL == lpfnRoutine)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					lpBuffer,
					nBufLen,
					(nPriority & 0xFF));

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	

	lpMsg->nCallbackType = CT_JOBADD;		//JobAdd����
	lpMsg->dwWaitEvent = 0; //û�еȴ��¼�
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	//�������ȼ����ӵ�������ȥ
	BOOL bSetEventImmediately = GSBInfo_InsertMsgPack(pGSBInfo,lpMsg);		

	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;
}


//�¼��������ٻص�,Ϊ�첽��������
BOOL WINAPI GSB_JobAddFast(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5)
{
	if(NULL == hBus || NULL == lpfnRoutine)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_FAST_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					GSB_PRIORITY_DEFAULT);

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	

	lpMsg->nCallbackType = CT_FASTJOBADD;		//JobAdd����
	lpMsg->dwWaitEvent = 0; //û�еȴ��¼�
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//���ӵ�������ȥ
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;
}

//�¼��������ٻص�,Ϊ�첽��������
BOOL WINAPI GSB_JobAddFastEx(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5,
								UINT nPriority)
{
	if(NULL == hBus || NULL == lpfnRoutine)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_FAST_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					(nPriority & 0xFF));

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	

	lpMsg->nCallbackType = CT_FASTJOBADD;		//JobAdd����
	lpMsg->dwWaitEvent = 0; //û�еȴ��¼�
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	//�������ȼ����ӵ�������ȥ
	BOOL bSetEventImmediately = GSBInfo_InsertMsgPack(pGSBInfo,lpMsg);		

	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;
}


//�¼�����ֱ�ӻص�,�˺����ȴ�������ŷ���,����ֵΪlpfnRoutine�ķ���ֵ
int WINAPI GSB_WaitJobAdd(SBHANDLE hBus,
								GSBFUNCPTR lpfnRoutine,
								LPCSTR lpBuffer,
								int nLen,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5)
{
	if(NULL == hBus || NULL == lpfnRoutine)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	if(GETTASKID() == pGSBInfo->m_nThreadID)	//����Ǳ��̣߳�ֱ�ӵ���
	{
		return (*lpfnRoutine)(lpBuffer,nLen,nParam1,nParam2,nParam3,nParam4,nParam5);
	}
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					lpBuffer,
					nLen,
					GSB_PRIORITY_DEFAULT);
	
	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return 0;
	}

	GSB_EVENT eventWait;		//����һ���ȴ��¼�
#ifdef USE_FAST_WAIT_EVENT //����Event
	GSB_EVENT_ALLOC(eventWait,pGSBInfo);
#else
	GSBEvent_Init(eventWait);
#endif //USE_FAST_WAIT_EVENT
	lpMsg->dwWaitEvent = (DWORD)eventWait; //����Ϣ֪ͨ�������

	int nRet = 0;							//add by tbh 2006.04.03 ��������ֵ��Bug
	//nTimerIdָ�򱣴淵��ֵ��intָ��
	lpMsg->nIDEvent = (UINT)&nRet;			//add by tbh 2006.04.03 ��������ֵ��Bug

	lpMsg->nCallbackType = CT_JOBADD;		//JobAdd����
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;
	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//���ӵ�������ȥ
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}

	//�ȴ��¼�
	GSBEvent_WaitEvent(eventWait,(UINT)-1);
	//ȡ�÷���ֵ,ע�⣬��ʱ�ķ���ֵ�Ѿ���GSB_ProcessMessage����������....
//	int nRet = (int)lpMsg->dwWaitEvent;	//delete by tbh 2006.04.03 ��������ֵ��Bug

#ifdef USE_FAST_WAIT_EVENT //����Event
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	GSB_EVENT_FREE(eventWait,pGSBInfo);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
#else
	//ɾ���¼�����
	GSBEvent_Deinit(eventWait);
#endif //USE_FAST_WAIT_EVENT

	return nRet;
}


//�¼�����ֱ�ӻص�,�˺����ȴ�������ŷ���,����ֵΪlpfnRoutine�ķ���ֵ
int WINAPI GSB_WaitJobAddEx(SBHANDLE hBus,
								GSBFUNCPTR lpfnRoutine,
								LPCSTR lpBuffer,
								int nLen,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5,
								UINT nPriority)
{
	if(NULL == hBus || NULL == lpfnRoutine)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	if(GETTASKID() == pGSBInfo->m_nThreadID)	//����Ǳ��̣߳�ֱ�ӵ���
	{
		return (*lpfnRoutine)(lpBuffer,nLen,nParam1,nParam2,nParam3,nParam4,nParam5);
	}
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					lpBuffer,
					nLen,
					(nPriority & 0xFF) );
	
	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return 0;
	}

	GSB_EVENT eventWait;		//����һ���ȴ��¼�
#ifdef USE_FAST_WAIT_EVENT //����Event
	GSB_EVENT_ALLOC(eventWait,pGSBInfo);
#else
	GSBEvent_Init(eventWait);
#endif //USE_FAST_WAIT_EVENT
	lpMsg->dwWaitEvent = (DWORD)eventWait; //����Ϣ֪ͨ�������

	int nRet = 0;							//add by tbh 2006.04.03 ��������ֵ��Bug
	//nTimerIdָ�򱣴淵��ֵ��intָ��
	lpMsg->nIDEvent = (UINT)&nRet;			//add by tbh 2006.04.03 ��������ֵ��Bug

	lpMsg->nCallbackType = CT_JOBADD;		//JobAdd����
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	//�������ȼ����ӵ�������ȥ
	BOOL bSetEventImmediately = GSBInfo_InsertMsgPack(pGSBInfo,lpMsg);		

	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}

	//�ȴ��¼�
	GSBEvent_WaitEvent(eventWait,(UINT)-1);
	//ȡ�÷���ֵ,ע�⣬��ʱ�ķ���ֵ�Ѿ���GSB_ProcessMessage����������....
//	int nRet = (int)lpMsg->dwWaitEvent;	//delete by tbh 2006.04.03 ��������ֵ��Bug

#ifdef USE_FAST_WAIT_EVENT //����Event
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	GSB_EVENT_FREE(eventWait,pGSBInfo);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
#else
	//ɾ���¼�����
	GSBEvent_Deinit(eventWait);
#endif //USE_FAST_WAIT_EVENT

	return nRet;
}

//�¼��������ٻص�,Ϊͬ����������
int WINAPI GSB_WaitJobAddFast(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5)
{
	if(NULL == hBus || NULL == lpfnRoutine)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	if(GETTASKID() == pGSBInfo->m_nThreadID)	//����Ǳ��̣߳�ֱ�ӵ���
	{
		return (*lpfnRoutine)(nParam1,nParam2,nParam3,nParam4,nParam5);
	}
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_FAST_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					GSB_PRIORITY_DEFAULT);
	
	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return 0;
	}

	GSB_EVENT eventWait;		//����һ���ȴ��¼�
#ifdef USE_FAST_WAIT_EVENT //����Event
	GSB_EVENT_ALLOC(eventWait,pGSBInfo);
#else
	GSBEvent_Init(eventWait);
#endif //USE_FAST_WAIT_EVENT
	lpMsg->dwWaitEvent = (DWORD)eventWait; //����Ϣ֪ͨ�������

	int nRet = 0;							//add by tbh 2006.04.03 ��������ֵ��Bug
	//nTimerIdָ�򱣴淵��ֵ��intָ��
	lpMsg->nIDEvent = (UINT)&nRet;			//add by tbh 2006.04.03 ��������ֵ��Bug

	lpMsg->nCallbackType = CT_FASTJOBADD;		//JobAdd����
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;
	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//���ӵ�������ȥ
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}

	//�ȴ��¼�
	GSBEvent_WaitEvent(eventWait,(UINT)-1);
	//ȡ�÷���ֵ,ע�⣬��ʱ�ķ���ֵ�Ѿ���GSB_ProcessMessage����������....
//	int nRet = (int)lpMsg->dwWaitEvent;	//delete by tbh 2006.04.03 ��������ֵ��Bug

#ifdef USE_FAST_WAIT_EVENT //����Event
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	GSB_EVENT_FREE(eventWait,pGSBInfo);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
#else
	//ɾ���¼�����
	GSBEvent_Deinit(eventWait);
#endif //USE_FAST_WAIT_EVENT

	return nRet;	
}

//�¼��������ٻص�,Ϊͬ����������
int WINAPI GSB_WaitJobAddFastEx(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5,
								UINT nPriority)
{

	if(NULL == hBus || NULL == lpfnRoutine)
	{
		return FALSE;
	}
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	if(GETTASKID() == pGSBInfo->m_nThreadID)	//����Ǳ��̣߳�ֱ�ӵ���
	{
		return (*lpfnRoutine)(nParam1,nParam2,nParam3,nParam4,nParam5);
	}
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_FAST_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					(nPriority & 0xFF) );
	
	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return 0;
	}

	GSB_EVENT eventWait;		//����һ���ȴ��¼�
#ifdef USE_FAST_WAIT_EVENT //����Event
	GSB_EVENT_ALLOC(eventWait,pGSBInfo);
#else
	GSBEvent_Init(eventWait);
#endif //USE_FAST_WAIT_EVENT
	lpMsg->dwWaitEvent = (DWORD)eventWait; //����Ϣ֪ͨ�������

	int nRet = 0;							//add by tbh 2006.04.03 ��������ֵ��Bug
	//nTimerIdָ�򱣴淵��ֵ��intָ��
	lpMsg->nIDEvent = (UINT)&nRet;			//add by tbh 2006.04.03 ��������ֵ��Bug

	lpMsg->nCallbackType = CT_FASTJOBADD;		//JobAdd����
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	//�������ȼ����ӵ�������ȥ
	BOOL bSetEventImmediately = GSBInfo_InsertMsgPack(pGSBInfo,lpMsg);		

	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//����֪ͨǰ̨
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}

	//�ȴ��¼�
	GSBEvent_WaitEvent(eventWait,(UINT)-1);
	//ȡ�÷���ֵ,ע�⣬��ʱ�ķ���ֵ�Ѿ���GSB_ProcessMessage����������....
//	int nRet = (int)lpMsg->dwWaitEvent;	//delete by tbh 2006.04.03 ��������ֵ��Bug

#ifdef USE_FAST_WAIT_EVENT //����Event
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	GSB_EVENT_FREE(eventWait,pGSBInfo);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
#else
	//ɾ���¼�����
	GSBEvent_Deinit(eventWait);
#endif //USE_FAST_WAIT_EVENT

	return nRet;
}


//�¼�����ֱ�ӻص�
UINT WINAPI GSB_DelayJobAdd(SBHANDLE hBus,
								GSBFUNCPTR lpfnRoutine,
								LPCSTR lpBuffer,
								int nLen,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5,
								DWORD dwDelayTime)
{
	if(0 == dwDelayTime)
	{
		GSB_JobAdd(hBus,lpfnRoutine,lpBuffer,nLen,nParam1,nParam2,nParam3,nParam4,nParam5);
		return 0;
	}
	
	//�������
	UINT nIDEvent;
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					lpBuffer,
					nLen,
					GSB_PRIORITY_DEFAULT);

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	lpMsg->nCallbackType = CT_JOBADD;		//JobAdd����
	lpMsg->dwWaitEvent = 0; //û�еȴ��¼�
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	//���϶�ʱ��ID��Ϊ0
	if( 0 == pGSBInfo->m_nCurrentTimerID)
	{
		pGSBInfo->m_nCurrentTimerID = 1;
	}
	//���ö�ʱ��ID
	nIDEvent = lpMsg->nIDEvent = pGSBInfo->m_nCurrentTimerID++;

	//���ö�ʱ���
	lpMsg->nElapse = dwDelayTime;
	lpMsg->nJitterTime = 0;
	lpMsg->nLoopTimes = 1;						//ֻѭ��һ��

	//ȡ�õ�ǰʱ��
	TICKTYPE dwTickCount = GET_TICK_COUNT(pGSBInfo);

	//�����´ε���ʱʱ��
	dwTickCount += dwDelayTime;
	lpMsg->dwNextTime = lpMsg->dwDelayTime = dwTickCount;
	
	GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsg);
	//�������Ķ�ʱ����Ϣ��Ҫ���ϴ���,�򴥷�,�����ʾԭ���и����ȵĶ�ʱ����ǰ��Ⱥ�,
	//���ʱ��Ͳ���Ҫ������̨�߳���
	BOOL bSetEventImmediately = FALSE;

	//���Ҫ�����ϴ���,�������´δ�����ʱ��
	if(pGSBInfo->m_listTimer.pHeadNode == lpMsg &&
		( LOOP_FOREVER == pGSBInfo->m_dwWaitTime ||
		 pGSBInfo->m_dwLastTickCount + pGSBInfo->m_dwWaitTime > lpMsg->dwNextTime) )
	{
		bSetEventImmediately = TRUE;
	}

	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����

	//��Զ�ʱ�̵���Ϣ���ϴ�����̨��ʱ�߳�
	if(bSetEventImmediately)
	{
		//������ʱ���¼�
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return nIDEvent;
}

//�¼�������ʱ�첽���ٻص�
UINT WINAPI GSB_DelayJobAddFast(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5,
								DWORD dwDelayTime)
{
	if(0 == dwDelayTime)
	{
		GSB_JobAddFast(hBus,lpfnRoutine,nParam1,nParam2,nParam3,nParam4,nParam5);
		return 0;
	}
	
	//�������
	UINT nIDEvent;
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����

	//���º��GSB_BuildMessage�ȼ�
	BUILD_FAST_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					GSB_PRIORITY_DEFAULT);

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����
		return FALSE;
	}
	lpMsg->nCallbackType = CT_FASTJOBADD;		//JobAdd����
	lpMsg->dwWaitEvent = 0; //û�еȴ��¼�
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	//���϶�ʱ��ID��Ϊ0
	if( 0 == pGSBInfo->m_nCurrentTimerID)
	{
		pGSBInfo->m_nCurrentTimerID = 1;
	}
	//���ö�ʱ��ID
	nIDEvent = lpMsg->nIDEvent = pGSBInfo->m_nCurrentTimerID++;

	//���ö�ʱ���
	lpMsg->nElapse = dwDelayTime;
	lpMsg->nJitterTime = 0;
	lpMsg->nLoopTimes = 1;						//ֻѭ��һ��

	//ȡ�õ�ǰʱ��
	TICKTYPE dwTickCount = GET_TICK_COUNT(pGSBInfo);

	//�����´ε���ʱʱ��
	dwTickCount += dwDelayTime;
	lpMsg->dwNextTime = lpMsg->dwDelayTime = dwTickCount;
	
	GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsg);
	//�������Ķ�ʱ����Ϣ��Ҫ���ϴ���,�򴥷�,�����ʾԭ���и����ȵĶ�ʱ����ǰ��Ⱥ�,
	//���ʱ��Ͳ���Ҫ������̨�߳���
	BOOL bSetEventImmediately = FALSE;

	//���Ҫ�����ϴ���,�������´δ�����ʱ��
	if(pGSBInfo->m_listTimer.pHeadNode == lpMsg &&
		( LOOP_FOREVER == pGSBInfo->m_dwWaitTime ||
		 pGSBInfo->m_dwLastTickCount + pGSBInfo->m_dwWaitTime > lpMsg->dwNextTime) )
	{
		bSetEventImmediately = TRUE;
	}

	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module���ݷ���ͬ��,����

	//��Զ�ʱ�̵���Ϣ���ϴ�����̨��ʱ�߳�
	if(bSetEventImmediately)
	{
		//������ʱ���¼�
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return nIDEvent;
}

//ȡ��ָ��ģ������ж�ʱ��add by tbh 2006.01.06
BOOL WINAPI GSB_KillAllModuleTimer(SBHANDLE hBus,
						UINT nModuleId)
{
	if(NULL == hBus)
	{
		return FALSE;
	}
	BOOL fOK = FALSE;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	LPGSBMSGEX lpTimerMsg;
	LPGSBMSGEX lpTempTimerMsg;
	//�ͷŴ�����ϵ���Դ
	GSBFast_Lock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	for(lpTimerMsg = pGSBInfo->m_listTimer.pHeadNode;lpTimerMsg; )
	{	
		lpTempTimerMsg = lpTimerMsg->pNextMsg;

		if(nModuleId == lpTimerMsg->nModuleDestID)
		{
			TimerList_DeleteNode(&pGSBInfo->m_listTimer,lpTimerMsg);
			//�ͷ���Դ
			GSBInfo_FreeMsgPack(pGSBInfo,lpTimerMsg);
			fOK = TRUE;
		}

		lpTimerMsg = lpTempTimerMsg;
	}
	GSBFast_Unlock(pGSBInfo->m_csLock);		//���ݷ���ͬ��,����
	
	return fOK;
}

//ȡ�������ϵͳ��ص��¼����
UINT WINAPI GSB_GetSystemMsgEvent(SBHANDLE hBus)
{
	if(NULL == hBus)
	{
		return 0;
	}
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	
	return (UINT)pGSBInfo->m_objEvent;
}


BOOL WINAPI GSB_IntPostMessage(SBHANDLE hBus,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,
							LPCSTR lpBuffer,
							int nBufLen)
{


	if(NULL == hBus)
	{
		return FALSE;
	}

	REGISTER int nOldLevel;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;

	//�����ж��͵���Ϣ���Ͷ����Ա�����ж϶�ջ�Ŀ���
	GSBMSGINT msg;

	LPCSTR lpszTemp;
	
	if(nBufLen < 0 )
	{
		//nBufLen = strlen(lpBuffer);
		lpszTemp = lpBuffer;

		while('\0' != *lpszTemp)
		{
			lpszTemp ++;
		}
		//ȡ���ַ�������
		nBufLen = lpszTemp - lpBuffer;
	}

	//2007.08.23 ������ƵĻ����С�����Ż���С����������
	if((DWORD)nBufLen > pGSBInfo->m_dwMaxBufferSize ||
		nBufLen > (int)0xffff )
	{
		return FALSE;
	}

	//���ò���
	msg.nModuleSrcID = nModuleSrcID;
	msg.nModuleDestID = nModuleDestID;
	msg.nMsg = nMsg;
	msg.wParam = wParam;
	msg.lParam = lParam;
	
	msg.nCallbackType = CT_NORMAL;			//��ͨ�ص�����
	msg.nBufLen = (UINT16)nBufLen;					//���峤��

	//���÷���ֵ
	BOOL bRet = FALSE;

	//���ж�
	nOldLevel = intLock();

	if(gsbRngFreeBytes(&pGSBInfo->m_RingBuf) <= ((int)sizeof(GSBMSGINT) + nBufLen) )
	{
#ifdef NSL_OS_VXWORKS
		logMsg((char*)"GSB_IntPostMessage(%p): ring buf over flow!\n",(int)hBus,0,0,0,0,0);
#endif //WIN32
		//�����ж�
		intUnlock (nOldLevel);
		return FALSE;		
	}

	//��¼ԭ����λ��
	int nOldPos = pGSBInfo->m_RingBuf.pToBuf;

	if( sizeof(GSBMSGINT) == (size_t)gsbRngBufPut(&pGSBInfo->m_RingBuf,(char*)&msg,(int)sizeof(GSBMSGINT)) )
	{
		//���Ƴɹ�
		bRet = TRUE;
		
		//���ƻ���
		if(nBufLen > 0 )
		{
			//����ʧ��?
			if(nBufLen != gsbRngBufPut(&pGSBInfo->m_RingBuf,(char*)lpBuffer,nBufLen) )
			{
				bRet = FALSE;	
			}
		}
	}

	//������Ƴɹ���������һ����Ϣ������ԭλ��
	if(bRet)
	{
		pGSBInfo->m_nIntMsgCount ++;
	}
	else
	{
		pGSBInfo->m_RingBuf.pToBuf = nOldPos;
	}
	

	//�����ж�
	intUnlock (nOldLevel);

	//����֪ͨǰ̨
	GSBEvent_SetEvent(pGSBInfo->m_objEvent);	

	return bRet;
}

BOOL WINAPI GSB_IntJobAdd(SBHANDLE hBus,
								GSBFUNCPTR lpfnRoutine,
								LPCSTR lpBuffer,
								int nBufLen,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5)
{
	if(NULL == hBus)
	{
		return FALSE;
	}

	REGISTER int nOldLevel;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;

	//�����ж��͵���Ϣ���Ͷ����Ա�����ж϶�ջ�Ŀ���
	GSBMSGINT msg;
	LPCSTR lpszTemp;
	
	if(nBufLen < 0 )
	{
		//nBufLen = strlen(lpBuffer);
		lpszTemp = lpBuffer;

		while('\0' != *lpszTemp)
		{
			lpszTemp ++;
		}
		//ȡ���ַ�������
		nBufLen = lpszTemp - lpBuffer;
	}

	//2007.08.23 ������ƵĻ����С�����Ż���С����������
	if((DWORD)nBufLen > pGSBInfo->m_dwMaxBufferSize ||
		nBufLen > (int)0xffff )
	{
		return FALSE;
	}

	//���ò���
	msg.nModuleSrcID = (UINT)nParam1;
	msg.nModuleDestID = (UINT)nParam2;
	msg.nMsg = (UINT)nParam3;
	msg.wParam = (WPARAM)nParam4;
	msg.lParam = (LPARAM)nParam5;
	
	msg.nCallbackType = CT_JOBADD;						//Add Job����
	msg.lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;		//�ص�����
	msg.nBufLen = (UINT16)nBufLen;								//���峤��

	//���÷���ֵ
	BOOL bRet = FALSE;

	//���ж�
	nOldLevel = intLock();
	
	if(gsbRngFreeBytes(&pGSBInfo->m_RingBuf) <= ((int)sizeof(GSBMSGINT) + nBufLen) )
	{
#ifdef NSL_OS_VXWORKS
		logMsg((char*)"GSB_IntJobAdd(%p): ring buf over flow!\n",(int)hBus,0,0,0,0,0);
#endif //WIN32
		//�����ж�
		intUnlock (nOldLevel);
		return FALSE;		
	}

	//��¼ԭ����λ��
	int nOldPos = pGSBInfo->m_RingBuf.pToBuf;

	if( sizeof(GSBMSGINT) == (size_t)gsbRngBufPut(&pGSBInfo->m_RingBuf,(char*)&msg,(int)sizeof(GSBMSGINT)) )
	{
		//���Ƴɹ�
		bRet = TRUE;
		
		//���ƻ���
		if(nBufLen > 0 )
		{
			//����ʧ��?
			if(nBufLen != gsbRngBufPut(&pGSBInfo->m_RingBuf,(char*)lpBuffer,nBufLen) )
			{
				bRet = FALSE;	
			}
		}
	}

	//������Ƴɹ���������һ����Ϣ������ԭλ��
	if(bRet)
	{
		pGSBInfo->m_nIntMsgCount ++;
	}
	else
	{
		pGSBInfo->m_RingBuf.pToBuf = nOldPos;
	}
	

	//�����ж�
	intUnlock (nOldLevel);

	//����֪ͨǰ̨
	GSBEvent_SetEvent(pGSBInfo->m_objEvent);	

	return bRet;

}


//INT Job Add�Ŀ��ٰ汾
BOOL WINAPI GSB_IntJobAddFast(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5)
{
	if(NULL == hBus)
	{
		return FALSE;
	}

	REGISTER int nOldLevel;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;

	//�����ж��͵���Ϣ���Ͷ����Ա�����ж϶�ջ�Ŀ���
	GSBMSGINT msg;

	//���ò���
	msg.nModuleSrcID = (UINT)nParam1;
	msg.nModuleDestID = (UINT)nParam2;
	msg.nMsg = (UINT)nParam3;
	msg.wParam = (WPARAM)nParam4;
	msg.lParam = (LPARAM)nParam5;
	
	msg.nCallbackType = CT_FASTJOBADD;						//Add Job����
	msg.lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;			//�ص�����
	msg.nBufLen = 0;										//���峤��

	//���ж�
	nOldLevel = intLock();
	
	if(gsbRngFreeBytes(&pGSBInfo->m_RingBuf) <= ((int)sizeof(GSBMSGINT)) )
	{
#ifdef NSL_OS_VXWORKS
		logMsg((char*)"GSB_IntJobAddFast(%p): ring buf over flow!\n",(int)hBus,0,0,0,0,0);
#endif //WIN32
		//�����ж�
		intUnlock (nOldLevel);
		return FALSE;		
	}

	//������Ϣ���е����λ�����
	gsbRngBufPut(&pGSBInfo->m_RingBuf,(char*)&msg,sizeof(GSBMSGINT) );

	//���Ƴɹ�������һ����Ϣ
	pGSBInfo->m_nIntMsgCount ++;

	//�����ж�
	intUnlock (nOldLevel);

	//����֪ͨǰ̨
	GSBEvent_SetEvent(pGSBInfo->m_objEvent);	

	return TRUE;

}














