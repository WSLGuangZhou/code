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
//连接模块定义: BISP_UTIL
//BISP 基础工具定义

#ifndef __BISPUTIL_H_INCLUDED
#define __BISPUTIL_H_INCLUDED

//#include "BISPDebug.h"

//可变参数的回调函数
#ifdef __cplusplus
typedef int 	(*JOBFUNCPTR) (...);					 /* ptr to function returning int */
typedef int 	(*JOBTIMERFUNCPTR) (UINT nTimerId,...);  /* ptr to timer function returning int */
#else
typedef int 	(*JOBFUNCPTR) ();						/* ptr to function returning int */
typedef int 	(*JOBTIMERFUNCPTR) ();					/* ptr to timer function returning int */
#endif	


//取最大值
#define BISPMAX(a,b) ((a)>(b)? (a):(b))
//取最小值
#define BISPMIN(a,b) ((a)<(b)? (a):(b))

//把2个字节转换成1个WORD
#define BISP_MAKE_WORD(h,l) \
	((WORD)( (WORD)(h)<<8 | (WORD)(l)))

//把4个字节换成1个DWORD
#define BISP_MAKE_DWORD(hh,hl,lh,ll) \
	((DWORD)((DWORD)(hh)<<24 | (DWORD)(hl)<<16 | (DWORD)(lh)<<8 | (DWORD)(ll)))

////////////////////////////////////////////
//位定义

#define BIT0  (0x1u<<0)
#define BIT1  (0x1u<<1)
#define BIT2  (0x1u<<2)
#define BIT3  (0x1u<<3)
#define BIT4  (0x1u<<4)
#define BIT5  (0x1u<<5)
#define BIT6  (0x1u<<6)
#define BIT7  (0x1u<<7)
#define BIT8  (0x1u<<8)
#define BIT9  (0x1u<<9)
#define BIT10 (0x1u<<10)
#define BIT11 (0x1u<<11)
#define BIT12 (0x1u<<12)
#define BIT13 (0x1u<<13)
#define BIT14 (0x1u<<14)
#define BIT15 (0x1u<<15)
#define BIT16 (0x1u<<16)
#define BIT17 (0x1u<<17)
#define BIT18 (0x1u<<18)
#define BIT19 (0x1u<<19)
#define BIT20 (0x1u<<20)
#define BIT21 (0x1u<<21)
#define BIT22 (0x1u<<22)
#define BIT23 (0x1u<<23)
#define BIT24 (0x1u<<24)
#define BIT25 (0x1u<<25)
#define BIT26 (0x1u<<26)
#define BIT27 (0x1u<<27)
#define BIT28 (0x1u<<28)
#define BIT29 (0x1u<<29)
#define BIT30 (0x1u<<30)
#define BIT31 (0x1u<<31)


////////////////////////////////////////////
//以下是位运算宏定义:

#ifdef BIT_TEST
#undef BIT_TEST
#endif //BIT_TEST

//测试一个BIT是否置1
#define BIT_TEST(f,b)	((f) & (b))

#ifdef BIT_COMPARE
#undef BIT_COMPARE
#endif //BIT_COMPARE

//测试多个BIT是否置1
#define BIT_COMPARE(f,b)	(((f) & (b)) == (b) )


#ifdef BIT_SET
#undef BIT_SET
#endif //BIT_SET

//BIT设置
#define BIT_SET(f,b)	((f) |= (b))

#ifdef BIT_CLEAR
#undef BIT_CLEAR
#endif //BIT_CLEAR

//BIT清除
#define BIT_CLEAR(f,b)	((f) &= ~(b))

///////////////////////////////////////////
//寄存器定义

//8位寄存器操作
#define BISP_REG8(addr) \
   (*((volatile unsigned char *)(addr)))

//16位寄存器操作
#define BISP_REG16(addr) \
   (*((volatile unsigned short *)(addr)))

//32位寄存器操作
#define BISP_REG32(addr) \
   (*((volatile unsigned long *)(addr)))

