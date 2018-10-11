/********************************************************************
	created:	2018/04/09	17:24
	filename: 	DTSVirtualNetManagerFuncCallTestStub.cpp
	author:		sl.wu
	
	purpose:	对外调用的测试桩
*********************************************************************/
#include "../../DTSCommon/DTSCommon.h"
#include "../../DTSCommon/DTSTestUnit.h"
#include "DTSVirtualNetManagerTestFunc.h"
#include "../DTSVirtualNetManagerInterface.h"
#include "../DTSVirtualNetManagerFuncCall.h"
#include "../DTSVirtualNetManager.h"
#include "../DTSVirtualNetManagerPath.h"

#ifdef DTS_TEST


BOOL l_bTestIsPathConculateSucceed = TRUE;

LOCAL UINT l_nTestPathID = 100;

typedef map<DWORD,LPDTSVNMPath>		DTSDstMapPath,*LPDTSDstMapPath;
typedef DTSDstMapPath::iterator		DTSDstMapPathIter;

typedef map<DWORD,LPDTSDstMapPath>	DTSMapPath,*LPDTSMapPath;
typedef DTSMapPath::iterator		DTSMapPathIter;

LOCAL DTSMapPath l_VNMTestPathMap;
LOCAL DTSMapPath l_VNMTestBackupPathMap;

// LPDTSVNMPath DTSVNMTestGetBackupPath(DWORD dwSrcLB,DWORD dwDstLB)
// {
// 
// 	return NULL;
// }



LPDTSVNMPath DTSVNMTestGetPath(DWORD dwSrcLB,DWORD dwDstLB,LPDTSMapPath pPathMap)
{
	BISPASSERT(NULL!=pPathMap);
	DTSMapPathIter		iterMapPath;
	DTSDstMapPathIter	iterDstMapPath;
//	LPDTSMapPath pPathMap = &l_VNMTestPathMap;

	iterMapPath = pPathMap->find(dwSrcLB);
	if (iterMapPath == pPathMap->end())
	{
		return NULL;
	}

	iterDstMapPath = iterMapPath->second->find(dwDstLB);
	if (iterDstMapPath == iterMapPath->second->end())
	{
		return NULL;
	}
	l_nTestPathID++;
	iterDstMapPath->second->m_nPathID = l_nTestPathID;
	return iterDstMapPath->second;
}

LOCAL int DTSVNMTestCreatePortMac(LPDTSVNMLabelSwitch pLabelSwitch)
{
	BISPASSERT(NULL!=pLabelSwitch);
	DWORD* pdwTmp = NULL;
	pLabelSwitch->m_arrbInMac[0] = DTS_VNM_TEST_MAC_0_IN;
	pLabelSwitch->m_arrbInMac[1] = pLabelSwitch->m_nInPortIndex;
	pdwTmp = (DWORD*)(&(pLabelSwitch->m_arrbInMac[2]));
	*pdwTmp = pLabelSwitch->m_dwSwitcherLoopBack;

	pLabelSwitch->m_arrbOutMac[0] = DTS_VNM_TEST_MAC_0_OUT;
	pLabelSwitch->m_arrbOutMac[1] = pLabelSwitch->m_nOutPortIndex;
	pdwTmp = (DWORD*)(&(pLabelSwitch->m_arrbOutMac[2]));
	*pdwTmp = pLabelSwitch->m_dwSwitcherLoopBack;
	
	return VNM_RESULT_OK;
}

