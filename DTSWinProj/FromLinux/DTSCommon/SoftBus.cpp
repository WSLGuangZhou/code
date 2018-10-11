///////////////////////////////////////////////////////////////
//	文 件 名 : SoftBus.cpp
//	文件功能 : BISP软件框架核心软件总线实现(V3)
//	作    者 : tbh
//	创建时间 : 2005年4月
//	项目名称 : 
//	备    注 : 当前版本 V3.2.20080707
//	历史记录 : 
//			V2版本对于V1版本的改进:
//			   2005.04.15 根据原有的基于STL作为链表结构的软件总线加入定时功能重新改写
//			   2005.04.20 初步测试通过
//			   2005.07.28 进一步进行优化，用GSBFast_Lock代替GSBCrisec_Lock
//			   2005.08.24 修正了定时器中多次重复投递同一个总线消息的Bug,
//							在GSB_KillTimer函数中回收总线节点内存
//			   2006.01.05 增加GSB_KillAllModuleTimer函数，并在删除模块时调用
//							GSB_KillAllModuleTimer函数，修正模块删除后定时器还起作用的Bug
//			   2006.01.06 增加GSB_GetSystemMsgEvent函数，提供对Windows系统类似MsgWaitForMutipleObjects的支持
//			   2006.01.18 修正通播模块目标ID为0xFFFFFFFF的Bug
//			   2006.04.03 在SendMessage调用或WaitJobAdd调用中,把GSBMSG中的成员变量nTimerId(定时器序号)
//							作为返回值指向的指针传递返回值,修正SendMessage调用或WaitJobAdd调用中有时候返回值变成0的Bug
//			   2006.04.24 增加对0定时器的处理，允许在GSB_SetTimer和GSB_SetTimerByID函数中的参数UINT uElapse,
//							在uElapse == 0时函数调用不会失败,这样相当于增加一个可以自由删除的JobAdd的功能
//				2006.10.26 修改#include "HNCDebug.h"为#include "BISPDebug.h",使之融入BISP平台项目
//			V3版本历史记录:
//				2007.01.31 修正在gsbWaitForMessage函数中可能会丢掉一条消息的处理
//				2007.04.14 根据vxworks ringLib.c完善了gsbRngBufGet、gsbRngBufPut函数
//							去掉结构体1字节对其的PACKET_STRUCT定义，让CPU去优化内存访问
//							改进了gshGetModuleHashIndex函数的HASH算法
//							增加GSB_PostQuitMessage函数，使其能安全正常的退出
//				2007.04.27 修正了GSB_PostQuitMessage函数中Release版本编译的BUG，增加一个BUILD_NULL_MSG宏
//							用重复利用的事件加快了WaitJobAdd函数的响应速度,参见USE_FAST_WAIT_EVENT
//				2007.07.28 修正了内存分配为61-63字节时缓冲越界引起非法指针操作的BUG
//				2007.08.23 修正了在GSB_Create函数中把MALLOC_BLOCK_SIZE当做元素个数的BUG，该BUG导致在指定
//							MaxBufferSize为2K时分配32M的内存，从而出现内存不够的情况
//							GSB_BuildMessage函数，删除对最大消息长度的检查，支持不限制长度大小的消息发送
//							GSBInfo_AllocMsgPack函数，增加对内存分配失败时的容错处理
//							在GSB_IntPostMessage和GSB_IntJobAdd函数中,如果复制的缓冲大小多于优化大小，则不允许发送
//				2007.11.03  修正在gsbWaitForMessage函数中处理中断消息intLock时间过长的问题
//				2007.11.08  修正GSB_IntJobAdd的BUG问题,该BUG令GSB_IntJobAdd无法像预期一样工作
//				2007.11.14  修正m_dwWaitTime变量以毫秒或vxworks tick为单位时不一致的问题，该问题导致定时器不准。
//				2007.12.11  增加队列优先级系列函数，支持软件总线队列优先级
//							增加GSB_JobAddFast、GSB_IntJobAddFast快速投递函数支持IntJobAdd和JobAdd的快速投递
//				2007.12.12	增加GSB_WaitJobAddFast、GSB_SetTimerFast等系列快速投递函数，支持快速投递功能
//							修正在gsbWaitForMessage函数中处理中断消息的BUG，该BUG会导致中断消息处理不及时
//				2007.12.13  增加对m_nCurrentTimerID的处理，保证定时器ID不为0
//				2008.01.11  增加了SetJitterTimer系列函数，支持设置延时抖动定时器
//				2008.03.17  增加GSB_GetTimerLeftSleep函数，用来统计定时器的下次触发时间
//				2008.03.26  修改GSB_LoopMessage和GSB_PeekMessage的返回值处理，增加Timeout的指示
//                           修正GSB_Destroy退出时释放节点缓冲的处理
//				2008.05.20 修正gsbWaitForMessage函数中在vxworks平台运行时,
//							在没有消息的情况下不会进入永远等待状态的问题，减少了空闲时的CPU资源占用率
//				2008.07.07 修改GSB_SetJitterTimerXX系列函数,增加对nJitterTime的快速判断，优化了代码
//				2009.04.10 增加semMCreate中SEM_DELETE_SAFE标志位，避免总线删除问题
//			    2010.01.15 修改GET_TICK_COUNT相关的代码，把时间变成64位，解决若干时间后
//						   TickCount归零引发的定时器和时间不正常的问题
//				2010.04.22 修改GETTASKID()宏定义，改为更快的调用
//	  SoftBug V2为可用的版本
//	  SoftBus V3 优化改进要点(2006.05.25):
//              1.用散列桶数组代替现有的数组来优化模块查找
//				2.把Timer线程并列到调度线程，减少定时器的调用开销
//				3.考虑在中断中触发定时器和发送消息的情况
//				4.为消息的节点进行内存分配的优化(分为Buffer长度为0,64,和指定最大长度的情况)
//			   *5.分布式的软件总线(结合可靠组播/单播和多板卡通信),进入一次临界区就完成内存分配、
//					消息添加和定时器队列的处理
//			   *6.在触发事件时使用双检测技术(触发前检测一次队列大小，之后再一次检测队列大小)
//			    7.支持延时抖动定时器
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
//忽略警告：local variable 'XXX' may be used without having been initialized
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
  //Windows版本强制内联
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


//是否使用64位时间?
#define GSB_USE_TIME64

#ifdef GSB_USE_TIME64
//64位时间类型的定义
typedef UINT64 TICKTYPE;
#else
//32位时间类型的定义
typedef DWORD TICKTYPE;
#endif //GSB_USE_TIME64


#define MAX_MODULE (64L)								//最多64个模块
#define MAX_BUFFER_SIZE (2*1024L)						//传递的数据最多2K
#define GSB_FLAG (DWORD)('G'<<24|'S'<<16|'B'<<8|'F')	//合法标志

//#define MAX_FREE_GSBMSG_COUNT 200L	//最多容纳200条自由GSB消息

//#define MAX_FREE_TIMERMSG_COUNT 400L	//最多容纳400条自由Timer消息

//等待的最长时间
#define LOOP_FOREVER ((UINT)-1)

//等待的次数
#define MAX_WAIT_TIME LOOP_FOREVER

//快速等待Event处理
#define USE_FAST_WAIT_EVENT

//定义可以使用的Event池个数
#define MAX_EVENT_POOL_COUNT 8L

enum BISP_CALLBACK_TYPEMASK
{
	CT_NORMAL			= 0,		//普通回调类型
	CT_TIMERNORMAL		= 1,		//Timer 普通回调类型
	CT_BROADCAST		= 2,		//广播类型
	CT_TIMERBROADCAST	= 3,		//Timer 广播类型
	CT_JOBADD			= 4,		//Add Job类型
	CT_FASTJOBADD		= 5,		//Fast Int Job类型
	CT_TIMERJOBADD		= 6,		//Timer Add Job类型
	CT_FASTTIMERJOBADD	= 7,		//Fast Timer Add Job类型
	CT_EXIT				= 8			//退出消息
};

//以下为平台相关的临界区和事件的定义


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

	//Linux平台暂定没有intLock
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

	//定义后台任务的栈大小
	#define TIMERTASK_STACKSIZE 2048L

	#define GSB_CRISEC SEM_ID
	//以下定义semMCreate是为了防止有优先级反转的问题发生
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
	//改为更快的调用2010.04.22
	#define GETTASKID() ((UINT)taskIdCurrent)

	//#define SYS_GETTICKCOUNT() ((UINT)tickGet())

	//以下用intLock或TASK_LOCK代替可以提高速度
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


//模块Hash链表
#define MODULE_HASH_LEN 8L

//优化的缓冲大小
#define OPTIMIZ_BUFFER_SIZE 64L

//4K Ring Buffer Size
#define RING_BUFFER_SIZE (512*sizeof(GSBMSGINT))

//4K Fixed Malloc MSG Block Size
#define MALLOC_BLOCK_SIZE_MSG (1024*4)

//4K Fixed Malloc 64 Block Size
#define MALLOC_BLOCK_SIZE_64 (1024*4)

//12K Fixed Malloc MaxBuffer Block Size
#define MALLOC_BLOCK_SIZE_MAX (1024*12)

//圆整字节
#define ROUND(x,y) (((x)+(y-1))&~(y-1))
#define ROUND4(x) ROUND(x, 4)

//一个固定的内存块
typedef struct tagGSBPlex
{
	tagGSBPlex*		pNext;
}GSBPLEX,*LPGSBPLEX;

//一块小块内存
typedef struct tagGSBMemNode
{
	tagGSBMemNode*	pNext;				 // only valid when in free list
}GSBMEMNODE,*LPGSBMEMNODE;


//固定内存结构
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

//Buffer对其字节
#define GSBBUF_ALIGN_SIZE 4L
//缓冲额外的大小
#define GSBBUF_EXTACT_SIZE (sizeof(GSBBUF) - GSBBUF_ALIGN_SIZE)

//携带的缓冲定义
typedef struct tagGSBBuf
{
	int				nRefCount;						//引用引用计数
	BYTE			lpBuffer[GSBBUF_ALIGN_SIZE];	//后面紧跟实际的缓冲
}GSBBUF,*LPGSBBUF;

//增强型的消息节点链表
typedef struct tagGSBMsgEx
{
	tagGSBMsgEx*	pNextMsg;					//下一个链表节点(内部使用)
	tagGSBMsgEx*	pPrevMsg;					//上一个链表节点(内部使用)
	UINT			nModuleSrcID;				//请求的软件总线模块ID,nParam1
	UINT			nModuleDestID;				//目标软件总线模块ID,nParam2
	UINT			nMsg;						//消息,nParam3
	WPARAM			wParam;						//参数1,nParam4
	LPARAM			lParam;						//参数2,nParam5
//	DWORD			dwPrivate;					//私有数据
	//以下为增强的数据
	UINT16			nCallbackType;				//回调类型
	UINT16			nPriority;					//消息的优先级
	LPONBUSMESSAGE	lpfnCallback;				//函数事件回调
	UINT			nIDEvent;					//定时器ID
	TICKTYPE		dwDelayTime;				//下一次的延时时间(不含抖动)
	TICKTYPE		dwNextTime;					//下一次的延时时间(含抖动)
	UINT			nElapse;					//定时间隔
	UINT			nJitterTime;				//抖动时间(允许抖动的时间)
	UINT			nLoopTimes;					//循环次数	
	DWORD			dwWaitEvent;				//消息完成的等待事件,并且作为返回值传递
	//缓冲定义:
	int				nBufLen;					//缓冲长度
	LPGSBBUF		pAttachBuffer;				//缓冲
}GSBMSGEX,*LPGSBMSGEX;