//////////////////////////////////////
//

#ifdef NSL_OS_WINDOWS
#define SYSAPICALL __stdcall
#define SYS_DELAY(nTick) Sleep(nTick)
#define SYS_TICKS_PERSEC ((int)1000L)
#elif defined(NSL_OS_VXWORKS)
#define SYSAPICALL
#define SYS_DELAY(nTick) taskDelay(nTick)
#define SYS_TICKS_PERSEC ((int)sysClkRateGet())
#elif defined(NSL_OS_LINUX)
#define SYSAPICALL
extern long sys_clk_tck;
#define SYS_DELAY(nTick) ((int)usleep((nTick*1000)/sys_clk_tck))
#define SYS_TICKS_PERSEC ((int)sys_clk_tck)
#endif /* NSL_OS_WINDOWS */


//系统切换任务
#define SYS_TASK_SWITCH() SYS_DELAY(0)

//类似Windows的函数调用
#if defined(NSL_OS_WINDOWS) || defined(NSL_OS_VXWORKS)
	typedef int (SYSAPICALL *BISPTASKPROC)(void* pParam);
#endif

//Linux Define
#if defined(NSL_OS_LINUX)
	typedef void*(*BISPTASKPROC)(void *arg);
#endif

//类似VxWorks的函数调用
#ifdef __cplusplus
typedef int (*BISPTASKFUNC)(...);
#else
typedef int (*BISPTASKFUNC)();
#endif //__cplusplus

//无限等待(s)
#ifndef LOOP_FOREVER
#define LOOP_FOREVER ((UINT)-1)
#endif //LOOP_FOREVER

//无限时间
#ifndef BISP_INFINITE
#define BISP_INFINITE 0xFFFFFFFF
#endif //BISP_INFINITE

//1秒=1000毫秒
#define MSEC_PER_SECOND 1000L

//默认的优先级和堆栈大小
#define BISP_DEFAULT_TASK_PRIORITY 100
//默认的堆栈大小
#define BISP_DEFAULT_STACK_SIZE 0x4000


/////////////////////////////////////////////////////
//跨平台的临界区定义
#ifdef NSL_OS_WINDOWS

//对Windows系统临界区的跨平台操作
#define BISP_CRISEC CRITICAL_SECTION
#define BISPCrisec_Init(crisec) ::InitializeCriticalSection(&(crisec))
#define BISPCrisec_Deinit(crisec) ::DeleteCriticalSection(&(crisec))
#define BISPCrisec_Lock(crisec) ::EnterCriticalSection(&(crisec))
#define BISPCrisec_Unlock(crisec) ::LeaveCriticalSection(&(crisec))

//对Windows系统事件的跨平台操作
#define BISP_EVENT HANDLE
#define BISPEvent_Init(hEvent) ((hEvent) = ::CreateEvent(NULL,FALSE,FALSE,NULL))
#define BISPEvent_Deinit(hEvent) ::CloseHandle(hEvent)
#define BISPEvent_WaitEvent(hEvent,dwWaitTime) ::WaitForSingleObject(hEvent, dwWaitTime)
#define BISPEvent_SetEvent(hEvent) ::SetEvent(hEvent)

//对Windows系统任务ID的跨平台操作
#define BISPGETTASKID() ((UINT)GetCurrentThreadId())
#define BISPSYS_GETTICKCOUNT() ((UINT)GetTickCount())

//Windows平台的intlock定义
#define intLock() 0L
#define intUnlock(oldlevel) ((oldlevel) = 0)

#elif defined(NSL_OS_VXWORKS)

#include "semLib.h"
#include "taskLib.h"
#include "intLib.h"