LOCAL int DTSVNMTestCreate2HopPath( DWORD dwSrcLB,UINT nSrcTrunkPort,
									DWORD dwTrunkLB,UINT nTrunkInPort,UINT nTrunkOutPort,
									DWORD dwDstLB,UINT nDstTrunkPort,
									LPDTSMapPath pPathMap)
{
	LPDTSVNMPath		pPathResult = NULL;
	LPDTSDstMapPath		pDstPath = NULL;
	DTSVNMLabelSwitch	LableSwitch;
	DTSMapPathIter		iterPathMap;
	DTSDstMapPathIter	iterDstPathMap;

	//第一级以源交换机为索引map
	iterPathMap = pPathMap->find(dwSrcLB);
	if (iterPathMap == pPathMap->end())
	{
		pDstPath = new DTSDstMapPath;
		pPathMap->insert(pair<DWORD,LPDTSDstMapPath>(dwSrcLB,pDstPath));
	}
	else
	{
		pDstPath = iterPathMap->second;
	}
	//第二级以目标交换机为索引map
	pPathResult = new DTSVNMPath;
	pDstPath->insert(pair<DWORD,LPDTSVNMPath>(dwDstLB,pPathResult)); 
	//	l_VNMTestPathMap[dwSrcLB] = pDstPath;
	pPathMap->insert(pair<DWORD,LPDTSDstMapPath>(dwSrcLB,pDstPath)); 
	l_nTestPathID++;
	pPathResult->m_nPathID = l_nTestPathID;
	pPathResult->m_nSwitcherCount = 3;
	pPathResult->m_dwBeginSwitcherLoopBack = dwSrcLB;
	pPathResult->m_nBeginPortIndex = 0;
	pPathResult->m_dwEndSwitcherLoopBack = dwDstLB;
	pPathResult->m_nEndPortIndex = 0;

	//第一个交换机
	LableSwitch.m_dwSwitcherLoopBack = dwSrcLB;
	LableSwitch.m_pPath = (void*)pPathResult;
	LableSwitch.m_nInPortIndex = 0;
	LableSwitch.m_nOutPortIndex = nSrcTrunkPort;
	DTSVNMTestCreatePortMac(&LableSwitch);
	pPathResult->m_vectLabelSwitch.push_back(LableSwitch);
	

	//第二个交换机（中继）
	LableSwitch.m_dwSwitcherLoopBack = dwTrunkLB;
	LableSwitch.m_pPath = (void*)pPathResult;
	LableSwitch.m_nInPortIndex = nTrunkInPort;
	LableSwitch.m_nOutPortIndex = nTrunkOutPort;
	DTSVNMTestCreatePortMac(&LableSwitch);
	pPathResult->m_vectLabelSwitch.push_back(LableSwitch);


	//第三个交换机
	LableSwitch.m_dwSwitcherLoopBack = dwDstLB;
	LableSwitch.m_pPath = (void*)pPathResult;
	LableSwitch.m_nInPortIndex = nDstTrunkPort;
	LableSwitch.m_nOutPortIndex = 0;
	DTSVNMTestCreatePortMac(&LableSwitch);
	pPathResult->m_vectLabelSwitch.push_back(LableSwitch);


	return VNM_RESULT_OK;
}

LOCAL int DTSVNMTestCreateNHopPath( DWORD dwSrcLB,UINT nSrcTrunkPort,
									DWORD* arrdwTrunkLB,UINT* arrnTrunkInPort,UINT* arrnTrunkOutPort,
									UINT nHop,DWORD dwDstLB,UINT nDstTrunkPort,LPDTSMapPath pPathMap)
{
	LPDTSVNMPath		pPathResult = NULL;
	LPDTSDstMapPath		pDstPath = NULL;
	DTSVNMLabelSwitch	LableSwitch;
	DTSMapPathIter		iterPathMap;
	DTSDstMapPathIter	iterDstPathMap;

	//第一级以源交换机为索引map
	iterPathMap = pPathMap->find(dwSrcLB);
	if (iterPathMap == pPathMap->end())
	{
		pDstPath = new DTSDstMapPath;
		pPathMap->insert(pair<DWORD,LPDTSDstMapPath>(dwSrcLB,pDstPath));
	}
	else
	{
		pDstPath = iterPathMap->second;
	}
	//第二级以目标交换机为索引map
	pPathResult = new DTSVNMPath;
	pDstPath->insert(pair<DWORD,LPDTSVNMPath>(dwDstLB,pPathResult)); 
	//	l_VNMTestPathMap[dwSrcLB] = pDstPath;
	pPathMap->insert(pair<DWORD,LPDTSDstMapPath>(dwSrcLB,pDstPath)); 
	l_nTestPathID++;
	pPathResult->m_nPathID = l_nTestPathID;
	pPathResult->m_nSwitcherCount = 3;
	pPathResult->m_dwBeginSwitcherLoopBack = dwSrcLB;
	pPathResult->m_nBeginPortIndex = 0;
	pPathResult->m_dwEndSwitcherLoopBack = dwDstLB;
	pPathResult->m_nEndPortIndex = 0;

	//第一个交换机
	LableSwitch.m_dwSwitcherLoopBack = dwSrcLB;
	LableSwitch.m_pPath = (void*)pPathResult;
	LableSwitch.m_nInPortIndex = 0;
	LableSwitch.m_nOutPortIndex = nSrcTrunkPort;
	DTSVNMTestCreatePortMac(&LableSwitch);
	pPathResult->m_vectLabelSwitch.push_back(LableSwitch);
	
	//中继交换机
	UINT i = 0;
	for(i=0; i<nHop ; i++)
	{
		LableSwitch.m_dwSwitcherLoopBack = arrdwTrunkLB[i];
		LableSwitch.m_pPath = (void*)pPathResult;
		LableSwitch.m_nInPortIndex = arrnTrunkInPort[i];
		LableSwitch.m_nOutPortIndex = arrnTrunkOutPort[i];
		DTSVNMTestCreatePortMac(&LableSwitch);
		pPathResult->m_vectLabelSwitch.push_back(LableSwitch);		
	}

// 	LableSwitch.m_dwSwitcherLoopBack = dwTrunkLB;
// 	LableSwitch.m_pPath = (void*)pPathResult;
// 	LableSwitch.m_nInPortIndex = nTrunkInPort;
// 	LableSwitch.m_nOutPortIndex = nTrunkOutPort;
// 	pPathResult->m_vectLabelSwitch.push_back(LableSwitch);

	//第三个交换机
	LableSwitch.m_dwSwitcherLoopBack = dwDstLB;
	LableSwitch.m_pPath = (void*)pPathResult;
	LableSwitch.m_nInPortIndex = nDstTrunkPort;
	LableSwitch.m_nOutPortIndex = 0;
	DTSVNMTestCreatePortMac(&LableSwitch);
	pPathResult->m_vectLabelSwitch.push_back(LableSwitch);

	return VNM_RESULT_OK;
}



