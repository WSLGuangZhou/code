///////////////////////////////////////////////////////////////
//	文 件 名 : BISPUtil.cpp
//	文件功能 : BISP 常用工具模块
//	作    者 : 谭 北 华
//	创建时间 : 2006年10月25日
//	项目名称 : 
//	备    注 :
//	历史记录 :  2007.11.08 修正对设置堆栈大小的处理
//				2007.12.11 修正BISPCheckSum函数中在计算奇数个字节时，
//							在大字节顺序的机器中会导致校验和计算错误的BUG
//			    2008.01.11 增加随机数函数BISPRandInit和BISPRandGetValue
//				2009.02.06 把BISPGetTickCount函数改为多媒体函数提高精度
//			    2010.01.15  修改BispGetTickCount函数相关的代码，把时间变成64位，解决若干时间后
//							TickCount归零引发的定时器和时间不正常的问题
//				2011.10.21  增加BISPStrToXX系列函数，把控制台字符串转换系列函数进行了重构
///////////////////////////////////////////////////////////////
//#include "stdafx.h"
#include "DTSCommon.h"
#include "BISPUtil.h"
#include <time.h>
#include <stdlib.h>
#include "BISPMem.h"

#include "BISPModLink.h"
BISP_DECLARE_MODULE_LINK(BISP_UTIL)

#ifdef NSL_OS_WINDOWS
#include <process.h>
//warning C4201: nonstandard extension used : nameless struct/union
#pragma warning(disable:4201)
#include <mmsystem.h>
#pragma comment(lib,"Winmm.lib")

typedef HANDLE  (WINAPI *PFN_OpenThread)(
  DWORD dwDesiredAccess,  // access right
  BOOL bInheritHandle,    // handle inheritance option
  DWORD dwThreadId        // thread identifier
);

//以下类用来自动初始化多媒体定时器2009.02.06
class CMMTimeInit
{
public:
	CMMTimeInit()
	{
		timeBeginPeriod(1);
	}
	~CMMTimeInit()
	{
		timeEndPeriod(1);
	}
	DWORD GetTick() const
	{
		return timeGetTime();
	}
};
static CMMTimeInit g_MMTimeInit;
//#pragma comment(lib,"../Lib/softbus.lib")
#endif //NSL_OS_WINDOWS

#ifdef NSL_OS_VXWORKS
#include <taskLib.h>
#endif //NSL_OS_VXWORKS


#ifdef NSL_OS_LINUX
#include <pthread.h>
#endif


//初始化Eigrp任务
int BISPTaskCreate(BISPTASKPROC pfnEntry,
					 void* pParam,
					 int nPriority,
					 int nStackSize,
					 const char* pTaskName)
{
	BISPASSERT(NULL != pfnEntry);

	int nTaskHandle = 0;

//////////////////////////////////////////////////////////////////////////
#ifdef NSL_OS_WINDOWS

	typedef unsigned (__stdcall *WINTHREADPTR)(void*);

	unsigned uId;
	nTaskHandle = (int)_beginthreadex(NULL,0,(WINTHREADPTR)pfnEntry,pParam,0,(unsigned*)&uId);
#endif //	NSL_OS_WINDOWS

//////////////////////////////////////////////////////////////////////////
#ifdef NSL_OS_VXWORKS

	/* 设置nStackSize = 时另其为默认的堆栈大小*/
	if(nStackSize <= 0)
	{
		nStackSize = BISP_DEFAULT_STACK_SIZE;
	}

	nTaskHandle = taskSpawn((char*)pTaskName,
					nPriority,
					VX_FP_TASK,
					nStackSize,
					(FUNCPTR)pfnEntry,
					(int)pParam,
					0,0,0,0,0,0,0,0,0);


#endif //NSL_OS_VXWORKS

//////////////////////////////////////////////////////////////////////////
#ifdef NSL_OS_LINUX
	pthread_t pthread;
	int nRet;
	nRet = pthread_create(&pthread,NULL,pfnEntry,pParam);
	if (!nRet)
	{
		errno = nRet;
		perror("pthread_create");
		return -1;
	}

#endif
		
	return nTaskHandle;
}