//中断型的消息链表
typedef struct tagGSBMsgInt
{
	UINT			nModuleSrcID;				//请求的软件总线模块ID,nParam1
	UINT			nModuleDestID;				//目标软件总线模块ID,nParam2
	UINT			nMsg;						//消息,nParam3
	WPARAM			wParam;						//参数1,nParam4
	LPARAM			lParam;						//参数2,nParam5
	//以下为增强的数据
	UINT16			nCallbackType;				//回调类型
	UINT16			nBufLen;					//缓冲长度
	LPONBUSMESSAGE	lpfnCallback;				//函数事件回调
}GSBMSGINT,*LPGSBMSGINT;


//取得紧跟的缓冲
#define GET_BUFFER(pMsgPack) (void*)((pMsgPack)->pAttachBuffer->lpBuffer)


//先入先出链表 FIFO List
typedef struct tagGSBList	
{
	int				nCount;						//个数
	LPGSBMSGEX		pHeadNode;					//链表头
	LPGSBMSGEX		pTailNode;					//链表尾
}GSBLIST,*LPGSBLIST;

typedef GSBLIST TIMERLIST,*LPTIMERLIST;

/*
//先入先出链表 FIFO List
typedef struct tagTimerList	
{
	int				nCount;						//个数
	LPGSBMSGEX		pHeadNode;					//链表头
	LPGSBMSGEX		pTailNode;					//链表尾
}TIMERLIST,*LPTIMERLIST;
*/

//注册的模块定义
typedef struct tagModuleInfo
{
	UINT			nModuleID;					//模块ID
	LPONBUSMESSAGE	lpfnCallback;				//回调函数
	DWORD			dwUserData;					//自定义数据
}MODULEINFO,*LPMODULEINFO;

//软件总线数据结构定义
typedef struct tagGSBINFO
{
	DWORD			m_dwFlag;						//合法标志
	UINT			m_nThreadID;					//隶属的线程ID
	DWORD			m_dwMaxModuleCount;				//最多模块个数
	DWORD			m_dwMaxBufferSize;				//最大数据缓冲(优化和中断时使用)
	GSBLIST			m_listQueue;					//未处理消息队列(排队队列)
	GSB_CRISEC		m_csLock;						//操作临界区
	GSB_EVENT		m_objEvent;						//接收等待事件
	//定时器的数据结构
	UINT			m_nCurrentTimerID;				//当前定时器ID
	TIMERLIST		m_listTimer;					//定时器队列
	TICKTYPE		m_dwLastTickCount;				//在等待ms
	TICKTYPE		m_dwWaitTime;					//等待的超时长度ms
#ifdef GSB_USE_TIME64
	TICKTYPE		m_nBaseSysTick;					//基本系统时间
	UINT			m_nLastSysTick;					//上次系统时间
#endif //GSB_USE_TIME64	  
	//中断要处理的数据结构:
	int				m_nIntMsgCount;					//中断处理消息条数
	GSBRINGBUF		m_RingBuf;						//循环缓冲
	//Event分配数量(快速WaitEvent处理)
#ifdef USE_FAST_WAIT_EVENT
	UINT			m_nEventCount;					//可用的Event个数
	GSB_EVENT		m_eventWait[MAX_EVENT_POOL_COUNT];	//Event池的个数
#endif //USE_FAST_WAIT_EVENT
	//内存管理数据
	GSBALLOC		m_MemAllocMsg;					//节点分配内存管理(固定分配大小)	
	GSBALLOC		m_MemAlloc64;					//64字节缓冲内存管理(sizeof(GSBBUF) + 64)	
	GSBALLOC		m_MemAllocMax;					//最大缓冲管理内存(sizeof(GSBBUF) + MAXBUFSIZE)
	//若干个挂接的模块信息
	DWORD			m_dwModuleCount[MODULE_HASH_LEN];//模块个数
	LPMODULEINFO	m_lpModuleInfo[MODULE_HASH_LEN];//模块信息
}GSBINFO,*LPGSBINFO;

#ifdef USE_FAST_WAIT_EVENT

//分配一个等待的信号量
#define GSB_EVENT_ALLOC(_theEvent,_pGSBInfo) \
	if((_pGSBInfo)->m_nEventCount > 0 ) { \
			BISPASSERT((_pGSBInfo)->m_nEventCount <= MAX_EVENT_POOL_COUNT); \
			(_pGSBInfo)->m_nEventCount --; \
			(_theEvent) = (_pGSBInfo)->m_eventWait[(_pGSBInfo)->m_nEventCount]; \
	} else 	\
		GSBEvent_Init(_theEvent)

//释放一个等待的信号量
#define GSB_EVENT_FREE(_theEvent,_pGSBInfo) \
	if((_pGSBInfo)->m_nEventCount < MAX_EVENT_POOL_COUNT) { \
		(_pGSBInfo)->m_eventWait[(_pGSBInfo)->m_nEventCount] = (_theEvent); \
		(_pGSBInfo)->m_nEventCount ++; \
	} else  \
		GSBEvent_Deinit(_theEvent)

#endif //USE_FAST_WAIT_EVENT

//全局变量,空的缓冲
LOCAL GSBBUF g_NullBuffer = {1,0,0,0,0};

//GET_TICK_COUNT的定义

#ifdef NSL_OS_WINDOWS
  #ifdef GSB_USE_TIME64

  LOCAL TICKTYPE GSB_GetTickCount(LPGSBINFO pGSBInfo)
  {
	  ULONG nCurTick = GetTickCount();
	  if(nCurTick < pGSBInfo->m_nLastSysTick)	//如果计数器饱满了
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
		if(nCurTick < pGSBInfo->m_nLastSysTick)	//如果计数器饱满了
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
			if(nCurTick < pGSBInfo->m_nLastSysTick)	//如果计数器饱满了
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

//复制一段缓冲
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
		//一次分配64字节
		pBuf = (LPGSBBUF)gsbFastAllocData(&pGSBInfo->m_MemAlloc64);
	}
	else if(nBufLen <= (int)(pGSBInfo->m_dwMaxBufferSize) )
	{
		//一次分配最大字节
		pBuf = (LPGSBBUF)gsbFastAllocData(&pGSBInfo->m_MemAllocMax);
	}
	else
	{
		pBuf = (LPGSBBUF)GSB_MALLOC( (sizeof(GSBBUF) - GSBBUF_ALIGN_SIZE) + (size_t)nBufLen);
	}

	//缓冲引用计数
	if(NULL != pBuf)
	{
		pBuf->nRefCount = 1;
	}

	return pBuf;
}

LOCAL INLINE
void gsbFreeBuffer(REGISTER LPGSBINFO pGSBInfo,int nBufLen,LPGSBBUF pBuf)
{
	//注意g_NullBuffer的长度为0
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

//复制一段缓冲
LOCAL INLINE
LPGSBBUF gsbDuplicateBuffer(REGISTER LPGSBBUF pBuf,int nBufLen)
{
	if(0 != nBufLen)
	{
		pBuf->nRefCount ++;
	}
	return pBuf;
}

//分配一条消息
LOCAL INLINE
LPGSBMSGEX gsbMallocMsg(REGISTER LPGSBINFO pGSBInfo)
{
	return (LPGSBMSGEX)gsbFastAllocData(&pGSBInfo->m_MemAllocMsg);
}

//释放一条消息
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

	//返回拷贝的字节个数
	return nCopyBytes;
}


//////////////////////////////////////////////////////////////////////
//list functions:

//是否调试了链表结构
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


//初始化
LOCAL INLINE 
void GSBList_Init(REGISTER LPGSBLIST pList)
{
	BISPASSERT(NULL != pList);
	GSBMemZero(pList,sizeof(GSBLIST));
}

//大小
LOCAL INLINE 
int GSBList_Size(REGISTER LPGSBLIST pList)
{
	BISPASSERT(NULL != pList);
//	CHECK_LIST(pList);
	return pList->nCount;
}

//删除
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

//加到链表尾部
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

//从头部弹出
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
	//返回头节点
	return pMsg;
}


//在指定节点前插入一个节点
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

//加到链表尾部
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

//从头部弹出
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

	//返回头节点
	return pMsg;
}
//删除一个节点
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
//在指定节点后插入一个节点
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

//在指定节点前插入一个节点
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


//取得模块ID的Hash值
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


//查找指定ID的模块
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

		//2007.08.23如果内存分配失败，就释放内存，消息发送失败!
		if(NULL == pMsgPack->pAttachBuffer)
		{
			gsbFreeMsg(pGSBInfo,pMsgPack);
			//消息分配失败!
			pMsgPack = NULL;
		}
	}
		
	BISPASSERT(pMsgPack);
	return pMsgPack;
}

//把数据加入到包尾
LOCAL INLINE 
BOOL GSBInfo_AddMsgPack(REGISTER LPGSBINFO pGSBInfo,LPGSBMSGEX pMsgPack)
{
	BISPASSERT(pMsgPack);
	GSBList_PushBack(&pGSBInfo->m_listQueue,pMsgPack);
	return (1 == GSBList_Size(&pGSBInfo->m_listQueue));
	//产生事件
//	GSBEvent_SetEvent(pGSBInfo->m_objEvent);		
}