//对VxWorks系统临界区的跨平台操作
#define BISP_CRISEC SEM_ID
//以下定义semMCreate是为了防止有优先级反转的问题发生
#define BISPCrisec_Init(crisec) ((crisec) = semMCreate(SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_DELETE_SAFE))
//#define BISPCrisec_Init(crisec) ((crisec) = semBCreate(SEM_Q_PRIORITY,SEM_FULL))
#define BISPCrisec_Deinit(crisec) semDelete(crisec)
#define BISPCrisec_Lock(crisec) semTake(crisec,WAIT_FOREVER)
#define BISPCrisec_Unlock(crisec) semGive(crisec)

//对VxWorks系统事件的跨平台操作
#define BISP_EVENT SEM_ID
#define BISPEvent_Init(hEvent) ((hEvent) = semBCreate(SEM_Q_PRIORITY,SEM_EMPTY))
#define BISPEvent_Deinit(hEvent) semDelete(hEvent)
#define BISPEvent_WaitEvent(hEvent,dwWaitTime) semTake(hEvent, (int)(dwWaitTime))
#define BISPEvent_SetEvent(hEvent) semGive(hEvent)

//对VxWorks系统任务ID的跨平台操作
#define BISPGETTASKID() ((UINT)taskIdSelf())
#define BISPSYS_GETTICKCOUNT() ((UINT)tickGet())

#elif defined(NSL_OS_LINUX)
#include <pthread.h>
#include <semaphore.h>

#define BISP_CRISEC               pthread_mutex_t
#define BISPCrisec_Init(crisec)   pthread_mutex_init(&(crisec), NULL)
#define BISPCrisec_Deinit(crisec) pthread_mutex_destroy(&(crisec))
#define BISPCrisec_Lock(crisec)   pthread_mutex_lock(&(crisec))
#define BISPCrisec_Unlock(crisec) pthread_mutex_unlock(&(crisec))

#define WAIT_FOREVER							(UINT(-1))
#define BISP_EVENT                              sem_t*
#define BISPEvent_Init(hEvent)                  hEvent=(sem_t*)malloc(sizeof(sem_t)); if(NULL!=hEvent){BISPTRACE("sem_t malloc succeed\r\n"); sem_init((hEvent), 0, 0);}
#define BISPEvent_Deinit(hEvent)                sem_destroy(hEvent); if(NULL!=hEvent){BISPTRACE("sem_t free \r\n");free(hEvent);}
#define BISPEvent_WaitEvent(hEvent,dwWaitTime)  if(NULL!=hEvent){BISPTRACE("sem_t WaitEvent \r\n");BISPEventWaitEvent(hEvent,dwWaitTime);}
#define BISPEvent_SetEvent(hEvent)              if(NULL!=hEvent){BISPTRACE("sem_t post \r\n");sem_post(hEvent);}

#define BISPGETTASKID()           (pthread_self())
//#define BISPSYS_GETTICKCOUNT()    (BISPGetTickCount())

//Linux平台暂定没有intLock
#define intLock() 0L
#define intUnlock(oldlevel) ((oldlevel) = 0)

#endif /* NSL_OS_WINDOWS */


//分钟、秒、毫秒转换宏

//毫秒转换成分钟
#define BISP_MS_TO_MIN(ms) ((ms)/(60*1000))
//毫秒转换成秒
#define BISP_MS_TO_SEC(ms) ((ms)/1000)
//秒余数
#define BISP_SEC_IN_MIN(s) ((s)%60)
//毫秒余数
#define BISP_MS_IN_SEC(ms) ((ms)%1000)
//分钟转换成秒
#define BISP_MIN_TO_SEC(m) ((m)*(60))
//分钟转换成毫秒
#define BISP_MIN_TO_MS(m) ((m)*(60*1000))
//秒转换成毫秒
#define BISP_SEC_TO_MS(s) ((s)*1000)


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifndef BISP_CONFIG_NO_BISP_TASK

//初始化系统任务,采用和Windows类似的调用版本
int BISPTaskCreate(BISPTASKPROC pfnEntry,
					 void* pParam,
					 int nPriority,
					 int nStackSize,
					 const char* pTaskName);

