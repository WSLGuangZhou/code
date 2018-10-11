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
//����ģ�鶨��: BISP_UTIL
//BISP �������߶���

#ifndef __BISPUTIL_H_INCLUDED
#define __BISPUTIL_H_INCLUDED

//#include "BISPDebug.h"

//�ɱ�����Ļص�����
#ifdef __cplusplus
typedef int 	(*JOBFUNCPTR) (...);					 /* ptr to function returning int */
typedef int 	(*JOBTIMERFUNCPTR) (UINT nTimerId,...);  /* ptr to timer function returning int */
#else
typedef int 	(*JOBFUNCPTR) ();						/* ptr to function returning int */
typedef int 	(*JOBTIMERFUNCPTR) ();					/* ptr to timer function returning int */
#endif	


//ȡ���ֵ
#define BISPMAX(a,b) ((a)>(b)? (a):(b))
//ȡ��Сֵ
#define BISPMIN(a,b) ((a)<(b)? (a):(b))

//��2���ֽ�ת����1��WORD
#define BISP_MAKE_WORD(h,l) \
	((WORD)( (WORD)(h)<<8 | (WORD)(l)))

//��4���ֽڻ���1��DWORD
#define BISP_MAKE_DWORD(hh,hl,lh,ll) \
	((DWORD)((DWORD)(hh)<<24 | (DWORD)(hl)<<16 | (DWORD)(lh)<<8 | (DWORD)(ll)))

////////////////////////////////////////////
//λ����

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
//������λ����궨��:

#ifdef BIT_TEST
#undef BIT_TEST
#endif //BIT_TEST

//����һ��BIT�Ƿ���1
#define BIT_TEST(f,b)	((f) & (b))

#ifdef BIT_COMPARE
#undef BIT_COMPARE
#endif //BIT_COMPARE

//���Զ��BIT�Ƿ���1
#define BIT_COMPARE(f,b)	(((f) & (b)) == (b) )


#ifdef BIT_SET
#undef BIT_SET
#endif //BIT_SET

//BIT����
#define BIT_SET(f,b)	((f) |= (b))

#ifdef BIT_CLEAR
#undef BIT_CLEAR
#endif //BIT_CLEAR

//BIT���
#define BIT_CLEAR(f,b)	((f) &= ~(b))

///////////////////////////////////////////
//�Ĵ�������

//8λ�Ĵ�������
#define BISP_REG8(addr) \
   (*((volatile unsigned char *)(addr)))

//16λ�Ĵ�������
#define BISP_REG16(addr) \
   (*((volatile unsigned short *)(addr)))

//32λ�Ĵ�������
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


//ϵͳ�л�����
#define SYS_TASK_SWITCH() SYS_DELAY(0)

//����Windows�ĺ�������
#if defined(NSL_OS_WINDOWS) || defined(NSL_OS_VXWORKS)
	typedef int (SYSAPICALL *BISPTASKPROC)(void* pParam);
#endif

//Linux Define
#if defined(NSL_OS_LINUX)
	typedef void*(*BISPTASKPROC)(void *arg);
#endif

//����VxWorks�ĺ�������
#ifdef __cplusplus
typedef int (*BISPTASKFUNC)(...);
#else
typedef int (*BISPTASKFUNC)();
#endif //__cplusplus

//���޵ȴ�(s)
#ifndef LOOP_FOREVER
#define LOOP_FOREVER ((UINT)-1)
#endif //LOOP_FOREVER

//����ʱ��
#ifndef BISP_INFINITE
#define BISP_INFINITE 0xFFFFFFFF
#endif //BISP_INFINITE

//1��=1000����
#define MSEC_PER_SECOND 1000L

//Ĭ�ϵ����ȼ��Ͷ�ջ��С
#define BISP_DEFAULT_TASK_PRIORITY 100
//Ĭ�ϵĶ�ջ��С
#define BISP_DEFAULT_STACK_SIZE 0x4000


/////////////////////////////////////////////////////
//��ƽ̨���ٽ�������
#ifdef NSL_OS_WINDOWS

