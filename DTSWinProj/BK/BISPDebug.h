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
//连接模块定义: BISP_DEBUG

//调试模块定义
#ifndef __BISPDEBUG_H_INCLUDED
#define __BISPDEBUG_H_INCLUDED

#ifndef NSL_OS_LINUX
//LOCAL的定义
#ifndef LOCAL
#define LOCAL static
#endif //LOCAL

//内联及寄存器定义

#define BISP_REGISTER register

//输出函数EXTERN定义:
#ifdef __cplusplus
#define BISP_EXTERN extern "C"
#define BISP_CFUNC extern "C"
#define BISP_INLINE inline
#else
#define BISP_EXTERN extern
#define BISP_CFUNC
#define BISP_INLINE
#endif //__cplusplus


#ifdef _DEBUG

///////////////////////////////////////////////////////////////////////
#if !(defined(WIN32) || defined(_WINDOWS)) //VxWorks

#ifdef NDEBUG
#undef NDEBUG
#endif //NDEBUG

/*lint -function(exit,assert) */
/*lint -emacro(506,assert) */

//void cast of void expression
/*lint -emacro(792,BISPASSERT) */
//do ... while(0);
/*lint -emacro(717,BISPASSERT) */

#include <assert.h>
#include <stdio.h>
#include <taskLib.h>

BISP_EXTERN
void BISPAssertFailedLine(const char* lpszFileName, int nLine,const char* lpszExpres);


//Constant value Boolean
/*lint -emacro(506,BISPASSERT) */
//Boolean within 'if' always evaluates to True/False 
/*lint -emacro(774,BISPASSERT) */

/*lint -function(exit,BISPAssertFailedLine) */

#define __BISPASSERT(f,exp,v) (void)( (f) || (BISPAssertFailedLine(__FILE__, __LINE__,exp), v) )

#define BISPASSERT(f) __BISPASSERT(f,#f,0)

