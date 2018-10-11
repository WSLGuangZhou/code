///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//	测试桩记录模块，采用
//#include "StdAfx.h"

#include "DTSCommon.h"
//#include "DTSWinGtestInclude.h"
#include "DTSTestUnit.h"


//////////////////////////////////////////////////////////////////////////
//
LOCAL DTSTESTUNITLOGENTRY l_arrTestLog[DTS_TEST_UNIT_TYPE_COUNT][DTS_MAX_LOG_COUNT_PERTYPE] ={0};
LOCAL int l_arrnTestLogCount[DTS_TEST_UNIT_TYPE_COUNT] = {0};


LOCAL void DTSTestUnitLogReset(UINT nType)
{
	l_arrnTestLogCount[nType] = 0;
}
LOCAL int DTSTestUnitLogGetCount(UINT nType)
{
	return l_arrnTestLogCount[nType];
}
LOCAL LPDTSTESTUNITLOGENTRY DTSTestUnitLogGetEntry(int nIndex,UINT nType)
{
	assert(nIndex<DTS_MAX_LOG_COUNT_PERTYPE);
	return &l_arrTestLog[nType][nIndex];
}
LOCAL bool DTSTestUnitLogAddEntry(LPDTSTESTUNITLOGENTRY pLogEntry,UINT nType)
{
	assert(l_arrnTestLogCount[nType]<DTS_MAX_LOG_COUNT_PERTYPE-1);
	int nIndex = l_arrnTestLogCount[nType];
	memcpy(&l_arrTestLog[nType][nIndex],pLogEntry,sizeof(DTSTESTUNITLOGENTRY));
	l_arrnTestLogCount[nType] ++;
	return true;
}




void DTSTestUnitMDLLogReset()
{
	DTSTestUnitLogReset(DTS_TEST_UNIT_TYPE_GENERIC);
}
int DTSTestUnitMDLLogGetCount()
{
	return DTSTestUnitLogGetCount(DTS_TEST_UNIT_TYPE_GENERIC);
}
LPDTSTESTUNITLOGENTRY DTSTestUnitMDLLogGetEntry(int nIndex)
{
// 	assert(nIndex<DTS_MAX_LOG_COUNT_PERTYPE);
// 	return &l_arrTestLog[DTS_TEST_UNIT_TYPE_GENERIC][nIndex];
	return DTSTestUnitLogGetEntry(nIndex,DTS_TEST_UNIT_TYPE_GENERIC);
}
bool DTSTestUnitMDLLogAddEntry(LPDTSTESTUNITLOGENTRY pLogEntry)
{
// 	assert(l_arrnTestLogCount[DTS_TEST_UNIT_TYPE_GENERIC]<DTS_MAX_LOG_COUNT_PERTYPE-1);
// 	int nIndex = l_arrnTestLogCount[DTS_TEST_UNIT_TYPE_GENERIC];
// 	memcpy(&l_arrTestLog[DTS_TEST_UNIT_TYPE_GENERIC][nIndex],pLogEntry,sizeof(DTSTESTUNITLOGENTRY));
// 	l_arrnTestLogCount[DTS_TEST_UNIT_TYPE_GENERIC] ++;
// 	return true;
	return DTSTestUnitLogAddEntry(pLogEntry,DTS_TEST_UNIT_TYPE_GENERIC);
}


void DTSTestUnitMemeryLogReset()
{
	DTSTestUnitLogReset(DTS_TEST_UNIT_TYPE_MEMERY);
}
int DTSTestUnitMemeryLogGetCount()
{
	return DTSTestUnitLogGetCount(DTS_TEST_UNIT_TYPE_MEMERY);
}
LPDTSTESTUNITLOGENTRY DTSTestUnitMemeryLogGetEntry(int nIndex)
{
	// 	assert(nIndex<DTS_MAX_LOG_COUNT_PERTYPE);
	// 	return &l_arrTestLog[DTS_TEST_UNIT_TYPE_GENERIC][nIndex];
	return DTSTestUnitLogGetEntry(nIndex,DTS_TEST_UNIT_TYPE_MEMERY);
}
bool DTSTestUnitMemeryLogAddEntry(LPDTSTESTUNITLOGENTRY pLogEntry)
{
	// 	assert(l_arrnTestLogCount[DTS_TEST_UNIT_TYPE_GENERIC]<DTS_MAX_LOG_COUNT_PERTYPE-1);
	// 	int nIndex = l_arrnTestLogCount[DTS_TEST_UNIT_TYPE_GENERIC];
	// 	memcpy(&l_arrTestLog[DTS_TEST_UNIT_TYPE_GENERIC][nIndex],pLogEntry,sizeof(DTSTESTUNITLOGENTRY));
	// 	l_arrnTestLogCount[DTS_TEST_UNIT_TYPE_GENERIC] ++;
	// 	return true;
	return DTSTestUnitLogAddEntry(pLogEntry,DTS_TEST_UNIT_TYPE_MEMERY);
}




//////////////////////////////////////////////////////////////////////////
//
int DTSTestGetUnmatchcharIndex(char* pBuff1,char* pBuff2,int nSize)
{
	int nIndex = 0;
	for (nIndex=0;nIndex<nSize;nIndex++)
	{
		// 		if (0!=memcmp(pBuff1,pBuff2,nIndex))
		// 		{
		// 			return nIndex;
		// 		}
		if (pBuff1[nIndex] != pBuff2[nIndex])
		{
			return nIndex;
		}
	}

	return nIndex;
}


//////////////////////////////////////////////////////////////////////////
//
// BOOL l_bIsTestUnitEnable = FALSE;
// void DTSTestUnitEnable(void)
// {
// 	l_bIsTestUnitEnable = TRUE;
// }
// void DTSTestUnitDisable(void)
// {
// 	l_bIsTestUnitEnable = FALSE;
// }
// BOOL DTSTestUnitIsEnable(void)
// {
// 	return l_bIsTestUnitEnable;
// }
// 