//��Windowsϵͳ�ٽ����Ŀ�ƽ̨����
#define BISP_CRISEC CRITICAL_SECTION
#define BISPCrisec_Init(crisec) ::InitializeCriticalSection(&(crisec))
#define BISPCrisec_Deinit(crisec) ::DeleteCriticalSection(&(crisec))
#define BISPCrisec_Lock(crisec) ::EnterCriticalSection(&(crisec))
#define BISPCrisec_Unlock(crisec) ::LeaveCriticalSection(&(crisec))

//��Windowsϵͳ�¼��Ŀ�ƽ̨����
#define BISP_EVENT HANDLE
#define BISPEvent_Init(hEvent) ((hEvent) = ::CreateEvent(NULL,FALSE,FALSE,NULL))
#define BISPEvent_Deinit(hEvent) ::CloseHandle(hEvent)
#define BISPEvent_WaitEvent(hEvent,dwWaitTime) ::WaitForSingleObject(hEvent, dwWaitTime)
#define BISPEvent_SetEvent(hEvent) ::SetEvent(hEvent)

//��Windowsϵͳ����ID�Ŀ�ƽ̨����
#define BISPGETTASKID() ((UINT)GetCurrentThreadId())
#define BISPSYS_GETTICKCOUNT() ((UINT)GetTickCount())

//Windowsƽ̨��intlock����
#define intLock() 0L
#define intUnlock(oldlevel) ((oldlevel) = 0)

#elif defined(NSL_OS_VXWORKS)

#include "semLib.h"
#include "taskLib.h"
#include "intLib.h"

//��VxWorksϵͳ�ٽ����Ŀ�ƽ̨����
#define BISP_CRISEC SEM_ID
//���¶���semMCreate��Ϊ�˷�ֹ�����ȼ���ת�����ⷢ��
#define BISPCrisec_Init(crisec) ((crisec) = semMCreate(SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_DELETE_SAFE))
//#define BISPCrisec_Init(crisec) ((crisec) = semBCreate(SEM_Q_PRIORITY,SEM_FULL))
#define BISPCrisec_Deinit(crisec) semDelete(crisec)
#define BISPCrisec_Lock(crisec) semTake(crisec,WAIT_FOREVER)
#define BISPCrisec_Unlock(crisec) semGive(crisec)

//��VxWorksϵͳ�¼��Ŀ�ƽ̨����
#define BISP_EVENT SEM_ID
#define BISPEvent_Init(hEvent) ((hEvent) = semBCreate(SEM_Q_PRIORITY,SEM_EMPTY))
#define BISPEvent_Deinit(hEvent) semDelete(hEvent)
#define BISPEvent_WaitEvent(hEvent,dwWaitTime) semTake(hEvent, (int)(dwWaitTime))
#define BISPEvent_SetEvent(hEvent) semGive(hEvent)

//��VxWorksϵͳ����ID�Ŀ�ƽ̨����
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

//Linuxƽ̨�ݶ�û��intLock
#define intLock() 0L
#define intUnlock(oldlevel) ((oldlevel) = 0)

#endif /* NSL_OS_WINDOWS */


//���ӡ��롢����ת����

//����ת���ɷ���
#define BISP_MS_TO_MIN(ms) ((ms)/(60*1000))
//����ת������
#define BISP_MS_TO_SEC(ms) ((ms)/1000)
//������
#define BISP_SEC_IN_MIN(s) ((s)%60)
//��������
#define BISP_MS_IN_SEC(ms) ((ms)%1000)
//����ת������
#define BISP_MIN_TO_SEC(m) ((m)*(60))
//����ת���ɺ���
#define BISP_MIN_TO_MS(m) ((m)*(60*1000))
//��ת���ɺ���
#define BISP_SEC_TO_MS(s) ((s)*1000)


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifndef BISP_CONFIG_NO_BISP_TASK

//��ʼ��ϵͳ����,���ú�Windows���Ƶĵ��ð汾
int BISPTaskCreate(BISPTASKPROC pfnEntry,
					 void* pParam,
					 int nPriority,
					 int nStackSize,
					 const char* pTaskName);

