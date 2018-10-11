///////////////////////////////////////////////////////////////
//	测试桩记录模块，采用
#ifndef DTS_TEST_UNIT_H_INCLUDED
#define DTS_TEST_UNIT_H_INCLUDED

#include "DTSCommon.h"
#ifdef NSL_OS_WINDOWS
	#include "gtest/gtest.h"
	#pragma comment(lib,"../lib/gtest/gtestd.lib")
	#ifndef DTS_TEST
		#define DTS_TEST
	#endif
#endif

//////////////////////////////////////////////////////////////////////////
//Test_Log
enum DTS_TEST_UNIT_TYPE_GROUP
{
	DTS_TEST_UNIT_TYPE_GENERIC			=	0,
	DTS_TEST_UNIT_TYPE_MEMERY			=	1,

	DTS_TEST_UNIT_TYPE_COUNT
};


#define DTS_TEST_UNIT_PARAM_BUFF_LEN			256
#define DTS_TEST_UNIT_BUFF_LEN					2048
//每种测试类型一次测试记录的最大数量
#define DTS_MAX_LOG_COUNT_PERTYPE				100

typedef struct tagDTSTestUnitLogEntry
{
	//调用类型，各模块自定
	UINT		nMainType;	
	//调用类型，各模块自定
	UINT		nSubType;
	BYTE		pParamBuff[DTS_TEST_UNIT_PARAM_BUFF_LEN];
	//buff，可以是参数、发送的报文等
	BYTE		pSendDataBuff[DTS_TEST_UNIT_BUFF_LEN];	
	UINT		nSendDataBuffLen;
	BYTE*		pPoint;
}DTSTESTUNITLOGENTRY,*LPDTSTESTUNITLOGENTRY;

//////////////////////////////////////////////////////////////////////////
//在每个测试用例开头初始化一下
void DTSTestUnitMDLLogReset();

int DTSTestUnitMDLLogGetCount();
LPDTSTESTUNITLOGENTRY DTSTestUnitMDLLogGetEntry(int nIndex);
bool DTSTestUnitMDLLogAddEntry(LPDTSTESTUNITLOGENTRY pLogEntry);


//////////////////////////////////////////////////////////////////////////
//在每个测试用例开头初始化一下
void DTSTestUnitMemeryLogReset();

int DTSTestUnitMemeryLogGetCount();
LPDTSTESTUNITLOGENTRY DTSTestUnitMemeryLogGetEntry(int nIndex);
bool DTSTestUnitMemeryLogAddEntry(LPDTSTESTUNITLOGENTRY pLogEntry);



//////////////////////////////////////////////////////////////////////////
//其他测试功能函数
int DTSTestGetUnmatchByteIndex(char* pBuff1,char* pBuff2,int nSize);

//////////////////////////////////////////////////////////////////////////
//开关
// void DTSTestUnitEnable(void);
// void DTSTestUnitDisable(void);
// BOOL DTSTestUnitIsEnable(void);

#endif	//DTS_TEST_UNIT_H_INCLUDED






