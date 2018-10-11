///////////////////////////////////////////////////////////////
//	����׮��¼ģ�飬����
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
//ÿ�ֲ�������һ�β��Լ�¼���������
#define DTS_MAX_LOG_COUNT_PERTYPE				100

typedef struct tagDTSTestUnitLogEntry
{
	//�������ͣ���ģ���Զ�
	UINT		nMainType;	
	//�������ͣ���ģ���Զ�
	UINT		nSubType;
	BYTE		pParamBuff[DTS_TEST_UNIT_PARAM_BUFF_LEN];
	//buff�������ǲ��������͵ı��ĵ�
	BYTE		pSendDataBuff[DTS_TEST_UNIT_BUFF_LEN];	
	UINT		nSendDataBuffLen;
	BYTE*		pPoint;
}DTSTESTUNITLOGENTRY,*LPDTSTESTUNITLOGENTRY;

//////////////////////////////////////////////////////////////////////////
//��ÿ������������ͷ��ʼ��һ��
void DTSTestUnitMDLLogReset();

int DTSTestUnitMDLLogGetCount();
LPDTSTESTUNITLOGENTRY DTSTestUnitMDLLogGetEntry(int nIndex);
bool DTSTestUnitMDLLogAddEntry(LPDTSTESTUNITLOGENTRY pLogEntry);


//////////////////////////////////////////////////////////////////////////
//��ÿ������������ͷ��ʼ��һ��
void DTSTestUnitMemeryLogReset();

int DTSTestUnitMemeryLogGetCount();
LPDTSTESTUNITLOGENTRY DTSTestUnitMemeryLogGetEntry(int nIndex);
bool DTSTestUnitMemeryLogAddEntry(LPDTSTESTUNITLOGENTRY pLogEntry);



//////////////////////////////////////////////////////////////////////////
//�������Թ��ܺ���
int DTSTestGetUnmatchByteIndex(char* pBuff1,char* pBuff2,int nSize);

//////////////////////////////////////////////////////////////////////////
//����
// void DTSTestUnitEnable(void);
// void DTSTestUnitDisable(void);
// BOOL DTSTestUnitIsEnable(void);

#endif	//DTS_TEST_UNIT_H_INCLUDED