#ifdef NSL_OS_WINDOWS

//windows线程创建传递的参数
typedef struct tagWin32TaskParam
{
	HANDLE			hEvent;
	BISPTASKFUNC	pfnEntry;
	int				nParam1;
	int				nParam2;
	int				nParam3;
	int				nParam4;
	int				nParam5;
	int				nParam6;
	int				nParam7;
	int				nParam8;
	int				nParam9;
	int				nParam10;	
}WIN32TASKPARAM,*LPWIN32TASKPARAM;

//后台过渡线程
LOCAL int SYSAPICALL Win32TaskProc(LPWIN32TASKPARAM pParam)
{
	BISPASSERT(NULL != pParam);

	WIN32TASKPARAM win32Param = *pParam;
	
	//通知前台线程创建完成
	::SetEvent(win32Param.hEvent);

	//返回多参数调用结果
	return (*win32Param.pfnEntry)(win32Param.nParam1,
								  win32Param.nParam2,
								  win32Param.nParam3,
								  win32Param.nParam4,
								  win32Param.nParam5,
								  win32Param.nParam6,
								  win32Param.nParam7,
								  win32Param.nParam8,
								  win32Param.nParam9,
								  win32Param.nParam10);
}


#endif //NSL_OS_WINDOWS

//初始化系统任务,采用和VxWorks类似的调用版本
int BISPTaskSpawn(BISPTASKFUNC pfnEntry,
					 int nPriority,
					 int nStackSize,
					 const char* pTaskName,
					 int nParam1,
					 int nParam2,
					 int nParam3,
					 int nParam4,
					 int nParam5,
					 int nParam6,
					 int nParam7,
					 int nParam8,
					 int nParam9,
					 int nParam10)
{
	BISPASSERT(NULL != pfnEntry);

	int nTaskHandle = 0;

#ifdef NSL_OS_WINDOWS

	typedef unsigned (__stdcall *WINTHREADPTR)(void*);

	//传递参数进去
	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	WIN32TASKPARAM win32Param = {hEvent,pfnEntry,
								nParam1,nParam2,nParam3,nParam4,nParam5,
								nParam6,nParam7,nParam8,nParam9,nParam10};

	
	unsigned uId;
	nTaskHandle = (int)_beginthreadex(NULL,0,(WINTHREADPTR)Win32TaskProc,&win32Param,0,(unsigned*)&uId);
	
	//等待后台完成拷贝参数后就返回
	WaitForSingleObject(hEvent,INFINITE);
	CloseHandle(hEvent);
#endif //WIN32	

#ifdef NSL_OS_VXWORKS

	/* 设置nStackSize = 时另其为默认的堆栈大小*/
	if(nStackSize <= 0)
	{
		nStackSize = BISP_DEFAULT_STACK_SIZE;
	}

	nTaskHandle = taskSpawn((char*)pTaskName,
					nPriority,
					VX_FP_TASK,
					nStackSize,
					(FUNCPTR)pfnEntry,
					nParam1,nParam2,nParam3,nParam4,nParam5,
					nParam6,nParam7,nParam8,nParam9,nParam10);


#endif //NSL_OS_VXWORKS
	
	return nTaskHandle;
}

//结束清理任务
BOOL BISPTaskCleanup(int nTaskHandle)
{
#ifdef NSL_OS_WINDOWS
	if(0 != nTaskHandle)
	{
		return CloseHandle((HANDLE)nTaskHandle);
	}
#endif //NSL_OS_WINDOWS
	return TRUE;
}

//线程是否有效?
BOOL BISPTaskIsExist(int nTaskHandle)
{
#ifdef NSL_OS_WINDOWS
	DWORD dwExitCode;
	if(!GetExitCodeThread((HANDLE)nTaskHandle,&dwExitCode))
	{
		return FALSE;
	}
	return (STILL_ACTIVE == dwExitCode)? TRUE:FALSE;
#endif //NSL_OS_WINDOWS

#ifdef NSL_OS_VXWORKS
	return (OK == taskIdVerify(nTaskHandle) )? TRUE:FALSE;
#endif //NSL_OS_VXWORKS

	return TRUE;
}

