//////////////////////////////////////////////////////////////////////////
//
#include "DTSCommon.h"


DWORD g_arrydwDTSDebugFlag[DTS_DEBUG_OPTION_COUNT]={0x00010000,
													0x00020000,
													0x00040000,
													0x00800000,
													0x20000000,
													0xffffffff};
char g_szDTSDebugOptionName[DTS_DEBUG_OPTION_COUNT][32]={"DEBUG_ENTRY",
														"DEBUG_FSM",
														"DEBUG_FLOW",
														"DEBUG_ERROR",
														"DEBUG_KEEPALIVE",
														"DEBUG_ALL"};




void* DTSMalloc(int nSize)
{
	DTSTESTUNITLOGENTRY LogEntry;
	LogEntry.nMainType = DTS_TEST_MEMERY_MALLOC;
	LogEntry.nSendDataBuffLen = nSize;
	DTSTestUnitMemeryLogAddEntry(&LogEntry);

	return malloc(nSize);
}

void DTSFree(char* pBuff)
{
	DTSTESTUNITLOGENTRY LogEntry;
	LogEntry.nMainType = DTS_TEST_MEMERY_FREE;
	LogEntry.pPoint = (BYTE*)pBuff;
	DTSTestUnitMemeryLogAddEntry(&LogEntry);

	free(pBuff);
}