//把数据按优先级别插入到队列当中
LOCAL INLINE 
BOOL GSBInfo_InsertMsgPack(REGISTER LPGSBINFO pGSBInfo,LPGSBMSGEX pMsgPack)
{
	BISPASSERT(pMsgPack);
	REGISTER LPGSBMSGEX pInsertMsgPack = pGSBInfo->m_listQueue.pHeadNode;
	REGISTER UINT16 nPriority = pMsgPack->nPriority;
	for(;NULL != pInsertMsgPack; pInsertMsgPack = pInsertMsgPack->pNextMsg)
	{
		//找到插入点
		if(pInsertMsgPack->nPriority > nPriority)
		{
			GSBList_InsertBeforeNode(&pGSBInfo->m_listQueue,pMsgPack,pInsertMsgPack);
			//此时肯定多于两个，返回FALSE
			return FALSE;
		}
	}

	//没有事件就插入末尾
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

//复制消息包
LOCAL INLINE 
LPGSBMSGEX GSBInfo_DuplicateMsgPack(REGISTER LPGSBINFO pGSBInfo,LPGSBMSGEX pMsgPack)
{
	BISPASSERT(NULL != pMsgPack);

	LPGSBMSGEX pCopyMsgPack = gsbMallocMsg(pGSBInfo);
//	if(NULL != pMsgPack )
	{
		*pCopyMsgPack = *pMsgPack;
		//复制缓冲
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
			//开始下一次定时
			TimerList_InsertBeforeNode(&pGSBInfo->m_listTimer,pMsgPack,pTempNode);
			break;
		}
	}
	if(NULL == pTempNode)	//加到链表最后
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
	
	//最佳优化大小不能大于2048
	if(dwMaxBufferSize > MAX_BUFFER_SIZE)
	{
		dwMaxBufferSize = MAX_BUFFER_SIZE;
	}

	//最佳优化大小最小为128
	if(dwMaxBufferSize <= OPTIMIZ_BUFFER_SIZE)
	{
		dwMaxBufferSize = OPTIMIZ_BUFFER_SIZE*2;
	}
	
	//最小需要1个模块,避免一些没必要的问题2007.04.27
	if(0 == dwMaxModule)
	{
		dwMaxModule = 1;
	}

	//计算出要分配的内存大小
	LPGSBINFO pGSBInfo = (LPGSBINFO)GSB_MALLOC(sizeof(GSBINFO));
	if(NULL == pGSBInfo)
	{
		return NULL;
	}
	memset(pGSBInfo,0,sizeof(GSBINFO));
	pGSBInfo->m_dwFlag = GSB_FLAG;
	pGSBInfo->m_dwMaxBufferSize = dwMaxBufferSize;
	pGSBInfo->m_dwMaxModuleCount = dwMaxModule;
	pGSBInfo->m_nCurrentTimerID = 1;			//设置起始定时器ID
	//初始化链表
	GSBList_Init(&pGSBInfo->m_listQueue);
	GSBList_Init(&pGSBInfo->m_listTimer);
	//初始化临界区
	GSBCrisec_Init(pGSBInfo->m_csLock);
	//初始化事件
	GSBEvent_Init(pGSBInfo->m_objEvent);
	//取得本线程ID
	pGSBInfo->m_nThreadID = GETTASKID();

	pGSBInfo->m_dwLastTickCount = GET_TICK_COUNT(pGSBInfo);
	//设置等待时间为无限长
	pGSBInfo->m_dwWaitTime = LOOP_FOREVER;
	
	//初始化环形缓冲
	gsbRngCreate(&pGSBInfo->m_RingBuf,RING_BUFFER_SIZE);						//初始化循环缓冲
	
	//初始化快速内存管理数据
	gsbInitFastAlloc(&pGSBInfo->m_MemAllocMsg,ROUND4(sizeof(GSBMSGEX)),
						MALLOC_BLOCK_SIZE_MSG / ROUND4(sizeof(GSBMSGEX)) );
	
	//2007.07.28 修正了内存分配为61-63字节时缓冲越界引起非法指针操作的BUG
	// 注意m_MemAlloc64和m_MemAllocMax实际分配的字节要比指定的多GSBBUF_EXTACT_SIZE个字节
	gsbInitFastAlloc(&pGSBInfo->m_MemAlloc64,ROUND4(GSBBUF_EXTACT_SIZE + 64),
						MALLOC_BLOCK_SIZE_64 / ROUND4(GSBBUF_EXTACT_SIZE + 64));
	
	//取得圆整的内存大小
	dwSize = GSBBUF_EXTACT_SIZE + pGSBInfo->m_dwMaxBufferSize;
	dwSize = ROUND4(dwSize);
	//分配个数
	gsbInitFastAlloc(&pGSBInfo->m_MemAllocMax,dwSize,
					(dwSize/2 >= MALLOC_BLOCK_SIZE_MAX)? 2:(MALLOC_BLOCK_SIZE_MAX/dwSize) );

	//初始化模块Hash数组
	for(int i = 0; i< MODULE_HASH_LEN; i++)
	{
		pGSBInfo->m_lpModuleInfo[i] = (LPMODULEINFO)GSB_MALLOC(dwMaxModule*sizeof(MODULEINFO));
		GSBMemZero(pGSBInfo->m_lpModuleInfo[i],dwMaxModule*sizeof(MODULEINFO));
	}

#ifdef NSL_OS_VXWORKS	//VxWorks 对时间计算的修正
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
	//设置退出标志位

	//删除链表及节点
	GSBCrisec_Lock(pGSBInfo->m_csLock);

	//释放所有消息节点的内存缓冲
	while(NULL != (lpMsgPack = GSBList_PopFront(&pGSBInfo->m_listQueue) ))
	{
		GSBInfo_FreeMsgPack(pGSBInfo,lpMsgPack);
	}

	//释放所有定时器节点的内存缓冲
	while(NULL != (lpMsgPack = GSBList_PopFront(&pGSBInfo->m_listTimer) ))
	{
		GSBInfo_FreeMsgPack(pGSBInfo,lpMsgPack);
	}

	GSBList_Deinit(pGSBInfo,&pGSBInfo->m_listQueue,TRUE);
	GSBList_Deinit(pGSBInfo,&pGSBInfo->m_listTimer,TRUE);

	GSBCrisec_Unlock(pGSBInfo->m_csLock);

	//反初始化临界区
	GSBCrisec_Deinit(pGSBInfo->m_csLock);
	//反初始化事件
	GSBEvent_Deinit(pGSBInfo->m_objEvent);

	//释放环形缓冲
	gsbRngDelete(&pGSBInfo->m_RingBuf);

	//释放公用的事件
#ifdef USE_FAST_WAIT_EVENT
	for(i = 0; i< pGSBInfo->m_nEventCount; i++)
	{
		//反初始化wait事件池
		GSBEvent_Deinit(pGSBInfo->m_eventWait[i]);
	}
#endif //USE_FAST_WAIT_EVENT

	//释放已经分配的快速内存
	gsbDeinitFastAlloc(&pGSBInfo->m_MemAllocMsg);
	gsbDeinitFastAlloc(&pGSBInfo->m_MemAlloc64);
	gsbDeinitFastAlloc(&pGSBInfo->m_MemAllocMax);

	//释放模块Hash数组
	for(i = 0; i< MODULE_HASH_LEN; i++)
	{
		GSB_FREE(pGSBInfo->m_lpModuleInfo[i]);
	}

	//释放内存
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
	GSBFast_Lock(pGSBInfo->m_csLock);		//Module数据访问同步,锁定
	pGSBInfo->m_nThreadID = nTaskID;
	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,锁定

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
	
	GSBFast_Lock(pGSBInfo->m_csLock);		//Module数据访问同步,锁定

	//是否达到最大模块个数
	if(pGSBInfo->m_dwModuleCount[nIndex] >= pGSBInfo->m_dwMaxModuleCount)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}

	LPMODULEINFO lpModuleInfo = GSBInfo_FindModuleInfo(pGSBInfo,nModuleID);
	if(NULL == lpModuleInfo)	//New One
	{
		lpModuleInfo = &pGSBInfo->m_lpModuleInfo[nIndex][pGSBInfo->m_dwModuleCount[nIndex] ];
		pGSBInfo->m_dwModuleCount[nIndex]++;		
	}
	//储存Module信息
	lpModuleInfo->nModuleID = nModuleID;
	lpModuleInfo->lpfnCallback = lpfnCallback;
	lpModuleInfo->dwUserData = dwUserData;
	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁

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
	
	//修正模块删除后定时器还起作用的Bug,add by tbh 2006.01.06
	GSB_KillAllModuleTimer(hBus,nModuleID);

	UINT nIndex = gshGetModuleHashIndex(nModuleID);

	GSBFast_Lock(pGSBInfo->m_csLock);		//Module数据访问同步,锁定
	
	LPMODULEINFO lpModuleInfo = GSBInfo_FindModuleInfo(pGSBInfo,nModuleID);
	if(NULL == lpModuleInfo)	//原来没有的Module	
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	pGSBInfo->m_dwModuleCount[nIndex] --;	//Sub Module Count;
	
	LPMODULEINFO lpMoveModuleInfo = &pGSBInfo->m_lpModuleInfo[nIndex][pGSBInfo->m_dwModuleCount[nIndex] ];
	if(lpModuleInfo != lpMoveModuleInfo)	//Move forward
	{
		GSBMemCpy(lpModuleInfo,lpMoveModuleInfo,sizeof(MODULEINFO));
	}
	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
	
	return TRUE;
}

//消息处理函数（调用回调）
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
	
	switch(lpMsgPack->nCallbackType)	//只有低位有意义
	{
	case CT_NORMAL:			//普通回调类型
	case CT_TIMERNORMAL:			//Timer 普通回调类型
		{
			if(NULL == pFindModuleInfo)
			{
				GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
				//寻找目的模块			
				lpModuleInfo = GSBInfo_FindModuleInfo(pGSBInfo,lpMsgPack->nModuleDestID);
				if(NULL != lpModuleInfo)
				{
					lpfnCallback = lpModuleInfo->lpfnCallback;
					dwUserData = lpModuleInfo->dwUserData;
				}
				GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
			}
			else
			{
				lpModuleInfo = pFindModuleInfo;
				lpfnCallback = lpModuleInfo->lpfnCallback;
				dwUserData = lpModuleInfo->dwUserData;
			}
			if(NULL != lpfnCallback) //等价于NULL != lpModuleInfo	
			{
				//调用回调函数
				nRet = (*lpfnCallback)(lpMsgPack->nModuleSrcID,			//源ID
					lpMsgPack->nModuleDestID,					//目标ID
					lpMsgPack->nMsg,				//消息ID
					lpMsgPack->wParam,				//Add wParam
					lpMsgPack->lParam,				//Add lParam								
					(LPCSTR)GET_BUFFER(lpMsgPack),	//数据缓冲
					lpMsgPack->nBufLen,				//数据长度
					dwUserData);					//自定义数据
				
			}
		}
		break;
	case CT_BROADCAST:			//广播类型
	case CT_TIMERBROADCAST:			//Timer广播类型
		{
			GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
			UINT nModuleSrcID = lpMsgPack->nModuleSrcID;
			for(int i = 0; i< MODULE_HASH_LEN; i++)
			{
				lpModuleInfo = pGSBInfo->m_lpModuleInfo[i];
				LPMODULEINFO lpMaxModuleInfo = lpModuleInfo + pGSBInfo->m_dwModuleCount[i];
				//列举所有的模块
				for(; (DWORD)lpModuleInfo < (DWORD)lpMaxModuleInfo;lpModuleInfo++)
				{
					//对于是自身的模块,跳过
					if(nModuleSrcID == lpModuleInfo->nModuleID)
					{
						continue;
					}
					lpfnCallback = lpModuleInfo->lpfnCallback;
					dwUserData = lpModuleInfo->dwUserData;
					nModuleID = lpModuleInfo->nModuleID;	//add by tbh 2006.01.18

					GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
					//调用回调函数
					(void)((*lpfnCallback)(nModuleSrcID,		//源ID
						nModuleID,						//目标ID
						lpMsgPack->nMsg,				//消息ID
						lpMsgPack->wParam,				//Add wParam
						lpMsgPack->lParam,				//Add lParam
						(LPCSTR)GET_BUFFER(lpMsgPack),	//数据缓冲
						lpMsgPack->nBufLen,				//数据长度
						dwUserData));					//自定义数据 
					
					GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
					
				}
			}//end for(int i = 0; i< MODULE_HASH_LEN; i++)
			GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁			
		}
		break;
	case CT_JOBADD:				//Add Job类型
		{			
			BISPASSERT(lpMsgPack->lpfnCallback != NULL);
			nRet = (*((GSBFUNCPTR)lpMsgPack->lpfnCallback))(
											(LPCSTR)GET_BUFFER(lpMsgPack),	//数据缓冲
											lpMsgPack->nBufLen,				//数据长度
											(int)lpMsgPack->nModuleSrcID,
											(int)lpMsgPack->nModuleDestID,
											(int)lpMsgPack->nMsg,
											(int)lpMsgPack->wParam,
											(int)lpMsgPack->lParam);
			
		}
		break;
	case CT_FASTJOBADD:			//Fast Int Job类型	
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
	case CT_TIMERJOBADD:			//Timer Add Job类型
		{
			BISPASSERT(lpMsgPack->lpfnCallback != NULL);
			nRet = (*((GSBTIMERFUNCPTR)lpMsgPack->lpfnCallback))(lpMsgPack->nIDEvent,
											(LPCSTR)GET_BUFFER(lpMsgPack),	//数据缓冲
											lpMsgPack->nBufLen,				//数据长度
											(int)lpMsgPack->nModuleSrcID,
											(int)lpMsgPack->nModuleDestID,
											(int)lpMsgPack->nMsg,
											(int)lpMsgPack->wParam,
											(int)lpMsgPack->lParam);
		}
		break;
	case CT_FASTTIMERJOBADD:			//Timer Add Job类型
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
	//通知消息处理完成
	if(0 != lpMsgPack->dwWaitEvent)
	{
		GSB_EVENT event = (GSB_EVENT)lpMsgPack->dwWaitEvent;

		//lpMsgPack->dwWaitEvent = nRet;		//delete by tbh 2006.04.03 修正返回值的Bug
		//add by tbh 2006.04.03 修正返回值的Bug
		if(0 != lpMsgPack->nIDEvent)
		{
			*(int*)lpMsgPack->nIDEvent = nRet;
		}
		//end add by tbh 2006.04.03
		GSBEvent_SetEvent(event);	//通知消息处理完成
	}
	return nRet;
}