//静态检查(编译期间检查错误)
#define BISP_STATIC_CHECK(exp,msg) \
	char ERROR_##msg[(exp)? 1:0] = {0};\
	__BISPASSERT(exp,#exp,ERROR_##msg[0])


#define BISPTRACE printf

//////////////////////////////////////////////////////////////////////////////
#else //WIN32


#include <stdio.h>
#include <assert.h>
//#include <crtdbg.h>
#include <string.h>


#define BISPASSERT(f) assert(f)

//静态检查(编译期间检查错误)
/*lint -emacro(506,BISP_STATIC_CHECK) */
/*lint -emacro(1776,BISP_STATIC_CHECK) */
// #define BISP_STATIC_CHECK(exp,msg) \
// 	char ERROR_##msg[(exp)? 1:0] = {0};\
// 	(void)ERROR_##msg;\
// 	(void)( (exp) || (_assert(#exp, __FILE__, __LINE__), ERROR_##msg[0]) )
#define BISP_STATIC_CHECK(exp,msg) ((void)0)

/**********************************************
#define BISPASSERT(f) __BISPASSERT(f,#f)

#define __BISPASSERT(f,expression) \
	do { \
		if (!(f) && (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, expression))) \
             _CrtDbgBreak(); \
	} while (0)
**************************************************/

inline void _cdecl BISPTrace(const char* lpszFormat, ...)
{	
	va_list args;
	va_start(args, lpszFormat);
	
	int nBuf;
	char szBuffer[512];
	
	nBuf = vsprintf(szBuffer,lpszFormat, args);
	
	// was there an error? was the expanded string too long?
	BISPASSERT(nBuf >= 0);
#if (BISPOS_TYPE == BISPOS_WIN32)
	OutputDebugString((LPCWSTR)szBuffer);
#else
	OutputDebugString(szBuffer);
#endif	
	
	va_end(args);

	// In windows platform,if outputstring echo very fast ,some of string will not display
	// so sleep(1) to display all message!!!!
//	Sleep(1);
}

//#define BISPTRACE BISPTrace
#define BISPTRACE BISPDebugPrint

#endif //!(defined(WIN32) || defined(_WINDOWS))



//////////////////////////////////////////////////////////////
#else //RELEASE

#if !(defined(WIN32) || defined(_WINDOWS)) //VxWorks
#include "BISPTypes.h"
#endif //!(defined(WIN32) || defined(_WINDOWS)) //VxWorks

#ifdef WIN32

#pragma warning(disable:4710)	//禁止"function not expanded"警告

#endif //WIN32

#ifdef _lint	//对PC－LINT代码检测不报错的处理

//Constant value Boolean
/*lint -emacro(506,BISPASSERT) */
//Boolean within 'if' always evaluates to True/False 
/*lint -emacro(774,BISPASSERT) */

#define BISPASSERT(exp)   \
		if(!(exp)) exit(0)


#else //vxworks普通代码处理

#define BISPASSERT(exp)          ((void)0)

#endif //_lint

//2012.10.10 fixed c link error BUG.
#ifdef __cplusplus
inline void CDECL BISPTrace(LPCSTR /*lpszFormat*/, ...){}
//Expression-like macro 'Symbol' not parenthesized
//Constant value
/*lint -emacro(506,BISPTRACE) */
/*lint -save -e773 */
#define BISPTRACE 1? ((void)0) : ::BISPTrace
/*lint -restore */
#else
BISP_EXTERN
void CDECL BISPTraceVoid(LPCSTR lpszFormat, ...);
//Expression-like macro 'Symbol' not parenthesized
//Constant value
/*lint -emacro(506,BISPTRACE) */
/*lint -save -e773 */
#define BISPTRACE 1? ((void)0) : BISPTraceVoid
/*lint -restore */
#endif //__cplusplus



//静态检查
#define BISP_STATIC_CHECK(exp,msg) ((void)0)

#endif //_DEBUG
///////////////////////////////////////////////////////


#define BISPTRY(x) x;



#ifdef WIN32

//以下为对VC6中INT8、INT16类型和MSSDK兼容的处理:

//ADDRESS_TAG_BIT为MSSDK里面<BaseTsd.h>的宏定义，VC6里没有
#ifndef ADDRESS_TAG_BIT
typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
#endif //ADDRESS_TAG_BIT

#endif //WIN32


#else
#define BISPTRACE BISPDebugPrint

//#define BISPTRACE printf
#define HNCASSERT assert
#define BISP_STATIC_CHECK(exp,msg) ((void)0)
#endif /* NSL_OS_LINUX */


//BISP 控制台显示模式
enum BISP_CONSOLE_MODE
{
	BCM_EIGRP			=		0x00,					//EIGRP模式
	BCM_FRP				=		0x01					//FRP模式
};

//EIGRP模式名字，为EIGRP或FRP
BISP_EXTERN char g_szModeName[];
//月份名称
BISP_EXTERN const char* g_szMonName[];

//EIGRP 模式名字
#define EIGRP_MODE_NAME ((LPCSTR)g_szModeName)

//#ifndef NSL_OS_LINUX

//需要调试选项的协议的的个数
#define BISP_DEBUG_CTRL_COUNT 0x30

#define BISP_DEBUG_ALL	0xFFFFFFFF	//Debug all message!


//各个协议的DebugCtrl的索引号定义
enum BISP_DEBUG_INDEX
{
	//全局
	BDI_GLOBAL				=		0x00,		//全局调试选项
	//IP 路由协议
	BDI_EIGRP				=		0x01,		//EIGRP调试选项
	BDI_EIGRPNEI			=		0x02,		//EIGRP邻居调试选项
	BDI_OSPF				=		0x03,		//OSPF调试选项
	BDI_RIP					=		0x04,		//RIP调试选项
	BDI_BGP					=		0x05,		//BGP调试选项
	BDI_MPLS				=		0x06,		//MPLS调试选项
	BDI_PIM					=		0x07,		//PIM调试选相
	BDI_DVMRP				=		0x08,		//DVMRP调试选项
	BDI_IPPACKET			=		0x09,		//IP packet调试选项
	BDI_TIRP				=		0x0A,		//TIRP调试选项

	//话音信令
	BDI_ATMYTX				=		0x10,		//ATM中继群信令调试选项
	BDI_YTX					=		0x11,		//地域网野战中继群信令调试选项
	BDI_NO1					=		0x12,		//中国一号信令调试选项
	BDI_NO7					=		0x13,		//中国一号信令调试选项

	//系统状态
	BDI_THUNK				=		0x17,		//中继群通道调试(LL:开关和选型,LH:chan,H:数据长度)
	BDI_HDLC				=		0x18,		//HDLC通道调试(LL:开关和选型,LH:chan,H:数据长度)
	BDI_BOARD				=		0x19,		//系统板卡状态
	BDI_ICCP				=		0x1A,		//板间通信状态				
	BDI_DSP					=		0x1B,		//DSP通信状态
	BDI_SLIC				=		0x1C,		//SLIC状态

	BDI_COMMON				=		0x1D,		//Common模块
	BDI_RESMGR				=		0x1E		//资源管理
				
};

//调试控制选项
BISP_EXTERN DWORD g_dwDebugCtrlValue[];
//#endif /* NSL_OS_LINUX */
extern char line_delimiter[];

#define __LD__ ((const char *)line_delimiter)
//控制台指针
typedef void* HCONSOLE;

//控制台输入输出句柄
typedef void* FHANDLE;

//直接输出到控制台上
#define BISP_PRINT BISPPrint

//#ifndef NSL_OS_LINUX
//测试Debug条件是否满足(这里用了两层测试，因为一般DEBUG都是关闭的,这样子会使不DEBUG时更快一点)
#define BISP_DEBUG_TEST(Index,Option) \
	( (g_dwDebugCtrlValue[Index]) && ((Option) & (g_dwDebugCtrlValue[Index]) ) )

//Constant value Boolean
/*lint -emacro(506,BISP_DEBUG_PRINT) */

//调试输出，如果某个调试选项的相应调试开关打开，就在默认的控制台打印消息
#define BISP_DEBUG_PRINT(Index,Option) \
	BISPASSERT( (Index) >=0 && (Index) < BISP_DEBUG_CTRL_COUNT); \
	if(BISP_DEBUG_TEST(Index,Option) ) BISPDebugPrint
//#endif /* NSL_OS_LINUX */

//输出控制台字符串
typedef int (*PFN_OUTPUTSTRING)(FHANDLE hOutputFile,LPCSTR lpszString,int nLen);

//show debugging 函数注册
typedef void (*PFN_SHOWDEBUGGING)(int nIndex,HCONSOLE hConsole);

/*lint -printf( 1, BISPDebugPrint )*/

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

//打印信息
int	BISPPrint(HCONSOLE hConsole,const char* lpszFormat,...);	

//debug string to the default console, with current time
int	BISPDebugPrint(const char* _format,...);
//debug string to the default console
int BISPDebugPrintFast(const char* _format, ...);
int BISPDebugPrintString(const char* str, int str_len);

//#ifndef NSL_OS_LINUX
//使能Debug
void BISPDebugEnable(int nIndex,DWORD dwDebugCtrl);

//使不能Debug
void BISPDebugDisable(int nIndex,DWORD dwDebugCtrl);

//取得Debug控制字符
DWORD BISPDebugGetDebugCtrl(int nIndex);

//设置Debug控制字符
DWORD BISPDebugSetDebugCtrl(int nIndex,DWORD dwNewCtrl);
//#endif /* NSL_OS_LINUX */

//得到输出通道
FHANDLE BISPDebugGetOutput(PFN_OUTPUTSTRING* ppfnOutput);

//设置输出通道
FHANDLE BISPDebugSetOutput(FHANDLE hOutput,PFN_OUTPUTSTRING lpfnOutput);

//使能Debug打印
BOOL BISPDebugPrintEnable(BOOL bEnable);

//打印输出
int	BISPOutputBuffer(HCONSOLE hConsole,LPCSTR lpszString,int nLen);

//设置模式名字
BOOL BISPDebugSetModeName(LPCSTR lpszModeName);

//#ifndef NSL_OS_LINUX
//设置show debugging 函数
BOOL BISPDebugShowSet(int nIndex,PFN_SHOWDEBUGGING pfnShowDebugging);

//打印协议信息
BOOL BISPDebugShowPrint(int nIndex,HCONSOLE hConsole);
//#endif /* NSL_OS_LINUX */

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //__BISPDEBUG_H_INCLUDED

