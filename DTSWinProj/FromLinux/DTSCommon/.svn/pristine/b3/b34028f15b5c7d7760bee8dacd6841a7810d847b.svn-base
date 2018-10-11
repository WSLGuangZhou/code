///////////////////////////////////////////////////////////////
//	调试信息打印控制
#ifndef DTS_DEBUG_H_INCLUDED
#define DTS_DEBUG_H_INCLUDED

#if defined(NSL_OS_WINDOWS) || defined(NSL_OS_LINUX)
	#define  BISPTRACE		printf
	#define  DTS_PRINTF		printf
#endif

enum DTS_MEMERY_TEST
{
	DTS_TEST_MEMERY_NONE	,
	DTS_TEST_MEMERY_MALLOC	,
	DTS_TEST_MEMERY_FREE	
};

void*	DTSMalloc(int nSize);
void	DTSFree(char* pBuff);

#ifdef DTS_TEST
	#define DTS_MALLOC(nSize)		DTSMalloc(nSize)
	#define DTS_FREE(pBuff)		    DTSFree(pBuff)
#else
	#define DTS_MALLOC(nSize)		malloc(nSize)
	#define DTS_FREE(pBuff)			free(pBuff)
#endif


enum DTS_DEBUG_OPTION
{
	DTS_DEBUG_OPTION_ENTRY				,
	DTS_DEBUG_OPTION_FSM				,
	DTS_DEBUG_OPTION_FLOW				,
	DTS_DEBUG_OPTION_ERROR				,
	DTS_DEBUG_OPTION_KEEPALIVE			,
	DTS_DEBUG_OPTION_ALL				,
	//增加一个Option，需要同步增加下面的宏定义，以及DTSDebug.cpp里面的初始化变量数组
	
	DTS_DEBUG_OPTION_COUNT
};

#define DTS_DEBUG_STRING_LENGTH			32
extern DWORD g_arrydwDTSDebugFlag[DTS_DEBUG_OPTION_COUNT];
extern char g_szDTSDebugOptionName[DTS_DEBUG_OPTION_COUNT][DTS_DEBUG_STRING_LENGTH];

//Debug_option 定义
#define DEBUG_ENTRY					g_arrydwDTSDebugFlag[DTS_DEBUG_OPTION_ENTRY]//0x00010000 /* 函数出入口等级			*/
#define DEBUG_FSM					g_arrydwDTSDebugFlag[DTS_DEBUG_OPTION_FSM] /* 模块状态机等级			*/
#define DEBUG_FLOW					g_arrydwDTSDebugFlag[DTS_DEBUG_OPTION_FLOW] /* 函数内部流程等级			*/
#define DEBUG_ERROR					g_arrydwDTSDebugFlag[DTS_DEBUG_OPTION_ERROR]
#define DEBUG_KEEPALIVE				g_arrydwDTSDebugFlag[DTS_DEBUG_OPTION_KEEPALIVE]
#define DEBUG_ALL					g_arrydwDTSDebugFlag[DTS_DEBUG_OPTION_ALL]


// #define DTS_DEBUG_ENTRY_NAME				"DEBUG_INTERFACE"
// #define DTS_DEBUG_FSM_NAME					"DEBUG_FSM"
// #define DTS_DEBUG_FLOW_NAME					"DEBUG_FLOW"
// #define DTS_DEBUG_ERROR_NAME				"DEBUG_ERROR"
// #define DTS_DEBUG_KEEPALIVE_NAME			"DEBUG_KEEPALIVE"
// #define DTS_DEBUG_ALL_NAME					"DEBUG_ALL"



#endif //DTS_DEBUG_H_INCLUDED