//以下代码专门为了优化而用:
#ifdef _DEBUG	//Debug 版本调试用
								
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

	/*****2007.08.23 删除以下代码不检查最大BUFFER大小，允许所有长度的消息发送
	//检查Buffer长度	
	if((DWORD)nBufLen > pGSBInfo->m_dwMaxBufferSize)	//不能超过最大长度
	{
		BISPASSERT((DWORD)nBufLen <= pGSBInfo->m_dwMaxBufferSize);
		nBufLen = (int)pGSBInfo->m_dwMaxBufferSize;
	}
	***********************************/

	LPGSBMSGEX lpMsg = GSBInfo_AllocMsgPack(pGSBInfo,nBufLen);
	if(NULL != lpMsg)	//分配失败!!!!
	{
		lpMsg->pNextMsg = NULL;
		lpMsg->pPrevMsg = NULL;
		lpMsg->nModuleSrcID = nModuleSrcID;
		lpMsg->nModuleDestID = nModuleDestID;
		lpMsg->nMsg = nMsg;
		lpMsg->wParam = wParam;
		lpMsg->lParam = lParam;
		lpMsg->nIDEvent = 0;	//add by tbh 2006.04.03
		lpMsg->dwWaitEvent = 0; //没有等待事件
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

#else	//优化实现

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
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	//以下宏和GSB_BuildMessage等价
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
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	lpMsg->nCallbackType = (UINT16)
		((BROASTCAST_MODULE_ID == nModuleDestID)? CT_BROADCAST:CT_NORMAL);//普通回调函数
	lpMsg->dwWaitEvent = 0;												//没有等待事件

	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//增加到链表中去

	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
	
	//马上通知前台
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
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	//以下宏和GSB_BuildMessage等价
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
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	lpMsg->nCallbackType = (UINT16)
		((BROASTCAST_MODULE_ID == nModuleDestID)? CT_BROADCAST:CT_NORMAL);//普通回调函数
	lpMsg->dwWaitEvent = 0;												//没有等待事件

	//按照优先级增加到链表中去
	BOOL bSetEventImmediately = GSBInfo_InsertMsgPack(pGSBInfo,lpMsg);		

	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
	
	//马上通知前台
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_PostQuitMessage
//	函数功能 : 发送一条软件总线循环的退出命令
//	作    者 : 谭 北 华
//	时    间 : 2007年4月14日
//	返 回 值 : BOOL
//	参数说明 : SBHANDLE hBus
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
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	//以下宏和GSB_BuildMessage等价
	BULID_NULL_MSG(lpMsg,pGSBInfo,GSB_PRIORITY_DEFAULT);
	
	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	lpMsg->nCallbackType = CT_EXIT;				//退出消息

	lpMsg->dwWaitEvent = 0;						//没有等待事件

	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//增加到链表中去

	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
	
	//马上通知前台
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;	
}

//Post投递延时消息
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
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	//以下宏和GSB_BuildMessage等价
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
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return 0;
	}

	lpMsg->nCallbackType = (UINT16)
		((BROASTCAST_MODULE_ID == nModuleDestID)? CT_TIMERBROADCAST:CT_TIMERNORMAL);//Timer 普通回调函数
	lpMsg->nMsg = nMsg;
	lpMsg->dwWaitEvent = 0; //没有等待事件

	//保障定时器ID不为0
	if( 0 == pGSBInfo->m_nCurrentTimerID)
	{
		pGSBInfo->m_nCurrentTimerID = 1;
	}

	//设置定时器ID
	nIDEvent = lpMsg->nIDEvent = pGSBInfo->m_nCurrentTimerID++;

	//设置定时间隔
	lpMsg->nElapse = dwDelayTime;
	lpMsg->nJitterTime = 0;
	lpMsg->nLoopTimes = 1;						//只循环一次

	//取得当前时间
	TICKTYPE dwTickCount = GET_TICK_COUNT(pGSBInfo);

	//计算下次的延时时间
	dwTickCount += dwDelayTime;
	lpMsg->dwNextTime = lpMsg->dwDelayTime = dwTickCount;
	
	GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsg);
	//如果插入的定时器消息需要马上触发,则触发,否则表示原来有更优先的定时器在前面等候,
	//这个时候就不需要触发后台线程了
	BOOL bSetEventImmediately = FALSE;

	//如果要求马上触发,则计算出下次触发的时间
	if(pGSBInfo->m_listTimer.pHeadNode == lpMsg &&
		( LOOP_FOREVER == pGSBInfo->m_dwWaitTime ||
		 pGSBInfo->m_dwLastTickCount + pGSBInfo->m_dwWaitTime > lpMsg->dwNextTime) )
	{
		bSetEventImmediately = TRUE;
	}

	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁

	//针对定时短的消息马上触发后台定时线程
	if(bSetEventImmediately)
	{
		//产生定时器事件
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
	if(BROASTCAST_MODULE_ID == nModuleDestID)	//通播方式
	{
		GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
		
		for(int i = 0; i< MODULE_HASH_LEN; i++)
		{
			lpModuleInfo = pGSBInfo->m_lpModuleInfo[i];
			LPMODULEINFO lpMaxModuleInfo = lpModuleInfo + pGSBInfo->m_dwModuleCount[i];
			
			for(; (DWORD)lpModuleInfo < (DWORD)lpMaxModuleInfo;lpModuleInfo++)
			{
				//对于不是自身的模块
				if(nModuleSrcID != lpModuleInfo->nModuleID)
				{
					lpfnCallback = lpModuleInfo->lpfnCallback;
					nModuleID = lpModuleInfo->nModuleID;
					dwUserData = lpModuleInfo->dwUserData;

					GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
					
					//调用回调函数
					(void)((*lpfnCallback)(nModuleSrcID,			//源ID
						nModuleID,			//目标ID
						nMsg,				//消息ID
						wParam,				//Add wParam
						lParam,				//Add lParam
						(LPCSTR)lpBuffer,	//数据缓冲
						nBufLen,				//数据长度
						dwUserData));		//自定义数据

					GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
					
				}
			}	

		} //end for(int i = 0; i< MODULE_HASH_LEN; i++)

		GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
		return 0;		//通播结束,返回
	}
	
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	lpModuleInfo = GSBInfo_FindModuleInfo(pGSBInfo,nModuleDestID);
	if(NULL == lpModuleInfo)	
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
		return FALSE;
	}
	
	lpfnCallback = lpModuleInfo->lpfnCallback;
	dwUserData = lpModuleInfo->dwUserData;
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
				
	//调用回调函数
	int nRet = (*lpfnCallback)(nModuleSrcID,			//源ID
							nModuleDestID,				//目标ID
							nMsg,						//消息ID
							wParam,						//Add wParam
							lParam,						//Add lParam
							lpBuffer,					//数据缓冲
							nBufLen,					//数据长度
							dwUserData);				//自定义数据
	
	
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
	if(GETTASKID() == pGSBInfo->m_nThreadID)	//如果是本线程，直接调用
	{
		return GSB_SendMessageSynch(pGSBInfo,nModuleSrcID,nModuleDestID,nMsg,wParam,lParam,lpBuffer,nBufLen);
	}
	////////////////////////////////////////////
	//以下走等待流程.....
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	//以下宏和GSB_BuildMessage等价
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
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}

	GSB_EVENT eventWait;		//产生一个等待事件
#ifdef USE_FAST_WAIT_EVENT //快速Event
	GSB_EVENT_ALLOC(eventWait,pGSBInfo);
#else
	GSBEvent_Init(eventWait);
#endif //USE_FAST_WAIT_EVENT
	lpMsg->dwWaitEvent = (DWORD)eventWait; //让消息通知处理完毕

	int nRet = 0;							//add by tbh 2006.04.03 修正返回值的Bug
	//nTimerId指向保存返回值的int指针
	lpMsg->nIDEvent = (UINT)&nRet;			//add by tbh 2006.04.03 修正返回值的Bug

	lpMsg->nCallbackType = (UINT16)
		((BROASTCAST_MODULE_ID == nModuleDestID)? CT_BROADCAST:CT_NORMAL);//普通回调函数
	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//增加到链表中去
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁

	//马上通知前台
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}

	//等待事件
	GSBEvent_WaitEvent(eventWait,(UINT)-1);
	//取得返回值,注意，此时的返回值已经由GSB_ProcessMessage函数设置了....
//	int nRet = (int)lpMsg->dwWaitEvent;	//delete by tbh 2006.04.03 修正返回值的Bug

#ifdef USE_FAST_WAIT_EVENT //快速Event
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	GSB_EVENT_FREE(eventWait,pGSBInfo);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
#else
	//删除事件对象
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
	if(GETTASKID() == pGSBInfo->m_nThreadID)	//如果是本线程，直接调用
	{
		return GSB_SendMessageSynch(pGSBInfo,nModuleSrcID,nModuleDestID,nMsg,wParam,lParam,lpBuffer,nBufLen);
	}
	////////////////////////////////////////////
	//以下走等待流程.....
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	//以下宏和GSB_BuildMessage等价
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
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}

	GSB_EVENT eventWait;		//产生一个等待事件
#ifdef USE_FAST_WAIT_EVENT //快速Event
	GSB_EVENT_ALLOC(eventWait,pGSBInfo);
