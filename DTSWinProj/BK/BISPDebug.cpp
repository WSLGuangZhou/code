///////////////////////////////////////////////////////////////
//	文 件 名 : BISPDebug.cpp
//	文件功能 : 通用调试与打印
//	作    者 : 谭 北 华
//	创建时间 : 2006年10月25日
//	项目名称 : BISP
//	备    注 :
//	历史记录 : 2007.12.11 忽略打印一个空的字符串
//			   2008.03.12 修改BISPDebugPrint函数中直接采用printf的方式，
//						   该方式导致某些结果打印不正确，改为printf("%s",...
//			   2008.07.30 修改BISPASSERT定义，把BISPAssertFailedLine函数加入cpp中实现	
//			   2010.01.15  修改BispGetTickCount函数相关的代码，把时间变成64位，解决若干时间后
//							TickCount归零引发的定时器和时间不正常的问题
//			   2010.04.14  修改BISPDebugGetOutput函数，可以返回当前的打印指针
//			   2010.04.21  增加BISPDebugPrintEnable函数支持Telnet时禁止协议打印功能  
//			   2010.04.27  修改BISPDebugPrint和BISPPrint函数，禁止在中断中进行打印	
//			   2010.06.02  修改BISPAssertFailedLine函数，避免在logMsg函数中传递字符串变量
//			   2011.06.17  增加BISPDebugSetDebugCtrl函数
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

//调试控制项
DWORD g_dwDebugCtrlValue[BISP_DEBUG_CTRL_COUNT] = {0};

//12个月的英文简称
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

//show debugging函数项
LOCAL PFN_SHOWDEBUGGING g_lpfnShowDebugging[BISP_DEBUG_CTRL_COUNT] = {NULL};

//默认的输出位置!
LOCAL FHANDLE	g_hDebugOutput = NULL;

//输出函数
LOCAL PFN_OUTPUTSTRING g_pfnOutput = NULL;

//模式名字(EIGRP或FRP)
char g_szModeName[8] = "EIGRP";

//是否禁用调试打印?
LOCAL BOOL g_bDisableDebugPrint = FALSE;

////////////////////////////////////////////////////////////////////

#ifndef WIN32

#include <intLib.h>
#include <logLib.h>

//字符串定义
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

		//这里不能用中断logMsg传递字符串参数
		//logMsg((char*)g_pAssertMsg,(int)lpszFileName,(int)nLine,0,(int)"(Interrupt Level)",(int)lpszExpres,0);
	}
	else
	{
		printf(g_pAssertMsg,lpszFileName,nLine,taskIdSelf(),"",lpszExpres);
	}

	return;
}

#endif //WIN32

//空函数
void CDECL BISPTraceVoid(LPCSTR lpszFormat, ...)
{

}

//使能Debug
void BISPDebugEnable(int nIndex,DWORD dwDebugCtrl)
{
	BISPASSERT( nIndex >=0 && nIndex < BISP_DEBUG_CTRL_COUNT);

	g_dwDebugCtrlValue[nIndex] |= dwDebugCtrl;
}

//使不能Debug
void BISPDebugDisable(int nIndex,DWORD dwDebugCtrl)
{
	BISPASSERT( nIndex >=0 && nIndex < BISP_DEBUG_CTRL_COUNT);
	g_dwDebugCtrlValue[nIndex] &= ~dwDebugCtrl;
}

//取得Debug控制字符
DWORD BISPDebugGetDebugCtrl(int nIndex)
{
	BISPASSERT( nIndex >=0 && nIndex < BISP_DEBUG_CTRL_COUNT);
	return g_dwDebugCtrlValue[nIndex];
}

//设置Debug控制字符
DWORD BISPDebugSetDebugCtrl(int nIndex,DWORD dwNewCtrl)
{
	BISPASSERT( nIndex >=0 && nIndex < BISP_DEBUG_CTRL_COUNT);
	DWORD dwOldCtrl = g_dwDebugCtrlValue[nIndex];
	g_dwDebugCtrlValue[nIndex] = dwNewCtrl;
	return dwOldCtrl;
}

//得到输出通道
FHANDLE BISPDebugGetOutput(PFN_OUTPUTSTRING* ppfnOutput)
{
	if(NULL != ppfnOutput)
	{
		*ppfnOutput = g_pfnOutput;
	}
	return g_hDebugOutput;
}


