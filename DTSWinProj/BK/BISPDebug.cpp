///////////////////////////////////////////////////////////////
//	�� �� �� : BISPDebug.cpp
//	�ļ����� : ͨ�õ������ӡ
//	��    �� : ̷ �� ��
//	����ʱ�� : 2006��10��25��
//	��Ŀ���� : BISP
//	��    ע :
//	��ʷ��¼ : 2007.12.11 ���Դ�ӡһ���յ��ַ���
//			   2008.03.12 �޸�BISPDebugPrint������ֱ�Ӳ���printf�ķ�ʽ��
//						   �÷�ʽ����ĳЩ�����ӡ����ȷ����Ϊprintf("%s",...
//			   2008.07.30 �޸�BISPASSERT���壬��BISPAssertFailedLine��������cpp��ʵ��	
//			   2010.01.15  �޸�BispGetTickCount������صĴ��룬��ʱ����64λ���������ʱ���
//							TickCount���������Ķ�ʱ����ʱ�䲻����������
//			   2010.04.14  �޸�BISPDebugGetOutput���������Է��ص�ǰ�Ĵ�ӡָ��
//			   2010.04.21  ����BISPDebugPrintEnable����֧��Telnetʱ��ֹЭ���ӡ����  
//			   2010.04.27  �޸�BISPDebugPrint��BISPPrint��������ֹ���ж��н��д�ӡ	
//			   2010.06.02  �޸�BISPAssertFailedLine������������logMsg�����д����ַ�������
//			   2011.06.17  ����BISPDebugSetDebugCtrl����
///////////////////////////////////////////////////////////////

#include "DTSCommon.h"
//#include "stdafx.h"
#include "BISPDebug.h"
#include <time.h>
#include "BISPConsole.h"
#include "BISPUtil.h"

#include "BISPModLink.h"
BISP_DECLARE_MODULE_LINK(BISP_DEBUG)

#ifndef WIN32
#include <sysLib.h>
#include <intLib.h>
#endif //WIN32

//���Կ�����
DWORD g_dwDebugCtrlValue[BISP_DEBUG_CTRL_COUNT] = {0};

//12���µ�Ӣ�ļ��
const char* g_szMonName[12] = 
{
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec",
};

//show debugging������
LOCAL PFN_SHOWDEBUGGING g_lpfnShowDebugging[BISP_DEBUG_CTRL_COUNT] = {NULL};

//Ĭ�ϵ����λ��!
LOCAL FHANDLE	g_hDebugOutput = NULL;

//�������
LOCAL PFN_OUTPUTSTRING g_pfnOutput = NULL;

//ģʽ����(EIGRP��FRP)
char g_szModeName[8] = "EIGRP";

//�Ƿ���õ��Դ�ӡ?
LOCAL BOOL g_bDisableDebugPrint = FALSE;

////////////////////////////////////////////////////////////////////

#ifndef WIN32

#include <intLib.h>
#include <logLib.h>

//�ַ�������
LOCAL const char g_pAssertMsg[] = 
"Debug Assertion Failed!:\n  File:  %s\n  Line:  %d\n  Thread Id: %x%s\n  Expression:  %s\n";
LOCAL char	g_nInterruptAssertMsg[512];

BISP_CFUNC
void BISPAssertFailedLine(const char* lpszFileName, int nLine,const char* lpszExpres)
{
	if(INT_CONTEXT())
	{
		sprintf(g_nInterruptAssertMsg,g_pAssertMsg,lpszFileName,nLine,0,"(Interrupt Level)",lpszExpres,0);
		logMsg((char*)g_nInterruptAssertMsg,1,2,3,4,5,6);

		//���ﲻ�����ж�logMsg�����ַ�������
		//logMsg((char*)g_pAssertMsg,(int)lpszFileName,(int)nLine,0,(int)"(Interrupt Level)",(int)lpszExpres,0);
	}
	else
	{
		printf(g_pAssertMsg,lpszFileName,nLine,taskIdSelf(),"",lpszExpres);
	}

	return;
}

