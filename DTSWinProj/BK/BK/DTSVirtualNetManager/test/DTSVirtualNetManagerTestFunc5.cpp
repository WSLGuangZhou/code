/********************************************************************
	created:	2018/04/18	14:53
	filename: 	DTSVirtualNetManagerTestFunc5.cpp
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


int DTSVNMTestLinkDownNoBackupPathTest12()
{
	//虚网
	LPDTSVirtualNet pVirNet_1,pVirNet_2,pVirNet_3;
	//交换机
	LPDTSVNMSwitcher pSwitcher1,pSwitcher2,pSwitcher3,pSwitcher4;
	//出口
	LPDTSVNMPort pOPortS1_1,pOPortS1_4;
	LPDTSVNMPort pOPortS2_1,pOPortS2_5;
	LPDTSVNMPort pOPortS3_1,pOPortS3_5;
	LPDTSVNMPort pOPortS4_1;
	//中继接口
	LPDTSVNMPort pTPortS1_2toS4_2,pTPortS1_3toS2_3;
	LPDTSVNMPort pTPortS2_2toS3_2,pTPortS2_3toS1_3;
	LPDTSVNMPort pTPortS3_2toS2_2,pTPortS3_3toS4_3;
	LPDTSVNMPort pTPortS4_2toS1_2,pTPortS4_3toS3_3;
	//path
//	LPDTSVNMPath pVNet1Path2to1;
//	LPDTSVNMPath pVNet1Path3to2;
	LPDTSVNMPath pVNet1Path3to1;
	LPDTSVNMPath pVNet2Path4to2;
//	LPDTSVNMPath pVNet2Path3to2;
	LPDTSVNMPath pVNet2Path3to4;
	//
	int nLogcountBef,nLogcountaft,nRet;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";
	
	pVNet2Path4to2 = DTSVNMGetPathByVNetSrcDst(2,0x04040404,0x02020202);
	BISPASSERT(NULL!=pVNet2Path4to2);
	UINT64 nVNet2Path4to2Cookie = pVNet2Path4to2->m_n64Cookie;

	//////////////////////////////////////////////////////////////////////////
	//执行，无备用链路，故障链路S2_3---S1_3，S_2已经成为孤岛
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMLinkDown(0x02020202,3,0x01010101,3);
	BISPASSERT(VNM_RESULT_OK==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	//----------------------------------------------------------------------------
	//虚网列表，应该还有3个出口，S_2的出口应该被保留
	pVirNet_1 = DTSVNMGetVirNetByNo(1);
	BISPASSERT(NULL!=pVirNet_1);
	BISPASSERT(pVirNet_1->m_nVirNetNo==1);
	BISPASSERT(pVirNet_1->m_nVirLabelRef==1);
	BISPASSERT(DTS_VNM_STATS_ACTIVE == pVirNet_1->m_nStats);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),1);
	BISPASSERT(0==strcmp(szTmp,pVirNet_1->m_szVirNetName));
	//减少
	BISPASSERT(3==pVirNet_1->m_mapOutPort.size());
	BISPASSERT(1==pVirNet_1->m_mapOutPort.count(0x01010101));
	BISPASSERT(1==pVirNet_1->m_mapOutPort.count(0x02020202));
	BISPASSERT(1==pVirNet_1->m_mapOutPort.count(0x03030303));

	BISPASSERT(1==pVirNet_1->m_mapOutPort[0x01010101]);
	BISPASSERT(1==pVirNet_1->m_mapOutPort[0x02020202]);
	BISPASSERT(1==pVirNet_1->m_mapOutPort[0x03030303]);

	pVirNet_2 = DTSVNMGetVirNetByNo(2);
	BISPASSERT(NULL!=pVirNet_2);
	BISPASSERT(pVirNet_2->m_nVirNetNo==2);
	BISPASSERT(pVirNet_2->m_nVirLabelRef==2);
	BISPASSERT(DTS_VNM_STATS_ACTIVE == pVirNet_2->m_nStats);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),2);
	BISPASSERT(0==strcmp(szTmp,pVirNet_2->m_szVirNetName));
	BISPASSERT(3==pVirNet_2->m_mapOutPort.size());
	BISPASSERT(1==pVirNet_2->m_mapOutPort.count(0x04040404));
	BISPASSERT(1==pVirNet_2->m_mapOutPort.count(0x02020202));
	BISPASSERT(1==pVirNet_2->m_mapOutPort.count(0x03030303));

	BISPASSERT(1==pVirNet_2->m_mapOutPort[0x04040404]);
	BISPASSERT(5==pVirNet_2->m_mapOutPort[0x02020202]);
	BISPASSERT(5==pVirNet_2->m_mapOutPort[0x03030303]);

	pVirNet_3 = DTSVNMGetVirNetByNo(3);
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
	nRet = DTSVNMGetSwitcherCount();
	BISPASSERT(4 == nRet);

	//0x01010101 不变
	pSwitcher1 = DTSVNMGetSwitcherByLoopBack(0x01010101);
	BISPASSERT(NULL!=pSwitcher1);
	nRet = pSwitcher1->m_mapPort.size();
	BISPASSERT(4==nRet);

	//0x02020202 不变
	pSwitcher2 = DTSVNMGetSwitcherByLoopBack(0x02020202);
	BISPASSERT(NULL!=pSwitcher2);
	BISPASSERT(4==pSwitcher2->m_mapPort.size());

	//0x03030303 不变
	pSwitcher3 = DTSVNMGetSwitcherByLoopBack(0x03030303);
	BISPASSERT(NULL!=pSwitcher3);	
	BISPASSERT(4==pSwitcher3->m_mapPort.size());

	//0x04040404 不变
	pSwitcher4 = DTSVNMGetSwitcherByLoopBack(0x04040404);
	BISPASSERT(NULL!=pSwitcher4);	
	BISPASSERT(3==pSwitcher4->m_mapPort.size());
	
	//////////////////////////////////////////////////////////////////////////
	//端口
	//0x04040404 3个端口
	pOPortS4_1 = DTSVNMGetPort(0x04040404,1);
	BISPASSERT(NULL!=pOPortS4_1);
	BISPASSERT(pOPortS4_1->m_bIsOutPort);
	BISPASSERT(pOPortS4_1->m_bIsAcitve);
	BISPASSERT(pOPortS4_1->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS4_1->m_nVirNetNo == 2);
	BISPASSERT(1==pOPortS4_1->m_mapLabelReffrence.size());
	BISPASSERT(1==pOPortS4_1->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS4_1->m_vectPathID.size());
// 	pVNet2Path4to2 = DTSVNMGetPath(pOPortS4_1->m_vectPathID[0]);
// 	BISPASSERT(NULL!=pVNet2Path4to2);
// 	BISPASSERT(2==pVNet2Path4to2->m_nVirNetNo);
// 	BISPASSERT(0x04040404 == pVNet2Path4to2->m_dwBeginSwitcherLoopBack);
// 	BISPASSERT(0x02020202 == pVNet2Path4to2->m_dwEndSwitcherLoopBack);
	pVNet2Path3to4 = DTSVNMGetPathByID(pOPortS4_1->m_vectPathID[0]);
	BISPASSERT(NULL!=pVNet2Path3to4);
	BISPASSERT(2==pVNet2Path3to4->m_nVirNetNo);
	BISPASSERT(0x03030303 == pVNet2Path3to4->m_dwBeginSwitcherLoopBack);
	BISPASSERT(0x04040404 == pVNet2Path3to4->m_dwEndSwitcherLoopBack);

	pTPortS4_2toS1_2 = DTSVNMGetPort(0x04040404,2);
	BISPASSERT(NULL!=pTPortS4_2toS1_2);
	BISPASSERT(!pTPortS4_2toS1_2->m_bIsOutPort);
	BISPASSERT(pTPortS4_2toS1_2->m_bIsAcitve);
	BISPASSERT(pTPortS4_2toS1_2->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_2toS1_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
//	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);
	BISPASSERT(0==pTPortS4_2toS1_2->m_mapLabelReffrence.count(pVirNet_2->m_nVirLabelRef));
	BISPASSERT(1==pTPortS4_2toS1_2->m_vectPathID.size());
//	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS4_2toS1_2->m_vectPathID[0]);
	pVNet1Path3to1 = DTSVNMGetPathByID(pTPortS4_2toS1_2->m_vectPathID[0]);
	BISPASSERT(NULL!=pVNet1Path3to1);
	BISPASSERT(pVNet1Path3to1->m_nVirNetNo==1);
	BISPASSERT(0x03030303 == pVNet1Path3to1->m_dwBeginSwitcherLoopBack);
	BISPASSERT(0x01010101 == pVNet1Path3to1->m_dwEndSwitcherLoopBack);
// 	pVNet1Path3to2 = DTSVNMGetPath(pTPortS4_2toS1_2->m_vectPathID[2]);
// 	BISPASSERT(NULL!=pVNet1Path3to2);
// 	BISPASSERT(pVNet1Path3to2->m_nVirNetNo==1);
// 	BISPASSERT(0x03030303 == pVNet1Path3to2->m_dwBeginSwitcherLoopBack);
// 	BISPASSERT(0x02020202 == pVNet1Path3to2->m_dwEndSwitcherLoopBack);
// 	pVNet2Path3to2 = DTSVNMGetPath(pTPortS4_2toS1_2->m_vectPathID[3]);
// 	BISPASSERT(NULL!=pVNet2Path3to2);
// 	BISPASSERT(pVNet2Path3to2->m_nVirNetNo==2);
// 	BISPASSERT(0x03030303 == pVNet2Path3to2->m_dwBeginSwitcherLoopBack);
// 	BISPASSERT(0x02020202 == pVNet2Path3to2->m_dwEndSwitcherLoopBack);	

	pTPortS4_3toS3_3 = DTSVNMGetPort(0x04040404,3);
	BISPASSERT(NULL!=pTPortS4_3toS3_3);
	BISPASSERT(!pTPortS4_3toS3_3->m_bIsOutPort);
	BISPASSERT(pTPortS4_3toS3_3->m_bIsAcitve);
	BISPASSERT(pTPortS4_3toS3_3->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_3toS3_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(2==pTPortS4_3toS3_3->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS4_3toS3_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS4_3toS3_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(2==pTPortS4_3toS3_3->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to4->m_nPathID == pTPortS4_3toS3_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to4->m_nVirNetNo==2);
	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS4_3toS3_3->m_vectPathID[1]);
	BISPASSERT(pVNet1Path3to1->m_nVirNetNo==1);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS4_3toS3_3->m_vectPathID[2]);
// 	BISPASSERT(pVNet1Path3to2->m_nVirNetNo==1);
// 	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS4_3toS3_3->m_vectPathID[3]);
// 	BISPASSERT(pVNet2Path3to2->m_nVirNetNo==2);


	//////////////////////////////////////////////////////////////////////////
	//0x03030303 4个端口
	pOPortS3_5 = DTSVNMGetPort(0x03030303,5);
	BISPASSERT(NULL!=pOPortS3_5);	
	BISPASSERT(pOPortS3_5->m_bIsOutPort);
	BISPASSERT(pOPortS3_5->m_bIsAcitve);
	BISPASSERT(pOPortS3_5->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_5->m_nVirNetNo == 2);
	BISPASSERT(1==pOPortS3_5->m_mapLabelReffrence.size());
	BISPASSERT(1==pOPortS3_5->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS3_5->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to4->m_nPathID == pOPortS3_5->m_vectPathID[0]);
//	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS3_5->m_vectPathID[1]);
	
	//S3_1
	pOPortS3_1 = DTSVNMGetPort(0x03030303,1);
	BISPASSERT(NULL!=pOPortS3_1);	
	BISPASSERT(pOPortS3_1->m_bIsOutPort);
	BISPASSERT(pOPortS3_1->m_bIsAcitve);
	BISPASSERT(pOPortS3_1->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_1->m_nVirNetNo == 1);
	BISPASSERT(1==pOPortS3_1->m_mapLabelReffrence.size());
	BISPASSERT(1==pOPortS3_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS3_1->m_vectPathID.size());
	BISPASSERT(pVNet1Path3to1->m_nPathID == pOPortS3_1->m_vectPathID[0]);
	BISPASSERT(pVNet1Path3to1->m_nVirNetNo==1);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pOPortS3_1->m_vectPathID[1]);
// 	BISPASSERT(pVNet1Path3to2->m_nVirNetNo==1);
	
	//
	pTPortS3_2toS2_2 = DTSVNMGetPort(0x03030303,2);
	BISPASSERT(NULL!=pTPortS3_2toS2_2);
	BISPASSERT(!pTPortS3_2toS2_2->m_bIsOutPort);
	BISPASSERT(!pTPortS3_2toS2_2->m_bIsAcitve);
	BISPASSERT(pTPortS3_2toS2_2->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS3_2toS2_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pTPortS3_2toS2_2->m_mapLabelReffrence.size());
	//	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);
	//	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pTPortS3_2toS2_2->m_vectPathID.size());
	//	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);
	//	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);
	//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);
		
	pTPortS3_3toS4_3 =  DTSVNMGetPort(0x03030303,3);
	BISPASSERT(NULL!=pTPortS3_3toS4_3);
	BISPASSERT(!pTPortS3_3toS4_3->m_bIsOutPort);
	BISPASSERT(pTPortS3_3toS4_3->m_bIsAcitve);
	BISPASSERT(pTPortS3_3toS4_3->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS3_3toS4_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(2==pTPortS3_3toS4_3->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS3_3toS4_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);	
	BISPASSERT(1==pTPortS3_3toS4_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);	
	BISPASSERT(2==pTPortS3_3toS4_3->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to4->m_nPathID == pTPortS3_3toS4_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to4->m_nVirNetNo==2);
	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS3_3toS4_3->m_vectPathID[1]);
	BISPASSERT(pVNet1Path3to1->m_nVirNetNo==1);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS3_3toS4_3->m_vectPathID[2]);
// 	BISPASSERT(pVNet1Path3to2->m_nVirNetNo==1);
// 	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS3_3toS4_3->m_vectPathID[3]);
// 	BISPASSERT(pVNet2Path3to2->m_nVirNetNo==2);

	//////////////////////////////////////////////////////////////////////////
	//0x02020202, 4个端口
	pOPortS2_1 = DTSVNMGetPort(0x02020202,1);
	BISPASSERT(NULL!=pOPortS2_1);
	BISPASSERT(pOPortS2_1->m_bIsOutPort);
	BISPASSERT(pOPortS2_1->m_bIsAcitve);
	BISPASSERT(pOPortS2_1->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS2_1->m_nVirNetNo == 1);
	BISPASSERT(0==pOPortS2_1->m_mapLabelReffrence.size());
//	BISPASSERT(2==pOPortS2_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);	
	BISPASSERT(0==pOPortS2_1->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(0==pOPortS2_1->m_vectPathID.size());
// 	pVNet1Path2to1 = DTSVNMGetPath(pOPortS2_1->m_vectPathID[0]);
// 	BISPASSERT(NULL!=pVNet1Path2to1);
// 	BISPASSERT(1==pVNet1Path2to1->m_nVirNetNo);
// 	BISPASSERT(pVNet1Path2to1->m_dwBeginSwitcherLoopBack == 0x02020202);
// 	BISPASSERT(pVNet1Path2to1->m_dwEndSwitcherLoopBack == 0x01010101);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pOPortS2_1->m_vectPathID[1]);
// 	BISPASSERT(1==pVNet1Path3to2->m_nVirNetNo);

	pOPortS2_5 = DTSVNMGetPort(0x02020202,5);
	BISPASSERT(NULL!=pOPortS2_5);
	BISPASSERT(pOPortS2_5->m_bIsOutPort);
	BISPASSERT(pOPortS2_5->m_bIsAcitve);
	BISPASSERT(pOPortS2_5->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS2_5->m_nVirNetNo == 2);
	BISPASSERT(0==pOPortS2_5->m_mapLabelReffrence.size());
	BISPASSERT(0==pOPortS2_5->m_mapLabelReffrence.count(pVirNet_2->m_nVirLabelRef));	
//	BISPASSERT(2==pOPortS2_5->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);	
	BISPASSERT(0==pOPortS2_5->m_vectPathID.size());
// 	BISPASSERT(pVNet2Path4to2->m_nPathID == pOPortS2_5->m_vectPathID[0]);
// 	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS2_5->m_vectPathID[1]);

	pTPortS2_3toS1_3 = DTSVNMGetPort(0x02020202,3);
	BISPASSERT(NULL!=pTPortS2_3toS1_3);
	BISPASSERT(!pTPortS2_3toS1_3->m_bIsOutPort);
	BISPASSERT(!pTPortS2_3toS1_3->m_bIsAcitve);
	BISPASSERT(pTPortS2_3toS1_3->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_3toS1_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pTPortS2_3toS1_3->m_mapLabelReffrence.size());
// 	BISPASSERT(2==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
// 	BISPASSERT(2==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);
	BISPASSERT(0==pTPortS2_3toS1_3->m_vectPathID.size());
// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[0]);
// 	BISPASSERT(pVNet1Path2to1->m_nVirNetNo==1);
// 	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[1]);
// 	BISPASSERT(pVNet2Path4to2->m_nVirNetNo==2);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[2]);
// 	BISPASSERT(pVNet1Path3to2->m_nVirNetNo==1);
// 	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[3]);
// 	BISPASSERT(pVNet2Path3to2->m_nVirNetNo==2);

	pTPortS2_2toS3_2 = DTSVNMGetPort(0x02020202,2);
	BISPASSERT(NULL!=pTPortS2_2toS3_2);
	BISPASSERT(!pTPortS2_2toS3_2->m_bIsOutPort);
	BISPASSERT(!pTPortS2_2toS3_2->m_bIsAcitve);
	BISPASSERT(pTPortS2_2toS3_2->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_2toS3_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pTPortS2_2toS3_2->m_mapLabelReffrence.size());
	BISPASSERT(0==pTPortS2_2toS3_2->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(0==pTPortS2_2toS3_2->m_mapLabelReffrence.count(pVirNet_2->m_nVirLabelRef));
	BISPASSERT(0==pTPortS2_2toS3_2->m_vectPathID.size());


	//////////////////////////////////////////////////////////////////////////
	//0x01010101 4个端口
	pOPortS1_1 = DTSVNMGetPort(0x01010101,1);
	BISPASSERT(NULL!=pOPortS1_1);
	BISPASSERT(pOPortS1_1->m_bIsOutPort);
	BISPASSERT(pOPortS1_1->m_bIsAcitve);
	BISPASSERT(pOPortS1_1->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS1_1->m_nVirNetNo == 1);
	BISPASSERT(1==pOPortS1_1->m_mapLabelReffrence.size());
	BISPASSERT(1==pOPortS1_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS1_1->m_vectPathID.size());	
//	BISPASSERT(pVNet1Path2to1->m_nPathID == pOPortS1_1->m_vectPathID[0]);
	BISPASSERT(pVNet1Path3to1->m_nPathID == pOPortS1_1->m_vectPathID[0]);

	//
	pOPortS1_4 = DTSVNMGetPort(0x01010101,4);
	BISPASSERT(NULL!=pOPortS1_4);
	BISPASSERT(pOPortS1_4->m_bIsOutPort);
	BISPASSERT(pOPortS1_4->m_bIsAcitve);
	BISPASSERT(pOPortS1_4->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS1_4->m_nVirNetNo == 3);
	BISPASSERT(0==pOPortS1_4->m_mapLabelReffrence.size());
	BISPASSERT(0==pOPortS1_4->m_vectPathID.size());	

	//
	pTPortS1_3toS2_3 = DTSVNMGetPort(0x01010101,3);
	BISPASSERT(NULL!=pTPortS1_3toS2_3);
	BISPASSERT(!pTPortS1_3toS2_3->m_bIsOutPort);
	BISPASSERT(!pTPortS1_3toS2_3->m_bIsAcitve);
	BISPASSERT(pTPortS1_3toS2_3->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_3toS2_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pTPortS1_3toS2_3->m_mapLabelReffrence.size());
// 	BISPASSERT(2==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
// 	BISPASSERT(2==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);
	BISPASSERT(0==pTPortS1_3toS2_3->m_vectPathID.size());	
// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[0]);
// 	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[1]);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[2]);
// 	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[3]);

	//
	pTPortS1_2toS4_2 = DTSVNMGetPort(0x01010101,2);
	BISPASSERT(NULL!=pTPortS1_2toS4_2);
	BISPASSERT(!pTPortS1_2toS4_2->m_bIsOutPort);
	BISPASSERT(pTPortS1_2toS4_2->m_bIsAcitve);
	BISPASSERT(pTPortS1_2toS4_2->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_2toS4_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS1_2toS4_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS1_2toS4_2->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(0==pTPortS1_2toS4_2->m_mapLabelReffrence.count(pVirNet_2->m_nVirLabelRef));
	BISPASSERT(1==pTPortS1_2toS4_2->m_vectPathID.size());	
//	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[0]);
	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[0]);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[2]);
// 	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[3]);
	
	//////////////////////////////////////////////////////////////////////////
	//Path
	BISPASSERT(2==DTSVNMGetPathCount());

	//////////////////////////////////////////////////////////////////////////
	//调用，路径计算4次，删除8次，建立0次
	BISPASSERT(44==nLogcountaft-nLogcountBef);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-44);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-43);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
	ofp_pri_vpls_pw* pOPFPWMsg = (ofp_pri_vpls_pw*)pLogEntry->pSendDataBuff;
	BISPASSERT(OFPT_SET_PRIVATE == pOPFPWMsg->h.ofp_h.bType);
	BISPASSERT(OFP_PRI_OPT_REMOVE == pOPFPWMsg->h.bOpt);
	BISPASSERT(OFP_PRI_TYPE_VPLS == pOPFPWMsg->h.bType);
	BISPASSERT(OFP_PRI_SUBTYPE_VPLS_PW == pOPFPWMsg->h.bSubtype);
	BISPASSERT(0==strcmp("VNet1",pOPFPWMsg->alias));
	BISPASSERT(OFP_PRI_VPLS_F_PW_USE_ASYMMETRIC == ntohs(pOPFPWMsg->wflag));
	BISPASSERT(DTS_VNM_TEST_MAC_0_IN == pOPFPWMsg->mac[0]);
	BISPASSERT(3 == pOPFPWMsg->mac[1]);
	BISPASSERT(0x01 == pOPFPWMsg->mac[2]);
	BISPASSERT(0x01 == pOPFPWMsg->mac[3]);
	BISPASSERT(0x01 == pOPFPWMsg->mac[4]);
	BISPASSERT(0x01 == pOPFPWMsg->mac[5]);
	BISPASSERT(3==ntohl(pOPFPWMsg->dwPort));
	BISPASSERT(1000==ntohl(pOPFPWMsg->dwOut_pw_label));
	BISPASSERT(2001==ntohl(pOPFPWMsg->dwOut_vc_label));
	BISPASSERT(2000==ntohl(pOPFPWMsg->dwIn_pw_label));
	BISPASSERT(1001==ntohl(pOPFPWMsg->dwIn_vc_label));
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-42);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-41);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
	pOPFPWMsg = (ofp_pri_vpls_pw*)pLogEntry->pSendDataBuff;
	BISPASSERT(OFPT_SET_PRIVATE == pOPFPWMsg->h.ofp_h.bType);
	BISPASSERT(OFP_PRI_OPT_REMOVE == pOPFPWMsg->h.bOpt);
	BISPASSERT(OFP_PRI_TYPE_VPLS == pOPFPWMsg->h.bType);
	BISPASSERT(OFP_PRI_SUBTYPE_VPLS_PW == pOPFPWMsg->h.bSubtype);
	BISPASSERT(0==strcmp("VNet1",pOPFPWMsg->alias));
	BISPASSERT(OFP_PRI_VPLS_F_PW_USE_ASYMMETRIC == ntohs(pOPFPWMsg->wflag));
	BISPASSERT(DTS_VNM_TEST_MAC_0_OUT == pOPFPWMsg->mac[0]);
	BISPASSERT(3 == pOPFPWMsg->mac[1]);
	BISPASSERT(0x02 == pOPFPWMsg->mac[2]);
	BISPASSERT(0x02 == pOPFPWMsg->mac[3]);
	BISPASSERT(0x02 == pOPFPWMsg->mac[4]);
	BISPASSERT(0x02 == pOPFPWMsg->mac[5]);
	BISPASSERT(3==ntohl(pOPFPWMsg->dwPort));
	BISPASSERT(2000==ntohl(pOPFPWMsg->dwOut_pw_label));
	BISPASSERT(1001==ntohl(pOPFPWMsg->dwOut_vc_label));
	BISPASSERT(1000==ntohl(pOPFPWMsg->dwIn_pw_label));
	BISPASSERT(2001==ntohl(pOPFPWMsg->dwIn_vc_label));

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-40);
	BISPASSERT(VNM_FUNC_CALL_TOPO_CONCULATE==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(0x01010101==pCallParam->m_dwOppoLoopBack);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-39);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-38);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-37);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(3==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-36);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
	ofp_flow_mod* pOPFFlowModMsg = (ofp_flow_mod*)(pLogEntry->pSendDataBuff);
	BISPASSERT(OFPT_FLOW_MOD == pOPFFlowModMsg->header.bType);
	BISPASSERT(OFPFC_DELETE == pOPFFlowModMsg->bCommand);
	BISPASSERT(nVNet2Path4to2Cookie == pOPFFlowModMsg->n64Cookie);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-35);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(2==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-34);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
	pOPFFlowModMsg = (ofp_flow_mod*)(pLogEntry->pSendDataBuff);
	BISPASSERT(nVNet2Path4to2Cookie == pOPFFlowModMsg->n64Cookie);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-33);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-32);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-31);
	BISPASSERT(VNM_FUNC_CALL_TOPO_CONCULATE==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(0x02020202==pCallParam->m_dwOppoLoopBack);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-30);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-29);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-28);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(2==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-27);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-26);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(3==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-25);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-24);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(3==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-23);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
//	pOPFFlowModMsg = (ofp_flow_mod*)pLogEntry->pDataBuff;
//	BISPASSERT(pOPFFlowModMsg->n64Cookie == );
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-22);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(2==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-21);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-20);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-19);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-18);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(1==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-17);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-16);
	BISPASSERT(VNM_FUNC_CALL_TOPO_CONCULATE==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(0x02020202==pCallParam->m_dwOppoLoopBack);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-15);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-14);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-13);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(2==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-12);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-11);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(3==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-10);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-9);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(3==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-8);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-7);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(2==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-6);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-5);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-4);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-3);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(5==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-2);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	BISPASSERT(VNM_FUNC_CALL_TOPO_CONCULATE==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(0x02020202==pCallParam->m_dwOppoLoopBack);
	
	return 0;
}


int DTSVNMTestLinkDownNoBackupPathTest13()
{

	//虚网
	LPDTSVirtualNet pVirNet_1,pVirNet_2,pVirNet_3;
	//交换机
	LPDTSVNMSwitcher pSwitcher1,pSwitcher2,pSwitcher3,pSwitcher4;
	//出口
	LPDTSVNMPort pOPortS1_1,pOPortS1_4;
	LPDTSVNMPort pOPortS2_1,pOPortS2_5;
	LPDTSVNMPort pOPortS3_1,pOPortS3_5;
	LPDTSVNMPort pOPortS4_1;
	//中继接口
	LPDTSVNMPort pTPortS1_2toS4_2,pTPortS1_3toS2_3;
	LPDTSVNMPort pTPortS2_2toS3_2,pTPortS2_3toS1_3;
	LPDTSVNMPort pTPortS3_2toS2_2,pTPortS3_3toS4_3;
	LPDTSVNMPort pTPortS4_2toS1_2,pTPortS4_3toS3_3;
	//path
//	LPDTSVNMPath pVNet1Path2to1;
//	LPDTSVNMPath pVNet1Path3to2;
//	LPDTSVNMPath pVNet1Path3to1;
//	LPDTSVNMPath pVNet2Path4to2;
//	LPDTSVNMPath pVNet2Path3to2;
//	LPDTSVNMPath pVNet2Path3to4;
	//
	int nLogcountBef,nLogcountaft,nRet;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";

	//////////////////////////////////////////////////////////////////////////
	//执行，无备用链路，故障链路S3_3---S4_3，只剩S1_2--S4_2的链路，但也没有交换
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMLinkDown(0x03030303,3,0x04040404,3);
	BISPASSERT(VNM_RESULT_OK==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	//----------------------------------------------------------------------------
	//虚网列表，应该还有3个出口，S_2的出口应该被保留
	pVirNet_1 = DTSVNMGetVirNetByNo(1);
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

	pVirNet_2 = DTSVNMGetVirNetByNo(2);
	BISPASSERT(NULL!=pVirNet_2);
	BISPASSERT(pVirNet_2->m_nVirNetNo==2);
	BISPASSERT(pVirNet_2->m_nVirLabelRef==2);
	BISPASSERT(DTS_VNM_STATS_ACTIVE == pVirNet_2->m_nStats);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),2);
	BISPASSERT(0==strcmp(szTmp,pVirNet_2->m_szVirNetName));
	BISPASSERT(3==pVirNet_2->m_mapOutPort.size());
	BISPASSERT(1==pVirNet_2->m_mapOutPort.count(0x04040404));
	BISPASSERT(1==pVirNet_2->m_mapOutPort.count(0x02020202));
	BISPASSERT(1==pVirNet_2->m_mapOutPort.count(0x03030303));
	BISPASSERT(1==pVirNet_2->m_mapOutPort[0x04040404]);
	BISPASSERT(5==pVirNet_2->m_mapOutPort[0x02020202]);
	BISPASSERT(5==pVirNet_2->m_mapOutPort[0x03030303]);

	pVirNet_3 = DTSVNMGetVirNetByNo(3);
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
	nRet = DTSVNMGetSwitcherCount();
	BISPASSERT(4 == nRet);

	//0x01010101 不变
	pSwitcher1 = DTSVNMGetSwitcherByLoopBack(0x01010101);
	BISPASSERT(NULL!=pSwitcher1);
	nRet = pSwitcher1->m_mapPort.size();
	BISPASSERT(4==nRet);

	//0x02020202 不变
	pSwitcher2 = DTSVNMGetSwitcherByLoopBack(0x02020202);
	BISPASSERT(NULL!=pSwitcher2);
	BISPASSERT(4==pSwitcher2->m_mapPort.size());

	//0x03030303 不变
	pSwitcher3 = DTSVNMGetSwitcherByLoopBack(0x03030303);
	BISPASSERT(NULL!=pSwitcher3);	
	BISPASSERT(4==pSwitcher3->m_mapPort.size());

	//0x04040404 不变
	pSwitcher4 = DTSVNMGetSwitcherByLoopBack(0x04040404);
	BISPASSERT(NULL!=pSwitcher4);	
	BISPASSERT(3==pSwitcher4->m_mapPort.size());

	//////////////////////////////////////////////////////////////////////////
	//端口
	//0x04040404 3个端口
	pOPortS4_1 = DTSVNMGetPort(0x04040404,1);
	BISPASSERT(NULL!=pOPortS4_1);
	BISPASSERT(pOPortS4_1->m_bIsOutPort);
	BISPASSERT(pOPortS4_1->m_bIsAcitve);
	BISPASSERT(pOPortS4_1->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS4_1->m_nVirNetNo == 2);
	BISPASSERT(0==pOPortS4_1->m_mapLabelReffrence.size());
//	BISPASSERT(1==pOPortS4_1->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);
	BISPASSERT(0==pOPortS4_1->m_vectPathID.size());
// 	pVNet2Path4to2 = DTSVNMGetPath(pOPortS4_1->m_vectPathID[0]);
// 	BISPASSERT(NULL!=pVNet2Path4to2);
// 	BISPASSERT(2==pVNet2Path4to2->m_nVirNetNo);
// 	BISPASSERT(0x04040404 == pVNet2Path4to2->m_dwBeginSwitcherLoopBack);
// 	BISPASSERT(0x02020202 == pVNet2Path4to2->m_dwEndSwitcherLoopBack);
// 	pVNet2Path3to4 = DTSVNMGetPath(pOPortS4_1->m_vectPathID[0]);
// 	BISPASSERT(NULL!=pVNet2Path3to4);
// 	BISPASSERT(2==pVNet2Path3to4->m_nVirNetNo);
// 	BISPASSERT(0x03030303 == pVNet2Path3to4->m_dwBeginSwitcherLoopBack);
// 	BISPASSERT(0x04040404 == pVNet2Path3to4->m_dwEndSwitcherLoopBack);

	pTPortS4_2toS1_2 = DTSVNMGetPort(0x04040404,2);
	BISPASSERT(NULL!=pTPortS4_2toS1_2);
	BISPASSERT(!pTPortS4_2toS1_2->m_bIsOutPort);
	BISPASSERT(!pTPortS4_2toS1_2->m_bIsAcitve);
	BISPASSERT(pTPortS4_2toS1_2->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_2toS1_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pTPortS4_2toS1_2->m_mapLabelReffrence.size());
//	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
//	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);
	BISPASSERT(0==pTPortS4_2toS1_2->m_mapLabelReffrence.count(pVirNet_2->m_nVirLabelRef));
	BISPASSERT(0==pTPortS4_2toS1_2->m_vectPathID.size());
//	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS4_2toS1_2->m_vectPathID[0]);
// 	pVNet1Path3to1 = DTSVNMGetPath(pTPortS4_2toS1_2->m_vectPathID[0]);
// 	BISPASSERT(NULL!=pVNet1Path3to1);
// 	BISPASSERT(pVNet1Path3to1->m_nVirNetNo==1);
// 	BISPASSERT(0x03030303 == pVNet1Path3to1->m_dwBeginSwitcherLoopBack);
// 	BISPASSERT(0x01010101 == pVNet1Path3to1->m_dwEndSwitcherLoopBack);
// 	pVNet1Path3to2 = DTSVNMGetPath(pTPortS4_2toS1_2->m_vectPathID[2]);
// 	BISPASSERT(NULL!=pVNet1Path3to2);
// 	BISPASSERT(pVNet1Path3to2->m_nVirNetNo==1);
// 	BISPASSERT(0x03030303 == pVNet1Path3to2->m_dwBeginSwitcherLoopBack);
// 	BISPASSERT(0x02020202 == pVNet1Path3to2->m_dwEndSwitcherLoopBack);
// 	pVNet2Path3to2 = DTSVNMGetPath(pTPortS4_2toS1_2->m_vectPathID[3]);
// 	BISPASSERT(NULL!=pVNet2Path3to2);
// 	BISPASSERT(pVNet2Path3to2->m_nVirNetNo==2);
// 	BISPASSERT(0x03030303 == pVNet2Path3to2->m_dwBeginSwitcherLoopBack);
// 	BISPASSERT(0x02020202 == pVNet2Path3to2->m_dwEndSwitcherLoopBack);	

	pTPortS4_3toS3_3 = DTSVNMGetPort(0x04040404,3);
	BISPASSERT(NULL!=pTPortS4_3toS3_3);
	BISPASSERT(!pTPortS4_3toS3_3->m_bIsOutPort);
	BISPASSERT(!pTPortS4_3toS3_3->m_bIsAcitve);
	BISPASSERT(pTPortS4_3toS3_3->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_3toS3_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pTPortS4_3toS3_3->m_mapLabelReffrence.size());
// 	BISPASSERT(1==pTPortS4_3toS3_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);
// 	BISPASSERT(1==pTPortS4_3toS3_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pTPortS4_3toS3_3->m_vectPathID.size());
// 	BISPASSERT(pVNet2Path3to4->m_nPathID == pTPortS4_3toS3_3->m_vectPathID[0]);
// 	BISPASSERT(pVNet2Path3to4->m_nVirNetNo==2);
// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS4_3toS3_3->m_vectPathID[1]);
// 	BISPASSERT(pVNet1Path3to1->m_nVirNetNo==1);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS4_3toS3_3->m_vectPathID[2]);
// 	BISPASSERT(pVNet1Path3to2->m_nVirNetNo==1);
// 	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS4_3toS3_3->m_vectPathID[3]);
// 	BISPASSERT(pVNet2Path3to2->m_nVirNetNo==2);

	//////////////////////////////////////////////////////////////////////////
	//0x03030303 4个端口
	pOPortS3_5 = DTSVNMGetPort(0x03030303,5);
	BISPASSERT(NULL!=pOPortS3_5);	
	BISPASSERT(pOPortS3_5->m_bIsOutPort);
	BISPASSERT(pOPortS3_5->m_bIsAcitve);
	BISPASSERT(pOPortS3_5->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_5->m_nVirNetNo == 2);
	BISPASSERT(0==pOPortS3_5->m_mapLabelReffrence.size());
//	BISPASSERT(1==pOPortS3_5->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);
	BISPASSERT(0==pOPortS3_5->m_vectPathID.size());
//	BISPASSERT(pVNet2Path3to4->m_nPathID == pOPortS3_5->m_vectPathID[0]);
//	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS3_5->m_vectPathID[1]);

	//S3_1
	pOPortS3_1 = DTSVNMGetPort(0x03030303,1);
	BISPASSERT(NULL!=pOPortS3_1);	
	BISPASSERT(pOPortS3_1->m_bIsOutPort);
	BISPASSERT(pOPortS3_1->m_bIsAcitve);
	BISPASSERT(pOPortS3_1->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_1->m_nVirNetNo == 1);
	BISPASSERT(0==pOPortS3_1->m_mapLabelReffrence.size());
//	BISPASSERT(1==pOPortS3_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pOPortS3_1->m_vectPathID.size());
// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pOPortS3_1->m_vectPathID[0]);
// 	BISPASSERT(pVNet1Path3to1->m_nVirNetNo==1);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pOPortS3_1->m_vectPathID[1]);
// 	BISPASSERT(pVNet1Path3to2->m_nVirNetNo==1);

	//
	pTPortS3_2toS2_2 = DTSVNMGetPort(0x03030303,2);
	BISPASSERT(NULL!=pTPortS3_2toS2_2);
	BISPASSERT(!pTPortS3_2toS2_2->m_bIsOutPort);
	BISPASSERT(!pTPortS3_2toS2_2->m_bIsAcitve);
	BISPASSERT(pTPortS3_2toS2_2->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS3_2toS2_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pTPortS3_2toS2_2->m_mapLabelReffrence.size());
	//	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);
	//	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pTPortS3_2toS2_2->m_vectPathID.size());
	//	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);
	//	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);
	//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);

	pTPortS3_3toS4_3 =  DTSVNMGetPort(0x03030303,3);
	BISPASSERT(NULL!=pTPortS3_3toS4_3);
	BISPASSERT(!pTPortS3_3toS4_3->m_bIsOutPort);
	BISPASSERT(!pTPortS3_3toS4_3->m_bIsAcitve);
	BISPASSERT(pTPortS3_3toS4_3->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS3_3toS4_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pTPortS3_3toS4_3->m_mapLabelReffrence.size());
// 	BISPASSERT(1==pTPortS3_3toS4_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);	
// 	BISPASSERT(1==pTPortS3_3toS4_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);	
	BISPASSERT(0==pTPortS3_3toS4_3->m_vectPathID.size());
// 	BISPASSERT(pVNet2Path3to4->m_nPathID == pTPortS3_3toS4_3->m_vectPathID[0]);
// 	BISPASSERT(pVNet2Path3to4->m_nVirNetNo==2);
// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS3_3toS4_3->m_vectPathID[1]);
// 	BISPASSERT(pVNet1Path3to1->m_nVirNetNo==1);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS3_3toS4_3->m_vectPathID[2]);
// 	BISPASSERT(pVNet1Path3to2->m_nVirNetNo==1);
// 	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS3_3toS4_3->m_vectPathID[3]);
// 	BISPASSERT(pVNet2Path3to2->m_nVirNetNo==2);

	//////////////////////////////////////////////////////////////////////////
	//0x02020202, 4个端口
	pOPortS2_1 = DTSVNMGetPort(0x02020202,1);
	BISPASSERT(NULL!=pOPortS2_1);
	BISPASSERT(pOPortS2_1->m_bIsOutPort);
	BISPASSERT(pOPortS2_1->m_bIsAcitve);
	BISPASSERT(pOPortS2_1->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS2_1->m_nVirNetNo == 1);
	BISPASSERT(0==pOPortS2_1->m_mapLabelReffrence.size());
//	BISPASSERT(2==pOPortS2_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);	
	BISPASSERT(0==pOPortS2_1->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(0==pOPortS2_1->m_vectPathID.size());
// 	pVNet1Path2to1 = DTSVNMGetPath(pOPortS2_1->m_vectPathID[0]);
// 	BISPASSERT(NULL!=pVNet1Path2to1);
// 	BISPASSERT(1==pVNet1Path2to1->m_nVirNetNo);
// 	BISPASSERT(pVNet1Path2to1->m_dwBeginSwitcherLoopBack == 0x02020202);
// 	BISPASSERT(pVNet1Path2to1->m_dwEndSwitcherLoopBack == 0x01010101);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pOPortS2_1->m_vectPathID[1]);
// 	BISPASSERT(1==pVNet1Path3to2->m_nVirNetNo);

	pOPortS2_5 = DTSVNMGetPort(0x02020202,5);
	BISPASSERT(NULL!=pOPortS2_5);
	BISPASSERT(pOPortS2_5->m_bIsOutPort);
	BISPASSERT(pOPortS2_5->m_bIsAcitve);
	BISPASSERT(pOPortS2_5->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS2_5->m_nVirNetNo == 2);
	BISPASSERT(0==pOPortS2_5->m_mapLabelReffrence.size());
	BISPASSERT(0==pOPortS2_5->m_mapLabelReffrence.count(pVirNet_2->m_nVirLabelRef));	
//	BISPASSERT(2==pOPortS2_5->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);	
	BISPASSERT(0==pOPortS2_5->m_vectPathID.size());
// 	BISPASSERT(pVNet2Path4to2->m_nPathID == pOPortS2_5->m_vectPathID[0]);
// 	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS2_5->m_vectPathID[1]);

	pTPortS2_3toS1_3 = DTSVNMGetPort(0x02020202,3);
	BISPASSERT(NULL!=pTPortS2_3toS1_3);
	BISPASSERT(!pTPortS2_3toS1_3->m_bIsOutPort);
	BISPASSERT(!pTPortS2_3toS1_3->m_bIsAcitve);
	BISPASSERT(pTPortS2_3toS1_3->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_3toS1_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pTPortS2_3toS1_3->m_mapLabelReffrence.size());
// 	BISPASSERT(2==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
// 	BISPASSERT(2==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);
	BISPASSERT(0==pTPortS2_3toS1_3->m_vectPathID.size());
// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[0]);
// 	BISPASSERT(pVNet1Path2to1->m_nVirNetNo==1);
// 	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[1]);
// 	BISPASSERT(pVNet2Path4to2->m_nVirNetNo==2);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[2]);
// 	BISPASSERT(pVNet1Path3to2->m_nVirNetNo==1);
// 	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[3]);
// 	BISPASSERT(pVNet2Path3to2->m_nVirNetNo==2);

	pTPortS2_2toS3_2 = DTSVNMGetPort(0x02020202,2);
	BISPASSERT(NULL!=pTPortS2_2toS3_2);
	BISPASSERT(!pTPortS2_2toS3_2->m_bIsOutPort);
	BISPASSERT(!pTPortS2_2toS3_2->m_bIsAcitve);
	BISPASSERT(pTPortS2_2toS3_2->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_2toS3_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pTPortS2_2toS3_2->m_mapLabelReffrence.size());
	BISPASSERT(0==pTPortS2_2toS3_2->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(0==pTPortS2_2toS3_2->m_mapLabelReffrence.count(pVirNet_2->m_nVirLabelRef));
	BISPASSERT(0==pTPortS2_2toS3_2->m_vectPathID.size());
	
	//////////////////////////////////////////////////////////////////////////
	//0x01010101 4个端口
	pOPortS1_1 = DTSVNMGetPort(0x01010101,1);
	BISPASSERT(NULL!=pOPortS1_1);
	BISPASSERT(pOPortS1_1->m_bIsOutPort);
	BISPASSERT(pOPortS1_1->m_bIsAcitve);
	BISPASSERT(pOPortS1_1->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS1_1->m_nVirNetNo == 1);
	BISPASSERT(0==pOPortS1_1->m_mapLabelReffrence.size());
//	BISPASSERT(1==pOPortS1_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pOPortS1_1->m_vectPathID.size());	
//	BISPASSERT(pVNet1Path2to1->m_nPathID == pOPortS1_1->m_vectPathID[0]);
//	BISPASSERT(pVNet1Path3to1->m_nPathID == pOPortS1_1->m_vectPathID[0]);

	//
	pOPortS1_4 = DTSVNMGetPort(0x01010101,4);
	BISPASSERT(NULL!=pOPortS1_4);
	BISPASSERT(pOPortS1_4->m_bIsOutPort);
	BISPASSERT(pOPortS1_4->m_bIsAcitve);
	BISPASSERT(pOPortS1_4->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS1_4->m_nVirNetNo == 3);
	BISPASSERT(0==pOPortS1_4->m_mapLabelReffrence.size());
	BISPASSERT(0==pOPortS1_4->m_vectPathID.size());	

	//
	pTPortS1_3toS2_3 = DTSVNMGetPort(0x01010101,3);
	BISPASSERT(NULL!=pTPortS1_3toS2_3);
	BISPASSERT(!pTPortS1_3toS2_3->m_bIsOutPort);
	BISPASSERT(!pTPortS1_3toS2_3->m_bIsAcitve);
	BISPASSERT(pTPortS1_3toS2_3->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_3toS2_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pTPortS1_3toS2_3->m_mapLabelReffrence.size());
// 	BISPASSERT(2==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
// 	BISPASSERT(2==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabel]);
	BISPASSERT(0==pTPortS1_3toS2_3->m_vectPathID.size());	
// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[0]);
// 	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[1]);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[2]);
// 	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[3]);

	//
	pTPortS1_2toS4_2 = DTSVNMGetPort(0x01010101,2);
	BISPASSERT(NULL!=pTPortS1_2toS4_2);
	BISPASSERT(!pTPortS1_2toS4_2->m_bIsOutPort);
	BISPASSERT(!pTPortS1_2toS4_2->m_bIsAcitve);
	BISPASSERT(pTPortS1_2toS4_2->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_2toS4_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pTPortS1_2toS4_2->m_mapLabelReffrence.size());
//	BISPASSERT(1==pTPortS1_2toS4_2->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pTPortS1_2toS4_2->m_mapLabelReffrence.count(pVirNet_2->m_nVirLabelRef));
	BISPASSERT(0==pTPortS1_2toS4_2->m_vectPathID.size());	
//	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[0]);
//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[0]);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[2]);
// 	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[3]);

	//////////////////////////////////////////////////////////////////////////
	//Path
	BISPASSERT(0==DTSVNMGetPathCount());

	//////////////////////////////////////////////////////////////////////////
	//调用，路径计算2次，删除10次，建立0次
	BISPASSERT(22==nLogcountaft-nLogcountBef);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-22);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(5==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_DEL==pCallParam->m_nFTAction);
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-21);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-20);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-19);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-18);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-17);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-16);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(2==pCallParam->m_nVNetLabel);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(1==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-15);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-14);
	BISPASSERT(VNM_FUNC_CALL_TOPO_CONCULATE==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(2==pCallParam->m_nVirNetNo);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(0x04040404==pCallParam->m_dwOppoLoopBack);
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-13);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(1==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-12);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-11);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-10);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-9);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(2==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-8);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-7);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x04040404==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(3==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-6);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-5);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-4);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-3);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(1==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-2);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	BISPASSERT(VNM_FUNC_CALL_TOPO_CONCULATE==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(0x01010101==pCallParam->m_dwOppoLoopBack);

	return 0;
}


#endif //DTS_TEST