//线程是否活动?
BOOL BISPTaskIsActive(int nTaskHandle)
{
#ifdef NSL_OS_WINDOWS
	DWORD dwExitCode;
	if(!GetExitCodeThread((HANDLE)nTaskHandle,&dwExitCode))
	{
		return FALSE;
	}
	return (STILL_ACTIVE == dwExitCode)? TRUE:FALSE;
#endif //WIN32

#ifdef NSL_OS_VXWORKS
	return (OK == taskIdVerify(nTaskHandle) && !taskIsSuspended(nTaskHandle) )? TRUE:FALSE;
#endif //NSL_OS_VXWORKS

	return TRUE;
}

//取得当前句柄
int BISPTaskGetCurrentHandle(void)
{
#ifdef NSL_OS_WINDOWS
	//这里直接调用OpenThread函数有在不同版本的VC上编译，有时候会出现连接错误，改为函数指针调用方式
	PFN_OpenThread lpfnOpenThread = (PFN_OpenThread)::GetProcAddress(GetModuleHandle((LPCWSTR)"kernel32.dll"),
																	"OpenThread");
	
	HANDLE hThread = NULL;
	if(NULL != lpfnOpenThread)
	{
		hThread = (*lpfnOpenThread)(THREAD_QUERY_INFORMATION,FALSE,GetCurrentThreadId());
	}
	return (int)hThread;
#endif //WIN32

#ifdef NSL_OS_VXWORKS
	return taskIdSelf();
#endif //NSL_OS_VXWORKS

	return TRUE;
}

//强行终止一个线程
BOOL BISPTaskTerminate(int nTaskHandle,int nExitCode)
{
#ifdef NSL_OS_WINDOWS
	return TerminateThread((HANDLE)nTaskHandle,(DWORD)nExitCode);
#endif //NSL_OS_WINDOWS

#ifdef NSL_OS_VXWORKS
	return (OK == taskDelete(nTaskHandle) )? TRUE:FALSE;
#endif //NSL_OS_WINDOWS

	return TRUE;
}

#ifdef NSL_OS_WINDOWS

//转化成大写字母
//LOCAL char* strupr(char* string)
//{
//	return _strupr(string);
//}

//转化成小写字母
//LOCAL char* strlwr(char* string)
//{
//	return _strlwr(string);
//}
#endif //NSL_OS_WINDOWS

#ifdef NSL_OS_VXWORKS

//转化成大写字母
LOCAL char* strupr(char* string)
{
	char * cp;

	for (cp=string; *cp; ++cp)
	{
		if ('a' <= *cp && *cp <= 'z')
		{
			*cp += (char)('A' - 'a');
		}
	}
	return(string);
}

//转化成小写字母
LOCAL char* strlwr(char* string)
{
	char * cp;

	for (cp=string; *cp; ++cp)
	{ 
		if ('A' <= *cp && *cp <= 'Z')
		{
			*cp += ('a' - 'A');
		}
	}
	return(string);
}

#endif //NSL_OS_VXWORKS