#else
	GSBEvent_Init(eventWait);
#endif //USE_FAST_WAIT_EVENT
	lpMsg->dwWaitEvent = (DWORD)eventWait; //让消息通知处理完毕

	int nRet = 0;							//add by tbh 2006.04.03 修正返回值的Bug
	//nTimerId指向保存返回值的int指针
	lpMsg->nIDEvent = (UINT)&nRet;			//add by tbh 2006.04.03 修正返回值的Bug

	lpMsg->nCallbackType = (UINT16)
		((BROASTCAST_MODULE_ID == nModuleDestID)? CT_BROADCAST:CT_NORMAL);//普通回调函数

	//按照优先级增加到链表中去
	BOOL bSetEventImmediately = GSBInfo_InsertMsgPack(pGSBInfo,lpMsg);		

	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁

	//马上通知前台
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}

	//等待事件
	GSBEvent_WaitEvent(eventWait,(UINT)-1);
	//取得返回值,注意，此时的返回值已经由GSB_ProcessMessage函数设置了....
//	int nRet = (int)lpMsg->dwWaitEvent;	//delete by tbh 2006.04.03 修正返回值的Bug

#ifdef USE_FAST_WAIT_EVENT //快速Event
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	GSB_EVENT_FREE(eventWait,pGSBInfo);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
#else
	//删除事件对象
	GSBEvent_Deinit(eventWait);
#endif //USE_FAST_WAIT_EVENT


	return nRet;
}


LOCAL INLINE
void gsbFillModuleInfo(REGISTER LPGSBINFO pGSBInfo,LPGSBMSGEX lpMsgPack,LPMODULEINFO& pModuleInfo,MODULEINFO& moduleInfo)
{
	//优化FindModule的操作
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
	//计算下一次定时时间
	LPGSBMSGEX lpTempMsgPack = pGSBInfo->m_listTimer.pHeadNode;
	if(NULL != lpTempMsgPack)
	{
		//等待一定的时间触发
		if(lpTempMsgPack->dwNextTime > dwTickCount )
		{
			pGSBInfo->m_dwWaitTime = (lpTempMsgPack->dwNextTime - dwTickCount);
		}
		else	//不用等待,下一次定时器马上超时
		{
			pGSBInfo->m_dwWaitTime = 0;
		}
	}
	else
	{
		//设置下次的等待时间
		pGSBInfo->m_dwWaitTime = LOOP_FOREVER;
	}
}

//由本次时间计算下一次抖动时间
LOCAL INLINE
TICKTYPE gsbCalcJitterTime(REGISTER TICKTYPE nDelayTime,
						 REGISTER UINT uElase,
						 REGISTER UINT nJitterTime)
{
	BISPASSERT(uElase >= nJitterTime*2);

	if(0 != nJitterTime)
	{
		//退让到最后位置
		uElase -= nJitterTime;

		//计算新的抖动时间
		uElase += nJitterTime*(UINT)rand()*2/RAND_MAX;
	}
	
	//返回抖动后的时间
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

	//取得当前的时间TickCount
	dwTickCount = GET_TICK_COUNT(pGSBInfo);

	///////////////////////////////////////////////////////
	//1.优先处理中断消息
	if(pGSBInfo->m_nIntMsgCount > 0  )
	{
		//采用中断型的消息类型定义以便减少中断堆栈的开销
		GSBMSGINT msg;
		msg.nBufLen = 0;

		//锁中断
		int nOldLevel = intLock();
		//双重判断
		if(pGSBInfo->m_nIntMsgCount > 0 )
		{
			nCount = gsbRngBufGet(&pGSBInfo->m_RingBuf,(char*)&msg,sizeof(GSBMSGINT));
			//减少中断函数处理个数
			pGSBInfo->m_nIntMsgCount --;
		}
		else
		{
			nCount = 0;
		}
		intUnlock(nOldLevel);

		//取出缓冲
		if(sizeof(GSBMSGINT) ==  (size_t)nCount)
		{
			//bRet = TRUE;
			GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

			//以下宏和GSB_BuildMessage等价
			lpMsgPack = GSBInfo_AllocMsgPack(pGSBInfo,msg.nBufLen);
	
			if(NULL != lpMsgPack)
			{
				//赋值
				lpMsgPack->pNextMsg = NULL;
				lpMsgPack->pPrevMsg = NULL;
				lpMsgPack->nModuleSrcID = msg.nModuleSrcID;
				lpMsgPack->nModuleDestID = msg.nModuleDestID;
				lpMsgPack->nMsg = msg.nMsg;
				lpMsgPack->wParam = msg.wParam;
				lpMsgPack->lParam = msg.lParam;
				//以下为增强的数据
				lpMsgPack->nCallbackType = msg.nCallbackType;	//回调类型
				lpMsgPack->lpfnCallback = msg.lpfnCallback;		//函数事件回调
			#if 0	//由于只是JobAdd，不需要设置定时器相关的参数
				lpMsgPack->nIDEvent = 0;						//定时器ID
				lpMsgPack->dwDelayTime = 0;						//下一次的延时时间(不含抖动)
				lpMsgPack->dwNextTime = 0;						//下一次的延时时间(含抖动)
				lpMsgPack->nElapse = 0;							//定时间隔
				lpMsgPack->nJitterTime = 0;						//抖动时间(允许抖动的时间)
				lpMsgPack->nLoopTimes = 0;						//循环次数	
			#endif
				lpMsgPack->dwWaitEvent = 0;						//消息完成的等待事件,并且作为返回值传递

				lpMsgPack->nBufLen = msg.nBufLen;

				if(msg.nBufLen > 0 )
				{		
					nOldLevel = intLock();
					//从环形缓冲中复制数据
					gsbRngBufGet(&pGSBInfo->m_RingBuf,(char*)GET_BUFFER(lpMsgPack),msg.nBufLen);
					intUnlock(nOldLevel);
				}
			}
			//填充Module
			gsbFillModuleInfo(pGSBInfo,lpMsgPack,pModuleInfo,moduleInfo);

			//取得下次等待时间
			gsbCalcNextWaitTime(pGSBInfo,dwTickCount);

			GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		}	
		
	} //end if(pGSBInfo->m_nIntMsgCount > 0  )

	//////////////////////////////////////////////////////////////////////
	//2.次优先处理普通消息
	if(NULL == lpMsgPack&&
		GSBList_Size(&pGSBInfo->m_listQueue) > 0  )
	{
		GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
		lpMsgPack = GSBList_PopFront(&pGSBInfo->m_listQueue);
		
		//填充Module
		gsbFillModuleInfo(pGSBInfo,lpMsgPack,pModuleInfo,moduleInfo);
		
		//取得下次等待时间
		gsbCalcNextWaitTime(pGSBInfo,dwTickCount);

		GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
	}

	//如果没有消息需要处理才进入等待过程
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
			//转换成操作系统Tick时间 2007.11.14
			// 对等待时间进行判断，是否无限等待? 2008.05.20
			if(LOOP_FOREVER != qWaitTime)
			{
				qWaitTime = qWaitTime*g_nSysRate/1000;
			}
#endif //WIN32

			//阻塞等待.
			GSBEvent_WaitEvent(pGSBInfo->m_objEvent,(UINT)qWaitTime);
			
			//取得当前的时间TickCount
			dwTickCount = GET_TICK_COUNT(pGSBInfo);
		}
	}
	
	//储存下一次的Tick
	pGSBInfo->m_dwLastTickCount = dwTickCount;

	//////////////////////////////////////////////////////////////
	//3.定时器的处理
	if(NULL == lpMsgPack  && 
		GSBList_Size(&pGSBInfo->m_listTimer) > 0)
	{
		GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
		if(GSBList_Size(&pGSBInfo->m_listTimer) > 0 )
		{
			//取得定时器链表的头结点
			lpMsgPack = pGSBInfo->m_listTimer.pHeadNode;

			if(NULL != lpMsgPack &&
				lpMsgPack->dwNextTime <= dwTickCount) //规定的时间到了,有事件需要处理
			{	

				if(LOOP_FOREVER != lpMsgPack->nLoopTimes)	//非无限循环计数器减1
				{
					lpMsgPack->nLoopTimes --;
				}

				//删除头节点
				TimerList_PopFront(&pGSBInfo->m_listTimer);

				//表明要复制定时消息节点
				if(0 != lpMsgPack->nLoopTimes)
				{			
					//上次调用的函数占用时间过多，复位操作
					if(lpMsgPack->dwDelayTime + lpMsgPack->nElapse <= dwTickCount)
					{
						dwDelayTime = dwTickCount;
					}
					else	//采用上次的延时时间
					{
						//在这里的偏移采用相对于前一次的方式，减少了多次累积误差
						dwDelayTime = lpMsgPack->dwDelayTime; 
					}

					//计算下一次的抖动时间
					lpMsgPack->dwNextTime = gsbCalcJitterTime(dwDelayTime,
																lpMsgPack->nElapse,
																lpMsgPack->nJitterTime);					

					//计算下一次的不含抖动时间
					lpMsgPack->dwDelayTime = dwDelayTime + lpMsgPack->nElapse;
					
					//排序插入到链表
					GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsgPack);				

					//复制节点
					lpMsgPack = GSBInfo_DuplicateMsgPack(pGSBInfo,lpMsgPack);

				}
			}
			else //没有定时消息需要处理
			{
				lpMsgPack = NULL;
			}
		}

		//填充Module
		gsbFillModuleInfo(pGSBInfo,lpMsgPack,pModuleInfo,moduleInfo);

		//取得下次等待时间
		gsbCalcNextWaitTime(pGSBInfo,dwTickCount);

		GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁

	} //end if(NULL == lpMsgPack  )
	
	return lpMsgPack;
}

int WINAPI GSB_LoopMessage(SBHANDLE hBus,
							DWORD dwWaitTime)
{

	//变量定义:
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

	//无限等待?
	if(dwWaitTime == MAX_WAIT_TIME)
	{
		//无限等待模式
		do 
		{
			lpMsgPack = gsbWaitForMessage(pGSBInfo,pModuleInfo,moduleInfo,dwWaitTime);

		} while(NULL == lpMsgPack);
	}
	else	//超时等待
	{
		TICKTYPE dwCurTick = GET_TICK_COUNT(pGSBInfo);
		TICKTYPE dwLastTick = dwCurTick + dwWaitTime;

		//超时等待模式
		for(;;) 
		{
			lpMsgPack = gsbWaitForMessage(pGSBInfo,pModuleInfo,moduleInfo,dwWaitTime);
			
			if(NULL != lpMsgPack )
			{
				break;
			}
			
			//得到新的时间
			dwCurTick = GET_TICK_COUNT(pGSBInfo);

			//是否已经超时了?
			if(dwCurTick >= dwLastTick)
			{
				break;
			}
			
			//计算新的等待时间
			dwWaitTime = (DWORD)(dwLastTick - dwCurTick); 
		}

		//消息队列中没有消息
		if(NULL == lpMsgPack)
		{
			//超时返回
			return GSB_TIMEOUT;
		}
	}

	//收到退出消息
	if(CT_EXIT == lpMsgPack->nCallbackType)	
	{
		//返回线程退出消息
		return GSB_EXIT;
	}

	//处理消息
	GSB_ProcessMessage(pGSBInfo,lpMsgPack,pModuleInfo);

	//释放处理完毕的资源
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	//释放资源
	GSBInfo_FreeMsgPack(pGSBInfo,lpMsgPack);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁

	//返回成功
	return GSB_OK;

}