LOCAL int DTSVNMTestCreateP2PPath(DWORD dwSrcLB,UINT nSrcTrunkPort,
								DWORD dwDstLB,UINT nDstTrunkPort, LPDTSMapPath pPathMap)
{
	LPDTSVNMPath		pPathResult = NULL;
	LPDTSDstMapPath		pDstPath = NULL;
	DTSVNMLabelSwitch	LableSwitch;
	DTSMapPathIter		iterPathMap;
	DTSDstMapPathIter	iterDstPathMap;

	//第一级以源交换机为索引map
//	iterPathMap = l_VNMTestPathMap.find(dwSrcLB);
	iterPathMap = pPathMap->find(dwSrcLB);
	if (iterPathMap == pPathMap->end())
	{
		pDstPath = new DTSDstMapPath;
//		l_VNMTestPathMap.insert(pair<DWORD,LPDTSDstMapPath>(dwSrcLB,pDstPath));
		pPathMap->insert(pair<DWORD,LPDTSDstMapPath>(dwSrcLB,pDstPath));
	}
	else
	{
		pDstPath = iterPathMap->second;
	}
	//第二级以目标交换机为索引map
	pPathResult = new DTSVNMPath;
	pDstPath->insert(pair<DWORD,LPDTSVNMPath>(dwDstLB,pPathResult)); 
//	l_VNMTestPathMap[dwSrcLB] = pDstPath;
	pPathMap->insert(pair<DWORD,LPDTSDstMapPath>(dwSrcLB,pDstPath)); 
//	l_nTestPathID++;
	pPathResult->m_nPathID = l_nTestPathID;
	pPathResult->m_nSwitcherCount = 2;
	pPathResult->m_dwBeginSwitcherLoopBack = dwSrcLB;
	pPathResult->m_nBeginPortIndex = 0;
	pPathResult->m_dwEndSwitcherLoopBack = dwDstLB;
	pPathResult->m_nEndPortIndex = 0;

	//第一个交换机
	LableSwitch.m_dwSwitcherLoopBack = dwSrcLB;
	LableSwitch.m_pPath = (void*)pPathResult;
	LableSwitch.m_nInPortIndex = 0;
	LableSwitch.m_nOutPortIndex = nSrcTrunkPort;
	DTSVNMTestCreatePortMac(&LableSwitch);
	pPathResult->m_vectLabelSwitch.push_back(LableSwitch);

	//第二个交换机
	LableSwitch.m_dwSwitcherLoopBack = dwDstLB;
	LableSwitch.m_pPath = (void*)pPathResult;
	LableSwitch.m_nInPortIndex = nDstTrunkPort;
	LableSwitch.m_nOutPortIndex = 0;
	DTSVNMTestCreatePortMac(&LableSwitch);
	pPathResult->m_vectLabelSwitch.push_back(LableSwitch);
	
	return VNM_RESULT_OK;
}


