//////////////////////////////////////////////////////////////////////////
//测试Common中的模块
#include "DTSCommon.h"
#include "DTSTestCommonLib.h"
#include "DTSTestUnit.h"
#include "DTSCommonLibFunc.h"

// LOCAL SBHANDLE l_hDTSBusMain = NULL;
// 
// LOCAL int SYSAPICALL DTSSoftbusLoop(void* pParam)
// {
// 	SBHANDLE hBus = (NULL==pParam)?l_hDTSBusMain:(SBHANDLE)pParam;
// 
// 	DTS_PRINTF("TestCommonLib DTSSoftbusLoop Begin\n");
// 
// 	GSB_SetOwnerTaskId(hBus,BISPGETTASKID());
// 
// 	GSBMSG msg;
// 
// 	while(GSB_PeekMessage(hBus,&msg,(DWORD)-1))
// 	{
// 		GSB_DispatchMessage(hBus,&msg);
// 	}
// 
// 	return 0; 
// }


LOCAL int DTSTestCommonOnJobAdd(LPCSTR	lpBuffer	,
								int		nLen		,
								int		nParam1		,
								int		nParam2	,
								int		nParam3	,
								int		nParam4	,
								int		nParam5	)
{
	int i;
	DTS_PRINTF("DTSTestCommonOnJobAdd Buff(len %d) :",nLen);
	for (i=0;i<nLen;i++)
	{
		DTS_PRINTF("%d ",lpBuffer[i]);
	}
	DTS_PRINTF("\nParam1(%d) Param2(%d) Param3(%d) Param4(%d) Param5(%d) \n\n",
			nParam1,nParam2,nParam3,nParam4,nParam5);
	
	return TRUE;
}

int DTSTestCommonLibOnTimer(UINT	nTimerId,
							LPCSTR	lpBuffer,
							int		nLen	,
							int		nParam1	,
							int		nParam2	,
							int		nParam3	,
							int		nParam4	,
							int		nParam5	)
{
	int i;
	DTS_PRINTF("DTSTestCommonLibOnTimer Buff(len %d) :",nLen);
	for (i=0;i<nLen;i++)
	{
		DTS_PRINTF("%d ",lpBuffer[i]);
	}
	DTS_PRINTF("\nParam1(%d) Param2(%d) Param3(%d) Param4(%d) Param5(%d) \n\n",
		nParam1,nParam2,nParam3,nParam4,nParam5);
	return 0;
}

int DTSTestUnitFuncCall(int nFuncType,LPDTSTESTCALLPARAM pParam) 
{

	DTSTESTUNITLOGENTRY LogEntry;
	LogEntry.nMainType = nFuncType;
	memcpy(LogEntry.pParamBuff,pParam,sizeof(DTSTESTCALLPARAM));
	DTSTestUnitMDLLogAddEntry(&LogEntry);

	return 0;
}


LOCAL int DTSTestCommonLibTestUint()
{
	///////////////////////////////////////////////////////////////////////////
	//测试TestUint

	DTS_PRINTF("TestCommonLib TEST Unit \n");

	DTSTestUnitMDLLogReset();
	int nLogcountBef,nLogcountaft;
	LPDTSTESTUNITLOGENTRY pLogEntry;
	LPDTSTESTCALLPARAM pCallParam;

	DTSTESTCALLPARAM CallParam = {0};
	CallParam.nParam1 = 1;
	CallParam.nParam2 = 2;
	CallParam.nParam3 = 3;

	nLogcountBef = DTSTestUnitMDLLogGetCount();
	DTSTestUnitFuncCall(DTS_TEST_CALL_TYPE_1,&CallParam);	
	nLogcountaft = DTSTestUnitMDLLogGetCount();
	BISPASSERT(1==nLogcountaft-nLogcountBef);
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	pCallParam = (LPDTSTESTCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(DTS_TEST_CALL_TYPE_1 == pLogEntry->nMainType);
	BISPASSERT(1==pCallParam->nParam1);
	BISPASSERT(2==pCallParam->nParam2);
	BISPASSERT(3==pCallParam->nParam3);


	CallParam.nParam1 = 4;
	CallParam.nParam2 = 5;
	CallParam.nParam3 = 6;
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	DTSTestUnitFuncCall(DTS_TEST_CALL_TYPE_2,&CallParam);	
	nLogcountaft = DTSTestUnitMDLLogGetCount();
	BISPASSERT(1==nLogcountaft-nLogcountBef);
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	pCallParam = (LPDTSTESTCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(DTS_TEST_CALL_TYPE_2 == pLogEntry->nMainType);
	BISPASSERT(4==pCallParam->nParam1);
	BISPASSERT(5==pCallParam->nParam2);
	BISPASSERT(6==pCallParam->nParam3);

	return 0;
}


//初始化软件总线，启动一条消息总线线程
BOOL DTSTestCommonLib(int nParam)
{
	//////////////////////////////////////////////////////////////////////////
	//启动总线
// 	int nHandle ;
// 	l_hDTSBusMain = GSB_Create(64,1500);
// 	if (NULL==l_hDTSBusMain) 
// 	{
// 		DTS_PRINTF("GSB_Create Fail \n");
// 	}
// 	nHandle = BISPTaskCreate( (BISPTASKPROC)DTSSoftbusLoop,
// 								NULL,
// 								80,
// 								BISP_DEFAULT_STACK_SIZE,
// 								"tSoBu");
//	DTSCommonLibInit();

	//////////////////////////////////////////////////////////////////////////
	//JobAdd测试
	#define TEST_BUFF_LEN	5
	BYTE arrbTest[TEST_BUFF_LEN] = {1,2,3,4,5};
	GSB_JobAdd(	g_hDTSBusMain		,
		(GSBFUNCPTR)DTSTestCommonOnJobAdd,
					(LPCSTR)arrbTest		,
					TEST_BUFF_LEN			,
					6,7,8,9,10);	


	//////////////////////////////////////////////////////////////////////////
	//Timmer测试
	#define TEST_TIME_OUT 3000
	UINT nTimerID = GSB_SetTimer(g_hDTSBusMain,
								TEST_TIME_OUT,
								3,
								//LOOP_FOREVER,
								(GSBTIMERFUNCPTR)DTSTestCommonLibOnTimer,
								(LPCSTR)arrbTest,TEST_BUFF_LEN,6,7,8,9,10);	

	//////////////////////////////////////////////////////////////////////////
	//测试TestUnit
	DTSTestCommonLibTestUint();

	
	return TRUE;
}	