#endif //WIN32

//�պ���
void CDECL BISPTraceVoid(LPCSTR lpszFormat, ...)
{

}

//ʹ��Debug
void BISPDebugEnable(int nIndex,DWORD dwDebugCtrl)
{
	BISPASSERT( nIndex >=0 && nIndex < BISP_DEBUG_CTRL_COUNT);

	g_dwDebugCtrlValue[nIndex] |= dwDebugCtrl;
}

//ʹ����Debug
void BISPDebugDisable(int nIndex,DWORD dwDebugCtrl)
{
	BISPASSERT( nIndex >=0 && nIndex < BISP_DEBUG_CTRL_COUNT);
	g_dwDebugCtrlValue[nIndex] &= ~dwDebugCtrl;
}

//ȡ��Debug�����ַ�
DWORD BISPDebugGetDebugCtrl(int nIndex)
{
	BISPASSERT( nIndex >=0 && nIndex < BISP_DEBUG_CTRL_COUNT);
	return g_dwDebugCtrlValue[nIndex];
}

//����Debug�����ַ�
DWORD BISPDebugSetDebugCtrl(int nIndex,DWORD dwNewCtrl)
{
	BISPASSERT( nIndex >=0 && nIndex < BISP_DEBUG_CTRL_COUNT);
	DWORD dwOldCtrl = g_dwDebugCtrlValue[nIndex];
	g_dwDebugCtrlValue[nIndex] = dwNewCtrl;
	return dwOldCtrl;
}

//�õ����ͨ��
FHANDLE BISPDebugGetOutput(PFN_OUTPUTSTRING* ppfnOutput)
{
	if(NULL != ppfnOutput)
	{
		*ppfnOutput = g_pfnOutput;
	}
	return g_hDebugOutput;
}


//�������ͨ��
FHANDLE BISPDebugSetOutput(FHANDLE hOutput,PFN_OUTPUTSTRING lpfnOutput)
{
	FHANDLE hOldFile = g_hDebugOutput;

	//�����ÿյľ��
	g_hDebugOutput = NULL;
	//Ȼ��ı亯��ָ��
	g_pfnOutput = lpfnOutput;
	//��������µľ��
	g_hDebugOutput = hOutput;

	//ÿ�������µķǿմ�ӡ��Ҫ��ʹ�ܴ�ӡ����
	if(NULL != hOutput || NULL != lpfnOutput)
	{
		g_bDisableDebugPrint = FALSE;
	}

	//����ԭ���ľ��
	return hOldFile;
}

//��ֹ��ӡ
BOOL BISPDebugPrintEnable(BOOL bEnable)
{
	BOOL bOldValue = g_bDisableDebugPrint;
	g_bDisableDebugPrint = !bEnable;
	return bOldValue;
}

//��ӡ��Ϣ
int	BISPPrint(HCONSOLE hConsole,const char* lpszFormat,...)
{
	//����Ǿ�Ĭ�������ʲôҲ����ӡ�����.
	if(NULL == hConsole || 0 == *lpszFormat
#ifndef WIN32
		|| INT_CONTEXT()	//��������жϵ��е��ã�Ҳ��ֹ��ӡ
#endif //WIN32
		)
	{
		return 0;
	}

	int nRet;
	//��ӡ�����ָ���Ŀ���̨
	va_list argList;
	va_start(argList, lpszFormat);
	nRet = BISPConsoleOutput(hConsole,lpszFormat,argList);
	va_end(argList);

	return nRet;
}


