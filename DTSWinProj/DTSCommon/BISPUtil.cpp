///////////////////////////////////////////////////////////////
//	�� �� �� : BISPUtil.cpp
//	�ļ����� : BISP ���ù���ģ��
//	��    �� : ̷ �� ��
//	����ʱ�� : 2006��10��25��
//	��Ŀ���� : 
//	��    ע :
//	��ʷ��¼ :  2007.11.08 ���������ö�ջ��С�Ĵ���
//				2007.12.11 ����BISPCheckSum�������ڼ����������ֽ�ʱ��
//							�ڴ��ֽ�˳��Ļ����лᵼ��У��ͼ�������BUG
//			    2008.01.11 �������������BISPRandInit��BISPRandGetValue
//				2009.02.06 ��BISPGetTickCount������Ϊ��ý�庯����߾���
//			    2010.01.15  �޸�BispGetTickCount������صĴ��룬��ʱ����64λ���������ʱ���
//							TickCount���������Ķ�ʱ����ʱ�䲻����������
//				2011.10.21  ����BISPStrToXXϵ�к������ѿ���̨�ַ���ת��ϵ�к����������ع�
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

//�����������Զ���ʼ����ý�嶨ʱ��2009.02.06
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


//��ʼ��Eigrp����
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

	/* ����nStackSize = ʱ����ΪĬ�ϵĶ�ջ��С*/
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

//windows�̴߳������ݵĲ���
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