//字符串替换函数
int BISPStrReplace(LPSTR lpszString,LPCSTR lpszOld, LPCSTR lpszNew)
{
	int nSourceLen = (int)strlen(lpszOld);
	if (nSourceLen == 0 || NULL == lpszString)
	{
		return 0;
	}
	
	//字符串长度
	int nOldLength = (int)strlen(lpszString);
	
	int nReplacementLen = (int)strlen(lpszNew);

	// loop once to figure out the size of the result string
	int nCount = 0;
	LPSTR lpszStart = lpszString;
	LPSTR lpszEnd = lpszString + nOldLength;
	LPSTR lpszTarget;
	while (lpszStart < lpszEnd)
	{
		while ((lpszTarget = strstr(lpszStart, lpszOld)) != NULL)
		{
			nCount++;
			lpszStart = lpszTarget + nSourceLen;
		}
		lpszStart += strlen(lpszStart) + 1;
	}

	// if any changes were made, make them
	if (nCount > 0)
	{
		int nNewLength =  nOldLength + (nReplacementLen - nSourceLen) * nCount;

		lpszStart = lpszString;
		lpszEnd = lpszString + nNewLength;

		// loop again to actually do the work
		while (lpszStart < lpszEnd)
		{
			while ( (lpszTarget = strstr(lpszStart, lpszOld)) != NULL)
			{
				int nBalance = nOldLength - ((int)(DWORD)(lpszTarget - lpszString) + nSourceLen);
				
				BISPMemMove(lpszTarget + nReplacementLen, 
						lpszTarget + nSourceLen, 
						(size_t)nBalance * sizeof(CHAR));

				BISPMemCpy(lpszTarget, lpszNew, (size_t)nReplacementLen * sizeof(CHAR));
				lpszStart = lpszTarget + nReplacementLen;
				lpszStart[nBalance] = '\0';
				nOldLength += (nReplacementLen - nSourceLen);
			}
			lpszStart += strlen(lpszStart) + 1;
		}
		BISPASSERT(lpszString[nNewLength] == '\0');
	}

	return nCount;
}



//把一个字符串转化成大写，Linux暂不支持
// char* BISPStrUpper(char* pString)
// {
// 	return strupr(pString);
// }

//把一个字符串转化成小写，Linux暂不支持
// char* BISPStrLower(char* pString)
// {
// 	return strlwr(pString);
// }

//数据校验
USHORT BISPCheckSum(USHORT *pBuffer, int nSize)
{
	unsigned long nCheckSum = 0;

	while(nSize >1 ) 
	{
		nCheckSum += *pBuffer++;
		nSize -= (int)sizeof(USHORT);
	}
  
	if(nSize >0) 
	{
		//2007.12.11 修正在大字节顺序的机器中会导致校验和计算错误的BUG
#ifdef NSL_OS_WINDOWS 	//widows
		nCheckSum += *(UCHAR*)pBuffer;
#endif //WIN32

#if defined(NSL_OS_VXWORKS ) || defined(NSL_OS_LINUX)
  #if	_BYTE_ORDER==_BIG_ENDIAN
         nCheckSum += (USHORT)((*(UCHAR*)pBuffer)<<8);
  #endif	/* _BYTE_ORDER==_BIG_ENDIAN */

  #if	_BYTE_ORDER==_LITTLE_ENDIAN
        nCheckSum += *(UCHAR*)pBuffer;
  #endif	/* _BYTE_ORDER==_LITTLE_ENDIAN */
#endif //NSL_OS_VXWORKS
	}

	nCheckSum = (nCheckSum >> 16) + (nCheckSum & 0xffff);
	nCheckSum += (nCheckSum >>16);

	return (USHORT)(~nCheckSum);	
}

//计算简单的校验和(短波电台用到)
//采用按字节简单异或校验，得到1 个Byte,
//取该Byte 的高4 个bit,作为校验的第一个字节的低4bit,高位填0; 
//取该Byte 的低4 个bit,作为校验的第二个字节的低4bit,高位填0。
USHORT BISPGetSimpleCRC(char *pBuffer, int nSize)
{
	int i;
	BYTE bCRC = 0;
	for(i=0; i<nSize; i++)
	{
		bCRC ^= pBuffer[i];
	}
	//填充CRC字段
	BYTE bCRCH = (BYTE)(bCRC>>4);
	BYTE bCRCL = (BYTE)(bCRC&0x0F);
	USHORT nSimpleCRC = (USHORT)(bCRCH<<8|bCRCL);
	return nSimpleCRC;
}

//取得当前的系统Tick数
UINT64 BISPGetTickCount(void)
{
	static UINT64 s_nBaseSysTick = 0;
	static ULONG s_nCurSysTick = 0;

#ifdef NSL_OS_WINDOWS
	//2009.02.06 把BISPGetTickCount函数改为多媒体函数提高精度
	ULONG nCurTick = (UINT)timeGetTime();
	//ULONG nCurTick = (UINT)GetTickCount();
	if(nCurTick < s_nCurSysTick)
	{
		s_nBaseSysTick += (UINT64)0xFFFFFFFF;	
	}
	s_nCurSysTick = nCurTick;
	return s_nBaseSysTick + (UINT64)nCurTick;
#endif //WIN32

#ifdef NSL_OS_VXWORKS

	ULONG nCurTick = (UINT)tickGet();
	if(nCurTick < s_nCurSysTick)
	{
		s_nBaseSysTick += (UINT64)0xFFFFFFFF;	
	}
	s_nCurSysTick = nCurTick;

	//返回折算的tick
	return (s_nBaseSysTick+nCurTick)*1000/(UINT)sysClkRateGet();

#endif //NSL_OS_VXWORKS
	return 0;
}