int WINAPI GSB_PeekMessage(SBHANDLE hBus,
							LPGSBMSG lpMsg,
							DWORD dwWaitTime)
{
	//变量定义:
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

	//无限等待?
	if(dwWaitTime == MAX_WAIT_TIME)
	{
		//无限等待模式
		do 
		{
			lpMsgPack = gsbWaitForMessage(pGSBInfo,pModuleInfo,moduleInfo,dwWaitTime);

		} while(NULL == lpMsgPack);
	}
	else	//超时等待
	{
		TICKTYPE dwCurTick = GET_TICK_COUNT(pGSBInfo);
		TICKTYPE dwLastTick = dwCurTick + dwWaitTime;

		//超时等待模式
		for(;;) 
		{
			lpMsgPack = gsbWaitForMessage(pGSBInfo,pModuleInfo,moduleInfo,dwWaitTime);
			
			if(NULL != lpMsgPack )
			{
				break;
			}
			
			//得到新的时间
			dwCurTick = GET_TICK_COUNT(pGSBInfo);

			//是否已经超时了?
			if(dwCurTick >= dwLastTick)
			{
				break;
			}
			
			//计算新的等待时间
			dwWaitTime = (DWORD)(dwLastTick - dwCurTick); 
		}

		//消息队列中没有消息
		if(NULL == lpMsgPack)
		{
			//没有消息需要处理
			lpMsg->dwPrivate = 0;
			//超时返回
			return GSB_TIMEOUT;
		}
	}

	//收到退出消息
	if(CT_EXIT == lpMsgPack->nCallbackType)	
	{
		//返回线程退出消息
		return GSB_EXIT;
	}	

	lpMsg->nModuleSrcID = lpMsgPack->nModuleSrcID;		//请求的软件总线模块ID,nParam1
	lpMsg->nModuleDestID = lpMsgPack->nModuleDestID;	//目标软件总线模块ID,nParam2
	lpMsg->nMsg = lpMsgPack->nMsg;						//消息,nParam3
	lpMsg->wParam = lpMsgPack->wParam;					//参数1,nParam4
	lpMsg->lParam = lpMsgPack->lParam;					//参数2,nParam5
	lpMsg->dwPrivate = (DWORD)lpMsgPack;

	//返回成功
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
	
	//取得原始的数据
	LPGSBMSGEX lpMsgPack = (LPGSBMSGEX)(lpMsg->dwPrivate);

	if(NULL != lpMsgPack)
	{
		//确保数据不变
		BISPASSERT(lpMsgPack->nModuleDestID == lpMsg->nModuleDestID &&
					lpMsgPack->nModuleSrcID == lpMsg->nModuleSrcID &&
					lpMsgPack->nMsg == lpMsg->nMsg);

		//处理消息
		nRet = GSB_ProcessMessage(pGSBInfo,lpMsgPack,NULL);

		//释放处理完毕的资源
		GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
		//释放资源
		GSBInfo_FreeMsgPack(pGSBInfo,lpMsgPack);
		GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
	}
		
	return nRet;
}

//设定一个延时抖动定时器
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

	//参数检查(注意这里uElapse参数为0是允许的) fixed by tbh 2006.04.24
	if(NULL == hBus || (0 == uElapse && nLoopTimes == LOOP_FOREVER) || 
				0 == nLoopTimes || NULL == lpfnRoutine)
	{
		return 0;
	}
	
	//延时抖动不能超过50%
	if( (nJitterTime > 0) && (nJitterTime > (uElapse>>1)) )
	{
		nJitterTime = (uElapse>>1);
	}

	UINT nIDEvent;
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);

	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
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
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	lpMsg->nCallbackType = CT_TIMERJOBADD;		//JobAdd类型
	lpMsg->dwWaitEvent = 0; //没有等待事件
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;
	
	//保障定时器ID不为0
	if( 0 == pGSBInfo->m_nCurrentTimerID)
	{
		pGSBInfo->m_nCurrentTimerID = 1;
	}
	//设置定时器ID
	nIDEvent = lpMsg->nIDEvent = pGSBInfo->m_nCurrentTimerID++;
				
	//设置定时间隔
	lpMsg->nElapse = uElapse;
	lpMsg->nJitterTime = nJitterTime;
	lpMsg->nLoopTimes = nLoopTimes;	
	
	//取得当前时间
	TICKTYPE dwTickCount = GET_TICK_COUNT(pGSBInfo);

	//计算下次不含抖动的时间
	lpMsg->dwDelayTime = dwTickCount + uElapse;
	//计算下次含抖动的时间
	lpMsg->dwNextTime = gsbCalcJitterTime(dwTickCount,uElapse,nJitterTime);

	
	GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsg);
	//如果插入的定时器消息需要马上触发,则触发,否则表示原来有更优先的定时器在前面等候,
	//这个时候就不需要触发后台线程了
	BOOL bSetEventImmediately = FALSE;

	//如果要求马上触发,则计算出下次触发的时间
	if(pGSBInfo->m_listTimer.pHeadNode == lpMsg &&
		( LOOP_FOREVER == pGSBInfo->m_dwWaitTime ||
		 pGSBInfo->m_dwLastTickCount + pGSBInfo->m_dwWaitTime > lpMsg->dwNextTime) )
	{
		bSetEventImmediately = TRUE;
	}

	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁

	//针对定时短的消息马上触发后台定时线程
	if(bSetEventImmediately)
	{
		//产生定时器事件
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return nIDEvent;

}



//定时器支持
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
	//直接调用延时抖动定时器
	return GSB_SetJitterTimer(hBus,uElapse,0,nLoopTimes,lpfnRoutine,
								lpBuffer,nBufLen,nParam1,nParam2,nParam3,nParam4,nParam5);

}


//设定一个延时抖动定时器,快速函数回调版本
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
	//参数检查(注意这里uElapse参数为0是允许的) fixed by tbh 2006.04.24
	if(NULL == hBus || (0 == uElapse && nLoopTimes == LOOP_FOREVER) || 
				0 == nLoopTimes || NULL == lpfnRoutine)
	{
		return 0;
	}

	//延时抖动不能超过50%
	if( (nJitterTime > 0) && (nJitterTime > (uElapse>>1)) )
	{
		nJitterTime = (uElapse>>1);
	}

	UINT nIDEvent;
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);

	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
	BUILD_FAST_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					GSB_PRIORITY_DEFAULT);

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	lpMsg->nCallbackType = CT_FASTTIMERJOBADD;		//FastTimerJobAdd类型
	lpMsg->dwWaitEvent = 0; //没有等待事件
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;
	
	//保障定时器ID不为0
	if( 0 == pGSBInfo->m_nCurrentTimerID)
	{
		pGSBInfo->m_nCurrentTimerID = 1;
	}
	//设置定时器ID
	nIDEvent = lpMsg->nIDEvent = pGSBInfo->m_nCurrentTimerID++;

	//设置定时间隔
	lpMsg->nElapse = uElapse;
	lpMsg->nJitterTime = nJitterTime;
	lpMsg->nLoopTimes = nLoopTimes;	

	//取得当前时间
	TICKTYPE dwTickCount = GET_TICK_COUNT(pGSBInfo);

	//计算下次不含抖动的时间
	lpMsg->dwDelayTime = dwTickCount + uElapse;
	//计算下次含抖动的时间
	lpMsg->dwNextTime = gsbCalcJitterTime(dwTickCount,uElapse,nJitterTime);
	
	GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsg);
	//如果插入的定时器消息需要马上触发,则触发,否则表示原来有更优先的定时器在前面等候,
	//这个时候就不需要触发后台线程了
	BOOL bSetEventImmediately = FALSE;

	//如果要求马上触发,则计算出下次触发的时间
	if(pGSBInfo->m_listTimer.pHeadNode == lpMsg &&
		( LOOP_FOREVER == pGSBInfo->m_dwWaitTime ||
		 pGSBInfo->m_dwLastTickCount + pGSBInfo->m_dwWaitTime > lpMsg->dwNextTime) )
	{
		bSetEventImmediately = TRUE;
	}

	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁

	//针对定时短的消息马上触发后台定时线程
	if(bSetEventImmediately)
	{
		//产生定时器事件
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return nIDEvent;
}


//设定一个定时器,快速函数回调版本
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
	//直接调用快速延时抖动定时器
	return GSB_SetJitterTimerFast(hBus,uElapse,0,nLoopTimes,lpfnRoutine,
									nParam1,nParam2,nParam3,nParam4,nParam5);
}

//以模块ID为对象设置抖动定时器
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
	//参数检查(注意这里uElapse参数为0是允许的) fixed by tbh 2006.04.24
	if(NULL == hBus || (0 == uElapse && nLoopTimes == LOOP_FOREVER) || 0 == nLoopTimes)
	{
		return 0;
	}

	//延时抖动不能超过50%
	if( (nJitterTime > 0) && (nJitterTime > (uElapse>>1)) )
	{
		nJitterTime = (uElapse>>1);
	}

	UINT nIDEvent;
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
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
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	lpMsg->nCallbackType = CT_TIMERNORMAL;		//JobAdd类型
	lpMsg->dwWaitEvent = 0; //没有等待事件

	//保障定时器ID不为0
	if( 0 == pGSBInfo->m_nCurrentTimerID)
	{
		pGSBInfo->m_nCurrentTimerID = 1;
	}	
	//设置定时器ID
	nIDEvent = lpMsg->nIDEvent = pGSBInfo->m_nCurrentTimerID++;

	//设置定时间隔
	lpMsg->nElapse = uElapse;
	lpMsg->nJitterTime = nJitterTime;
	lpMsg->nLoopTimes = nLoopTimes;	

	//取得当前时间
	TICKTYPE dwTickCount = GET_TICK_COUNT(pGSBInfo);

	//计算下次不含抖动的时间
	lpMsg->dwDelayTime = dwTickCount + uElapse;
	//计算下次含抖动的时间
	lpMsg->dwNextTime = gsbCalcJitterTime(dwTickCount,uElapse,nJitterTime);

	
	GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsg);
	//如果插入的定时器消息需要马上触发,则触发,否则表示原来有更优先的定时器在前面等候,
	//这个时候就不需要触发后台线程了
	BOOL bSetEventImmediately = FALSE;

	//如果要求马上触发,则计算出下次触发的时间
	if(pGSBInfo->m_listTimer.pHeadNode == lpMsg &&
		( LOOP_FOREVER == pGSBInfo->m_dwWaitTime ||
		 pGSBInfo->m_dwLastTickCount + pGSBInfo->m_dwWaitTime > lpMsg->dwNextTime) )
	{
		bSetEventImmediately = TRUE;
	}

	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁

	//针对定时短的消息马上触发后台定时线程
	if(bSetEventImmediately)
	{
		//产生定时器事件
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	
	return nIDEvent;

}