int	BISPDebugPrint(const char* lpszFormat,...)
{
//	return 0;

	char szBuffer[1024];
	
	int nRet;

	//���ԿյĴ�ӡ���ͽ�ֹ��ӡ����
	if(g_bDisableDebugPrint || 0 == *lpszFormat
#ifndef WIN32
		|| INT_CONTEXT()	//��������жϵ��е��ã�Ҳ��ֹ��ӡ
#endif //WIN32
		)
	{
		return 0;
	}

	char* pTempBuf = szBuffer;

	UINT64 dwTickCount = BISPGetTickCount();

#ifndef DEBUG_TICK_TIME

	time_t tmCur;
	tmCur = time(NULL);
	tm* pTime = localtime((time_t*)&tmCur);
	//*Mar  1 01:58:22.830: EIGRP: 
	nRet = sprintf(pTempBuf,"*%-3s %-2d %02d:%02d:%02d.%03d: ",
								g_szMonName[pTime->tm_mon],pTime->tm_mday,
								pTime->tm_hour,pTime->tm_min,pTime->tm_sec,
								(UINT)(dwTickCount % 1000) );

#else 
		
	//��ӡ����
	nRet = sprintf(pTempBuf,"%02d:%02d:%02d.%03d  ", 
						(UINT)(( dwTickCount/(1000*60*60) ) % 24) ,(UINT)((dwTickCount/(1000*60) ) % 60),
						(UINT)((dwTickCount/(1000)) % 60,dwTickCount % 1000));

#endif //DEBUG_TICK_TIME

	if(nRet <= 0)
	{
		return nRet;
	}
	
	//��ǰƫ��
	pTempBuf += nRet;

	//��ӡ����
	va_list argList;
	va_start(argList, lpszFormat);

	nRet += vsprintf(pTempBuf,lpszFormat,argList);

	va_end(argList);
	
	if(nRet <=0 || nRet >= 1024)
	{
		return nRet;
	}
	
	if(NULL == g_hDebugOutput || NULL == g_pfnOutput)
	{
		printf("%s",szBuffer);
	}
	else	//�����������������
	{
		(void)((*g_pfnOutput)(g_hDebugOutput,szBuffer,nRet));
	}

	/* Win32 DEBUG output:
#if defined(_DEBUG) && defined(WIN32)
	
	char szTempBuffer[1024];
	sprintf(szTempBuffer,"%02d:%02d:%02d.%03d  %s",
							( dwTickCount/(1000*60*60) ) % 24 ,(dwTickCount/(1000*60) ) % 60,
							(dwTickCount/(1000)) % 60,dwTickCount % 1000,szBuffer);
	
	//���������Ϣ
	OutputDebugString(szTempBuffer);

#endif //defined(_DEBUG) && defined(WIN32)
  */


	return nRet;
}


//��ӡ���
int	BISPOutputBuffer(HCONSOLE hConsole,LPCSTR lpszString,int nLen)
{
	
	int nRet = 0;
	if(NULL == hConsole)
	{
		if(NULL != g_pfnOutput && NULL != g_hDebugOutput)
		{
			//�����������������
			nRet = (*g_pfnOutput)(g_hDebugOutput,lpszString,nLen);
		}
	}
	else
	{
		nRet = BISPConsoleWriteBuffer(hConsole,lpszString,nLen);
	}

	return nRet;
}

//����ģʽ����
BOOL BISPDebugSetModeName(LPCSTR lpszModeName)
{
	if(NULL == lpszModeName)
	{
		return FALSE;
	}
	size_t nLen = strlen(lpszModeName);
	if(nLen >= sizeof(g_szModeName) )
	{
		return FALSE;
	}
	strcpy(g_szModeName,lpszModeName);

	return TRUE;
}

//����show debugging ����
BOOL BISPDebugShowSet(int nIndex,PFN_SHOWDEBUGGING pfnShowDebugging)
{
	BISPASSERT( nIndex >=0 && nIndex < BISP_DEBUG_CTRL_COUNT);
	
	g_lpfnShowDebugging[nIndex] = pfnShowDebugging;
	
	return TRUE;
}

//��ӡЭ����Ϣ
BOOL BISPDebugShowPrint(int nIndex,HCONSOLE hConsole)
{
	BISPASSERT( nIndex >=0 && nIndex < BISP_DEBUG_CTRL_COUNT);
	
	if(NULL != g_lpfnShowDebugging[nIndex])
	{
		(*g_lpfnShowDebugging[nIndex])(nIndex,hConsole);
	}

	return TRUE;

}


