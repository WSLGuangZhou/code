/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   15:37
	filename: 	DTSVirtualNetManagerTestFunc2.cpp
	author:		sl.wu
	
	purpose:	Virtual Net Manager 模块测试用例
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




int DTSVNMTestVNet_1StatsActiveAddPortTest2()
{
	//VNet1加入0x03030303上的1端口，可测试到一跳(R2--R3)和两跳(R1-R4-R3)
	//0x02020202[1][2]--[2][1]0x03030303
	//0x01010101[1][3]---0x02020202[3][2]------[2][1]0x03030303
	UINT nVirNetNo = 1;
	LPDTSVirtualNet pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(2==pVirNet->m_mapOutPort.size());
	BISPASSERT(DTS_VNM_STATS_ACTIVE==pVirNet->m_nStats);

	LPDTSVNMSwitcher pSwitcher1 = DTSVNMGetSwitcherByLoopBack(0x01010101);
	BISPASSERT(NULL!=pSwitcher1);
//	BISPASSERT(3==pSwitcher1->m_mapPort.size());
	LPDTSVNMSwitcher pSwitcher2 = DTSVNMGetSwitcherByLoopBack(0x02020202);
	BISPASSERT(NULL!=pSwitcher2);
//	BISPASSERT(3==pSwitcher2->m_mapPort.size());
	LPDTSVNMSwitcher pSwitcher3 = NULL;
//	DTSVNMGetSwitcherByLoopBack(0x03030303);
	//前面没有S_3的端口
//	BISPASSERT(NULL==pSwitcher3);

	//出口
	LPDTSVNMPort	 pOPortS1_1 = DTSVNMGetPort(0x01010101,1);
	BISPASSERT(NULL!=pOPortS1_1);
	BISPASSERT(0!=pOPortS1_1->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);

	LPDTSVNMPort	 pOPortS2_1 = DTSVNMGetPort(0x02020202,1);
	BISPASSERT(NULL!=pOPortS2_1);
	BISPASSERT(0!=pOPortS2_1->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);

	LPDTSVNMPort	 pOPortS2_5 = DTSVNMGetPort(0x02020202,5);
	BISPASSERT(NULL!=pOPortS2_5);
	BISPASSERT(0==pOPortS2_5->m_mapLabelReffrence.size());
	BISPASSERT((pOPortS2_5->m_nVirNetNo == 2)||(pOPortS2_5->m_nVirNetNo == DTS_VNET_NO_NULL));
	
	LPDTSVNMPort	 pOPortS3_1 = DTSVNMGetPort(0x03030303,1);
	//前面没有S_3的端口
	if (NULL!=pOPortS3_1)
	{
		BISPASSERT(!pOPortS3_1->m_bIsAcitve);
	}
	
	//中继端口
	LPDTSVNMPort	 pTPortS1_3toS2_3 = DTSVNMGetPort(0x01010101,3);
	BISPASSERT(NULL!=pTPortS1_3toS2_3);
	BISPASSERT(1==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);

	LPDTSVNMPort	 pTPortS2_3toS1_3 = DTSVNMGetPort(0x02020202,3);
	BISPASSERT(NULL!=pTPortS2_3toS1_3);
	BISPASSERT(1==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);

	LPDTSVNMPort	 pTPortS2_2toS3_2 = DTSVNMGetPort(0x02020202,2);
	if(NULL!=pTPortS2_2toS3_2)
	{
		BISPASSERT(!pTPortS2_2toS3_2->m_bIsAcitve);
	}

	LPDTSVNMPort	 pTPortS3_2toS2_2 = DTSVNMGetPort(0x03030303,2);
	if(NULL!=pTPortS3_2toS2_2)
	{
		BISPASSERT(!pTPortS3_2toS2_2->m_bIsAcitve);
	}
	
	int nLogcountBef,nLogcountaft,nRet;
//	int	nSwitcherCountBef,nSwitcherCountAft;
	int	nPathCountBef,nPathCountAft;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	LPDTSVNMPath pPath3to2 = NULL;
	LPDTSVNMPath pPath3to1 = NULL;
	LPDTSVNMPath pPath2to1 = NULL;	
	LPDTSVNMPath pPathCheck3to2 = NULL;
	LPDTSVNMPath pPathCheck3to1 = NULL;
	LPDTSVNMPath pPathCheck2to1 = NULL;	
	char szTmp[32]="";
	BOOL bIsMatch = FALSE;

//	nSwitcherCountBef = DTSVNMGetSwitcherCount();
	nPathCountBef = DTSVNMGetPathCount();
	//////////////////////////////////////////////////////////////////////////
	//执行
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMAddOutPort(nVirNetNo,0x03030303,1);
	BISPASSERT(VNM_RESULT_OK==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

// 	pPath3to2 = DTSVNMTestGetPath(0x03030303,0x02020202);
// 	pPath3to1 = DTSVNMTestGetPath(0x03030303,0x01010101);
// 	pPath2to1 = DTSVNMTestGetPath(0x02020202,0x01010101);
// 	BISPASSERT(NULL!=pPath3to2);
// 	BISPASSERT(NULL!=pPath3to1);
// 	BISPASSERT(NULL!=pPath2to1);

	//----------------------------------------------------------------------------
	//虚网列表
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(pVirNet->m_nVirNetNo==nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==nVirNetNo);
	BISPASSERT(DTS_VNM_STATS_ACTIVE == pVirNet->m_nStats);
	BISPASSERT(3==pVirNet->m_mapOutPort.size());
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),nVirNetNo);
	BISPASSERT(0==strcmp(szTmp,pVirNet->m_szVirNetName));
	BISPASSERT(1==pVirNet->m_mapOutPort.count(0x03030303));
	BISPASSERT(1==pVirNet->m_mapOutPort.count(0x02020202));
	BISPASSERT(1==pVirNet->m_mapOutPort.count(0x01010101));

	BISPASSERT(1==pVirNet->m_mapOutPort[0x03030303]);
	BISPASSERT(1==pVirNet->m_mapOutPort[0x02020202]);
	BISPASSERT(1==pVirNet->m_mapOutPort[0x01010101]);
	

	//-----------------------------------------------------------------------------
	//交换机列表，增加一个Switcher 0x03030303
//	nSwitcherCountAft = DTSVNMGetSwitcherCount();
//	BISPASSERT(1==nSwitcherCountAft-nSwitcherCountBef);

	//0x01010101 不变
	pSwitcher1 = DTSVNMGetSwitcherByLoopBack(0x01010101);
	BISPASSERT(NULL!=pSwitcher1);
	BISPASSERT(3<=pSwitcher1->m_mapPort.size());

	//0x02020202 增加一个端口
	pSwitcher2 = DTSVNMGetSwitcherByLoopBack(0x02020202);
	BISPASSERT(NULL!=pSwitcher2);
	BISPASSERT(4==pSwitcher2->m_mapPort.size());

	//0x03030303 新创建，2个端口
	pSwitcher3 = DTSVNMGetSwitcherByLoopBack(0x03030303);
	BISPASSERT(NULL!=pSwitcher3);	
	BISPASSERT(2<=pSwitcher3->m_mapPort.size());
	
	//-----------------------------------------------------------------------------
	//端口
	//0x03030303 2个端口
	pOPortS3_1 = DTSVNMGetPort(0x03030303,1);
	BISPASSERT(pOPortS3_1->m_bIsOutPort);
	BISPASSERT(pOPortS3_1->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_1->m_nVirNetNo == nVirNetNo);
	BISPASSERT(2==pOPortS3_1->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS3_1->m_mapLabelReffrence.size());
	BISPASSERT(2==pOPortS3_1->m_vectPathID.size());
	pPath3to2 = DTSVNMGetPathByID(pOPortS3_1->m_vectPathID[1]);
	BISPASSERT(NULL!=pPath3to2);
	pPath3to1 = DTSVNMGetPathByID(pOPortS3_1->m_vectPathID[0]);
	BISPASSERT(NULL!=pPath3to1);
