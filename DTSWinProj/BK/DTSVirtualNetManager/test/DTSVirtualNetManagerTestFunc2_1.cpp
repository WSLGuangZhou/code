/********************************************************************
	created:	2018/05/03	9:37
	filename: 	DTSVirtualNetManagerTestFunc2_1.cpp
	author:		sl.wu
	
	purpose:	
*********************************************************************/



#include "../../DTSCommon/DTSCommon.h"
#include "../../DTSCommon/DTSTestUnit.h"

#ifdef DTS_TEST

#include "DTSVirtualNetManagerTestFunc.h"
#include "../DTSVirtualNetManagerInterface.h"
#include "../DTSVirtualNetManagerFuncCall.h"
#include "../DTSVirtualNetManager.h"
#include "../DTSVirtualNetManagerPath.h"
#include "../DTSOpenFlowCommon.h"
#include "../DTSOpenFlowPrivate.h"


int DTSVNMTestVNet_2StatsOneOutPortAddPortTest3()
{
	//VNet2增加0x04040404的1端口，原有0x02020202的5端口
	//0x04040404[2]---0x01010101[2][3]------[3]0x02020202
	UINT nVirNetNo = 2;
	LPDTSVirtualNet pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(1==pVirNet->m_mapOutPort.size());
	BISPASSERT(DTS_VNM_STATS_ONE_OUT_PORT==pVirNet->m_nStats);

	//交换机
	LPDTSVNMSwitcher pSwitcher1 = DTSVNMGetSwitcherByLoopBack(0x01010101);
	BISPASSERT(NULL!=pSwitcher1);
	BISPASSERT(3<=pSwitcher1->m_mapPort.size());
	LPDTSVNMSwitcher pSwitcher2 = DTSVNMGetSwitcherByLoopBack(0x02020202);
	BISPASSERT(NULL!=pSwitcher2);
	BISPASSERT(4==pSwitcher2->m_mapPort.size());
	LPDTSVNMSwitcher pSwitcher3 = DTSVNMGetSwitcherByLoopBack(0x03030303);
	BISPASSERT(NULL!=pSwitcher3);
	BISPASSERT(2<=pSwitcher3->m_mapPort.size());
	LPDTSVNMSwitcher pSwitcher4 = NULL;
	//	DTSVNMGetSwitcherByLoopBack(0x04040404);
	//前面没有S_4的端口
	//	BISPASSERT(NULL==pSwitcher4);

	//出口
	LPDTSVNMPort	 pOPortS4_1 = DTSVNMGetPort(0x04040404,1);
	if(NULL!=pOPortS4_1)
	{
		BISPASSERT(!pOPortS4_1->m_bIsAcitve);
	}
	LPDTSVNMPort	 pOPortS2_5 = DTSVNMGetPort(0x02020202,5);
	BISPASSERT(NULL!=pOPortS2_5);
	BISPASSERT(0==pOPortS2_5->m_mapLabelReffrence.size());
	BISPASSERT(pOPortS2_5->m_nVirNetNo == nVirNetNo);

	//中继端口
	LPDTSVNMPort	pTPortS4_2toS1_2 = DTSVNMGetPort(0x04040404,2);
	if(NULL!=pTPortS4_2toS1_2)
	{
		BISPASSERT(!pTPortS4_2toS1_2->m_bIsAcitve);
	}

	LPDTSVNMPort	pTPortS1_2toS4_2 =  DTSVNMGetPort(0x01010101,2);
	if(NULL!=pTPortS1_2toS4_2)
	{
		BISPASSERT(!pTPortS1_2toS4_2->m_bIsAcitve);
	}

	LPDTSVNMPort	pTPortS1_3toS2_3 =  DTSVNMGetPort(0x01010101,3);
	BISPASSERT(NULL!=pTPortS1_3toS2_3);


	LPDTSVNMPort	pTPortS2_3toS1_3 =  DTSVNMGetPort(0x02020202,3);
	BISPASSERT(NULL!=pTPortS2_3toS1_3);


	int nLogcountBef,nLogcountaft,nRet;
	//	int	nSwitcherCountBef,nSwitcherCountAft;
	int	nPathCountBef,nPathCountAft;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	LPDTSVNMPath pPath4to2 = NULL;
	LPDTSVNMPath pPathCheck4to2 = NULL;
	char szTmp[32]="";
	BOOL bIsMatch = FALSE;

	//	nSwitcherCountBef = DTSVNMGetSwitcherCount();
	nPathCountBef = DTSVNMGetPathCount();
	//////////////////////////////////////////////////////////////////////////
	//执行
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMAddOutPort(2,0x04040404,1);
	BISPASSERT(VNM_RESULT_OK==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	// 	pPath4to2 = DTSVNMTestGetPath(0x04040404,0x02020202);
	// 	BISPASSERT(NULL!=pPath4to2);

	//----------------------------------------------------------------------------
	//虚网列表
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(pVirNet->m_nVirNetNo==nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==nVirNetNo);
	BISPASSERT(DTS_VNM_STATS_ACTIVE == pVirNet->m_nStats);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),nVirNetNo);
	BISPASSERT(0==strcmp(szTmp,pVirNet->m_szVirNetName));
	BISPASSERT(2==pVirNet->m_mapOutPort.size());
	BISPASSERT(1==pVirNet->m_mapOutPort.count(0x04040404));
	BISPASSERT(1==pVirNet->m_mapOutPort.count(0x02020202));

	BISPASSERT(1==pVirNet->m_mapOutPort[0x04040404]);
	BISPASSERT(5==pVirNet->m_mapOutPort[0x02020202]);

	//-----------------------------------------------------------------------------
	//交换机列表，增加一个Switcher 0x04040404
	//	nSwitcherCountAft = DTSVNMGetSwitcherCount();
	//	BISPASSERT(1==nSwitcherCountAft-nSwitcherCountBef);

	//0x01010101 增加一个中继端口
	pSwitcher1 = DTSVNMGetSwitcherByLoopBack(0x01010101);
	BISPASSERT(NULL!=pSwitcher1);
	BISPASSERT(4==pSwitcher1->m_mapPort.size());

	//0x02020202 不变
	pSwitcher2 = DTSVNMGetSwitcherByLoopBack(0x02020202);
	BISPASSERT(NULL!=pSwitcher2);
	BISPASSERT(4==pSwitcher2->m_mapPort.size());

	//0x03030303 不变
	pSwitcher3 = DTSVNMGetSwitcherByLoopBack(0x03030303);
	BISPASSERT(NULL!=pSwitcher3);	
	BISPASSERT(2<=pSwitcher3->m_mapPort.size());

	//0x04040404 新创建，2个端口
	pSwitcher4 = DTSVNMGetSwitcherByLoopBack(0x04040404);
	BISPASSERT(NULL!=pSwitcher4);	
	BISPASSERT(2<=pSwitcher4->m_mapPort.size());

	//////////////////////////////////////////////////////////////////////////
	//端口
	//0x04040404
	pOPortS4_1 = DTSVNMGetPort(0x04040404,1);
	BISPASSERT(NULL!=pOPortS4_1);
	BISPASSERT(pOPortS4_1->m_bIsOutPort);
	BISPASSERT(pOPortS4_1->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS4_1->m_nVirNetNo == nVirNetNo);
	BISPASSERT(1==pOPortS4_1->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS4_1->m_mapLabelReffrence.size());
	BISPASSERT(1==pOPortS4_1->m_vectPathID.size());
	pPath4to2 = DTSVNMGetPathByID(pOPortS4_1->m_vectPathID[0]);
	BISPASSERT(NULL!=pPath4to2);
	//	BISPASSERT(pPath4to2->m_nPathID == pOPortS4_1->m_vectPathID[0]);

	pTPortS4_2toS1_2 = DTSVNMGetPort(0x04040404,2);
	BISPASSERT(NULL!=pTPortS4_2toS1_2);
	BISPASSERT(!pTPortS4_2toS1_2->m_bIsOutPort);
	BISPASSERT(pTPortS4_2toS1_2->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_2toS1_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS4_2toS1_2->m_vectPathID.size());
	BISPASSERT(pPath4to2->m_nPathID == pTPortS4_2toS1_2->m_vectPathID[0]);
	//	BISPASSERT(pPath3to1->m_nPathID == pOPortS4_1->m_vectPathID[0]);

	//0x02020202
	pOPortS2_5 = DTSVNMGetPort(0x02020202,5);
	BISPASSERT(NULL!=pOPortS2_5);
	BISPASSERT(pOPortS2_5->m_bIsOutPort);
	BISPASSERT(pOPortS2_5->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS2_5->m_nVirNetNo == nVirNetNo);
	BISPASSERT(1==pOPortS2_5->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS2_5->m_mapLabelReffrence.size());
	BISPASSERT(1==pOPortS2_5->m_vectPathID.size());
	BISPASSERT(pPath4to2->m_nPathID == pOPortS2_5->m_vectPathID[0]);

	pTPortS2_3toS1_3 =  DTSVNMGetPort(0x02020202,3);
	BISPASSERT(NULL!=pTPortS2_3toS1_3);
	BISPASSERT(!pTPortS2_3toS1_3->m_bIsOutPort);
	BISPASSERT(pTPortS2_3toS1_3->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_3toS1_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(2==pTPortS2_3toS1_3->m_mapLabelReffrence[1]);
	BISPASSERT(2==pTPortS2_3toS1_3->m_mapLabelReffrence.size());
	BISPASSERT(3==pTPortS2_3toS1_3->m_vectPathID.size());
	//原有影响3-1、2-1两条路径，新增4-2
	// 	BISPASSERT(pPath3to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[1]);
	// 	BISPASSERT(pPath2to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[0]);
	BISPASSERT(pPath4to2->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[2]);

	//0x01010101
	pTPortS1_2toS4_2 =  DTSVNMGetPort(0x01010101,2);
	BISPASSERT(NULL!=pTPortS1_2toS4_2);
	BISPASSERT(!pTPortS1_2toS4_2->m_bIsOutPort);
	BISPASSERT(pTPortS1_2toS4_2->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_2toS4_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS1_2toS4_2->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS1_2toS4_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS1_2toS4_2->m_vectPathID.size());
	BISPASSERT(pPath4to2->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[0]);

	pTPortS1_3toS2_3 =  DTSVNMGetPort(0x01010101,3);
	BISPASSERT(NULL!=pTPortS1_3toS2_3);
	BISPASSERT(!pTPortS1_3toS2_3->m_bIsOutPort);
	BISPASSERT(pTPortS1_3toS2_3->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_3toS2_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(2==pTPortS1_3toS2_3->m_mapLabelReffrence[1]);
	BISPASSERT(2==pTPortS1_3toS2_3->m_mapLabelReffrence.size());
	BISPASSERT(3==pTPortS1_3toS2_3->m_vectPathID.size());
	//原有影响3-1、2-1两条路径，新增4-2
	// 	BISPASSERT(pPath3to1->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[1]);
	// 	BISPASSERT(pPath2to1->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[0]);
	BISPASSERT(pPath4to2->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[2]);

	//-----------------------------------------------------------------------------
	//路径列表
	nPathCountAft = DTSVNMGetPathCount();
	BISPASSERT(1==nPathCountAft-nPathCountBef);
	//0x04040404[1][2]---0x01010101[2][3]------[3][5]0x02020202
	//pPathCheck4to2
	pPathCheck4to2 = DTSVNMGetPathByID(pPath4to2->m_nPathID);
	BISPASSERT(NULL!=pPathCheck4to2);
	BISPASSERT(pPath4to2->m_nPathID == pPathCheck4to2->m_nPathID);
	BISPASSERT(2==pPathCheck4to2->m_nVirNetNo);
	BISPASSERT(3==pPathCheck4to2->m_nSwitcherCount);
	BISPASSERT(0x04040404==pPathCheck4to2->m_dwBeginSwitcherLoopBack);
	BISPASSERT(1==pPathCheck4to2->m_nBeginPortIndex);
	BISPASSERT(0x02020202==pPathCheck4to2->m_dwEndSwitcherLoopBack);
	BISPASSERT(5==pPathCheck4to2->m_nEndPortIndex);

	BISPASSERT(3==pPathCheck4to2->m_vectLabelSwitch.size());
	BISPASSERT(pPathCheck4to2==pPathCheck4to2->m_vectLabelSwitch[0].m_pPath);
	BISPASSERT(0x04040404==pPathCheck4to2->m_vectLabelSwitch[0].m_dwSwitcherLoopBack);
	BISPASSERT(1==pPathCheck4to2->m_vectLabelSwitch[0].m_nInPortIndex);
	//	BISPASSERT(2==pPathCheck4to2->m_vectLabelSwitch[0].m_nInPWLabel);
	BISPASSERT(2==pPathCheck4to2->m_vectLabelSwitch[0].m_nOutPortIndex);
	//	BISPASSERT(2==pPathCheck4to2->m_vectLabelSwitch[0].m_nInPWLabel);
	BISPASSERT(pPathCheck4to2==pPathCheck4to2->m_vectLabelSwitch[1].m_pPath);
	BISPASSERT(0x01010101==pPathCheck4to2->m_vectLabelSwitch[1].m_dwSwitcherLoopBack);
	BISPASSERT(2==pPathCheck4to2->m_vectLabelSwitch[1].m_nInPortIndex);
	//	BISPASSERT(2==pPathCheck4to2->m_vectLabelSwitch[1].m_nInPWLabel);
	BISPASSERT(3==pPathCheck4to2->m_vectLabelSwitch[1].m_nOutPortIndex);
	//	BISPASSERT(2==pPathCheck4to2->m_vectLabelSwitch[1].m_nInPWLabel);
	BISPASSERT(0x02020202==pPathCheck4to2->m_vectLabelSwitch[2].m_dwSwitcherLoopBack);
	BISPASSERT(3==pPathCheck4to2->m_vectLabelSwitch[2].m_nInPortIndex);
	//	BISPASSERT(2==pPathCheck4to2->m_vectLabelSwitch[2].m_nInPWLabel);
	BISPASSERT(5==pPathCheck4to2->m_vectLabelSwitch[2].m_nOutPortIndex);
	//	BISPASSERT(2==pPathCheck4to2->m_vectLabelSwitch[2].m_nInPWLabel);

	//----------------------------------------------------------------------------
	//外部调用
	//外部调用1次路径计算，中继端口加入虚网4次，出口交换2次
	BISPASSERT(13==nLogcountaft-nLogcountBef);
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-13);
	BISPASSERT(VNM_FUNC_CALL_TOPO_CONCULATE==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(pCallParam->m_nBandWidth == pVirNet->m_nBandWidth);
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(0x02020202==pCallParam->m_dwOppoLoopBack);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-12);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(1==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-11);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-10);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-9);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-8);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(3==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-7);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-6);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(2==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-5);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-4);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-3);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-2);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(5==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	return 0;
}



#endif //DTS_TEST

