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
//����ģ�鶨��: BISP_DEBUG

//����ģ�鶨��
#ifndef __BISPDEBUG_H_INCLUDED
#define __BISPDEBUG_H_INCLUDED

#ifndef NSL_OS_LINUX
//LOCAL�Ķ���
#ifndef LOCAL
#define LOCAL static
#endif //LOCAL

//�������Ĵ�������

#define BISP_REGISTER register

//�������EXTERN����:
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

//��̬���(�����ڼ������)
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

//��̬���(�����ڼ������)
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

#pragma warning(disable:4710)	//��ֹ"function not expanded"����

#endif //WIN32

#ifdef _lint	//��PC��LINT�����ⲻ����Ĵ���

//Constant value Boolean
/*lint -emacro(506,BISPASSERT) */
//Boolean within 'if' always evaluates to True/False 
/*lint -emacro(774,BISPASSERT) */

#define BISPASSERT(exp)   \
		if(!(exp)) exit(0)


#else //vxworks��ͨ���봦��

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



//��̬���
#define BISP_STATIC_CHECK(exp,msg) ((void)0)

#endif //_DEBUG
///////////////////////////////////////////////////////


#define BISPTRY(x) x;



#ifdef WIN32

//����Ϊ��VC6��INT8��INT16���ͺ�MSSDK���ݵĴ���:

//ADDRESS_TAG_BITΪMSSDK����<BaseTsd.h>�ĺ궨�壬VC6��û��
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


//BISP ����̨��ʾģʽ
enum BISP_CONSOLE_MODE
{
	BCM_EIGRP			=		0x00,					//EIGRPģʽ
	BCM_FRP				=		0x01					//FRPģʽ
};

//EIGRPģʽ���֣�ΪEIGRP��FRP
BISP_EXTERN char g_szModeName[];
//�·�����
BISP_EXTERN const char* g_szMonName[];

//EIGRP ģʽ����
#define EIGRP_MODE_NAME ((LPCSTR)g_szModeName)

//#ifndef NSL_OS_LINUX

//��Ҫ����ѡ���Э��ĵĸ���
#define BISP_DEBUG_CTRL_COUNT 0x30

#define BISP_DEBUG_ALL	0xFFFFFFFF	//Debug all message!


//����Э���DebugCtrl�������Ŷ���
enum BISP_DEBUG_INDEX
{
	//ȫ��
	BDI_GLOBAL				=		0x00,		//ȫ�ֵ���ѡ��
	//IP ·��Э��
	BDI_EIGRP				=		0x01,		//EIGRP����ѡ��
	BDI_EIGRPNEI			=		0x02,		//EIGRP�ھӵ���ѡ��
	BDI_OSPF				=		0x03,		//OSPF����ѡ��
	BDI_RIP					=		0x04,		//RIP����ѡ��
	BDI_BGP					=		0x05,		//BGP����ѡ��
	BDI_MPLS				=		0x06,		//MPLS����ѡ��
	BDI_PIM					=		0x07,		//PIM����ѡ��
	BDI_DVMRP				=		0x08,		//DVMRP����ѡ��
	BDI_IPPACKET			=		0x09,		//IP packet����ѡ��
	BDI_TIRP				=		0x0A,		//TIRP����ѡ��

	//��������
	BDI_ATMYTX				=		0x10,		//ATM�м�Ⱥ�������ѡ��
	BDI_YTX					=		0x11,		//������Ұս�м�Ⱥ�������ѡ��
	BDI_NO1					=		0x12,		//�й�һ���������ѡ��
	BDI_NO7					=		0x13,		//�й�һ���������ѡ��

	//ϵͳ״̬
	BDI_THUNK				=		0x17,		//�м�Ⱥͨ������(LL:���غ�ѡ��,LH:chan,H:���ݳ���)
	BDI_HDLC				=		0x18,		//HDLCͨ������(LL:���غ�ѡ��,LH:chan,H:���ݳ���)
	BDI_BOARD				=		0x19,		//ϵͳ�忨״̬
	BDI_ICCP				=		0x1A,		//���ͨ��״̬				
	BDI_DSP					=		0x1B,		//DSPͨ��״̬
	BDI_SLIC				=		0x1C,		//SLIC״̬