//以模块ID为对象设置定时器
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
	//直接调用延时抖动定时器
	return GSB_SetJitterTimerByID(hBus,uElapse,0,nLoopTimes,nModuleDestID,nMsg,
								wParam,lParam,lpBuffer,nLen);
}

//取消定时器
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
	GSBCrisec_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	for(lpTimerMsg = pGSBInfo->m_listTimer.pHeadNode;lpTimerMsg;lpTimerMsg=lpTimerMsg->pNextMsg)
	{
		if(nTimer == lpTimerMsg->nIDEvent)
		{

			TimerList_DeleteNode(&pGSBInfo->m_listTimer,lpTimerMsg);
			//把定时信息加到回收链表的尾部 modify by tbh 2005.06.25
			
			GSBInfo_FreeMsgPack(pGSBInfo,lpTimerMsg);
		
			fOK = TRUE;
			break;
		}
	}
	GSBCrisec_Unlock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	
	return fOK;
}

//取得时间触发剩余时间
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
	GSBCrisec_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	for(lpTimerMsg = pGSBInfo->m_listTimer.pHeadNode;lpTimerMsg;lpTimerMsg=lpTimerMsg->pNextMsg)
	{
		if(nTimer == lpTimerMsg->nIDEvent)
		{
			//取得下一次触发剩余时间 = 下一次时间 - 当前时间		
			nTimeLeft = lpTimerMsg->dwNextTime - GET_TICK_COUNT(pGSBInfo);
			break;
		}
	}
	GSBCrisec_Unlock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	
	return (UINT)nTimeLeft;
}

//事件函数直接回调
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
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
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
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	

	lpMsg->nCallbackType = CT_JOBADD;		//JobAdd类型
	lpMsg->dwWaitEvent = 0; //没有等待事件
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//增加到链表中去
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁

	//马上通知前台
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;
}


//事件函数直接回调
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
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
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
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	

	lpMsg->nCallbackType = CT_JOBADD;		//JobAdd类型
	lpMsg->dwWaitEvent = 0; //没有等待事件
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	//按照优先级增加到链表中去
	BOOL bSetEventImmediately = GSBInfo_InsertMsgPack(pGSBInfo,lpMsg);		

	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁

	//马上通知前台
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;
}


//事件函数快速回调,为异步函数调用
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
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
	BUILD_FAST_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					GSB_PRIORITY_DEFAULT);

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	

	lpMsg->nCallbackType = CT_FASTJOBADD;		//JobAdd类型
	lpMsg->dwWaitEvent = 0; //没有等待事件
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//增加到链表中去
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁

	//马上通知前台
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;
}

//事件函数快速回调,为异步函数调用
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
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
	BUILD_FAST_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					(nPriority & 0xFF));

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	

	lpMsg->nCallbackType = CT_FASTJOBADD;		//JobAdd类型
	lpMsg->dwWaitEvent = 0; //没有等待事件
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	//按照优先级增加到链表中去
	BOOL bSetEventImmediately = GSBInfo_InsertMsgPack(pGSBInfo,lpMsg);		

	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁

	//马上通知前台
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return TRUE;
}


//事件函数直接回调,此函数等待处理完才返回,返回值为lpfnRoutine的返回值
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
	if(GETTASKID() == pGSBInfo->m_nThreadID)	//如果是本线程，直接调用
	{
		return (*lpfnRoutine)(lpBuffer,nLen,nParam1,nParam2,nParam3,nParam4,nParam5);
	}
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
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
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return 0;
	}

	GSB_EVENT eventWait;		//产生一个等待事件
#ifdef USE_FAST_WAIT_EVENT //快速Event
	GSB_EVENT_ALLOC(eventWait,pGSBInfo);
#else
	GSBEvent_Init(eventWait);
#endif //USE_FAST_WAIT_EVENT
	lpMsg->dwWaitEvent = (DWORD)eventWait; //让消息通知处理完毕

	int nRet = 0;							//add by tbh 2006.04.03 修正返回值的Bug
	//nTimerId指向保存返回值的int指针
	lpMsg->nIDEvent = (UINT)&nRet;			//add by tbh 2006.04.03 修正返回值的Bug

	lpMsg->nCallbackType = CT_JOBADD;		//JobAdd类型
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;
	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//增加到链表中去
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁

	//马上通知前台
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}

	//等待事件
	GSBEvent_WaitEvent(eventWait,(UINT)-1);
	//取得返回值,注意，此时的返回值已经由GSB_ProcessMessage函数设置了....
//	int nRet = (int)lpMsg->dwWaitEvent;	//delete by tbh 2006.04.03 修正返回值的Bug

#ifdef USE_FAST_WAIT_EVENT //快速Event
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	GSB_EVENT_FREE(eventWait,pGSBInfo);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
#else
	//删除事件对象
	GSBEvent_Deinit(eventWait);
#endif //USE_FAST_WAIT_EVENT

	return nRet;
}


//事件函数直接回调,此函数等待处理完才返回,返回值为lpfnRoutine的返回值
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
	if(GETTASKID() == pGSBInfo->m_nThreadID)	//如果是本线程，直接调用
	{
		return (*lpfnRoutine)(lpBuffer,nLen,nParam1,nParam2,nParam3,nParam4,nParam5);
	}
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
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
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return 0;
	}

	GSB_EVENT eventWait;		//产生一个等待事件
#ifdef USE_FAST_WAIT_EVENT //快速Event
	GSB_EVENT_ALLOC(eventWait,pGSBInfo);
#else
	GSBEvent_Init(eventWait);
#endif //USE_FAST_WAIT_EVENT
	lpMsg->dwWaitEvent = (DWORD)eventWait; //让消息通知处理完毕

	int nRet = 0;							//add by tbh 2006.04.03 修正返回值的Bug
	//nTimerId指向保存返回值的int指针
	lpMsg->nIDEvent = (UINT)&nRet;			//add by tbh 2006.04.03 修正返回值的Bug

	lpMsg->nCallbackType = CT_JOBADD;		//JobAdd类型
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	//按照优先级增加到链表中去
	BOOL bSetEventImmediately = GSBInfo_InsertMsgPack(pGSBInfo,lpMsg);		

	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁

	//马上通知前台
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}

	//等待事件
	GSBEvent_WaitEvent(eventWait,(UINT)-1);
	//取得返回值,注意，此时的返回值已经由GSB_ProcessMessage函数设置了....
//	int nRet = (int)lpMsg->dwWaitEvent;	//delete by tbh 2006.04.03 修正返回值的Bug

#ifdef USE_FAST_WAIT_EVENT //快速Event
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	GSB_EVENT_FREE(eventWait,pGSBInfo);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
#else
	//删除事件对象
	GSBEvent_Deinit(eventWait);
#endif //USE_FAST_WAIT_EVENT

	return nRet;
}

//事件函数快速回调,为同步函数调用
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
	if(GETTASKID() == pGSBInfo->m_nThreadID)	//如果是本线程，直接调用
	{
		return (*lpfnRoutine)(nParam1,nParam2,nParam3,nParam4,nParam5);
	}
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
	BUILD_FAST_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					GSB_PRIORITY_DEFAULT);
	
	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return 0;
	}

	GSB_EVENT eventWait;		//产生一个等待事件
#ifdef USE_FAST_WAIT_EVENT //快速Event
	GSB_EVENT_ALLOC(eventWait,pGSBInfo);
#else
	GSBEvent_Init(eventWait);
#endif //USE_FAST_WAIT_EVENT
	lpMsg->dwWaitEvent = (DWORD)eventWait; //让消息通知处理完毕

	int nRet = 0;							//add by tbh 2006.04.03 修正返回值的Bug
	//nTimerId指向保存返回值的int指针
	lpMsg->nIDEvent = (UINT)&nRet;			//add by tbh 2006.04.03 修正返回值的Bug

	lpMsg->nCallbackType = CT_FASTJOBADD;		//JobAdd类型
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;
	BOOL bSetEventImmediately = GSBInfo_AddMsgPack(pGSBInfo,lpMsg);		//增加到链表中去
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁

	//马上通知前台
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}

	//等待事件
	GSBEvent_WaitEvent(eventWait,(UINT)-1);
	//取得返回值,注意，此时的返回值已经由GSB_ProcessMessage函数设置了....
//	int nRet = (int)lpMsg->dwWaitEvent;	//delete by tbh 2006.04.03 修正返回值的Bug

#ifdef USE_FAST_WAIT_EVENT //快速Event
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	GSB_EVENT_FREE(eventWait,pGSBInfo);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
#else
	//删除事件对象
	GSBEvent_Deinit(eventWait);
#endif //USE_FAST_WAIT_EVENT

	return nRet;	
}

//事件函数快速回调,为同步函数调用
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
	if(GETTASKID() == pGSBInfo->m_nThreadID)	//如果是本线程，直接调用
	{
		return (*lpfnRoutine)(nParam1,nParam2,nParam3,nParam4,nParam5);
	}
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
	BUILD_FAST_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					(nPriority & 0xFF) );
	
	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return 0;
	}

	GSB_EVENT eventWait;		//产生一个等待事件
#ifdef USE_FAST_WAIT_EVENT //快速Event
	GSB_EVENT_ALLOC(eventWait,pGSBInfo);
#else
	GSBEvent_Init(eventWait);
#endif //USE_FAST_WAIT_EVENT
	lpMsg->dwWaitEvent = (DWORD)eventWait; //让消息通知处理完毕

	int nRet = 0;							//add by tbh 2006.04.03 修正返回值的Bug
	//nTimerId指向保存返回值的int指针
	lpMsg->nIDEvent = (UINT)&nRet;			//add by tbh 2006.04.03 修正返回值的Bug

	lpMsg->nCallbackType = CT_FASTJOBADD;		//JobAdd类型
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	//按照优先级增加到链表中去
	BOOL bSetEventImmediately = GSBInfo_InsertMsgPack(pGSBInfo,lpMsg);		

	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁

	//马上通知前台
	if(bSetEventImmediately)
	{
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}

	//等待事件
	GSBEvent_WaitEvent(eventWait,(UINT)-1);
	//取得返回值,注意，此时的返回值已经由GSB_ProcessMessage函数设置了....
//	int nRet = (int)lpMsg->dwWaitEvent;	//delete by tbh 2006.04.03 修正返回值的Bug

#ifdef USE_FAST_WAIT_EVENT //快速Event
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	GSB_EVENT_FREE(eventWait,pGSBInfo);
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
#else
	//删除事件对象
	GSBEvent_Deinit(eventWait);
#endif //USE_FAST_WAIT_EVENT

	return nRet;
}