//��̨�����߳�
LOCAL int SYSAPICALL Win32TaskProc(LPWIN32TASKPARAM pParam)
{
	BISPASSERT(NULL != pParam);

	WIN32TASKPARAM win32Param = *pParam;
	
	//֪ͨǰ̨�̴߳������
	::SetEvent(win32Param.hEvent);

	//���ض�������ý��
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

//��ʼ��ϵͳ����,���ú�VxWorks���Ƶĵ��ð汾
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

	//���ݲ�����ȥ
	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	WIN32TASKPARAM win32Param = {hEvent,pfnEntry,
								nParam1,nParam2,nParam3,nParam4,nParam5,
								nParam6,nParam7,nParam8,nParam9,nParam10};

	
	unsigned uId;
	nTaskHandle = (int)_beginthreadex(NULL,0,(WINTHREADPTR)Win32TaskProc,&win32Param,0,(unsigned*)&uId);
	
	//�ȴ���̨��ɿ���������ͷ���
	WaitForSingleObject(hEvent,INFINITE);
	CloseHandle(hEvent);
#endif //WIN32	

#ifdef NSL_OS_VXWORKS

	/* ����nStackSize = ʱ����ΪĬ�ϵĶ�ջ��С*/
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

//������������
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

//�߳��Ƿ���Ч?
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

//�߳��Ƿ�?
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

//ȡ�õ�ǰ���
int BISPTaskGetCurrentHandle(void)
{
#ifdef NSL_OS_WINDOWS
	//����ֱ�ӵ���OpenThread�������ڲ�ͬ�汾��VC�ϱ��룬��ʱ���������Ӵ��󣬸�Ϊ����ָ����÷�ʽ
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

//ǿ����ֹһ���߳�
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

//ת���ɴ�д��ĸ
//LOCAL char* strupr(char* string)
//{
//	return _strupr(string);
//}

//ת����Сд��ĸ
//LOCAL char* strlwr(char* string)
//{
//	return _strlwr(string);
//}
#endif //NSL_OS_WINDOWS

#ifdef NSL_OS_VXWORKS

//ת���ɴ�д��ĸ
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

//ת����Сд��ĸ
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

//�ַ����滻����
int BISPStrReplace(LPSTR lpszString,LPCSTR lpszOld, LPCSTR lpszNew)
{
	int nSourceLen = (int)strlen(lpszOld);
	if (nSourceLen == 0 || NULL == lpszString)
	{
		return 0;
	}
	
	//�ַ�������
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



//��һ���ַ���ת���ɴ�д��Linux�ݲ�֧��
// char* BISPStrUpper(char* pString)
// {
// 	return strupr(pString);
// }

//��һ���ַ���ת����Сд��Linux�ݲ�֧��
// char* BISPStrLower(char* pString)
// {
// 	return strlwr(pString);
// }

//����У��
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
		//2007.12.11 �����ڴ��ֽ�˳��Ļ����лᵼ��У��ͼ�������BUG
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

//����򵥵�У���(�̲���̨�õ�)
//���ð��ֽڼ����У�飬�õ�1 ��Byte,
//ȡ��Byte �ĸ�4 ��bit,��ΪУ��ĵ�һ���ֽڵĵ�4bit,��λ��0; 
//ȡ��Byte �ĵ�4 ��bit,��ΪУ��ĵڶ����ֽڵĵ�4bit,��λ��0��
USHORT BISPGetSimpleCRC(char *pBuffer, int nSize)
{
	int i;
	BYTE bCRC = 0;
	for(i=0; i<nSize; i++)
	{
		bCRC ^= pBuffer[i];
	}
	//���CRC�ֶ�
	BYTE bCRCH = (BYTE)(bCRC>>4);
	BYTE bCRCL = (BYTE)(bCRC&0x0F);
	USHORT nSimpleCRC = (USHORT)(bCRCH<<8|bCRCL);
	return nSimpleCRC;
}

//ȡ�õ�ǰ��ϵͳTick��
UINT64 BISPGetTickCount(void)
{
	static UINT64 s_nBaseSysTick = 0;
	static ULONG s_nCurSysTick = 0;

#ifdef NSL_OS_WINDOWS
	//2009.02.06 ��BISPGetTickCount������Ϊ��ý�庯����߾���
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

	//���������tick
	return (s_nBaseSysTick+nCurTick)*1000/(UINT)sysClkRateGet();

#endif //NSL_OS_VXWORKS
	return 0;
}

//�Ƿ�������
BOOL BISPIsNumber(LPCSTR pszNumber)
{
	BOOL bIsNumber = TRUE;
	for(; '\0' != *pszNumber; pszNumber++ )
	{
		if(*pszNumber < '0' || *pszNumber > '9' )
		{
			//��������
			bIsNumber = FALSE;
			break;
		}
	}
	return bIsNumber;
}

//�Ƿ�������ӿ�����? �� 2/1�� 1��
BOOL BISPIsIntfNumber(LPCSTR pszNumber)
{
	//��֤��һ�����ַ�����'/'
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
			
			//��������
			bIsNumber = FALSE;
			break;
		}

		//��֤���һ���ַ�����'/'
		bLastSplit = FALSE;
	}
	
	//������һ���ַ���'/'����'/'�ַ��ĸ�������1�����Ͳ���
	if(bLastSplit || nSplitCount > 1)
	{
		bIsNumber = FALSE;
	}

	return bIsNumber;
}


//��ʼ�������
BOOL BISPRandInit(void)
{
	srand((unsigned int)time(NULL));

#ifdef NSL_OS_WINDOWS
	//����PC Lint����
	g_MMTimeInit.GetTick();
#endif //WIN32

	return TRUE;
}

//�õ�һ�������
INT16 BISPRandGetValue(INT16 nMinValue,INT16 nMaxValue)
{
	INT nValue = (INT)nMinValue + (INT)((INT)nMaxValue - (INT)nMinValue)*rand()/RAND_MAX;

	return (INT16)nValue;
}

////////////////////////////////////////////////////
//�����ǿ���̨�õ��Ĳ���ת��������

//����
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
//�޷�������
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
//�з�Χ���Ƶ�����: nMin <= nValue <= nMax
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
//�з�Χ���Ƶ��޷�������: nMin <= nValue <= nMax
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
//A.B.C.D��ʽ��IP��ַ
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
	// �õ�IP��ַ
	if(NULL != pdwIPAddr)
	{
		*pdwIPAddr = BISP_MAKE_DWORD(nIPs[0],nIPs[1],nIPs[2],nIPs[3]);
	}
	return TRUE;
}
//A.B.C.D/N��ʽ��IP����
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
	// �õ�IP��ַ
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
//X/Y��ʽ�Ľӿ���չ���룬����fa0/1
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