//��ʼ��ϵͳ����,���ú�VxWorks���Ƶĵ��ð汾���ݲ�֧��Linux
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

//������������
BOOL BISPTaskCleanup(int nTaskHandle);

//�߳��Ƿ���Ч?��Linux�ݲ�֧��
//BOOL BISPTaskIsExist(int nTaskHandle);

//�߳��Ƿ�?��Linux�ݲ�֧��
//BOOL BISPTaskIsActive(int nTaskHandle);

//ȡ�õ�ǰ�����Linux�ݲ�֧��
//int BISPTaskGetCurrentHandle(void);

//ǿ����ֹһ���̣߳�Linux�ݲ�֧��
//BOOL BISPTaskTerminate(int nTaskHandle,int nExitCode);

#endif /* BISP_CONFIG_NO_BISP_TASK */

//��һ���ַ���ת���ɴ�д��Linux�ݲ�֧��
//char* BISPStrUpper(char* pString);

//��һ���ַ���ת����Сд��Linux�ݲ�֧��
//char* BISPStrLower(char* pString);

//�ַ����滻����
int BISPStrReplace(LPSTR lpszString,LPCSTR lpszOld, LPCSTR lpszNew);

//ȡ�õ�ǰ��Tick�����ݲ�֧��Linux
//UINT64 BISPGetTickCount(void);

#ifdef NSL_OS_LINUX
	int BISPEventWaitEvent(sem_t *event, UINT milliseconds);
#endif /* NSL_OS_LINUX */

//#ifndef NSL_OS_LINUX
//����У���
USHORT BISPCheckSum(USHORT *pBuffer, int nSize);
//#endif /* NSL_OS_LINUX */

//����򵥵�У���(�̲���̨�õ�)
//���ð��ֽڼ����У�飬�õ�1 ��Byte,ȡ��Byte �ĸ�4 ��bit,
//��ΪУ��ĵ�һ���ֽڵĵ�4bit,��λ��0; ȡ��Byte �ĵ�4 ��bit,
//��ΪУ��ĵڶ����ֽڵĵ�4bit,��λ��0��
USHORT BISPGetSimpleCRC(char *pBuffer, int nSize);

//�Ƿ�������
BOOL BISPIsNumber(LPCSTR pszNumber);

//�Ƿ�������ӿ�����? �� 2/1�� 1��
BOOL BISPIsIntfNumber(LPCSTR pszNumber);

//��ʼ�������
BOOL BISPRandInit(void);

//�õ�һ�������
INT16 BISPRandGetValue(INT16 nMinValue,INT16 nMaxValue);


////////////////////////////////////////////////////
//�����ǿ���̨�õ��Ĳ���ת��������

//����
BOOL BISPStrToInt(LPCSTR lpszParam,int* pnValue);
//�޷�������
BOOL BISPStrToUint(LPCSTR lpszParam,UINT* pnValue);
//�з�Χ���Ƶ�����: nMin <= nValue <= nMax
BOOL BISPStrToIntRange(LPCSTR lpszParam,int* pnValue,int nMin,int nMax);
//�з�Χ���Ƶ��޷�������: nMin <= nValue <= nMax
BOOL BISPStrToUintRange(LPCSTR lpszParam,UINT* pnValue,UINT nMin,UINT nMax);
//A.B.C.D��ʽ��IP��ַ������
BOOL BISPStrToIpAddr(LPCSTR lpszParam,DWORD* pdwIPAddr);
//A.B.C.D/N��ʽ��IP����
BOOL BISPStrToIpNet(LPCSTR lpszParam,DWORD* pdwIPAddr,DWORD* pdwIPMask);
//X/Y��ʽ�Ľӿ���չ���룬����fa0/1
BOOL BISPStrToExNum(LPCSTR lpszParam,int* pnNum1,int* pnNum2);


#ifdef __cplusplus
};
#endif //__cplusplus



#endif //__BISPUTIL_H_INCLUDED