// 	BISPASSERT(pPath3to2->m_nPathID == pOPortS3_1->m_vectPathID[1]);
// 	BISPASSERT(pPath3to1->m_nPathID == pOPortS3_1->m_vectPathID[0]);
	
	BISPASSERT(NULL!=pOPortS3_1);
	pTPortS3_2toS2_2 = DTSVNMGetPort(0x03030303,2);
	BISPASSERT(NULL!=pTPortS3_2toS2_2);
	BISPASSERT(!pTPortS3_2toS2_2->m_bIsOutPort);
	BISPASSERT(pTPortS3_2toS2_2->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS3_2toS2_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(2==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(2==pTPortS3_2toS2_2->m_vectPathID.size());
	//影响3-2、3-1两条路径
	BISPASSERT(pPath3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[1]);
	BISPASSERT(pPath3to1->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);

	//0x02020202 4个端口
	pOPortS2_1 = DTSVNMGetPort(0x02020202,1);
	BISPASSERT(NULL!=pOPortS2_1);
	BISPASSERT(pOPortS2_1->m_bIsOutPort);
	BISPASSERT(pOPortS2_1->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS2_1->m_nVirNetNo == 1);
	BISPASSERT(2==pOPortS2_1->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS2_1->m_mapLabelReffrence.size());
	BISPASSERT(2==pOPortS2_1->m_vectPathID.size());
	//影响3-2、2-1两条路径
	BISPASSERT(pPath3to2->m_nPathID == pOPortS2_1->m_vectPathID[1]);
	pPath2to1 = DTSVNMGetPathByID(pOPortS2_1->m_vectPathID[0]);
	BISPASSERT(NULL!=pPath2to1);
//	BISPASSERT(pPath2to1->m_nPathID == pOPortS2_1->m_vectPathID[0]);

	pOPortS2_5 = DTSVNMGetPort(0x02020202,5);
	BISPASSERT(NULL!=pOPortS2_5);
	BISPASSERT(pOPortS2_5->m_bIsOutPort);
	BISPASSERT(pOPortS2_5->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS2_5->m_nVirNetNo != nVirNetNo);
	BISPASSERT((pOPortS2_5->m_nVirNetNo == DTS_VNET_NO_NULL)||(pOPortS2_5->m_nVirNetNo == 2));
//	BISPASSERT(2==pOPortS2_5->m_mapLabelReffrence[pVirNet->m_nVirLabel]);
	BISPASSERT(0==pOPortS2_5->m_mapLabelReffrence.size());
	BISPASSERT(0==pOPortS2_5->m_vectPathID.size());
	//影响3-2、2-1两条路径
//  	BISPASSERT(pPath3to2->m_nPathID == pOPortS2_5->m_vectPathID[1]);
//  	BISPASSERT(pPath2to1->m_nPathID == pOPortS2_5->m_vectPathID[0]);
	
	pTPortS2_3toS1_3 = DTSVNMGetPort(0x02020202,3);
	BISPASSERT(NULL!=pTPortS2_3toS1_3);
	BISPASSERT(!pTPortS2_3toS1_3->m_bIsOutPort);
	BISPASSERT(pTPortS2_3toS1_3->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_3toS1_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(2==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS2_3toS1_3->m_mapLabelReffrence.size());
	BISPASSERT(2==pTPortS2_3toS1_3->m_vectPathID.size());
	//影响3-1、2-1两条路径
	BISPASSERT(pPath3to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[1]);
	BISPASSERT(pPath2to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[0]);
	
	pTPortS2_2toS3_2 = DTSVNMGetPort(0x02020202,2);
	BISPASSERT(NULL!=pTPortS2_2toS3_2);
	BISPASSERT(!pTPortS2_2toS3_2->m_bIsOutPort);
	BISPASSERT(pTPortS2_2toS3_2->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_2toS3_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(2==pTPortS2_2toS3_2->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS2_2toS3_2->m_mapLabelReffrence.size());
	BISPASSERT(2==pTPortS2_2toS3_2->m_vectPathID.size());
	//影响3-2、3-1两条路径
	BISPASSERT(pPath3to2->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[1]);
	BISPASSERT(pPath3to1->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[0]);

	//0x01010101的3个端口
	pOPortS1_1 = DTSVNMGetPort(0x01010101,1);
	BISPASSERT(NULL!=pOPortS1_1);
	BISPASSERT(pOPortS1_1->m_bIsOutPort);
	BISPASSERT(pOPortS1_1->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS1_1->m_nVirNetNo == nVirNetNo);
	BISPASSERT(2==pOPortS1_1->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(2==pOPortS1_1->m_vectPathID.size());
	//影响3-1、2-1两条路径
	BISPASSERT(pPath3to1->m_nPathID == pOPortS1_1->m_vectPathID[1]);
	BISPASSERT(pPath2to1->m_nPathID == pOPortS1_1->m_vectPathID[0]);

	LPDTSVNMPort pOPortS1_4 = DTSVNMGetPort(0x01010101,4);
	BISPASSERT(NULL!=pOPortS1_4);
	BISPASSERT(pOPortS1_4->m_bIsOutPort);
	BISPASSERT(pOPortS1_4->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS1_4->m_nVirNetNo != nVirNetNo);
	BISPASSERT(pOPortS1_4->m_nVirNetNo != DTS_VNET_NO_NULL);
//	BISPASSERT(2==pOPortS1_4->m_mapLabelReffrence[pVirNet->m_nVirLabel]);
	BISPASSERT(0==pOPortS1_4->m_vectPathID.size());
	//影响3-1、2-1两条路径
	BISPASSERT(0 == pOPortS1_4->m_vectPathID.size());

	pTPortS1_3toS2_3 = DTSVNMGetPort(0x01010101,3);
	BISPASSERT(NULL!=pTPortS1_3toS2_3);
	BISPASSERT(!pTPortS1_3toS2_3->m_bIsOutPort);
	BISPASSERT(pTPortS1_3toS2_3->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_3toS2_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(2==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS1_3toS2_3->m_mapLabelReffrence.size());
	BISPASSERT(2==pTPortS1_3toS2_3->m_vectPathID.size());
	//影响3-1、2-1两条路径
	BISPASSERT(pPath3to1->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[1]);
	BISPASSERT(pPath2to1->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[0]);
	
	//-----------------------------------------------------------------------------
	//路径列表
	nPathCountAft = DTSVNMGetPathCount();
	BISPASSERT(2==nPathCountAft-nPathCountBef);

	//pPathCheck3to2
	pPathCheck3to2 = DTSVNMGetPathByID(pPath3to2->m_nPathID);
	BISPASSERT(NULL!=pPathCheck3to2);
	BISPASSERT(pPath3to2->m_nPathID == pPathCheck3to2->m_nPathID);
	BISPASSERT(1==pPath3to2->m_nVirNetNo);
	BISPASSERT(2==pPathCheck3to2->m_nSwitcherCount);
	BISPASSERT(0x03030303==pPathCheck3to2->m_dwBeginSwitcherLoopBack);
	BISPASSERT(1==pPathCheck3to2->m_nBeginPortIndex);
	BISPASSERT(0x02020202==pPathCheck3to2->m_dwEndSwitcherLoopBack);
	BISPASSERT(1==pPathCheck3to2->m_nEndPortIndex);

	BISPASSERT(2000==pPathCheck3to2->m_nBeginOutPWLabel);	
	BISPASSERT(3001==pPathCheck3to2->m_nBeginOutVCLabel);
	BISPASSERT(3000==pPathCheck3to2->m_nBeginInPWLabel);
	BISPASSERT(2001==pPathCheck3to2->m_nBeginInVCLabel);

	BISPASSERT(2==pPathCheck3to2->m_vectLabelSwitch.size());
	BISPASSERT(pPathCheck3to2==pPathCheck3to2->m_vectLabelSwitch[0].m_pPath);
	BISPASSERT(0x03030303==pPathCheck3to2->m_vectLabelSwitch[0].m_dwSwitcherLoopBack);
	BISPASSERT(1==pPathCheck3to2->m_vectLabelSwitch[0].m_nInPortIndex);
	BISPASSERT(2==pPathCheck3to2->m_vectLabelSwitch[0].m_nOutPortIndex);
	BISPASSERT(pPathCheck3to2->m_nBeginOutPWLabel==pPathCheck3to2->m_vectLabelSwitch[0].m_nOutPWLabel);	
	BISPASSERT(pPathCheck3to2->m_nBeginOutVCLabel==pPathCheck3to2->m_vectLabelSwitch[0].m_nOutVCLabel);
	BISPASSERT(pPathCheck3to2->m_nBeginInPWLabel==pPathCheck3to2->m_vectLabelSwitch[0].m_nInPWLabel);
	BISPASSERT(pPathCheck3to2->m_nBeginInVCLabel==pPathCheck3to2->m_vectLabelSwitch[0].m_nInVCLabel);

	BISPASSERT(pPathCheck3to2==pPathCheck3to2->m_vectLabelSwitch[1].m_pPath);
	BISPASSERT(0x02020202==pPathCheck3to2->m_vectLabelSwitch[1].m_dwSwitcherLoopBack);
	BISPASSERT(2==pPathCheck3to2->m_vectLabelSwitch[1].m_nInPortIndex);
	BISPASSERT(1==pPathCheck3to2->m_vectLabelSwitch[1].m_nOutPortIndex);
	BISPASSERT(pPathCheck3to2->m_nBeginInPWLabel==pPathCheck3to2->m_vectLabelSwitch[1].m_nOutPWLabel);	
	BISPASSERT(pPathCheck3to2->m_nBeginInVCLabel==pPathCheck3to2->m_vectLabelSwitch[1].m_nOutVCLabel);
	BISPASSERT(pPathCheck3to2->m_nBeginOutPWLabel==pPathCheck3to2->m_vectLabelSwitch[1].m_nInPWLabel);
	BISPASSERT(pPathCheck3to2->m_nBeginOutVCLabel==pPathCheck3to2->m_vectLabelSwitch[1].m_nInVCLabel);

	//pPathCheck3to1
	pPathCheck3to1 = DTSVNMGetPathByID(pPath3to1->m_nPathID);
	BISPASSERT(NULL!=pPathCheck3to1);
	BISPASSERT(pPath3to1->m_nPathID == pPathCheck3to1->m_nPathID);
	BISPASSERT(1==pPathCheck3to1->m_nVirNetNo);
	BISPASSERT(3==pPathCheck3to1->m_nSwitcherCount);
	BISPASSERT(0x03030303==pPathCheck3to1->m_dwBeginSwitcherLoopBack);
	BISPASSERT(1==pPathCheck3to1->m_nBeginPortIndex);
	BISPASSERT(0x01010101==pPathCheck3to1->m_dwEndSwitcherLoopBack);
	BISPASSERT(1==pPathCheck3to1->m_nEndPortIndex);

	BISPASSERT(1000==pPathCheck3to1->m_nBeginOutPWLabel);	
	BISPASSERT(3001==pPathCheck3to1->m_nBeginOutVCLabel);
	BISPASSERT(3000==pPathCheck3to1->m_nBeginInPWLabel);
	BISPASSERT(1001==pPathCheck3to1->m_nBeginInVCLabel);

	BISPASSERT(3==pPathCheck3to1->m_vectLabelSwitch.size());
	BISPASSERT(pPathCheck3to1==pPathCheck3to1->m_vectLabelSwitch[0].m_pPath);
	BISPASSERT(0x03030303==pPathCheck3to1->m_vectLabelSwitch[0].m_dwSwitcherLoopBack);
	BISPASSERT(1==pPathCheck3to1->m_vectLabelSwitch[0].m_nInPortIndex);
	BISPASSERT(2==pPathCheck3to1->m_vectLabelSwitch[0].m_nOutPortIndex);
	BISPASSERT(pPathCheck3to1->m_nBeginOutPWLabel==pPathCheck3to1->m_vectLabelSwitch[0].m_nOutPWLabel);
	BISPASSERT(pPathCheck3to1->m_nBeginOutVCLabel==pPathCheck3to1->m_vectLabelSwitch[0].m_nOutVCLabel);
	BISPASSERT(pPathCheck3to1->m_nBeginInPWLabel==pPathCheck3to1->m_vectLabelSwitch[0].m_nInPWLabel);
	BISPASSERT(pPathCheck3to1->m_nBeginInVCLabel==pPathCheck3to1->m_vectLabelSwitch[0].m_nInVCLabel);

	BISPASSERT(pPathCheck3to1==pPathCheck3to1->m_vectLabelSwitch[1].m_pPath);
	BISPASSERT(0x02020202==pPathCheck3to1->m_vectLabelSwitch[1].m_dwSwitcherLoopBack);
	BISPASSERT(2==pPathCheck3to1->m_vectLabelSwitch[1].m_nInPortIndex);
	BISPASSERT(3==pPathCheck3to1->m_vectLabelSwitch[1].m_nOutPortIndex);
	BISPASSERT(pPathCheck3to1->m_nBeginInPWLabel==pPathCheck3to1->m_vectLabelSwitch[1].m_nOutPWLabel);
	BISPASSERT(pPathCheck3to1->m_nBeginInVCLabel==pPathCheck3to1->m_vectLabelSwitch[1].m_nOutVCLabel);
	BISPASSERT(pPathCheck3to1->m_nBeginOutPWLabel==pPathCheck3to1->m_vectLabelSwitch[1].m_nInPWLabel);
	BISPASSERT(pPathCheck3to1->m_nBeginOutVCLabel==pPathCheck3to1->m_vectLabelSwitch[1].m_nInVCLabel);

	BISPASSERT(pPathCheck3to1==pPathCheck3to1->m_vectLabelSwitch[2].m_pPath);
	BISPASSERT(0x01010101==pPathCheck3to1->m_vectLabelSwitch[2].m_dwSwitcherLoopBack);
	BISPASSERT(3==pPathCheck3to1->m_vectLabelSwitch[2].m_nInPortIndex);
	BISPASSERT(1==pPathCheck3to1->m_vectLabelSwitch[2].m_nOutPortIndex);
	BISPASSERT(pPathCheck3to1->m_nBeginInPWLabel==pPathCheck3to1->m_vectLabelSwitch[2].m_nOutPWLabel);
	BISPASSERT(pPathCheck3to1->m_nBeginInVCLabel==pPathCheck3to1->m_vectLabelSwitch[2].m_nOutVCLabel);
	BISPASSERT(pPathCheck3to1->m_nBeginOutPWLabel==pPathCheck3to1->m_vectLabelSwitch[2].m_nInPWLabel);
	BISPASSERT(pPathCheck3to1->m_nBeginOutVCLabel==pPathCheck3to1->m_vectLabelSwitch[2].m_nInVCLabel);

	//pPathCheck2to1
	pPathCheck2to1 = DTSVNMGetPathByID(pPath2to1->m_nPathID);	
	BISPASSERT(NULL!=pPathCheck2to1);
	BISPASSERT(pPath2to1->m_nPathID == pPathCheck2to1->m_nPathID);
	BISPASSERT(1==pPathCheck2to1->m_nVirNetNo);
	BISPASSERT(2==pPathCheck2to1->m_nSwitcherCount);
	BISPASSERT(0x02020202==pPathCheck2to1->m_dwBeginSwitcherLoopBack);
	BISPASSERT(1==pPathCheck2to1->m_nBeginPortIndex);
	BISPASSERT(0x01010101==pPathCheck2to1->m_dwEndSwitcherLoopBack);
	BISPASSERT(1==pPathCheck2to1->m_nEndPortIndex);
	BISPASSERT(1000==pPathCheck2to1->m_nBeginOutPWLabel);	
	BISPASSERT(2001==pPathCheck2to1->m_nBeginOutVCLabel);
	BISPASSERT(2000==pPathCheck2to1->m_nBeginInPWLabel);
	BISPASSERT(1001==pPathCheck2to1->m_nBeginInVCLabel);

	BISPASSERT(2==pPathCheck2to1->m_vectLabelSwitch.size());
	BISPASSERT(pPathCheck2to1==pPathCheck2to1->m_vectLabelSwitch[0].m_pPath);
	BISPASSERT(0x02020202==pPathCheck2to1->m_vectLabelSwitch[0].m_dwSwitcherLoopBack);
	BISPASSERT(1==pPathCheck2to1->m_vectLabelSwitch[0].m_nInPortIndex);
	BISPASSERT(3==pPathCheck2to1->m_vectLabelSwitch[0].m_nOutPortIndex);
	BISPASSERT(pPathCheck2to1->m_nBeginOutPWLabel==pPathCheck2to1->m_vectLabelSwitch[0].m_nOutPWLabel);
	BISPASSERT(pPathCheck2to1->m_nBeginOutVCLabel==pPathCheck2to1->m_vectLabelSwitch[0].m_nOutVCLabel);
	BISPASSERT(pPathCheck2to1->m_nBeginInPWLabel==pPathCheck2to1->m_vectLabelSwitch[0].m_nInPWLabel);
	BISPASSERT(pPathCheck2to1->m_nBeginInVCLabel==pPathCheck2to1->m_vectLabelSwitch[0].m_nInVCLabel);

	BISPASSERT(pPathCheck2to1==pPathCheck2to1->m_vectLabelSwitch[1].m_pPath);
	BISPASSERT(0x01010101==pPathCheck2to1->m_vectLabelSwitch[1].m_dwSwitcherLoopBack);
	BISPASSERT(3==pPathCheck2to1->m_vectLabelSwitch[1].m_nInPortIndex);
	BISPASSERT(1==pPathCheck2to1->m_vectLabelSwitch[1].m_nOutPortIndex);
	BISPASSERT(pPathCheck2to1->m_nBeginInPWLabel==pPathCheck2to1->m_vectLabelSwitch[1].m_nOutPWLabel);
	BISPASSERT(pPathCheck2to1->m_nBeginInVCLabel==pPathCheck2to1->m_vectLabelSwitch[1].m_nOutVCLabel);
	BISPASSERT(pPathCheck2to1->m_nBeginOutPWLabel==pPathCheck2to1->m_vectLabelSwitch[1].m_nInPWLabel);
	BISPASSERT(pPathCheck2to1->m_nBeginOutVCLabel==pPathCheck2to1->m_vectLabelSwitch[1].m_nInVCLabel);

	//----------------------------------------------------------------------------
	//外部调用
	//外部调用2次路径计算，发送3--2出口加入2*1次，中继加入2*2次，发送3--1中继加入2*2次，交换2*2次
	BISPASSERT(16==nLogcountaft-nLogcountBef);
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-16);
	BISPASSERT(VNM_FUNC_CALL_TOPO_CONCULATE==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(pCallParam->m_nBandWidth == pVirNet->m_nBandWidth);
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(0x01010101==pCallParam->m_dwOppoLoopBack);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-15);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(1==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-14);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND==pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-13);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);
	BISPASSERT(DTS_VNM_TEST_MAC_0_IN == pCallParam->m_arrbOppoMac[0]);
	BISPASSERT(2 == pCallParam->m_arrbOppoMac[1]);
	BISPASSERT(0x02 == pCallParam->m_arrbOppoMac[2]);
	BISPASSERT(0x02 == pCallParam->m_arrbOppoMac[3]);
	BISPASSERT(0x02 == pCallParam->m_arrbOppoMac[4]);
	BISPASSERT(0x02 == pCallParam->m_arrbOppoMac[5]);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-12);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND==pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-11);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(3==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_ADD==pCallParam->m_nFTAction);
//  	BISPASSERT(pCallParam->m_arrbOutOppoMac[DTS_VNM_MAC_LEN];
//  	BISPASSERT(pCallParam->m_arrbOutOppoMac[DTS_VNM_MAC_LEN];
	//有效
	BISPASSERT(1000==pCallParam->m_nInPWLabel);
	BISPASSERT(3001==pCallParam->m_nInVCLabel);

	BISPASSERT(3000==pCallParam->m_nOutPWLabel);	
	BISPASSERT(1001==pCallParam->m_nOutVCLabel);
	BISPASSERT(DTS_VNM_TEST_MAC_0_IN == pCallParam->m_arrbOppoMac[0]);
	BISPASSERT(3 == pCallParam->m_arrbOppoMac[1]);
	BISPASSERT(0x01 == pCallParam->m_arrbOppoMac[2]);
	BISPASSERT(0x01 == pCallParam->m_arrbOppoMac[3]);
	BISPASSERT(0x01 == pCallParam->m_arrbOppoMac[4]);
	BISPASSERT(0x01 == pCallParam->m_arrbOppoMac[5]);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-10);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND==pLogEntry->nMainType);