//是否是数字
BOOL BISPIsNumber(LPCSTR pszNumber)
{
	BOOL bIsNumber = TRUE;
	for(; '\0' != *pszNumber; pszNumber++ )
	{
		if(*pszNumber < '0' || *pszNumber > '9' )
		{
			//不是数字
			bIsNumber = FALSE;
			break;
		}
	}
	return bIsNumber;
}

//是否是网络接口数字? 如 2/1或 1等
BOOL BISPIsIntfNumber(LPCSTR pszNumber)
{
	//保证第一个个字符不是'/'
	if(*pszNumber < '0' || *pszNumber > '9' )
	{
		return FALSE;
	}
	
	int nSplitCount = 0;
	BOOL bIsNumber = TRUE;
	BOOL bLastSplit = FALSE;

	for(pszNumber++; '\0' != *pszNumber; pszNumber++ )
	{
		if(*pszNumber == '/')
		{
			nSplitCount++;
			bLastSplit = TRUE;
			continue;
		}
		if(*pszNumber < '0' || *pszNumber > '9' )
		{
			
			//不是数字
			bIsNumber = FALSE;
			break;
		}

		//保证最后一个字符不是'/'
		bLastSplit = FALSE;
	}
	
	//如果最后一个字符是'/'或者'/'字符的个数超过1个，就不对
	if(bLastSplit || nSplitCount > 1)
	{
		bIsNumber = FALSE;
	}

	return bIsNumber;
}


//初始化随机数
BOOL BISPRandInit(void)
{
	srand((unsigned int)time(NULL));

#ifdef NSL_OS_WINDOWS
	//避免PC Lint警告
	g_MMTimeInit.GetTick();
#endif //WIN32

	return TRUE;
}

//得到一个随机数
INT16 BISPRandGetValue(INT16 nMinValue,INT16 nMaxValue)
{
	INT nValue = (INT)nMinValue + (INT)((INT)nMaxValue - (INT)nMinValue)*rand()/RAND_MAX;

	return (INT16)nValue;
}

////////////////////////////////////////////////////
//以下是控制台用到的参数转换函数：