//建立测试拓扑，PathMap，Backup PathMap
int  DTSVNMCreateTestTopoPath()
{
	DWORD dwSrcLB,dwDstLB,dwTrunkLB;
	UINT nSrcTrunkPort,nDstTrunkPort,nTrunkInPort,nTrunkOutPort;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//一跳路径
	//--------------------------------------------------------------------------------------
	//0x01010101[3]---0x02020202[3]
	dwSrcLB = 0x01010101;
	nSrcTrunkPort = 3;
	dwDstLB = 0x02020202;
	nDstTrunkPort = 3;
	DTSVNMTestCreateP2PPath(dwSrcLB,nSrcTrunkPort,dwDstLB,nDstTrunkPort,&l_VNMTestPathMap);
	//反向
	//0x02020202[3]---0x01010101[3]
	dwSrcLB = 0x02020202;
	nSrcTrunkPort = 3;
	dwDstLB = 0x01010101;
	nDstTrunkPort = 3;
	DTSVNMTestCreateP2PPath(dwSrcLB,nSrcTrunkPort,dwDstLB,nDstTrunkPort,&l_VNMTestPathMap);

	//--------------------------------------------------------------------------------------
	//0x01010101[2]---0x04040404[2]
	dwSrcLB = 0x01010101;
	nSrcTrunkPort = 2;
	dwDstLB = 0x04040404;
	nDstTrunkPort = 2;
	DTSVNMTestCreateP2PPath(dwSrcLB,nSrcTrunkPort,dwDstLB,nDstTrunkPort,&l_VNMTestPathMap);
	//反向
	//0x04040404[2]---0x01010101[2]
	dwSrcLB = 0x04040404;
	nSrcTrunkPort = 2;
	dwDstLB = 0x01010101;
	nDstTrunkPort = 2;
	DTSVNMTestCreateP2PPath(dwSrcLB,nSrcTrunkPort,dwDstLB,nDstTrunkPort,&l_VNMTestPathMap);
	
// 	//--------------------------------------------------------------------------------------
// 	//0x02020202[4]---0x04040404[4]
// 	dwSrcLB = 0x02020202;
// 	nSrcTrunkPort = 4;
// 	dwDstLB = 0x04040404;
// 	nDstTrunkPort = 4;
// 	DTSVNMTestCreateP2PPath(dwSrcLB,nSrcTrunkPort,dwDstLB,nDstTrunkPort);
// 	//反向
// 	//0x02020202[4]---0x04040404[4]
// 	dwSrcLB = 0x04040404;
// 	nSrcTrunkPort = 4;
// 	dwDstLB = 0x02020202;
// 	nDstTrunkPort = 4;
// 	DTSVNMTestCreateP2PPath(dwSrcLB,nSrcTrunkPort,dwDstLB,nDstTrunkPort);

	//--------------------------------------------------------------------------------------
	//0x02020202[2]---0x03030303[2]
	dwSrcLB = 0x02020202;
	nSrcTrunkPort = 2;
	dwDstLB = 0x03030303;
	nDstTrunkPort = 2;
	DTSVNMTestCreateP2PPath(dwSrcLB,nSrcTrunkPort,dwDstLB,nDstTrunkPort,&l_VNMTestPathMap);
	//反向
	//0x02020202[2]---0x04040404[2]
	dwSrcLB = 0x03030303;
	nSrcTrunkPort = 2;
	dwDstLB = 0x02020202;
	nDstTrunkPort = 2;
	DTSVNMTestCreateP2PPath(dwSrcLB,nSrcTrunkPort,dwDstLB,nDstTrunkPort,&l_VNMTestPathMap);
	
	//--------------------------------------------------------------------------------------
	//0x03030303[3]---0x04040404[3]
	dwSrcLB = 0x03030303;
	nSrcTrunkPort = 3;
	dwDstLB = 0x04040404;
	nDstTrunkPort = 3;
	DTSVNMTestCreateP2PPath(dwSrcLB,nSrcTrunkPort,dwDstLB,nDstTrunkPort,&l_VNMTestPathMap);
	//反向
	//0x04040404[3]---0x03030303[3]
	dwSrcLB = 0x04040404;
	nSrcTrunkPort = 3;
	dwDstLB = 0x03030303;
	nDstTrunkPort = 3;
	DTSVNMTestCreateP2PPath(dwSrcLB,nSrcTrunkPort,dwDstLB,nDstTrunkPort,&l_VNMTestPathMap);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//两跳路径
	//0x01010101[3]---0x02020202[3][2]------[2]0x03030303
	dwSrcLB = 0x01010101;
	nSrcTrunkPort = 3;
	dwTrunkLB = 0x02020202;
	nTrunkInPort = 3;
	nTrunkOutPort = 2;
	dwDstLB = 0x03030303;
	nDstTrunkPort = 2;
	DTSVNMTestCreate2HopPath(dwSrcLB,nSrcTrunkPort,
							dwTrunkLB,nTrunkInPort,nTrunkOutPort,
							dwDstLB,nDstTrunkPort,&l_VNMTestPathMap);
	//反向
	dwSrcLB = 0x03030303;
	nSrcTrunkPort = 2;
	dwTrunkLB = 0x02020202;
	nTrunkInPort = 2;
	nTrunkOutPort = 3;
	dwDstLB = 0x01010101;
	nDstTrunkPort = 3;
	DTSVNMTestCreate2HopPath(dwSrcLB,nSrcTrunkPort,
							dwTrunkLB,nTrunkInPort,nTrunkOutPort,
							dwDstLB,nDstTrunkPort,&l_VNMTestPathMap);

	//--------------------------------------------------------------------------------------
	//0x04040404[2]---0x01010101[2][3]------[3]0x02020202
	dwSrcLB = 0x04040404;
	nSrcTrunkPort = 2;
	dwTrunkLB = 0x01010101;
	nTrunkInPort = 2;
	nTrunkOutPort = 3;
	dwDstLB = 0x02020202;
	nDstTrunkPort = 3;
	DTSVNMTestCreate2HopPath(dwSrcLB,nSrcTrunkPort,
							dwTrunkLB,nTrunkInPort,nTrunkOutPort,
							dwDstLB,nDstTrunkPort,&l_VNMTestPathMap);
	//反向
	dwSrcLB = 0x02020202;
	nSrcTrunkPort = 3;
	dwTrunkLB = 0x01010101;
	nTrunkInPort = 3;
	nTrunkOutPort = 2;
	dwDstLB = 0x04040404;
	nDstTrunkPort = 2;
	DTSVNMTestCreate2HopPath(dwSrcLB,nSrcTrunkPort,
							dwTrunkLB,nTrunkInPort,nTrunkOutPort,
							dwDstLB,nDstTrunkPort,&l_VNMTestPathMap);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//备份链路
	//--------------------------------------------------------------------------------------
	//0x02020202[3]--[3]0x01010101[2]--[2]0x04040404[3]--[3]0x03030303
	dwSrcLB = 0x02020202;
	nSrcTrunkPort = 3;
	DWORD arrdwTrunkLB[2] = {0x01010101,0x04040404};
	UINT arrnTrunkInPort[2] = {3,2};
	UINT arrnTrunkOutPort[2] = {2,3};
	dwDstLB = 0x03030303;
	nDstTrunkPort = 3;
	DTSVNMTestCreateNHopPath(dwSrcLB,nSrcTrunkPort,
							arrdwTrunkLB,arrnTrunkInPort,arrnTrunkOutPort,2,
							dwDstLB,nDstTrunkPort,&l_VNMTestBackupPathMap);
	//反向
	//0x03030303[3]--[3]0x04040404[2]--[2]0x01010101[3]--[3]0x02020202
	dwSrcLB = 0x03030303;
	nSrcTrunkPort = 3;
	arrdwTrunkLB[0] = 0x04040404;
	arrdwTrunkLB[1] = 0x01010101;
	arrnTrunkInPort[0] = 3;
	arrnTrunkInPort[1] = 2;
	arrnTrunkOutPort[0] = 2;
	arrnTrunkOutPort[1] = 3;
	dwDstLB = 0x02020202;
	nDstTrunkPort = 3;
	DTSVNMTestCreateNHopPath(dwSrcLB,nSrcTrunkPort,
							arrdwTrunkLB,arrnTrunkInPort,arrnTrunkOutPort,2,
							dwDstLB,nDstTrunkPort,&l_VNMTestBackupPathMap);


	//--------------------------------------------------------------------------------------
	//0x01010101[2]--[2]0x04040404[3]--[3]0x03030303
	dwSrcLB = 0x01010101;
	nSrcTrunkPort = 2;
	arrdwTrunkLB[0] = 0x04040404;
//	arrdwTrunkLB[1] = 0x01010101;
	arrnTrunkInPort[0] = 2;
//	arrnTrunkInPort[1] = 2;
	arrnTrunkOutPort[0] = 3;
//	arrnTrunkOutPort[1] = 3;
	dwDstLB = 0x03030303;
	nDstTrunkPort = 3;
	DTSVNMTestCreateNHopPath(dwSrcLB,nSrcTrunkPort,
							arrdwTrunkLB,arrnTrunkInPort,arrnTrunkOutPort,1,
							dwDstLB,nDstTrunkPort,&l_VNMTestBackupPathMap);
	//反向
	//0x03030303[3]--[3]0x04040404[2]--[2]0x01010101
	dwSrcLB = 0x03030303;
	nSrcTrunkPort = 3;
	arrdwTrunkLB[0] = 0x04040404;
	//	arrdwTrunkLB[1] = 0x01010101;
	arrnTrunkInPort[0] = 3;
	//	arrnTrunkInPort[1] = 2;
	arrnTrunkOutPort[0] = 2;
	//	arrnTrunkOutPort[1] = 3;
	dwDstLB = 0x01010101;
	nDstTrunkPort = 2;
	DTSVNMTestCreateNHopPath(dwSrcLB,nSrcTrunkPort,
							arrdwTrunkLB,arrnTrunkInPort,arrnTrunkOutPort,1,
							dwDstLB,nDstTrunkPort,&l_VNMTestBackupPathMap);

	
	return VNM_RESULT_OK;
}