	BDI_COMMON				=		0x1D,		//Commonģ��
	BDI_RESMGR				=		0x1E		//��Դ����
				
};

//���Կ���ѡ��
BISP_EXTERN DWORD g_dwDebugCtrlValue[];
//#endif /* NSL_OS_LINUX */
extern char line_delimiter[];

#define __LD__ ((const char *)line_delimiter)
//����ָ̨��
typedef void* HCONSOLE;

//����̨����������
typedef void* FHANDLE;

//ֱ�����������̨��
#define BISP_PRINT BISPPrint

//#ifndef NSL_OS_LINUX
//����Debug�����Ƿ�����(��������������ԣ���Ϊһ��DEBUG���ǹرյ�,�����ӻ�ʹ��DEBUGʱ����һ��)
#define BISP_DEBUG_TEST(Index,Option) \
	( (g_dwDebugCtrlValue[Index]) && ((Option) & (g_dwDebugCtrlValue[Index]) ) )

//Constant value Boolean
/*lint -emacro(506,BISP_DEBUG_PRINT) */

//������������ĳ������ѡ�����Ӧ���Կ��ش򿪣�����Ĭ�ϵĿ���̨��ӡ��Ϣ
#define BISP_DEBUG_PRINT(Index,Option) \
	BISPASSERT( (Index) >=0 && (Index) < BISP_DEBUG_CTRL_COUNT); \
	if(BISP_DEBUG_TEST(Index,Option) ) BISPDebugPrint
//#endif /* NSL_OS_LINUX */

//�������̨�ַ���
typedef int (*PFN_OUTPUTSTRING)(FHANDLE hOutputFile,LPCSTR lpszString,int nLen);

//show debugging ����ע��
typedef void (*PFN_SHOWDEBUGGING)(int nIndex,HCONSOLE hConsole);

/*lint -printf( 1, BISPDebugPrint )*/

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

//��ӡ��Ϣ
int	BISPPrint(HCONSOLE hConsole,const char* lpszFormat,...);	

//debug string to the default console, with current time
int	BISPDebugPrint(const char* _format,...);
//debug string to the default console
int BISPDebugPrintFast(const char* _format, ...);
int BISPDebugPrintString(const char* str, int str_len);

//#ifndef NSL_OS_LINUX
//ʹ��Debug
void BISPDebugEnable(int nIndex,DWORD dwDebugCtrl);

//ʹ����Debug
void BISPDebugDisable(int nIndex,DWORD dwDebugCtrl);

//ȡ��Debug�����ַ�
DWORD BISPDebugGetDebugCtrl(int nIndex);

//����Debug�����ַ�
DWORD BISPDebugSetDebugCtrl(int nIndex,DWORD dwNewCtrl);
//#endif /* NSL_OS_LINUX */

//�õ����ͨ��
FHANDLE BISPDebugGetOutput(PFN_OUTPUTSTRING* ppfnOutput);

//�������ͨ��
FHANDLE BISPDebugSetOutput(FHANDLE hOutput,PFN_OUTPUTSTRING lpfnOutput);

//ʹ��Debug��ӡ
BOOL BISPDebugPrintEnable(BOOL bEnable);

//��ӡ���
int	BISPOutputBuffer(HCONSOLE hConsole,LPCSTR lpszString,int nLen);

//����ģʽ����
BOOL BISPDebugSetModeName(LPCSTR lpszModeName);

//#ifndef NSL_OS_LINUX
//����show debugging ����
BOOL BISPDebugShowSet(int nIndex,PFN_SHOWDEBUGGING pfnShowDebugging);

//��ӡЭ����Ϣ
BOOL BISPDebugShowPrint(int nIndex,HCONSOLE hConsole);
//#endif /* NSL_OS_LINUX */

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //__BISPDEBUG_H_INCLUDED