//整数
BOOL BISPStrToInt(LPCSTR lpszParam,int* pnValue)
{
	if(NULL == lpszParam || '\0' == *lpszParam)
	{
		return FALSE;
	}
	int nValue = atoi(lpszParam);
	if(NULL != pnValue)
	{
		*pnValue = nValue;
	}
	return TRUE;	
}
//无符号整数
BOOL BISPStrToUint(LPCSTR lpszParam,UINT* pnValue)
{
	if(NULL == lpszParam || '\0' == *lpszParam)
	{
		return FALSE;
	}
	UINT nValue = (UINT)atoi(lpszParam);
	if(NULL != pnValue)
	{
		*pnValue = nValue;
	}
	return TRUE;	
}
//有范围限制的整数: nMin <= nValue <= nMax
BOOL BISPStrToIntRange(LPCSTR lpszParam,int* pnValue,int nMin,int nMax)
{
	if(NULL == lpszParam || '\0' == *lpszParam)
	{
		return FALSE;
	}
	int nValue = atoi(lpszParam);
	if(nValue < nMin || nValue > nMax)
	{
		return FALSE;
	}
	if(NULL != pnValue)
	{
		*pnValue = nValue;
	}
	return TRUE;
}
//有范围限制的无符号整数: nMin <= nValue <= nMax
BOOL BISPStrToUintRange(LPCSTR lpszParam,UINT* pnValue,UINT nMin,UINT nMax)
{
	if(NULL == lpszParam || '\0' == *lpszParam)
	{
		return FALSE;
	}
	UINT nValue = (UINT)atoi(lpszParam);
	if(nValue < nMin || nValue > nMax)
	{
		return FALSE;
	}
	if(NULL != pnValue)
	{
		*pnValue = nValue;
	}
	return TRUE;
}
//A.B.C.D格式的IP地址
BOOL BISPStrToIpAddr(LPCSTR lpszParam,DWORD* pdwIPAddr)
{
	if(NULL == lpszParam || '\0' == *lpszParam)
	{
		return FALSE;
	}
	int nIPs[4];
	if(4 != sscanf(lpszParam,"%d.%d.%d.%d",&nIPs[0],&nIPs[1],&nIPs[2],&nIPs[3]) ||
		(nIPs[0] < 0 || nIPs[0] > 255 || nIPs[1] < 0 || nIPs[1] > 255 ||
		nIPs[2] < 0 || nIPs[2] > 255 || nIPs[3] < 0 || nIPs[3] > 255 ) )
	{
		return FALSE;
	}
	// 得到IP地址
	if(NULL != pdwIPAddr)
	{
		*pdwIPAddr = BISP_MAKE_DWORD(nIPs[0],nIPs[1],nIPs[2],nIPs[3]);
	}
	return TRUE;
}
//A.B.C.D/N格式的IP网段
BOOL BISPStrToIpNet(LPCSTR lpszParam,DWORD* pdwIPAddr,DWORD* pdwIPMask)
{
	if(NULL == lpszParam || '\0' == *lpszParam)
	{
		return FALSE;
	}
	int nMaskBit = 0;
	int nIPs[4];
	if(5 != sscanf(lpszParam,"%d.%d.%d.%d/%d",&nIPs[0],&nIPs[1],&nIPs[2],&nIPs[3],&nMaskBit) ||
		(nIPs[0] < 0 || nIPs[0] > 255 || nIPs[1] < 0 || nIPs[1] > 255 ||
		nIPs[2] < 0 || nIPs[2] > 255 || nIPs[3] < 0 || nIPs[3] > 255 ) ||
		(nMaskBit <0 || nMaskBit >32) )
	{
		return FALSE;
	}
	// 得到IP地址
	if(NULL != pdwIPAddr)
	{
		*pdwIPAddr = BISP_MAKE_DWORD(nIPs[0],nIPs[1],nIPs[2],nIPs[3]);
	}
	if(NULL != pdwIPMask)
	{
	//	*pdwIPMask = GET_MASK_IN_COUNT(nMaskBit);
		*pdwIPMask = (0xFFFFFFFF << (32 - ((BYTE)nMaskBit)) );
	}
	return TRUE;
}
//X/Y格式的接口扩展号码，比如fa0/1
BOOL BISPStrToExNum(LPCSTR lpszParam,int* pnNum1,int* pnNum2)
{
	if(NULL == lpszParam || '\0' == *lpszParam)
	{
		return FALSE;
	}
	int nNum1,nNum2;
	if(2 != sscanf(lpszParam,"%d/%d",&nNum1,&nNum2))
	{
		return FALSE;
	}
	if(NULL != pnNum1)
	{
		*pnNum1 = nNum1;
	}
	if(NULL != pnNum2)
	{
		*pnNum2 = nNum2;
	}
	return TRUE;
}

#ifdef NSL_OS_LINUX
	int BISPEventWaitEvent(sem_t *event, UINT milliseconds)
	{
		struct timespec ts = {0};
		ts.tv_sec = milliseconds/1000;
		ts.tv_nsec = milliseconds%1000;

// 		printf("BISPEventWaitEvent milliseconds(%d) ts.tv_sec(%d) ts.tv_nsec(%d)",
// 				milliseconds,ts.tv_sec,ts.tv_nsec);
		if (LOOP_FOREVER == milliseconds)
		{
			sem_wait(event);
		}
		else
		{
			sem_timedwait(event, &ts);
		}
		
		return 0;
	}
#endif /* NSL_OS_LINUX */