//设置输出通道
FHANDLE BISPDebugSetOutput(FHANDLE hOutput,PFN_OUTPUTSTRING lpfnOutput)
{
	FHANDLE hOldFile = g_hDebugOutput;

	//先设置空的句柄
	g_hDebugOutput = NULL;
	//然后改变函数指针
	g_pfnOutput = lpfnOutput;
	//最后设置新的句柄
	g_hDebugOutput = hOutput;

	//每次设置新的非空打印都要求使能打印开关
	if(NULL != hOutput || NULL != lpfnOutput)
	{
		g_bDisableDebugPrint = FALSE;
	}

	//返回原来的句柄
	return hOldFile;
}

//禁止打印
BOOL BISPDebugPrintEnable(BOOL bEnable)
{
	BOOL bOldValue = g_bDisableDebugPrint;
	g_bDisableDebugPrint = !bEnable;
	return bOldValue;
}

//打印消息
int	BISPPrint(HCONSOLE hConsole,const char* lpszFormat,...)
{
	//如果是静默输出，则什么也不打印就输出.
	if(NULL == hConsole || 0 == *lpszFormat
#ifndef WIN32
		|| INT_CONTEXT()	//如果是在中断当中调用，也禁止打印
#endif //WIN32
		)
	{
		return 0;
	}

	int nRet;
	//打印输出到指定的控制台
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

	//忽略空的打印语句和禁止打印调试
	if(g_bDisableDebugPrint || 0 == *lpszFormat
#ifndef WIN32
		|| INT_CONTEXT()	//如果是在中断当中调用，也禁止打印
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
		
	//打印日期
	nRet = sprintf(pTempBuf,"%02d:%02d:%02d.%03d  ", 
						(UINT)(( dwTickCount/(1000*60*60) ) % 24) ,(UINT)((dwTickCount/(1000*60) ) % 60),
						(UINT)((dwTickCount/(1000)) % 60,dwTickCount % 1000));

#endif //DEBUG_TICK_TIME

	if(nRet <= 0)
	{
		return nRet;
	}
	
	//往前偏移
	pTempBuf += nRet;

	//打印内容
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
	else	//定义了特殊输出函数
	{
		(void)((*g_pfnOutput)(g_hDebugOutput,szBuffer,nRet));
	}

	/* Win32 DEBUG output:
#if defined(_DEBUG) && defined(WIN32)
	
	char szTempBuffer[1024];
	sprintf(szTempBuffer,"%02d:%02d:%02d.%03d  %s",
							( dwTickCount/(1000*60*60) ) % 24 ,(dwTickCount/(1000*60) ) % 60,
							(dwTickCount/(1000)) % 60,dwTickCount % 1000,szBuffer);
	
	//输出调试信息
	OutputDebugString(szTempBuffer);

#endif //defined(_DEBUG) && defined(WIN32)
  */


	return nRet;
}


//打印输出
int	BISPOutputBuffer(HCONSOLE hConsole,LPCSTR lpszString,int nLen)
{
	
	int nRet = 0;
	if(NULL == hConsole)
	{
		if(NULL != g_pfnOutput && NULL != g_hDebugOutput)
		{
			//定义了特殊输出函数
			nRet = (*g_pfnOutput)(g_hDebugOutput,lpszString,nLen);
		}
	}
	else
	{
		nRet = BISPConsoleWriteBuffer(hConsole,lpszString,nLen);
	}

	return nRet;
}

//设置模式名字
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

//设置show debugging 函数
BOOL BISPDebugShowSet(int nIndex,PFN_SHOWDEBUGGING pfnShowDebugging)
{
	BISPASSERT( nIndex >=0 && nIndex < BISP_DEBUG_CTRL_COUNT);
	
	g_lpfnShowDebugging[nIndex] = pfnShowDebugging;
	
	return TRUE;
}

//打印协议信息
BOOL BISPDebugShowPrint(int nIndex,HCONSOLE hConsole)
{
	BISPASSERT( nIndex >=0 && nIndex < BISP_DEBUG_CTRL_COUNT);
	
	if(NULL != g_lpfnShowDebugging[nIndex])
	{
		(*g_lpfnShowDebugging[nIndex])(nIndex,hConsole);
	}

	return TRUE;

}