//	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pDataBuff;
	ofp_flow_mod* pOPFFlowModMsg = (ofp_flow_mod*)pLogEntry->pSendDataBuff;
	BISPASSERT(pLogEntry->nSendDataBuffLen == ntohs(pOPFFlowModMsg->header.wLength));
	BISPASSERT(OFP13_VERSION == pOPFFlowModMsg->header.bVersion);
	BISPASSERT(OFPT_FLOW_MOD == pOPFFlowModMsg->header.bType);
	BISPASSERT(0!=pOPFFlowModMsg->header.dwXid);		
	BISPASSERT(pPathCheck3to1->m_n64Cookie==pOPFFlowModMsg->n64Cookie);
	BISPASSERT(0==pOPFFlowModMsg->n64Cookie_mask);
	BISPASSERT(0==pOPFFlowModMsg->bTable_id);
	BISPASSERT(OFPFC_ADD==pOPFFlowModMsg->bCommand);
	BISPASSERT(OFP_FLOW_PERMANENT==ntohs(pOPFFlowModMsg->wIdle_timeout));
	BISPASSERT(OFP_FLOW_PERMANENT==ntohs(pOPFFlowModMsg->wHard_timeout));
	BISPASSERT(OFP_DEFAULT_PRIORITY==ntohs(pOPFFlowModMsg->wPriority));
	BISPASSERT(OFP_NO_BUFFER==ntohl(pOPFFlowModMsg->dwBuffer_id));
	BISPASSERT(3==ntohl(pOPFFlowModMsg->dwOut_port));
	BISPASSERT(OFPG_ANY==ntohl(pOPFFlowModMsg->dwOut_group));
	BISPASSERT(0==pOPFFlowModMsg->wFlags);
	BISPASSERT(OFPMT_OXM == ntohs(pOPFFlowModMsg->match.wType));
	BISPASSERT(0x0014 == ntohs(pOPFFlowModMsg->match.wLength));
	BYTE* pbByte = &pOPFFlowModMsg->match.pad[0];
	BISPASSERT(0x80 == *pbByte);
	BISPASSERT(0x00 == *(pbByte+1));
	BISPASSERT(0x44 == *(pbByte+2));
	BISPASSERT(0x04 == *(pbByte+3));
	BISPASSERT(0x00 == *(pbByte+4));
	BISPASSERT(0x00 == *(pbByte+5));
	BISPASSERT(0x03 == *(pbByte+6));
	BISPASSERT(0xE8 == *(pbByte+7));

	BISPASSERT(0x80 == *(pbByte+8));
	BISPASSERT(0x00 == *(pbByte+9));
	BISPASSERT(0x44 == *(pbByte+10));
	BISPASSERT(0x04 == *(pbByte+11));
	BISPASSERT(0x00 == *(pbByte+12));
	BISPASSERT(0x00 == *(pbByte+13));
	BISPASSERT(0x0B == *(pbByte+14));
	BISPASSERT(0xB9 == *(pbByte+15));

	ofp_instruction_actions* pInstruction = (ofp_instruction_actions*)(pbByte+20);
	BISPASSERT(OFPIT_APPLY_ACTIONS == ntohs(pInstruction->wType));
	BISPASSERT(8+16 + sizeof(ofp_action_output) == ntohs(pInstruction->wLen));
	pbByte = &pInstruction->pad[3];
	ofp_action_output* pOutPutAction = (ofp_action_output*)(pbByte+1);
	BISPASSERT(OFPAT_OUTPUT == ntohs(pOutPutAction->wType));
	BISPASSERT(sizeof(ofp_action_output) == ntohs(pOutPutAction->wLen));
	BISPASSERT(3==ntohl(pOutPutAction->dwPort));
	BISPASSERT(OFPCML_NO_BUFFER == ntohs(pOutPutAction->wMax_len));

	ofp_action_set_field* pSetFieldAction = (ofp_action_set_field*)(pOutPutAction+1);
	BISPASSERT(OFPAT_SET_FIELD == ntohs(pSetFieldAction->wType));
	BISPASSERT(16 == ntohs(pSetFieldAction->wLen));
	pbByte = &pSetFieldAction->field[0];
	BISPASSERT(0x80 == *pbByte);
	BISPASSERT(0x00 == *(pbByte+1));
	BISPASSERT(0x06 == *(pbByte+2));
	BISPASSERT(0x06 == *(pbByte+3));
	BISPASSERT(DTS_VNM_TEST_MAC_0_IN == *(pbByte+4));
	BISPASSERT(3 == *(pbByte+5));
	BISPASSERT(0x1 == *(pbByte+6));
	BISPASSERT(0x1 == *(pbByte+7));
	BISPASSERT(0x1 == *(pbByte+8));
	BISPASSERT(0x1 == *(pbByte+9));

	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-9);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(2==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_ADD==pCallParam->m_nFTAction);
	//  	BISPASSERT(pCallParam->m_arrbOutOppoMac[DTS_VNM_MAC_LEN];
	//  	BISPASSERT(pCallParam->m_arrbOutOppoMac[DTS_VNM_MAC_LEN];
	//有效
	BISPASSERT(3000==pCallParam->m_nInPWLabel);
	BISPASSERT(1001==pCallParam->m_nInVCLabel);

	BISPASSERT(1000==pCallParam->m_nOutPWLabel);	
	BISPASSERT(3001==pCallParam->m_nOutVCLabel);
	BISPASSERT(DTS_VNM_TEST_MAC_0_OUT == pCallParam->m_arrbOppoMac[0]);
	BISPASSERT(2 == pCallParam->m_arrbOppoMac[1]);
	BISPASSERT(0x03 == pCallParam->m_arrbOppoMac[2]);
	BISPASSERT(0x03 == pCallParam->m_arrbOppoMac[3]);
	BISPASSERT(0x03 == pCallParam->m_arrbOppoMac[4]);
	BISPASSERT(0x03 == pCallParam->m_arrbOppoMac[5]);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-8);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	pOPFFlowModMsg = (ofp_flow_mod*)pLogEntry->pSendDataBuff;
	BISPASSERT(pLogEntry->nSendDataBuffLen == ntohs(pOPFFlowModMsg->header.wLength));
	BISPASSERT(OFP13_VERSION == pOPFFlowModMsg->header.bVersion);
	BISPASSERT(OFPT_FLOW_MOD == pOPFFlowModMsg->header.bType);
	BISPASSERT(0!=pOPFFlowModMsg->header.dwXid);		
	BISPASSERT(pPathCheck3to1->m_n64Cookie==pOPFFlowModMsg->n64Cookie);
	BISPASSERT(0==pOPFFlowModMsg->n64Cookie_mask);
	BISPASSERT(0==pOPFFlowModMsg->bTable_id);
	BISPASSERT(OFPFC_ADD==pOPFFlowModMsg->bCommand);
	BISPASSERT(OFP_FLOW_PERMANENT==ntohs(pOPFFlowModMsg->wIdle_timeout));
	BISPASSERT(OFP_FLOW_PERMANENT==ntohs(pOPFFlowModMsg->wHard_timeout));
	BISPASSERT(OFP_DEFAULT_PRIORITY==ntohs(pOPFFlowModMsg->wPriority));
	BISPASSERT(OFP_NO_BUFFER==ntohl(pOPFFlowModMsg->dwBuffer_id));
	BISPASSERT(2==ntohl(pOPFFlowModMsg->dwOut_port));
	BISPASSERT(OFPG_ANY==ntohl(pOPFFlowModMsg->dwOut_group));
	BISPASSERT(0==pOPFFlowModMsg->wFlags);
	BISPASSERT(OFPMT_OXM == ntohs(pOPFFlowModMsg->match.wType));
	BISPASSERT(0x0014 == ntohs(pOPFFlowModMsg->match.wLength));
	pbByte = &pOPFFlowModMsg->match.pad[0];
	BISPASSERT(0x80 == *pbByte);
	BISPASSERT(0x00 == *(pbByte+1));
	BISPASSERT(0x44 == *(pbByte+2));
	BISPASSERT(0x04 == *(pbByte+3));
	BISPASSERT(0x00 == *(pbByte+4));
	BISPASSERT(0x00 == *(pbByte+5));
	BISPASSERT(0x0B == *(pbByte+6));
	BISPASSERT(0xB8 == *(pbByte+7));

	BISPASSERT(0x80 == *(pbByte+8));
	BISPASSERT(0x00 == *(pbByte+9));
	BISPASSERT(0x44 == *(pbByte+10));
	BISPASSERT(0x04 == *(pbByte+11));
	BISPASSERT(0x00 == *(pbByte+12));
	BISPASSERT(0x00 == *(pbByte+13));
	BISPASSERT(0x03 == *(pbByte+14));
	BISPASSERT(0xE9 == *(pbByte+15));

	pInstruction = (ofp_instruction_actions*)(pbByte+20);
	BISPASSERT(OFPIT_APPLY_ACTIONS == ntohs(pInstruction->wType));
	BISPASSERT(8+16 + sizeof(ofp_action_output) == ntohs(pInstruction->wLen));
	pbByte = &pInstruction->pad[3];
	pOutPutAction = (ofp_action_output*)(pbByte+1);
	BISPASSERT(OFPAT_OUTPUT == ntohs(pOutPutAction->wType));
	BISPASSERT(sizeof(ofp_action_output) == ntohs(pOutPutAction->wLen));
	BISPASSERT(2==ntohl(pOutPutAction->dwPort));
	BISPASSERT(OFPCML_NO_BUFFER == ntohs(pOutPutAction->wMax_len));

	pSetFieldAction = (ofp_action_set_field*)(pOutPutAction+1);
	BISPASSERT(OFPAT_SET_FIELD == ntohs(pSetFieldAction->wType));
	BISPASSERT(16 == ntohs(pSetFieldAction->wLen));
	pbByte = &pSetFieldAction->field[0];
	BISPASSERT(0x80 == *pbByte);
	BISPASSERT(0x00 == *(pbByte+1));
	BISPASSERT(0x06 == *(pbByte+2));
	BISPASSERT(0x06 == *(pbByte+3));
	BISPASSERT(DTS_VNM_TEST_MAC_0_OUT == *(pbByte+4));
	BISPASSERT(2 == *(pbByte+5));
	BISPASSERT(0x03 == *(pbByte+6));
	BISPASSERT(0x03 == *(pbByte+7));
	BISPASSERT(0x03 == *(pbByte+8));
	BISPASSERT(0x03 == *(pbByte+9));
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-7);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);
	BISPASSERT(DTS_VNM_TEST_MAC_0_OUT == pCallParam->m_arrbOppoMac[0]);
	BISPASSERT(3 == pCallParam->m_arrbOppoMac[1]);
	BISPASSERT(0x02 == pCallParam->m_arrbOppoMac[2]);
	BISPASSERT(0x02 == pCallParam->m_arrbOppoMac[3]);
	BISPASSERT(0x02 == pCallParam->m_arrbOppoMac[4]);
	BISPASSERT(0x02 == pCallParam->m_arrbOppoMac[5]);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-6);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND==pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-5);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(VNM_FUNC_CALL_TOPO_CONCULATE==pLogEntry->nMainType);
	BISPASSERT(pCallParam->m_nBandWidth == pVirNet->m_nBandWidth);
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(0x02020202==pCallParam->m_dwOppoLoopBack);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-4);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);
	BISPASSERT(DTS_VNM_TEST_MAC_0_IN == pCallParam->m_arrbOppoMac[0]);
	BISPASSERT(2 == pCallParam->m_arrbOppoMac[1]);
	BISPASSERT(0x02 == pCallParam->m_arrbOppoMac[2]);
	BISPASSERT(0x02 == pCallParam->m_arrbOppoMac[3]);
	BISPASSERT(0x02 == pCallParam->m_arrbOppoMac[4]);
	BISPASSERT(0x02 == pCallParam->m_arrbOppoMac[5]);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-3);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND==pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-2);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);
	BISPASSERT(DTS_VNM_TEST_MAC_0_OUT == pCallParam->m_arrbOppoMac[0]);
	BISPASSERT(2 == pCallParam->m_arrbOppoMac[1]);
	BISPASSERT(0x03 == pCallParam->m_arrbOppoMac[2]);
	BISPASSERT(0x03 == pCallParam->m_arrbOppoMac[3]);
	BISPASSERT(0x03 == pCallParam->m_arrbOppoMac[4]);
	BISPASSERT(0x03 == pCallParam->m_arrbOppoMac[5]);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND==pLogEntry->nMainType);


	return 0;
}