int DTSVNMFuncCall(int nFuncType,LPDTSVNMFUNCCALLPARAM pParam) 
{

	DTSTESTUNITLOGENTRY LogEntry = {0};
	LPDTSVNMPath pPathResult = NULL;
	LogEntry.nMainType = nFuncType;
	memcpy(LogEntry.pParamBuff,pParam,sizeof(DTSVNMFUNCCALLPARAM));
	DTSTestUnitMDLLogAddEntry(&LogEntry);

	DTS_VNM_DEBUG_PRINTF(DEBUG_ENTRY)
		("[ENTRY][VNM] DTSVNMFuncCall FunType(%s)\n",
		DTSVNMGetFuncTypeStr(nFuncType));

	if (VNM_FUNC_CALL_FLOWTABLE_SEND == nFuncType)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ENTRY)
			("VNet(%d) Port(0x%x)(%d) Action(%s)\n",
			pParam->m_nVirNetNo,
			pParam->m_dwSwitcherLoopBack,
			pParam->m_nPortIndex,
			DTSVNMGetFTActionStr(pParam->m_nFTAction));
	}


	if (VNM_FUNC_CALL_TOPO_CONCULATE == nFuncType)
	{
		if (!l_bTestIsPathConculateSucceed)
		{
			return VNM_RESULT_EXCEPTION;
		}
	
		LPDTSMapPath pPathMap = (VNM_CONCULATE_TYPE_AGAIN == pParam->m_nFTAction)?&l_VNMTestBackupPathMap:&l_VNMTestPathMap;

		pPathResult = DTSVNMTestGetPath(pParam->m_dwSwitcherLoopBack,
										pParam->m_dwOppoLoopBack,pPathMap);

		if (NULL==pPathResult)
		{
			return VNM_RESULT_EXCEPTION;
		}
		pParam->m_pReValueBuff = (char*)pPathResult;
		return VNM_RESULT_OK;
	}

	if (VNM_FUNC_CALL_FLOWTABLE_SEND == nFuncType)
	{
		LogEntry.nMainType = VNM_FUNC_CALL_MSG_SEND;
		LogEntry.nSendDataBuffLen = DTSVNMMakeOFPMsg(pParam,LogEntry.pSendDataBuff,DTS_TEST_UNIT_BUFF_LEN);
		DTS_VNM_DEBUG_PRINTF(DEBUG_ENTRY)
			("[ENTRY][VNM] DTSVNMFuncCall FunType(VNM_FUNC_CALL_MSG_SEND) len(%d)\n",
			LogEntry.nSendDataBuffLen);
		DTSTestUnitMDLLogAddEntry(&LogEntry);
	}
	
	return VNM_RESULT_OK;
}

// int DTSVNMSendBuff(DWORD dwDstAddr,BYTE* pBuff,int nLen)
// {
// 
// 	return  VNM_RESULT_OK;
// }


#endif //DTS_TEST