//事件函数直接回调
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
	
	//参数检查
	UINT nIDEvent;
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
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
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	lpMsg->nCallbackType = CT_JOBADD;		//JobAdd类型
	lpMsg->dwWaitEvent = 0; //没有等待事件
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	//保障定时器ID不为0
	if( 0 == pGSBInfo->m_nCurrentTimerID)
	{
		pGSBInfo->m_nCurrentTimerID = 1;
	}
	//设置定时器ID
	nIDEvent = lpMsg->nIDEvent = pGSBInfo->m_nCurrentTimerID++;

	//设置定时间隔
	lpMsg->nElapse = dwDelayTime;
	lpMsg->nJitterTime = 0;
	lpMsg->nLoopTimes = 1;						//只循环一次

	//取得当前时间
	TICKTYPE dwTickCount = GET_TICK_COUNT(pGSBInfo);

	//计算下次的延时时间
	dwTickCount += dwDelayTime;
	lpMsg->dwNextTime = lpMsg->dwDelayTime = dwTickCount;
	
	GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsg);
	//如果插入的定时器消息需要马上触发,则触发,否则表示原来有更优先的定时器在前面等候,
	//这个时候就不需要触发后台线程了
	BOOL bSetEventImmediately = FALSE;

	//如果要求马上触发,则计算出下次触发的时间
	if(pGSBInfo->m_listTimer.pHeadNode == lpMsg &&
		( LOOP_FOREVER == pGSBInfo->m_dwWaitTime ||
		 pGSBInfo->m_dwLastTickCount + pGSBInfo->m_dwWaitTime > lpMsg->dwNextTime) )
	{
		bSetEventImmediately = TRUE;
	}

	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁

	//针对定时短的消息马上触发后台定时线程
	if(bSetEventImmediately)
	{
		//产生定时器事件
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return nIDEvent;
}

//事件函数延时异步快速回调
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
	
	//参数检查
	UINT nIDEvent;
	LPGSBMSGEX lpMsg;
	REGISTER LPGSBINFO pGSBInfo = (LPGSBINFO)hBus;
	BISPASSERT(GSB_FLAG == pGSBInfo->m_dwFlag);
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定

	//以下宏和GSB_BuildMessage等价
	BUILD_FAST_MSG(lpMsg,pGSBInfo,
					(UINT)nParam1,
					(UINT)nParam2,
					(UINT)nParam3,
					(UINT)nParam4,
					(UINT)nParam5,
					GSB_PRIORITY_DEFAULT);

	if(NULL == lpMsg)
	{
		GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁
		return FALSE;
	}
	lpMsg->nCallbackType = CT_FASTJOBADD;		//JobAdd类型
	lpMsg->dwWaitEvent = 0; //没有等待事件
	lpMsg->lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;

	//保障定时器ID不为0
	if( 0 == pGSBInfo->m_nCurrentTimerID)
	{
		pGSBInfo->m_nCurrentTimerID = 1;
	}
	//设置定时器ID
	nIDEvent = lpMsg->nIDEvent = pGSBInfo->m_nCurrentTimerID++;

	//设置定时间隔
	lpMsg->nElapse = dwDelayTime;
	lpMsg->nJitterTime = 0;
	lpMsg->nLoopTimes = 1;						//只循环一次

	//取得当前时间
	TICKTYPE dwTickCount = GET_TICK_COUNT(pGSBInfo);

	//计算下次的延时时间
	dwTickCount += dwDelayTime;
	lpMsg->dwNextTime = lpMsg->dwDelayTime = dwTickCount;
	
	GSBInfo_AddTimerMsgPack(pGSBInfo,lpMsg);
	//如果插入的定时器消息需要马上触发,则触发,否则表示原来有更优先的定时器在前面等候,
	//这个时候就不需要触发后台线程了
	BOOL bSetEventImmediately = FALSE;

	//如果要求马上触发,则计算出下次触发的时间
	if(pGSBInfo->m_listTimer.pHeadNode == lpMsg &&
		( LOOP_FOREVER == pGSBInfo->m_dwWaitTime ||
		 pGSBInfo->m_dwLastTickCount + pGSBInfo->m_dwWaitTime > lpMsg->dwNextTime) )
	{
		bSetEventImmediately = TRUE;
	}

	GSBFast_Unlock(pGSBInfo->m_csLock);		//Module数据访问同步,解锁

	//针对定时短的消息马上触发后台定时线程
	if(bSetEventImmediately)
	{
		//产生定时器事件
		GSBEvent_SetEvent(pGSBInfo->m_objEvent);
	}
	return nIDEvent;
}

//取消指定模块的所有定时器add by tbh 2006.01.06
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
	//释放处理完毕的资源
	GSBFast_Lock(pGSBInfo->m_csLock);		//数据访问同步,锁定
	for(lpTimerMsg = pGSBInfo->m_listTimer.pHeadNode;lpTimerMsg; )
	{	
		lpTempTimerMsg = lpTimerMsg->pNextMsg;

		if(nModuleId == lpTimerMsg->nModuleDestID)
		{
			TimerList_DeleteNode(&pGSBInfo->m_listTimer,lpTimerMsg);
			//释放资源
			GSBInfo_FreeMsgPack(pGSBInfo,lpTimerMsg);
			fOK = TRUE;
		}

		lpTimerMsg = lpTempTimerMsg;
	}
	GSBFast_Unlock(pGSBInfo->m_csLock);		//数据访问同步,解锁
	
	return fOK;
}

//取得与操作系统相关的事件句柄
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

	//采用中断型的消息类型定义以便减少中断堆栈的开销
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
		//取得字符串长度
		nBufLen = lpszTemp - lpBuffer;
	}

	//2007.08.23 如果复制的缓冲大小多于优化大小，则不允许发送
	if((DWORD)nBufLen > pGSBInfo->m_dwMaxBufferSize ||
		nBufLen > (int)0xffff )
	{
		return FALSE;
	}

	//设置参数
	msg.nModuleSrcID = nModuleSrcID;
	msg.nModuleDestID = nModuleDestID;
	msg.nMsg = nMsg;
	msg.wParam = wParam;
	msg.lParam = lParam;
	
	msg.nCallbackType = CT_NORMAL;			//普通回调类型
	msg.nBufLen = (UINT16)nBufLen;					//缓冲长度

	//设置返回值
	BOOL bRet = FALSE;

	//锁中断
	nOldLevel = intLock();

	if(gsbRngFreeBytes(&pGSBInfo->m_RingBuf) <= ((int)sizeof(GSBMSGINT) + nBufLen) )
	{
#ifdef NSL_OS_VXWORKS
		logMsg((char*)"GSB_IntPostMessage(%p): ring buf over flow!\n",(int)hBus,0,0,0,0,0);
#endif //WIN32
		//解锁中断
		intUnlock (nOldLevel);
		return FALSE;		
	}

	//记录原来的位置
	int nOldPos = pGSBInfo->m_RingBuf.pToBuf;

	if( sizeof(GSBMSGINT) == (size_t)gsbRngBufPut(&pGSBInfo->m_RingBuf,(char*)&msg,(int)sizeof(GSBMSGINT)) )
	{
		//复制成功
		bRet = TRUE;
		
		//复制缓冲
		if(nBufLen > 0 )
		{
			//复制失败?
			if(nBufLen != gsbRngBufPut(&pGSBInfo->m_RingBuf,(char*)lpBuffer,nBufLen) )
			{
				bRet = FALSE;	
			}
		}
	}

	//如果复制成功，则增加一条消息，否则还原位置
	if(bRet)
	{
		pGSBInfo->m_nIntMsgCount ++;
	}
	else
	{
		pGSBInfo->m_RingBuf.pToBuf = nOldPos;
	}
	

	//解锁中断
	intUnlock (nOldLevel);

	//马上通知前台
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

	//采用中断型的消息类型定义以便减少中断堆栈的开销
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
		//取得字符串长度
		nBufLen = lpszTemp - lpBuffer;
	}

	//2007.08.23 如果复制的缓冲大小多于优化大小，则不允许发送
	if((DWORD)nBufLen > pGSBInfo->m_dwMaxBufferSize ||
		nBufLen > (int)0xffff )
	{
		return FALSE;
	}

	//设置参数
	msg.nModuleSrcID = (UINT)nParam1;
	msg.nModuleDestID = (UINT)nParam2;
	msg.nMsg = (UINT)nParam3;
	msg.wParam = (WPARAM)nParam4;
	msg.lParam = (LPARAM)nParam5;
	
	msg.nCallbackType = CT_JOBADD;						//Add Job类型
	msg.lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;		//回调函数
	msg.nBufLen = (UINT16)nBufLen;								//缓冲长度

	//设置返回值
	BOOL bRet = FALSE;

	//锁中断
	nOldLevel = intLock();
	
	if(gsbRngFreeBytes(&pGSBInfo->m_RingBuf) <= ((int)sizeof(GSBMSGINT) + nBufLen) )
	{
#ifdef NSL_OS_VXWORKS
		logMsg((char*)"GSB_IntJobAdd(%p): ring buf over flow!\n",(int)hBus,0,0,0,0,0);
#endif //WIN32
		//解锁中断
		intUnlock (nOldLevel);
		return FALSE;		
	}

	//记录原来的位置
	int nOldPos = pGSBInfo->m_RingBuf.pToBuf;

	if( sizeof(GSBMSGINT) == (size_t)gsbRngBufPut(&pGSBInfo->m_RingBuf,(char*)&msg,(int)sizeof(GSBMSGINT)) )
	{
		//复制成功
		bRet = TRUE;
		
		//复制缓冲
		if(nBufLen > 0 )
		{
			//复制失败?
			if(nBufLen != gsbRngBufPut(&pGSBInfo->m_RingBuf,(char*)lpBuffer,nBufLen) )
			{
				bRet = FALSE;	
			}
		}
	}

	//如果复制成功，则增加一条消息，否则还原位置
	if(bRet)
	{
		pGSBInfo->m_nIntMsgCount ++;
	}
	else
	{
		pGSBInfo->m_RingBuf.pToBuf = nOldPos;
	}
	

	//解锁中断
	intUnlock (nOldLevel);

	//马上通知前台
	GSBEvent_SetEvent(pGSBInfo->m_objEvent);	

	return bRet;

}


//INT Job Add的快速版本
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

	//采用中断型的消息类型定义以便减少中断堆栈的开销
	GSBMSGINT msg;

	//设置参数
	msg.nModuleSrcID = (UINT)nParam1;
	msg.nModuleDestID = (UINT)nParam2;
	msg.nMsg = (UINT)nParam3;
	msg.wParam = (WPARAM)nParam4;
	msg.lParam = (LPARAM)nParam5;
	
	msg.nCallbackType = CT_FASTJOBADD;						//Add Job类型
	msg.lpfnCallback = (LPONBUSMESSAGE)lpfnRoutine;			//回调函数
	msg.nBufLen = 0;										//缓冲长度

	//锁中断
	nOldLevel = intLock();
	
	if(gsbRngFreeBytes(&pGSBInfo->m_RingBuf) <= ((int)sizeof(GSBMSGINT)) )
	{
#ifdef NSL_OS_VXWORKS
		logMsg((char*)"GSB_IntJobAddFast(%p): ring buf over flow!\n",(int)hBus,0,0,0,0,0);
#endif //WIN32
		//解锁中断
		intUnlock (nOldLevel);
		return FALSE;		
	}

	//复制消息队列到环形缓冲中
	gsbRngBufPut(&pGSBInfo->m_RingBuf,(char*)&msg,sizeof(GSBMSGINT) );

	//复制成功，增加一条消息
	pGSBInfo->m_nIntMsgCount ++;

	//解锁中断
	intUnlock (nOldLevel);

	//马上通知前台
	GSBEvent_SetEvent(pGSBInfo->m_objEvent);	

	return TRUE;

}