int DTSVNMTestVNetStatsOneOutPortAddPortTest1()
{
	int nLogcountBef,nLogcountaft,nRet;
	//	int	nSwitcherCount;
	int	nSwitcherCountBef,nSwitcherCountAft;
	int	nPathCountBef,nPathCountAft;
	int	nSwitcherPortCountBef,nSwitcherPortCountAft;
	int nLabelRefBef,nLabelRefAft;

	int nOppoLabelRefBef,nOppoLabelRefAft;
	int	nOppoSwitcherPortCountBef,nOppoSwitcherPortCountAft;
	int	nVNetPortCountBef,nVNetPortCountAft;
	LPDTSVirtualNet pVirNet = NULL;
	LPDTSVNMSwitcher pSwitcher = NULL;
	LPDTSVNMSwitcher pOppoSwitcher = NULL;
	LPDTSVNMPort pOutPort = NULL;
	LPDTSVNMPort pTrunkPort = NULL;
	LPDTSVNMPort pOppoOutPort = NULL;
	LPDTSVNMPort pOppoTrunkPort = NULL;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	//	LPDTSVNMPath pPath = NULL;
	LPDTSVNMPath pPathCheck = NULL;
	char szTmp[32]="";
	BOOL bIsMatch = FALSE;
	UINT nPathID = 0;

	//[1]0x02020202[3]--[3]0x01010101[1]
	UINT nVirNetNo = 1;
	UINT nSwitcherLoopBack = 0x02020202;
	UINT nSwithcerOutPort = 1;
	UINT nTrunkPort = 3;
	UINT nOppoSwitcherLB = 0x01010101;
	UINT nOppoTrunkPort = 3;
	UINT nOppoOutPort = 1;

	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	nSwitcherCountBef = DTSVNMGetSwitcherCount();
	nPathCountBef = DTSVNMGetPathCount();
	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
	nSwitcherPortCountBef = (NULL==pSwitcher)?0:pSwitcher->m_mapPort.size();
	pTrunkPort = DTSVNMGetPort(nSwitcherLoopBack,nTrunkPort);
	nLabelRefBef = 0;
	pOppoSwitcher = DTSVNMGetSwitcherByLoopBack(nOppoSwitcherLB);
	nOppoSwitcherPortCountBef = (NULL==pOppoSwitcher)?0:pOppoSwitcher->m_mapPort.size();
	nVNetPortCountBef = (NULL==nVirNetNo)?0:pVirNet->m_mapOutPort.size();
	pOppoTrunkPort = DTSVNMGetPort(nSwitcherLoopBack,nTrunkPort);
	nOppoLabelRefBef = 0;

	//////////////////////////////////////////////////////////////////////////
	//执行
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMAddOutPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	BISPASSERT(VNM_RESULT_OK==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	// 	pPath = DTSVNMTestGetPath(nSwitcherLoopBack,nOppoSwitcherLB);
	// 	BISPASSERT(NULL!=pPath);

	//----------------------------------------------------------------------------
	//虚网列表
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(pVirNet->m_nVirNetNo==nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==nVirNetNo);
	BISPASSERT(DTS_VNM_STATS_ACTIVE == pVirNet->m_nStats);
	nVNetPortCountAft=pVirNet->m_mapOutPort.size();
	BISPASSERT(1==nVNetPortCountAft-nVNetPortCountBef);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),nVirNetNo);
	BISPASSERT(0==strcmp(szTmp,pVirNet->m_szVirNetName));
	BISPASSERT(1==pVirNet->m_mapOutPort.count(nSwitcherLoopBack));
	BISPASSERT(1==pVirNet->m_mapOutPort.count(nOppoSwitcherLB));
	BISPASSERT(nSwithcerOutPort==pVirNet->m_mapOutPort[nSwitcherLoopBack]);
	BISPASSERT(nOppoOutPort==pVirNet->m_mapOutPort[nOppoSwitcherLB]);

	//-----------------------------------------------------------------------------
	//交换机列表
	nSwitcherCountAft = DTSVNMGetSwitcherCount();
	BISPASSERT(0==nSwitcherCountAft-nSwitcherCountBef);

	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
	pOppoSwitcher = DTSVNMGetSwitcherByLoopBack(nOppoSwitcherLB);
	pOutPort = DTSVNMGetPort(nSwitcherLoopBack,nSwithcerOutPort);
	pTrunkPort = DTSVNMGetPort(nSwitcherLoopBack,nTrunkPort);
	pOppoOutPort = DTSVNMGetPort(nOppoSwitcherLB,nSwithcerOutPort);
	pOppoTrunkPort = DTSVNMGetPort(nOppoSwitcherLB,nTrunkPort);
	BISPASSERT(NULL!=pSwitcher);
	BISPASSERT(NULL!=pOppoSwitcher);
	BISPASSERT(NULL!=pOutPort);
	BISPASSERT(NULL!=pTrunkPort);
	BISPASSERT(NULL!=pOppoOutPort);
	BISPASSERT(NULL!=pOppoTrunkPort);

	//Switcher，增加一个出口和一个中继口
	BISPASSERT(pSwitcher->m_dwSwitcherLoopBack == nSwitcherLoopBack);
	nSwitcherPortCountAft = pSwitcher->m_mapPort.size();