//初始化系统任务,采用和VxWorks类似的调用版本，暂不支持Linux
// int BISPTaskSpawn(BISPTASKFUNC pfnEntry,
// 					 int nPriority,
// 					 int nStackSize,
// 					 const char* pTaskName,
// 					 int nParam1,
// 					 int nParam2,
// 					 int nParam3,
// 					 int nParam4,
// 					 int nParam5,
// 					 int nParam6,
// 					 int nParam7,
// 					 int nParam8,
// 					 int nParam9,
// 					 int nParam10);

//结束清理任务
BOOL BISPTaskCleanup(int nTaskHandle);

//线程是否有效?，Linux暂不支持
//BOOL BISPTaskIsExist(int nTaskHandle);

//线程是否活动?，Linux暂不支持
//BOOL BISPTaskIsActive(int nTaskHandle);

//取得当前句柄，Linux暂不支持
//int BISPTaskGetCurrentHandle(void);

//强行终止一个线程，Linux暂不支持
//BOOL BISPTaskTerminate(int nTaskHandle,int nExitCode);

#endif /* BISP_CONFIG_NO_BISP_TASK */

//把一个字符串转化成大写，Linux暂不支持
//char* BISPStrUpper(char* pString);

//把一个字符串转化成小写，Linux暂不支持
//char* BISPStrLower(char* pString);

//字符串替换函数
int BISPStrReplace(LPSTR lpszString,LPCSTR lpszOld, LPCSTR lpszNew);

//取得当前的Tick数，暂不支持Linux
//UINT64 BISPGetTickCount(void);

#ifdef NSL_OS_LINUX
	int BISPEventWaitEvent(sem_t *event, UINT milliseconds);
#endif /* NSL_OS_LINUX */

//#ifndef NSL_OS_LINUX
//计算校验和
USHORT BISPCheckSum(USHORT *pBuffer, int nSize);
//#endif /* NSL_OS_LINUX */

//计算简单的校验和(短波电台用到)
//采用按字节简单异或校验，得到1 个Byte,取该Byte 的高4 个bit,
//作为校验的第一个字节的低4bit,高位填0; 取该Byte 的低4 个bit,
//作为校验的第二个字节的低4bit,高位填0。
USHORT BISPGetSimpleCRC(char *pBuffer, int nSize);

//是否是数字
BOOL BISPIsNumber(LPCSTR pszNumber);

//是否是网络接口数字? 如 2/1或 1等
BOOL BISPIsIntfNumber(LPCSTR pszNumber);

//初始化随机数
BOOL BISPRandInit(void);

//得到一个随机数
INT16 BISPRandGetValue(INT16 nMinValue,INT16 nMaxValue);


////////////////////////////////////////////////////
//以下是控制台用到的参数转换函数：

//整数
BOOL BISPStrToInt(LPCSTR lpszParam,int* pnValue);
//无符号整数
BOOL BISPStrToUint(LPCSTR lpszParam,UINT* pnValue);
//有范围限制的整数: nMin <= nValue <= nMax
BOOL BISPStrToIntRange(LPCSTR lpszParam,int* pnValue,int nMin,int nMax);
//有范围限制的无符号整数: nMin <= nValue <= nMax
BOOL BISPStrToUintRange(LPCSTR lpszParam,UINT* pnValue,UINT nMin,UINT nMax);
//A.B.C.D格式的IP地址或掩码
BOOL BISPStrToIpAddr(LPCSTR lpszParam,DWORD* pdwIPAddr);
//A.B.C.D/N格式的IP网段
BOOL BISPStrToIpNet(LPCSTR lpszParam,DWORD* pdwIPAddr,DWORD* pdwIPMask);
//X/Y格式的接口扩展号码，比如fa0/1
BOOL BISPStrToExNum(LPCSTR lpszParam,int* pnNum1,int* pnNum2);


#ifdef __cplusplus
};
#endif //__cplusplus



#endif //__BISPUTIL_H_INCLUDED