//	BISPASSERT(2==nSwitcherPortCountAft-nSwitcherPortCountBef);

	//OppoSwitcher，增加一个中继口
	BISPASSERT(pOppoSwitcher->m_dwSwitcherLoopBack == nOppoSwitcherLB);
	nOppoSwitcherPortCountAft = pOppoSwitcher->m_mapPort.size();
//	BISPASSERT(1==nOppoSwitcherPortCountAft-nOppoSwitcherPortCountBef);

	//出口
	BISPASSERT(NULL!=pOutPort);
	//端口号
	BISPASSERT(nSwithcerOutPort==pOutPort->m_nPortIndex);
	//所属交换机
	BISPASSERT(nSwitcherLoopBack==pOutPort->m_dwSwitcherLoopBack);
	//是否为出口
	BISPASSERT(pOutPort->m_bIsOutPort);
	//如果是出口，则记录所属虚网
	BISPASSERT(nVirNetNo==pOutPort->m_nVirNetNo);	
	//对端端口
	//	BISPASSERT(NULL==pOutPort->m_pOppoPort);
	//涉及的路径PathID列表
	BISPASSERT(1==pOutPort->m_vectPathID.size());
	nPathID = pOutPort->m_vectPathID[0];
	// 	bIsMatch = FALSE;
	// 	for (nRet=0;(UINT)nRet < pOutPort->m_vectPathID.size();nRet++)
	// 	{
	// 		if (pOutPort->m_vectPathID[nRet] == pPath->m_nPathID)
	// 		{
	// 			bIsMatch = TRUE;
	// 		}		
	// 	}
	// 	BISPASSERT(bIsMatch);
	//标签引用计数
	BISPASSERT(1==pOutPort->m_mapLabelReffrence.size());
	BISPASSERT(1==pOutPort->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);

	//中继端口
	BISPASSERT(NULL!=pTrunkPort);
	//端口号
	BISPASSERT(nTrunkPort==pTrunkPort->m_nPortIndex);
	//所属交换机
	BISPASSERT(nSwitcherLoopBack==pTrunkPort->m_dwSwitcherLoopBack);
	//是否为出口
	BISPASSERT(!pTrunkPort->m_bIsOutPort);
	//如果是出口，则记录所属虚网
	BISPASSERT(DTS_VNET_NO_NULL==pTrunkPort->m_nVirNetNo);	
	//对端端口
	//	BISPASSERT(NULL==pTrunkPort->m_pOppoPort);
	//涉及的路径PathID列表
	BISPASSERT(1==pTrunkPort->m_vectPathID.size());
	BISPASSERT(nPathID == pTrunkPort->m_vectPathID[0]);
	// 	bIsMatch = FALSE;
	// 	for (nRet=0;(UINT)nRet < pTrunkPort->m_vectPathID.size();nRet++)
	// 	{
	// 		if (pTrunkPort->m_vectPathID[nRet] == pPath->m_nPathID)
	// 		{
	// 			bIsMatch = TRUE;
	// 		}		
	// 	}
	// 	BISPASSERT(bIsMatch);
	//标签引用计数
	BISPASSERT(0!=pTrunkPort->m_mapLabelReffrence.size());
	nLabelRefAft = pTrunkPort->m_mapLabelReffrence[pVirNet->m_nVirLabelRef];
	BISPASSERT(1==nLabelRefAft-nLabelRefBef);

	//对端出口
	BISPASSERT(NULL!=pOppoOutPort);
	//端口号
	BISPASSERT(nOppoOutPort==pOppoOutPort->m_nPortIndex);
	//所属交换机
	BISPASSERT(nOppoSwitcherLB==pOppoOutPort->m_dwSwitcherLoopBack);
	//是否为出口
	BISPASSERT(pOppoOutPort->m_bIsOutPort);
	//如果是出口，则记录所属虚网
	BISPASSERT(nVirNetNo==pOppoOutPort->m_nVirNetNo);	
	//对端端口
	//	BISPASSERT(NULL==pOppoOutPort->m_pOppoPort);
	//涉及的路径PathID列表
	BISPASSERT(1==pOppoOutPort->m_vectPathID.size());

	// 	bIsMatch = FALSE;
	// 	for (nRet=0;(UINT)nRet < pOutPort->m_vectPathID.size();nRet++)
	// 	{
	// 		if (pOutPort->m_vectPathID[nRet] == pPath->m_nPathID)
	// 		{
	// 			bIsMatch = TRUE;
	// 		}		
	// 	}
	// 	BISPASSERT(bIsMatch);
	//标签引用计数
	BISPASSERT(0!=pOppoOutPort->m_mapLabelReffrence.size());
	BISPASSERT(0<pOppoOutPort->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]);

	//对端中继端口
	BISPASSERT(NULL!=pOppoTrunkPort);
	//端口号
	BISPASSERT(nOppoTrunkPort==pOppoTrunkPort->m_nPortIndex);
	//所属交换机
	BISPASSERT(nOppoSwitcherLB==pOppoTrunkPort->m_dwSwitcherLoopBack);
	//是否为出口
	BISPASSERT(!pOppoTrunkPort->m_bIsOutPort);
	//如果是出口，则记录所属虚网
	BISPASSERT(DTS_VNET_NO_NULL==pOppoTrunkPort->m_nVirNetNo);	
	//对端端口
	//	BISPASSERT(pTrunkPort==pOppoTrunkPort->m_pOppoPort);
	//涉及的路径PathID列表
	BISPASSERT(1==pOppoTrunkPort->m_vectPathID.size());
	BISPASSERT(nPathID == pOppoTrunkPort->m_vectPathID[0]);
	// 	bIsMatch = FALSE;
	// 	for (nRet=0;(UINT)nRet < pOppoTrunkPort->m_vectPathID.size();nRet++)
	// 	{
	// 		if (pOppoTrunkPort->m_vectPathID[nRet] == pPath->m_nPathID)
	// 		{
	// 			bIsMatch = TRUE;
	// 		}		
	// 	}
	// 	BISPASSERT(bIsMatch);
	//标签引用计数
	BISPASSERT(0!=pOppoTrunkPort->m_mapLabelReffrence.size());
	nOppoLabelRefAft = pOppoTrunkPort->m_mapLabelReffrence[pVirNet->m_nVirLabelRef];
	BISPASSERT(1==nOppoLabelRefAft-nOppoLabelRefBef);

	//-----------------------------------------------------------------------------
	//路径列表
	nPathCountAft = DTSVNMGetPathCount();
	BISPASSERT(1==nPathCountAft-nPathCountBef);

	//[1]0x02020202[3]--[3]0x01010101[1]
	pPathCheck = DTSVNMGetPathByID(pOppoTrunkPort->m_vectPathID[0]);
	BISPASSERT(NULL!=pPathCheck);
	//	vector<DTSVNMLabelSwitch>	m_vectLabelSwitch;
	BISPASSERT(nPathID == pPathCheck->m_nPathID);
	BISPASSERT(nVirNetNo == pPathCheck->m_nVirNetNo);
	BISPASSERT(2 == pPathCheck->m_nSwitcherCount);
	BISPASSERT(nSwitcherLoopBack == pPathCheck->m_dwBeginSwitcherLoopBack);
	BISPASSERT(nSwithcerOutPort == pPathCheck->m_nBeginPortIndex);
	BISPASSERT(nOppoSwitcherLB == pPathCheck->m_dwEndSwitcherLoopBack);
	BISPASSERT(nOppoOutPort == pPathCheck->m_nEndPortIndex);
	BISPASSERT(0!=pPathCheck->m_n64Cookie);

	BISPASSERT(1000==pPathCheck->m_nBeginOutPWLabel);	
	BISPASSERT(2001==pPathCheck->m_nBeginOutVCLabel);
	BISPASSERT(2000==pPathCheck->m_nBeginInPWLabel);
	BISPASSERT(1001==pPathCheck->m_nBeginInVCLabel);

	BISPASSERT(2 == pPathCheck->m_vectLabelSwitch.size());
	BISPASSERT(nSwitcherLoopBack == pPathCheck->m_vectLabelSwitch[0].m_dwSwitcherLoopBack);
	BISPASSERT(pPathCheck == pPathCheck->m_vectLabelSwitch[0].m_pPath);
	BISPASSERT(nSwithcerOutPort == pPathCheck->m_vectLabelSwitch[0].m_nInPortIndex);
	BISPASSERT(nTrunkPort == pPathCheck->m_vectLabelSwitch[0].m_nOutPortIndex);
	BISPASSERT(pPathCheck->m_nBeginInPWLabel == pPathCheck->m_vectLabelSwitch[0].m_nInPWLabel);
	BISPASSERT(pPathCheck->m_nBeginInVCLabel == pPathCheck->m_vectLabelSwitch[0].m_nInVCLabel);
	BISPASSERT(pPathCheck->m_nBeginOutPWLabel == pPathCheck->m_vectLabelSwitch[0].m_nOutPWLabel);
	BISPASSERT(pPathCheck->m_nBeginOutVCLabel == pPathCheck->m_vectLabelSwitch[0].m_nOutVCLabel);

	BISPASSERT(nOppoSwitcherLB == pPathCheck->m_vectLabelSwitch[1].m_dwSwitcherLoopBack);
	BISPASSERT(pPathCheck == pPathCheck->m_vectLabelSwitch[1].m_pPath);
	BISPASSERT(nOppoTrunkPort == pPathCheck->m_vectLabelSwitch[1].m_nInPortIndex);
	BISPASSERT(nOppoOutPort == pPathCheck->m_vectLabelSwitch[1].m_nOutPortIndex);
	BISPASSERT(pPathCheck->m_nBeginInPWLabel == pPathCheck->m_vectLabelSwitch[1].m_nOutPWLabel);
	BISPASSERT(pPathCheck->m_nBeginInVCLabel == pPathCheck->m_vectLabelSwitch[1].m_nOutVCLabel);
	BISPASSERT(pPathCheck->m_nBeginOutPWLabel == pPathCheck->m_vectLabelSwitch[1].m_nInPWLabel);
	BISPASSERT(pPathCheck->m_nBeginOutVCLabel == pPathCheck->m_vectLabelSwitch[1].m_nInVCLabel);

	//----------------------------------------------------------------------------
	//外部调用
	//外部调用一次路径计算，中继端口加入虚网2次，出口交换2次
	BISPASSERT(9==nLogcountaft-nLogcountBef);
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-9);
	BISPASSERT(VNM_FUNC_CALL_TOPO_CONCULATE==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pCallParam->m_nBandWidth == pVirNet->m_nBandWidth);
	BISPASSERT(nSwitcherLoopBack==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(nOppoSwitcherLB==pCallParam->m_dwOppoLoopBack);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-8);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(nSwitcherLoopBack==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(nSwithcerOutPort==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_ADD==pCallParam->m_nFTAction);

	//发送FT
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-7);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
	BISPASSERT(pLogEntry->nSendDataBuffLen == sizeof(ofp_pri_vpls_vc));
	ofp_pri_vpls_vc* pOPFVCMsg = (ofp_pri_vpls_vc*)pLogEntry->pSendDataBuff;
	BISPASSERT(OFP13_VERSION == pOPFVCMsg->h.ofp_h.bVersion);
	BISPASSERT(OFPT_SET_PRIVATE == pOPFVCMsg->h.ofp_h.bType);
	BISPASSERT(sizeof(ofp_pri_vpls_vc) == ntohs(pOPFVCMsg->h.ofp_h.wLength));
	BISPASSERT(0!=pOPFVCMsg->h.ofp_h.dwXid);		
	BISPASSERT(OFP_PRI_TYPE_VPLS == pOPFVCMsg->h.bType);
	BISPASSERT(OFP_PRI_SUBTYPE_VPLS_VC == pOPFVCMsg->h.bSubtype);
	BISPASSERT(OFP_PRI_OPT_INSERT == pOPFVCMsg->h.bOpt);
	BISPASSERT(0==strcmp(pOPFVCMsg->alias,pVirNet->m_szVirNetName));
	BISPASSERT(ntohl(pOPFVCMsg->dwPort) == pCallParam->m_nPortIndex);
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-6);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(nSwitcherLoopBack==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(nTrunkPort==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);
	BISPASSERT(DTS_VNM_TEST_MAC_0_IN == pCallParam->m_arrbOppoMac[0]);
	BISPASSERT(3==pCallParam->m_arrbOppoMac[1]);
	BISPASSERT(0x01 == pCallParam->m_arrbOppoMac[2]);
	BISPASSERT(0x01 == pCallParam->m_arrbOppoMac[3]);
	BISPASSERT(0x01 == pCallParam->m_arrbOppoMac[4]);
	BISPASSERT(0x01 == pCallParam->m_arrbOppoMac[5]);
	BISPASSERT(1000 == pCallParam->m_nOutPWLabel);	
	BISPASSERT(2001 == pCallParam->m_nOutVCLabel);
	BISPASSERT(2000 == pCallParam->m_nInPWLabel);
	BISPASSERT(1001 == pCallParam->m_nInVCLabel);

	//FT
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-5);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
	BISPASSERT(pLogEntry->nSendDataBuffLen == sizeof(ofp_pri_vpls_pw));
	ofp_pri_vpls_pw* pOPFPWMsg = (ofp_pri_vpls_pw*)pLogEntry->pSendDataBuff;
	BISPASSERT(OFP13_VERSION == pOPFPWMsg->h.ofp_h.bVersion);
	BISPASSERT(OFPT_SET_PRIVATE == pOPFPWMsg->h.ofp_h.bType);
	BISPASSERT(sizeof(ofp_pri_vpls_pw) == ntohs(pOPFPWMsg->h.ofp_h.wLength));
	BISPASSERT(0!=pOPFPWMsg->h.ofp_h.dwXid);		
	BISPASSERT(OFP_PRI_TYPE_VPLS == pOPFPWMsg->h.bType);
	BISPASSERT(OFP_PRI_SUBTYPE_VPLS_PW == pOPFPWMsg->h.bSubtype);
	BISPASSERT(OFP_PRI_OPT_INSERT == pOPFPWMsg->h.bOpt);
	BISPASSERT(0==strcmp(pOPFPWMsg->alias,pVirNet->m_szVirNetName));
	BISPASSERT(ntohl(pOPFPWMsg->dwPort) == 3);
	BISPASSERT(ntohs(pOPFPWMsg->wflag) == OFP_PRI_VPLS_F_PW_USE_ASYMMETRIC);
	BISPASSERT(0==pOPFPWMsg->wPolicer_id);
	BISPASSERT(DTS_VNM_TEST_MAC_0_IN == pOPFPWMsg->mac[0]);
	BISPASSERT(3==pOPFPWMsg->mac[1]);
	BISPASSERT(0x01 == pOPFPWMsg->mac[2]);
	BISPASSERT(0x01 == pOPFPWMsg->mac[3]);
	BISPASSERT(0x01 == pOPFPWMsg->mac[4]);
	BISPASSERT(0x01 == pOPFPWMsg->mac[5]);
	BISPASSERT(1000 == ntohl(pOPFPWMsg->dwOut_pw_label));
	BISPASSERT(2001 == ntohl(pOPFPWMsg->dwOut_vc_label));
	BISPASSERT(2000 == ntohl(pOPFPWMsg->dwIn_pw_label));
	BISPASSERT(1001 == ntohl(pOPFPWMsg->dwIn_vc_label));
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-4);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(nOppoSwitcherLB==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(nOppoTrunkPort==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);
	BISPASSERT(2000== pCallParam->m_nOutPWLabel);	
	BISPASSERT(1001== pCallParam->m_nOutVCLabel);
	BISPASSERT(1000 == pCallParam->m_nInPWLabel);
	BISPASSERT(2001 == pCallParam->m_nInVCLabel);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-3);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
	pOPFPWMsg = (ofp_pri_vpls_pw*)pLogEntry->pSendDataBuff;
	BISPASSERT(pLogEntry->nSendDataBuffLen == sizeof(ofp_pri_vpls_pw));
	BISPASSERT(OFP13_VERSION == pOPFPWMsg->h.ofp_h.bVersion);
	BISPASSERT(OFPT_SET_PRIVATE == pOPFPWMsg->h.ofp_h.bType);
	BISPASSERT(sizeof(ofp_pri_vpls_pw) == ntohs(pOPFPWMsg->h.ofp_h.wLength));
	BISPASSERT(0!=pOPFPWMsg->h.ofp_h.dwXid);		
	BISPASSERT(OFP_PRI_TYPE_VPLS == pOPFPWMsg->h.bType);
	BISPASSERT(OFP_PRI_SUBTYPE_VPLS_PW == pOPFPWMsg->h.bSubtype);
	BISPASSERT(OFP_PRI_OPT_INSERT == pOPFPWMsg->h.bOpt);
	BISPASSERT(0==strcmp(pOPFPWMsg->alias,pVirNet->m_szVirNetName));
	BISPASSERT(ntohl(pOPFPWMsg->dwPort) == 3);
	BISPASSERT(ntohs(pOPFPWMsg->wflag) == OFP_PRI_VPLS_F_PW_USE_ASYMMETRIC);
	BISPASSERT(0==pOPFPWMsg->wPolicer_id);
	BISPASSERT(DTS_VNM_TEST_MAC_0_OUT == pOPFPWMsg->mac[0]);
	BISPASSERT(3==pOPFPWMsg->mac[1]);
	BISPASSERT(0x02 == pOPFPWMsg->mac[2]);
	BISPASSERT(0x02 == pOPFPWMsg->mac[3]);
	BISPASSERT(0x02 == pOPFPWMsg->mac[4]);
	BISPASSERT(0x02 == pOPFPWMsg->mac[5]);
	BISPASSERT(2000 == ntohl(pOPFPWMsg->dwOut_pw_label));
	BISPASSERT(1001 == ntohl(pOPFPWMsg->dwOut_vc_label));
	BISPASSERT(1000 == ntohl(pOPFPWMsg->dwIn_pw_label));
	BISPASSERT(2001 == ntohl(pOPFPWMsg->dwIn_vc_label));
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-2);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(nOppoSwitcherLB==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(nOppoOutPort==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
	BISPASSERT(pLogEntry->nSendDataBuffLen == sizeof(ofp_pri_vpls_vc));
	pOPFVCMsg = (ofp_pri_vpls_vc*)pLogEntry->pSendDataBuff;
	BISPASSERT(OFP13_VERSION == pOPFVCMsg->h.ofp_h.bVersion);
	BISPASSERT(OFPT_SET_PRIVATE == pOPFVCMsg->h.ofp_h.bType);
	BISPASSERT(sizeof(ofp_pri_vpls_vc) == ntohs(pOPFVCMsg->h.ofp_h.wLength));
	BISPASSERT(0!=pOPFVCMsg->h.ofp_h.dwXid);		
	BISPASSERT(OFP_PRI_TYPE_VPLS == pOPFVCMsg->h.bType);
	BISPASSERT(OFP_PRI_SUBTYPE_VPLS_VC == pOPFVCMsg->h.bSubtype);
	BISPASSERT(OFP_PRI_OPT_INSERT == pOPFVCMsg->h.bOpt);
	BISPASSERT(0==strcmp(pOPFVCMsg->alias,pVirNet->m_szVirNetName));
	BISPASSERT(ntohl(pOPFVCMsg->dwPort) == pCallParam->m_nPortIndex);

	return 0;
}




int DTSVNMTestVNet_2StatsActiveAddPortTest4()
{
	//VNet2增加0x03030303的5端口，原有0x02020202的5端口、0x04040404的1端口
	//0x03030303[2]--0x02020202[2],0x03030303[3]--0x04040404[3]
	UINT nVirNetNo = 2;
	LPDTSVirtualNet pVirNet_2 = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet_2);
	BISPASSERT(2==pVirNet_2->m_mapOutPort.size());
	BISPASSERT(DTS_VNM_STATS_ACTIVE==pVirNet_2->m_nStats);

	//交换机
	LPDTSVNMSwitcher pSwitcher1 = DTSVNMGetSwitcherByLoopBack(0x01010101);
	BISPASSERT(NULL!=pSwitcher1);
	BISPASSERT(4==pSwitcher1->m_mapPort.size());
	LPDTSVNMSwitcher pSwitcher2 = DTSVNMGetSwitcherByLoopBack(0x02020202);
	BISPASSERT(NULL!=pSwitcher2);
	BISPASSERT(4==pSwitcher2->m_mapPort.size());
	LPDTSVNMSwitcher pSwitcher3 = DTSVNMGetSwitcherByLoopBack(0x03030303);
	BISPASSERT(NULL!=pSwitcher3);
	BISPASSERT(2<=pSwitcher3->m_mapPort.size());
	LPDTSVNMSwitcher pSwitcher4 = DTSVNMGetSwitcherByLoopBack(0x04040404);
	BISPASSERT(NULL!=pSwitcher4);
	BISPASSERT(2<=pSwitcher4->m_mapPort.size());

	//出口
	LPDTSVNMPort	 pOPortS3_5 = DTSVNMGetPort(0x03030303,5);
//	BISPASSERT(NULL==pOPortS3_5);	
	if (NULL!=pOPortS3_5)
	{
		BISPASSERT(!pOPortS3_5->m_bIsAcitve);	
	}
	LPDTSVNMPort	 pOPortS2_5 = DTSVNMGetPort(0x02020202,5);
	BISPASSERT(NULL!=pOPortS2_5);
	BISPASSERT(1==pOPortS2_5->m_mapLabelReffrence.size());
	BISPASSERT(pOPortS2_5->m_nVirNetNo == 2);
	LPDTSVNMPort	 pOPortS4_1 = DTSVNMGetPort(0x02020202,5);
	BISPASSERT(NULL!=pOPortS4_1);
	BISPASSERT(1==pOPortS4_1->m_mapLabelReffrence.size());
	BISPASSERT(pOPortS4_1->m_nVirNetNo == 2);

	//中继端口
	LPDTSVNMPort	pTPortS3_2toS2_2 = DTSVNMGetPort(0x03030303,2);
	BISPASSERT(NULL!=pTPortS3_2toS2_2);
	LPDTSVNMPort	pTPortS2_2toS3_2 =  DTSVNMGetPort(0x02020202,2);
	BISPASSERT(NULL!=pTPortS2_2toS3_2);
	LPDTSVNMPort	pTPortS3_3toS4_3 =  DTSVNMGetPort(0x03030303,3);
	if (NULL!=pTPortS3_3toS4_3)
	{
		BISPASSERT(!pTPortS3_3toS4_3->m_bIsAcitve);	
	}
//	BISPASSERT(NULL==pTPortS3_3toS4_3);
	LPDTSVNMPort	pTPortS4_3toS3_3 =  DTSVNMGetPort(0x04040404,3);
	if (NULL!=pTPortS4_3toS3_3)
	{
		BISPASSERT(!pTPortS4_3toS3_3->m_bIsAcitve);	
	}
//	BISPASSERT(NULL==pTPortS4_3toS3_3);

	int nLogcountBef,nLogcountaft,nRet;
	int	nSwitcherCountBef,nSwitcherCountAft;
	int	nPathCountBef = 0;
	int nPathCountAft = 0;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	LPDTSVNMPath pVNet2Path3to2 = NULL;
	LPDTSVNMPath pVNet2Path3to4 = NULL;
	LPDTSVNMPath pVNet2Path4to2 = NULL;
	char szTmp[32]="";
	BOOL bIsMatch = FALSE;

	nSwitcherCountBef = DTSVNMGetSwitcherCount();
	nPathCountBef = DTSVNMGetPathCount();
	//////////////////////////////////////////////////////////////////////////
	//执行
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	//测试重复加入0x04040404的1端口
	nRet = DTSVNMAddOutPort(2,0x04040404,1);
	BISPASSERT(VNM_RESULT_REPEAT==nRet);
	nRet = DTSVNMAddOutPort(2,0x03030303,5);
	BISPASSERT(VNM_RESULT_OK==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	// 	pPath3to2 = DTSVNMTestGetPath(0x03030303,0x02020202);
	// 	BISPASSERT(NULL!=pPath3to2);
	// 	pPath3to4 =DTSVNMTestGetPath(0x03030303,0x04040404);
	// 	BISPASSERT(NULL!=pPath3to4);
	// 	pPath4to2 = DTSVNMTestGetPath(0x04040404,0x02020202);
	// 	BISPASSERT(NULL!=pPath4to2);

	//----------------------------------------------------------------------------
	//虚网列表
	LPDTSVirtualNet pVirNet_1 = DTSVNMGetVirNetByNo(1);
	BISPASSERT(NULL!=pVirNet_1);
	BISPASSERT(pVirNet_1->m_nVirNetNo==1);
	BISPASSERT(pVirNet_1->m_nVirLabelRef==1);
	BISPASSERT(DTS_VNM_STATS_ACTIVE == pVirNet_1->m_nStats);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),1);
	BISPASSERT(0==strcmp(szTmp,pVirNet_1->m_szVirNetName));
	BISPASSERT(3==pVirNet_1->m_mapOutPort.size());
	BISPASSERT(1==pVirNet_1->m_mapOutPort.count(0x01010101));
	BISPASSERT(1==pVirNet_1->m_mapOutPort.count(0x02020202));
	BISPASSERT(1==pVirNet_1->m_mapOutPort.count(0x03030303));

	BISPASSERT(1==pVirNet_1->m_mapOutPort[0x01010101]);
	BISPASSERT(1==pVirNet_1->m_mapOutPort[0x02020202]);
	BISPASSERT(1==pVirNet_1->m_mapOutPort[0x03030303]);


	pVirNet_2 = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet_2);
	BISPASSERT(pVirNet_2->m_nVirNetNo==nVirNetNo);
	BISPASSERT(pVirNet_2->m_nVirLabelRef==nVirNetNo);
	BISPASSERT(DTS_VNM_STATS_ACTIVE == pVirNet_2->m_nStats);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),nVirNetNo);
	BISPASSERT(0==strcmp(szTmp,pVirNet_2->m_szVirNetName));
	BISPASSERT(3==pVirNet_2->m_mapOutPort.size());
	BISPASSERT(1==pVirNet_2->m_mapOutPort.count(0x04040404));
	BISPASSERT(1==pVirNet_2->m_mapOutPort.count(0x02020202));
	BISPASSERT(1==pVirNet_2->m_mapOutPort.count(0x03030303));

	BISPASSERT(1==pVirNet_2->m_mapOutPort[0x04040404]);
	BISPASSERT(5==pVirNet_2->m_mapOutPort[0x02020202]);
	BISPASSERT(5==pVirNet_2->m_mapOutPort[0x03030303]);

	LPDTSVirtualNet pVirNet_3 = DTSVNMGetVirNetByNo(3);
	BISPASSERT(NULL!=pVirNet_3);
	BISPASSERT(pVirNet_3->m_nVirNetNo==3);
	BISPASSERT(pVirNet_3->m_nVirLabelRef==3);
	BISPASSERT(DTS_VNM_STATS_ONE_OUT_PORT == pVirNet_3->m_nStats);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),3);
	BISPASSERT(0==strcmp(szTmp,pVirNet_3->m_szVirNetName));
	BISPASSERT(1==pVirNet_3->m_mapOutPort.size());
	BISPASSERT(1==pVirNet_3->m_mapOutPort.count(0x01010101));
	BISPASSERT(4==pVirNet_3->m_mapOutPort[0x01010101]);


	//-----------------------------------------------------------------------------
	//交换机列表 没变化
	nSwitcherCountAft = DTSVNMGetSwitcherCount();
	BISPASSERT(0==nSwitcherCountAft-nSwitcherCountBef);

	//0x01010101 不变
	pSwitcher1 = DTSVNMGetSwitcherByLoopBack(0x01010101);
	BISPASSERT(NULL!=pSwitcher1);
	BISPASSERT(4==pSwitcher1->m_mapPort.size());

	//0x02020202 不变
	pSwitcher2 = DTSVNMGetSwitcherByLoopBack(0x02020202);
	BISPASSERT(NULL!=pSwitcher2);
	BISPASSERT(4==pSwitcher2->m_mapPort.size());

	//0x03030303 增加2个端口
	pSwitcher3 = DTSVNMGetSwitcherByLoopBack(0x03030303);
	BISPASSERT(NULL!=pSwitcher3);	
	BISPASSERT(4==pSwitcher3->m_mapPort.size());

	//0x04040404 增加1个端口
	pSwitcher4 = DTSVNMGetSwitcherByLoopBack(0x04040404);
	BISPASSERT(NULL!=pSwitcher4);	
	BISPASSERT(3==pSwitcher4->m_mapPort.size());

	//////////////////////////////////////////////////////////////////////////
	//端口
	//0x04040404 3个端口
	pOPortS4_1 = DTSVNMGetPort(0x04040404,1);
	BISPASSERT(pOPortS4_1->m_bIsAcitve);
	BISPASSERT(NULL!=pOPortS4_1);
	BISPASSERT(pOPortS4_1->m_bIsOutPort);
	BISPASSERT(pOPortS4_1->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS4_1->m_nVirNetNo == 2);
	BISPASSERT(2==pOPortS4_1->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS4_1->m_mapLabelReffrence.size());
	BISPASSERT(2==pOPortS4_1->m_vectPathID.size());
	pVNet2Path4to2 = DTSVNMGetPathByID(pOPortS4_1->m_vectPathID[0]);
	BISPASSERT(NULL!=pVNet2Path4to2);
	BISPASSERT(2==pVNet2Path4to2->m_nVirNetNo);
	pVNet2Path3to4 = DTSVNMGetPathByID(pOPortS4_1->m_vectPathID[1]);
	BISPASSERT(NULL!=pVNet2Path3to4);
	BISPASSERT(2==pVNet2Path3to4->m_nVirNetNo);
	// 	BISPASSERT(pPath4to2->m_nPathID == pOPortS4_1->m_vectPathID[0]);
	// 	BISPASSERT(pPath3to4->m_nPathID == pOPortS4_1->m_vectPathID[1]);

	LPDTSVNMPort pTPortS4_2toS1_2 = DTSVNMGetPort(0x04040404,2);
	BISPASSERT(NULL!=pTPortS4_2toS1_2);
	BISPASSERT(pTPortS4_2toS1_2->m_bIsAcitve);
	BISPASSERT(!pTPortS4_2toS1_2->m_bIsOutPort);
	BISPASSERT(pTPortS4_2toS1_2->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_2toS1_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS4_2toS1_2->m_vectPathID.size());
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS4_2toS1_2->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_nVirNetNo==2);

	pTPortS4_3toS3_3 = DTSVNMGetPort(0x04040404,3);
	BISPASSERT(NULL!=pTPortS4_3toS3_3);
	BISPASSERT(pTPortS4_3toS3_3->m_bIsAcitve);
	BISPASSERT(!pTPortS4_3toS3_3->m_bIsOutPort);
	BISPASSERT(pTPortS4_3toS3_3->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_3toS3_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS4_3toS3_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS4_3toS3_3->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS4_3toS3_3->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to4->m_nPathID == pTPortS4_3toS3_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to4->m_nVirNetNo==2);

	//0x03030303 4个端口
	pOPortS3_5 = DTSVNMGetPort(0x03030303,5);
	BISPASSERT(NULL!=pOPortS3_5);	
	BISPASSERT(pOPortS3_5->m_bIsAcitve);
	BISPASSERT(pOPortS3_5->m_bIsOutPort);
	BISPASSERT(pOPortS3_5->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_5->m_nVirNetNo == 2);
	BISPASSERT(2==pOPortS3_5->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS3_5->m_mapLabelReffrence.size());
	BISPASSERT(2==pOPortS3_5->m_vectPathID.size());
	//	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS3_5->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to4->m_nPathID == pOPortS3_5->m_vectPathID[1]);
	pVNet2Path3to2 = DTSVNMGetPathByID(pOPortS3_5->m_vectPathID[0]);
	BISPASSERT(NULL!=pVNet2Path3to2);
	BISPASSERT(2==pVNet2Path3to2->m_nVirNetNo);

	//	LPDTSVNMPath pPath3to1 = DTSVNMTestGetPath(0x03030303,0x01010101);
	LPDTSVNMPort	 pOPortS3_1 = DTSVNMGetPort(0x03030303,1);
	BISPASSERT(NULL!=pOPortS3_1);	
	BISPASSERT(pOPortS3_1->m_bIsAcitve);
	BISPASSERT(pOPortS3_1->m_bIsOutPort);
	BISPASSERT(pOPortS3_1->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_1->m_nVirNetNo == 1);
	BISPASSERT(2==pOPortS3_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS3_1->m_mapLabelReffrence.size());
	BISPASSERT(2==pOPortS3_1->m_vectPathID.size());
	//	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS3_1->m_vectPathID[1]);
	LPDTSVNMPath pVNet1Path3to2 = DTSVNMGetPathByID(pOPortS3_1->m_vectPathID[1]);
	BISPASSERT(NULL!=pVNet1Path3to2);
	BISPASSERT(1==pVNet1Path3to2->m_nVirNetNo);
	LPDTSVNMPath pVNet1Path3to1 = DTSVNMGetPathByID(pOPortS3_1->m_vectPathID[0]);
	BISPASSERT(NULL!=pVNet1Path3to1);
	BISPASSERT(1==pVNet1Path3to1->m_nVirNetNo);
	//	BISPASSERT(pPath3to1->m_nPathID == pOPortS3_1->m_vectPathID[0]);

	pTPortS3_2toS2_2 = DTSVNMGetPort(0x03030303,2);
	BISPASSERT(NULL!=pTPortS3_2toS2_2);
	BISPASSERT(!pTPortS3_2toS2_2->m_bIsOutPort);
	BISPASSERT(pTPortS3_2toS2_2->m_bIsAcitve);
	BISPASSERT(pTPortS3_2toS2_2->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS3_2toS2_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(2==pTPortS3_2toS2_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(2==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(3==pTPortS3_2toS2_2->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[2]);
	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[1]);
	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);

	pTPortS3_3toS4_3 =  DTSVNMGetPort(0x03030303,3);
	BISPASSERT(NULL!=pTPortS3_3toS4_3);
	BISPASSERT(!pTPortS3_3toS4_3->m_bIsOutPort);
	BISPASSERT(pTPortS3_3toS4_3->m_bIsAcitve);
	BISPASSERT(pTPortS3_3toS4_3->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS3_3toS4_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS3_3toS4_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS3_3toS4_3->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS3_3toS4_3->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to4->m_nPathID == pTPortS3_3toS4_3->m_vectPathID[0]);

	//0x02020202, 4个端口
	//	LPDTSVNMPath  pPath2to1 = DTSVNMTestGetPath(0x02020202,0x01010101);
	LPDTSVNMPort pOPortS2_1 = DTSVNMGetPort(0x02020202,1);
	BISPASSERT(NULL!=pOPortS2_1);
	BISPASSERT(pOPortS2_1->m_bIsOutPort);
	BISPASSERT(pOPortS2_1->m_bIsAcitve);
	BISPASSERT(pOPortS2_1->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS2_1->m_nVirNetNo == 1);
	BISPASSERT(2==pOPortS2_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS2_1->m_mapLabelReffrence.size());
	BISPASSERT(2==pOPortS2_1->m_vectPathID.size());
	//影响3-2、2-1两条路径
	//	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS2_1->m_vectPathID[1]);
	LPDTSVNMPath  pNet1Path3to2 = DTSVNMGetPathByID(pOPortS2_1->m_vectPathID[1]);
	BISPASSERT(NULL!=pNet1Path3to2);
	BISPASSERT(1==pNet1Path3to2->m_nVirNetNo);
	LPDTSVNMPath  pVNet1Path2to1 = DTSVNMGetPathByID(pOPortS2_1->m_vectPathID[0]);
	BISPASSERT(NULL!=pVNet1Path2to1);
	BISPASSERT(1==pVNet1Path2to1->m_nVirNetNo);
	//	BISPASSERT(pPath2to1->m_nPathID == pOPortS2_1->m_vectPathID[0]);

	pOPortS2_5 = DTSVNMGetPort(0x02020202,5);
	BISPASSERT(NULL!=pOPortS2_5);
	BISPASSERT(pOPortS2_5->m_bIsOutPort);
	BISPASSERT(pOPortS2_5->m_bIsAcitve);
	BISPASSERT(pOPortS2_5->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS2_5->m_nVirNetNo == 2);
	BISPASSERT(2==pOPortS2_5->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS2_5->m_mapLabelReffrence.size());
	//影响3-2、4-2 2条路径
	BISPASSERT(2==pOPortS2_5->m_vectPathID.size());
	BISPASSERT(pVNet2Path4to2->m_nPathID == pOPortS2_5->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS2_5->m_vectPathID[1]);

	LPDTSVNMPort pTPortS2_3toS1_3 = DTSVNMGetPort(0x02020202,3);
	BISPASSERT(NULL!=pTPortS2_3toS1_3);
	BISPASSERT(!pTPortS2_3toS1_3->m_bIsOutPort);
	BISPASSERT(pTPortS2_3toS1_3->m_bIsAcitve);
	BISPASSERT(pTPortS2_3toS1_3->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_3toS1_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(2==pTPortS2_3toS1_3->m_mapLabelReffrence.size());
	BISPASSERT(2==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	//影响3-1、2-1、4-2 3条路径
	BISPASSERT(3==pTPortS2_3toS1_3->m_vectPathID.size());
	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[1]);
	BISPASSERT(pVNet1Path2to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[2]);

	pTPortS2_2toS3_2 = DTSVNMGetPort(0x02020202,2);
	BISPASSERT(NULL!=pTPortS2_2toS3_2);
	BISPASSERT(!pTPortS2_2toS3_2->m_bIsOutPort);
	BISPASSERT(pTPortS2_2toS3_2->m_bIsAcitve);
	BISPASSERT(pTPortS2_2toS3_2->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_2toS3_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(2==pTPortS2_2toS3_2->m_mapLabelReffrence.size());
	BISPASSERT(2==pTPortS2_2toS3_2->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS2_2toS3_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	//影响3-2、3-1两条路径
	BISPASSERT(3==pTPortS2_2toS3_2->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[2]);
	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[1]);
	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[0]);

	//0x01010101 4个端口
	LPDTSVNMPort pOPortS1_1 = DTSVNMGetPort(0x01010101,1);
	BISPASSERT(NULL!=pOPortS1_1);
	BISPASSERT(pOPortS1_1->m_bIsOutPort);
	BISPASSERT(pOPortS1_1->m_bIsAcitve);
	BISPASSERT(pOPortS1_1->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS1_1->m_nVirNetNo == 1);
	BISPASSERT(1==pOPortS1_1->m_mapLabelReffrence.size());
	BISPASSERT(2==pOPortS1_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(2==pOPortS1_1->m_vectPathID.size());	
	BISPASSERT(pVNet1Path2to1->m_nPathID == pOPortS1_1->m_vectPathID[0]);
	BISPASSERT(pVNet1Path3to1->m_nPathID == pOPortS1_1->m_vectPathID[1]);

	LPDTSVNMPort pOPortS1_4 = DTSVNMGetPort(0x01010101,4);
	BISPASSERT(NULL!=pOPortS1_4);
	BISPASSERT(pOPortS1_4->m_bIsOutPort);
	BISPASSERT(pOPortS1_4->m_bIsAcitve);
	BISPASSERT(pOPortS1_4->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS1_4->m_nVirNetNo == 3);
	BISPASSERT(0==pOPortS1_4->m_mapLabelReffrence.size());
	//	BISPASSERT(2==pOPortS1_4->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pOPortS1_4->m_vectPathID.size());	
	// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pOPortS1_1->m_vectPathID[0]);
	// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pOPortS1_1->m_vectPathID[1]);

	LPDTSVNMPort pTPortS1_3ToS2_3 = DTSVNMGetPort(0x01010101,3);
	BISPASSERT(NULL!=pTPortS1_3ToS2_3);
	BISPASSERT(!pTPortS1_3ToS2_3->m_bIsOutPort);
	BISPASSERT(pTPortS1_3ToS2_3->m_bIsAcitve);
	BISPASSERT(pTPortS1_3ToS2_3->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_3ToS2_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(2==pTPortS1_3ToS2_3->m_mapLabelReffrence.size());
	BISPASSERT(2==pTPortS1_3ToS2_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS1_3ToS2_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(3==pTPortS1_3ToS2_3->m_vectPathID.size());	
	BISPASSERT(pVNet1Path2to1->m_nPathID == pTPortS1_3ToS2_3->m_vectPathID[0]);
	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS1_3ToS2_3->m_vectPathID[1]);
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS1_3ToS2_3->m_vectPathID[2]);

	LPDTSVNMPort pTPortS1_2ToS4_2 = DTSVNMGetPort(0x01010101,2);
	BISPASSERT(NULL!=pTPortS1_2ToS4_2);
	BISPASSERT(!pTPortS1_2ToS4_2->m_bIsOutPort);
	BISPASSERT(pTPortS1_2ToS4_2->m_bIsAcitve);
	BISPASSERT(pTPortS1_2ToS4_2->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_2ToS4_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS1_2ToS4_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS1_2ToS4_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS1_2ToS4_2->m_vectPathID.size());	
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS1_2ToS4_2->m_vectPathID[0]);
	//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS1_2ToS4_2->m_vectPathID[1]);

	//-----------------------------------------------------------------------------
	//路径列表
	BISPASSERT(6==DTSVNMGetPathCount());
	//VNet1
	BISPASSERT(pVNet1Path3to1->m_dwBeginSwitcherLoopBack == 0x03030303);
	BISPASSERT(pVNet1Path3to1->m_dwEndSwitcherLoopBack == 0x01010101);
	BISPASSERT(pVNet1Path3to1->m_nVirNetNo == 1);
	BISPASSERT(3==pVNet1Path3to1->m_vectLabelSwitch.size());

	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(pVNet1Path3to2->m_nVirNetNo == 1);
	BISPASSERT(2==pVNet1Path3to2->m_vectLabelSwitch.size());

	BISPASSERT(pVNet1Path2to1->m_dwBeginSwitcherLoopBack == 0x02020202);
	BISPASSERT(pVNet1Path2to1->m_dwEndSwitcherLoopBack == 0x01010101);
	BISPASSERT(pVNet1Path2to1->m_nVirNetNo == 1);
	BISPASSERT(2==pVNet1Path2to1->m_vectLabelSwitch.size());

	//VNet2
	BISPASSERT(pVNet2Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
	BISPASSERT(pVNet2Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(pVNet2Path3to2->m_nVirNetNo == 2);
	BISPASSERT(2==pVNet2Path3to2->m_vectLabelSwitch.size());

	BISPASSERT(pVNet2Path3to4->m_dwBeginSwitcherLoopBack == 0x03030303);
	BISPASSERT(pVNet2Path3to4->m_dwEndSwitcherLoopBack == 0x04040404);
	BISPASSERT(pVNet2Path3to4->m_nVirNetNo == 2);
	BISPASSERT(2==pVNet2Path3to4->m_vectLabelSwitch.size());

	BISPASSERT(pVNet2Path4to2->m_dwBeginSwitcherLoopBack == 0x04040404);
	BISPASSERT(pVNet2Path4to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(pVNet2Path4to2->m_nVirNetNo == 2);
	BISPASSERT(3==pVNet2Path4to2->m_vectLabelSwitch.size());

	//----------------------------------------------------------------------------
	//外部调用
	//外部调用2次路径计算，中继端口加入虚网4次，出口交换1次
	BISPASSERT(12==nLogcountaft-nLogcountBef);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-12);
	BISPASSERT(VNM_FUNC_CALL_TOPO_CONCULATE==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(nVirNetNo==pCallParam->m_nVirNetNo);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(0x02020202==pCallParam->m_dwOppoLoopBack);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-11);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(5==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-10);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-9);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-8);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-7);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-6);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-5);
	BISPASSERT(VNM_FUNC_CALL_TOPO_CONCULATE==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(0x04040404==pCallParam->m_dwOppoLoopBack);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-4);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-3);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-2);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_ADD==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	return 0;
} 



#endif //DTS_TEST




