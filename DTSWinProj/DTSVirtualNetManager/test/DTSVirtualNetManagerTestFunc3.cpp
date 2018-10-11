/********************************************************************
	created:	2018/04/11	15:52
	filename: 	DTSVirtualNetManagerTestFunc3.cpp
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

int DTSVNMTestVNet_1StatsActiveDelPortTest5()
{
	//VNet1删除的S1_1端口
	//删除S1_1端口
	//删除两条Path
	//交换3次：出端口S1_1移出VNet1，中继端口S1_3，S2_3移出VNet1

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
//	LPDTSVNMPath pVNet1Path2to1,pVNet1Path3to2,pVNet1Path3to1;
	LPDTSVNMPath pVNet1Path3to2;
	LPDTSVNMPath pVNet2Path4to2,pVNet2Path3to2,pVNet2Path3to4;
	//
	int nLogcountBef,nLogcountaft,nRet;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";

	//////////////////////////////////////////////////////////////////////////
	//执行
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMDelOutPort(1,0x01010101,1);
	BISPASSERT(VNM_RESULT_OK==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	//----------------------------------------------------------------------------
	//虚网列表
	pVirNet_1 = DTSVNMGetVirNetByNo(1);
	BISPASSERT(NULL!=pVirNet_1);
	BISPASSERT(pVirNet_1->m_nVirNetNo==1);
	BISPASSERT(pVirNet_1->m_nVirLabelRef==1);
	BISPASSERT(DTS_VNM_STATS_ACTIVE == pVirNet_1->m_nStats);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),1);
	BISPASSERT(0==strcmp(szTmp,pVirNet_1->m_szVirNetName));
	BISPASSERT(2==pVirNet_1->m_mapOutPort.size());
	BISPASSERT(0==pVirNet_1->m_mapOutPort.count(0x01010101));
	BISPASSERT(1==pVirNet_1->m_mapOutPort.count(0x02020202));
	BISPASSERT(1==pVirNet_1->m_mapOutPort.count(0x03030303));

//	BISPASSERT(1==pVirNet_1->m_mapOutPort[0x01010101]);
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
	//0x04040404 3个端口不变
	pOPortS4_1 = DTSVNMGetPort(0x04040404,1);
	BISPASSERT(NULL!=pOPortS4_1);
	BISPASSERT(pOPortS4_1->m_bIsOutPort);
	BISPASSERT(pOPortS4_1->m_bIsAcitve);
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

	pTPortS4_2toS1_2 = DTSVNMGetPort(0x04040404,2);
	BISPASSERT(NULL!=pTPortS4_2toS1_2);
	BISPASSERT(!pTPortS4_2toS1_2->m_bIsOutPort);
	BISPASSERT(pTPortS4_2toS1_2->m_bIsAcitve);
	BISPASSERT(pTPortS4_2toS1_2->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_2toS1_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS4_2toS1_2->m_vectPathID.size());
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS4_2toS1_2->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_nVirNetNo==2);

	pTPortS4_3toS3_3 = DTSVNMGetPort(0x04040404,3);
	BISPASSERT(NULL!=pTPortS4_3toS3_3);
	BISPASSERT(!pTPortS4_3toS3_3->m_bIsOutPort);
	BISPASSERT(pTPortS4_3toS3_3->m_bIsAcitve);
	BISPASSERT(pTPortS4_3toS3_3->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_3toS3_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS4_3toS3_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS4_3toS3_3->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS4_3toS3_3->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to4->m_nPathID == pTPortS4_3toS3_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to4->m_nVirNetNo==2);

	//0x03030303 4个端口
	//不变
	pOPortS3_5 = DTSVNMGetPort(0x03030303,5);
	BISPASSERT(NULL!=pOPortS3_5);	
	BISPASSERT(pOPortS3_5->m_bIsOutPort);
	BISPASSERT(pOPortS3_5->m_bIsAcitve);
	BISPASSERT(pOPortS3_5->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_5->m_nVirNetNo == 2);
	BISPASSERT(1==pOPortS3_5->m_mapLabelReffrence.size());
	BISPASSERT(2==pOPortS3_5->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(2==pOPortS3_5->m_vectPathID.size());
	//	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS3_5->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to4->m_nPathID == pOPortS3_5->m_vectPathID[1]);
	pVNet2Path3to2 = DTSVNMGetPathByID(pOPortS3_5->m_vectPathID[0]);
	BISPASSERT(NULL!=pVNet2Path3to2);
	BISPASSERT(2==pVNet2Path3to2->m_nVirNetNo);

	//减少Label_1的一个引用计数，PathID减少一个
	pOPortS3_1 = DTSVNMGetPort(0x03030303,1);
	BISPASSERT(NULL!=pOPortS3_1);	
	BISPASSERT(pOPortS3_1->m_bIsOutPort);
	BISPASSERT(pOPortS3_1->m_bIsAcitve);
	BISPASSERT(pOPortS3_1->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_1->m_nVirNetNo == 1);
	BISPASSERT(1==pOPortS3_1->m_mapLabelReffrence.size());
	BISPASSERT(1==pOPortS3_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(1==pOPortS3_1->m_vectPathID.size());
	//	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS3_1->m_vectPathID[1]);
	pVNet1Path3to2 = DTSVNMGetPathByID(pOPortS3_1->m_vectPathID[0]);
	BISPASSERT(NULL!=pVNet1Path3to2);
	BISPASSERT(1==pVNet1Path3to2->m_nVirNetNo);
	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
// 	pVNet1Path3to1 = DTSVNMGetPath(pOPortS3_1->m_vectPathID[0]);
// 	BISPASSERT(NULL!=pVNet1Path3to1);
//	BISPASSERT(1==pVNet1Path3to1->m_nVirNetNo);
	
	//变化
	pTPortS3_2toS2_2 = DTSVNMGetPort(0x03030303,2);
	BISPASSERT(NULL!=pTPortS3_2toS2_2);
	BISPASSERT(!pTPortS3_2toS2_2->m_bIsOutPort);
	BISPASSERT(pTPortS3_2toS2_2->m_bIsAcitve);
	BISPASSERT(pTPortS3_2toS2_2->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS3_2toS2_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(2==pTPortS3_2toS2_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	//减1
	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	//减1
	BISPASSERT(2==pTPortS3_2toS2_2->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[1]);
	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);
//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);

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
	//变化
	pOPortS2_1 = DTSVNMGetPort(0x02020202,1);
	BISPASSERT(NULL!=pOPortS2_1);
	BISPASSERT(pOPortS2_1->m_bIsOutPort);
	BISPASSERT(pOPortS2_1->m_bIsAcitve);
	BISPASSERT(pOPortS2_1->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS2_1->m_nVirNetNo == 1);
	BISPASSERT(1==pOPortS2_1->m_mapLabelReffrence.size());
	//减1
	BISPASSERT(1==pOPortS2_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	//减1
	BISPASSERT(1==pOPortS2_1->m_vectPathID.size());
	pVNet1Path3to2 = DTSVNMGetPathByID(pOPortS2_1->m_vectPathID[0]);
	BISPASSERT(NULL!=pVNet1Path3to2);
 	BISPASSERT(1==pVNet1Path3to2->m_nVirNetNo);
	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
// 	pVNet1Path2to1 = DTSVNMGetPath(pOPortS2_1->m_vectPathID[0]);
//	BISPASSERT(NULL!=pVNet1Path2to1);
//	BISPASSERT(1==pVNet1Path2to1->m_nVirNetNo);
	//	BISPASSERT(pPath2to1->m_nPathID == pOPortS2_1->m_vectPathID[0]);

	//不变
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

	//变化
	pTPortS2_3toS1_3 = DTSVNMGetPort(0x02020202,3);
	BISPASSERT(NULL!=pTPortS2_3toS1_3);
	BISPASSERT(!pTPortS2_3toS1_3->m_bIsOutPort);
	BISPASSERT(pTPortS2_3toS1_3->m_bIsAcitve);
	BISPASSERT(pTPortS2_3toS1_3->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_3toS1_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS2_3toS1_3->m_mapLabelReffrence.size());
//	BISPASSERT(2==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pTPortS2_3toS1_3->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(1==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	//影响3-1、2-1、4-2 3条路径
	//减2
	BISPASSERT(1==pTPortS2_3toS1_3->m_vectPathID.size());
//	pVNet1Path3to1 = 
// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[1]);
// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_dwBeginSwitcherLoopBack == 0x04040404);
	BISPASSERT(pVNet2Path4to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(2==pVNet2Path4to2->m_nVirNetNo);

	pTPortS2_2toS3_2 = DTSVNMGetPort(0x02020202,2);
	BISPASSERT(NULL!=pTPortS2_2toS3_2);
	BISPASSERT(!pTPortS2_2toS3_2->m_bIsOutPort);
	BISPASSERT(pTPortS2_2toS3_2->m_bIsAcitve);
	BISPASSERT(pTPortS2_2toS3_2->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_2toS3_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(2==pTPortS2_2toS3_2->m_mapLabelReffrence.size());
	//减1
	BISPASSERT(1==pTPortS2_2toS3_2->m_mapLabelReffrence[pVirNet_1->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS2_2toS3_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	//减1
	BISPASSERT(2==pTPortS2_2toS3_2->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[1]);
	BISPASSERT(pVNet2Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
	BISPASSERT(pVNet2Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(2==pVNet2Path3to2->m_nVirNetNo);
	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[0]);
	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(1==pVNet1Path3to2->m_nVirNetNo);

//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[0]);

	//0x01010101 4个端口
	//Deactive
	pOPortS1_1 = DTSVNMGetPort(0x01010101,1);
	BISPASSERT(NULL!=pOPortS1_1);
	BISPASSERT(pOPortS1_1->m_bIsOutPort);
	BISPASSERT(!pOPortS1_1->m_bIsAcitve);
	BISPASSERT(pOPortS1_1->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS1_1->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pOPortS1_1->m_mapLabelReffrence.size());
	BISPASSERT(0==pOPortS1_1->m_vectPathID.size());	
// 	BISPASSERT(2==pOPortS1_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
// 	BISPASSERT(2==pOPortS1_1->m_vectPathID.size());	
// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pOPortS1_1->m_vectPathID[0]);
// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pOPortS1_1->m_vectPathID[1]);

	//不变
	pOPortS1_4 = DTSVNMGetPort(0x01010101,4);
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

	//变化
	pTPortS1_3toS2_3 = DTSVNMGetPort(0x01010101,3);
	BISPASSERT(NULL!=pTPortS1_3toS2_3);
	BISPASSERT(!pTPortS1_3toS2_3->m_bIsOutPort);
	BISPASSERT(pTPortS1_3toS2_3->m_bIsAcitve);
	BISPASSERT(pTPortS1_3toS2_3->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_3toS2_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	//减1
	BISPASSERT(1==pTPortS1_3toS2_3->m_mapLabelReffrence.size());
//	BISPASSERT(2==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pTPortS1_3toS2_3->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(1==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	//减2
	BISPASSERT(1==pTPortS1_3toS2_3->m_vectPathID.size());	
// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[0]);
// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[1]);
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_dwBeginSwitcherLoopBack == 0x04040404);
	BISPASSERT(pVNet2Path4to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(2==pVNet2Path4to2->m_nVirNetNo);

	//不变
	pTPortS1_2toS4_2 = DTSVNMGetPort(0x01010101,2);
	BISPASSERT(NULL!=pTPortS1_2toS4_2);
	BISPASSERT(!pTPortS1_2toS4_2->m_bIsOutPort);
	BISPASSERT(pTPortS1_2toS4_2->m_bIsAcitve);
	BISPASSERT(pTPortS1_2toS4_2->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_2toS4_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS1_2toS4_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS1_2toS4_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS1_2toS4_2->m_vectPathID.size());	
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[0]);
	//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[1]);

	//-----------------------------------------------------------------------------
	//路径列表
	//减2
	BISPASSERT(4==DTSVNMGetPathCount());
	//VNet1
// 	BISPASSERT(pVNet1Path3to1->m_dwBeginSwitcherLoopBack == 0x03030303);
// 	BISPASSERT(pVNet1Path3to1->m_dwEndSwitcherLoopBack == 0x01010101);
// 	BISPASSERT(pVNet1Path3to1->m_nVirNetNo == 1);
// 	BISPASSERT(3==pVNet1Path3to1->m_vectLabelSwitch.size());

	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(pVNet1Path3to2->m_nVirNetNo == 1);
	BISPASSERT(2==pVNet1Path3to2->m_vectLabelSwitch.size());

// 	BISPASSERT(pVNet1Path2to1->m_dwBeginSwitcherLoopBack == 0x02020202);
// 	BISPASSERT(pVNet1Path2to1->m_dwEndSwitcherLoopBack == 0x01010101);
// 	BISPASSERT(pVNet1Path2to1->m_nVirNetNo == 1);
// 	BISPASSERT(2==pVNet1Path2to1->m_vectLabelSwitch.size());

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
	//删除VNet1的S1_1端口，外部调用
	//[1][S_1][3]--[3][S_2][1]，中继2
	//[1][S_1][3]--[3][S_2][2]--[2][S_3][1]，出口1，交换2，中继2
	BISPASSERT(14==nLogcountaft-nLogcountBef);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-14);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);
	
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-13);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-12);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-11);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-10);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-9);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-8);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet_1->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(3==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-7);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-6);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(pVirNet_1->m_nVirLabelRef==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(2==pCallParam->m_nOppoPortIndex);
	BISPASSERT(VNM_FT_ACT_SWITCH_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-5);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-4);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(3==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-3);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-2);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x01010101==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(1==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);


	return 0;
}


int DTSVNMTestVNet_1StatsActiveDelPortTest6()
{
	//删除VNet1的S2_1端口
	//删除S1_2端口
	//删除1条Path
	//交换4次：出端口S2_1、S3_1移出VNet1，中继端口S2_2，S3_2移出VNet1

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
//	LPDTSVNMPath pVNet1Path2to1,pVNet1Path3to2,pVNet1Path3to1;
	LPDTSVNMPath pVNet2Path4to2,pVNet2Path3to2,pVNet2Path3to4;
	//
	int nLogcountBef,nLogcountaft,nRet;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";

	//////////////////////////////////////////////////////////////////////////
	//执行
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMDelOutPort(1,0x02020202,1);
	BISPASSERT(VNM_RESULT_OK==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	//----------------------------------------------------------------------------
	//虚网列表
	pVirNet_1 = DTSVNMGetVirNetByNo(1);
	BISPASSERT(NULL!=pVirNet_1);
	BISPASSERT(pVirNet_1->m_nVirNetNo==1);
	BISPASSERT(pVirNet_1->m_nVirLabelRef==1);
	BISPASSERT(DTS_VNM_STATS_ONE_OUT_PORT == pVirNet_1->m_nStats);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),1);
	BISPASSERT(0==strcmp(szTmp,pVirNet_1->m_szVirNetName));
	//减少
	BISPASSERT(1==pVirNet_1->m_mapOutPort.size());
	BISPASSERT(0==pVirNet_1->m_mapOutPort.count(0x01010101));
	BISPASSERT(0==pVirNet_1->m_mapOutPort.count(0x02020202));
	BISPASSERT(1==pVirNet_1->m_mapOutPort.count(0x03030303));

//	BISPASSERT(1==pVirNet_1->m_mapOutPort[0x01010101]);
//	BISPASSERT(1==pVirNet_1->m_mapOutPort[0x02020202]);
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
	//0x04040404 3个端口不变
	pOPortS4_1 = DTSVNMGetPort(0x04040404,1);
	BISPASSERT(NULL!=pOPortS4_1);
	BISPASSERT(pOPortS4_1->m_bIsOutPort);
	BISPASSERT(pOPortS4_1->m_bIsAcitve);
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

	pTPortS4_2toS1_2 = DTSVNMGetPort(0x04040404,2);
	BISPASSERT(NULL!=pTPortS4_2toS1_2);
	BISPASSERT(!pTPortS4_2toS1_2->m_bIsOutPort);
	BISPASSERT(pTPortS4_2toS1_2->m_bIsAcitve);
	BISPASSERT(pTPortS4_2toS1_2->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_2toS1_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS4_2toS1_2->m_vectPathID.size());
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS4_2toS1_2->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_nVirNetNo==2);

	pTPortS4_3toS3_3 = DTSVNMGetPort(0x04040404,3);
	BISPASSERT(NULL!=pTPortS4_3toS3_3);
	BISPASSERT(!pTPortS4_3toS3_3->m_bIsOutPort);
	BISPASSERT(pTPortS4_3toS3_3->m_bIsAcitve);
	BISPASSERT(pTPortS4_3toS3_3->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_3toS3_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS4_3toS3_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS4_3toS3_3->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS4_3toS3_3->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to4->m_nPathID == pTPortS4_3toS3_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to4->m_nVirNetNo==2);

	//0x03030303 4个端口
	//不变
	pOPortS3_5 = DTSVNMGetPort(0x03030303,5);
	BISPASSERT(NULL!=pOPortS3_5);	
	BISPASSERT(pOPortS3_5->m_bIsOutPort);
	BISPASSERT(pOPortS3_5->m_bIsAcitve);
	BISPASSERT(pOPortS3_5->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_5->m_nVirNetNo == 2);
	BISPASSERT(1==pOPortS3_5->m_mapLabelReffrence.size());
	BISPASSERT(2==pOPortS3_5->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(2==pOPortS3_5->m_vectPathID.size());
	//	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS3_5->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to4->m_nPathID == pOPortS3_5->m_vectPathID[1]);
	pVNet2Path3to2 = DTSVNMGetPathByID(pOPortS3_5->m_vectPathID[0]);
	BISPASSERT(NULL!=pVNet2Path3to2);
	BISPASSERT(2==pVNet2Path3to2->m_nVirNetNo);

	//S3_1应该无Label引用和Path列表
	pOPortS3_1 = DTSVNMGetPort(0x03030303,1);
	BISPASSERT(NULL!=pOPortS3_1);	
	BISPASSERT(pOPortS3_1->m_bIsOutPort);
	BISPASSERT(pOPortS3_1->m_bIsAcitve);
	BISPASSERT(pOPortS3_1->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_1->m_nVirNetNo == 1);
	BISPASSERT(0==pOPortS3_1->m_mapLabelReffrence.size());
	//减1
	BISPASSERT(0==pOPortS3_1->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	//减1
	BISPASSERT(0==pOPortS3_1->m_vectPathID.size());
//	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS3_1->m_vectPathID[1]);
//	pVNet1Path3to2 = DTSVNMGetPath(pOPortS3_1->m_vectPathID[0]);
//	BISPASSERT(NULL!=pVNet1Path3to2);
// 	BISPASSERT(1==pVNet1Path3to2->m_nVirNetNo);
// 	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
// 	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
	// 	pVNet1Path3to1 = DTSVNMGetPath(pOPortS3_1->m_vectPathID[0]);
	// 	BISPASSERT(NULL!=pVNet1Path3to1);
	//	BISPASSERT(1==pVNet1Path3to1->m_nVirNetNo);

	//变化，只剩下VNet_2的数据	
	pTPortS3_2toS2_2 = DTSVNMGetPort(0x03030303,2);
	BISPASSERT(NULL!=pTPortS3_2toS2_2);
	BISPASSERT(!pTPortS3_2toS2_2->m_bIsOutPort);
	BISPASSERT(pTPortS3_2toS2_2->m_bIsAcitve);
	BISPASSERT(pTPortS3_2toS2_2->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS3_2toS2_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	//减1
	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	//减1
//	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	//减1
	BISPASSERT(1==pTPortS3_2toS2_2->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);
//	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);
	//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);

	//不变
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
	//变化
	pOPortS2_1 = DTSVNMGetPort(0x02020202,1);
	BISPASSERT(NULL!=pOPortS2_1);
	BISPASSERT(pOPortS2_1->m_bIsOutPort);
	BISPASSERT(!pOPortS2_1->m_bIsAcitve);
	BISPASSERT(pOPortS2_1->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS2_1->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pOPortS2_1->m_mapLabelReffrence.size());
	//减1
//	BISPASSERT(1==pOPortS2_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	//减1
	BISPASSERT(0==pOPortS2_1->m_vectPathID.size());
// 	pVNet1Path3to2 = DTSVNMGetPath(pOPortS2_1->m_vectPathID[0]);
// 	BISPASSERT(NULL!=pVNet1Path3to2);
// 	BISPASSERT(1==pVNet1Path3to2->m_nVirNetNo);
// 	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
// 	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
	// 	pVNet1Path2to1 = DTSVNMGetPath(pOPortS2_1->m_vectPathID[0]);
	//	BISPASSERT(NULL!=pVNet1Path2to1);
	//	BISPASSERT(1==pVNet1Path2to1->m_nVirNetNo);
	//	BISPASSERT(pPath2to1->m_nPathID == pOPortS2_1->m_vectPathID[0]);

	//不变
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

	//不变
	pTPortS2_3toS1_3 = DTSVNMGetPort(0x02020202,3);
	BISPASSERT(NULL!=pTPortS2_3toS1_3);
	BISPASSERT(!pTPortS2_3toS1_3->m_bIsOutPort);
	BISPASSERT(pTPortS2_3toS1_3->m_bIsAcitve);
	BISPASSERT(pTPortS2_3toS1_3->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_3toS1_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS2_3toS1_3->m_mapLabelReffrence.size());
	//	BISPASSERT(2==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pTPortS2_3toS1_3->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(1==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	//影响3-1、2-1、4-2 3条路径
	//减2
	BISPASSERT(1==pTPortS2_3toS1_3->m_vectPathID.size());
	//	pVNet1Path3to1 = 
	// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[1]);
	// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_dwBeginSwitcherLoopBack == 0x04040404);
	BISPASSERT(pVNet2Path4to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(2==pVNet2Path4to2->m_nVirNetNo);

	//变化,只剩VNet2的数据
	pTPortS2_2toS3_2 = DTSVNMGetPort(0x02020202,2);
	BISPASSERT(NULL!=pTPortS2_2toS3_2);
	BISPASSERT(!pTPortS2_2toS3_2->m_bIsOutPort);
	BISPASSERT(pTPortS2_2toS3_2->m_bIsAcitve);
	BISPASSERT(pTPortS2_2toS3_2->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_2toS3_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS2_2toS3_2->m_mapLabelReffrence.size());
	//减1
	BISPASSERT(0==pTPortS2_2toS3_2->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(1==pTPortS2_2toS3_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	//减1
	BISPASSERT(1==pTPortS2_2toS3_2->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
	BISPASSERT(pVNet2Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(2==pVNet2Path3to2->m_nVirNetNo);
// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[0]);
// 	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
// 	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
// 	BISPASSERT(1==pVNet1Path3to2->m_nVirNetNo);

	//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[0]);

	//0x01010101 4个端口
	//Deactive
	pOPortS1_1 = DTSVNMGetPort(0x01010101,1);
	BISPASSERT(NULL!=pOPortS1_1);
	BISPASSERT(pOPortS1_1->m_bIsOutPort);
	BISPASSERT(!pOPortS1_1->m_bIsAcitve);
	BISPASSERT(pOPortS1_1->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS1_1->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pOPortS1_1->m_mapLabelReffrence.size());
	BISPASSERT(0==pOPortS1_1->m_vectPathID.size());	
	// 	BISPASSERT(2==pOPortS1_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	// 	BISPASSERT(2==pOPortS1_1->m_vectPathID.size());	
	// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pOPortS1_1->m_vectPathID[0]);
	// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pOPortS1_1->m_vectPathID[1]);

	//不变
	pOPortS1_4 = DTSVNMGetPort(0x01010101,4);
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

	//不变
	pTPortS1_3toS2_3 = DTSVNMGetPort(0x01010101,3);
	BISPASSERT(NULL!=pTPortS1_3toS2_3);
	BISPASSERT(!pTPortS1_3toS2_3->m_bIsOutPort);
	BISPASSERT(pTPortS1_3toS2_3->m_bIsAcitve);
	BISPASSERT(pTPortS1_3toS2_3->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_3toS2_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS1_3toS2_3->m_mapLabelReffrence.size());
	//	BISPASSERT(2==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pTPortS1_3toS2_3->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(1==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS1_3toS2_3->m_vectPathID.size());	
	// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[0]);
	// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[1]);
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_dwBeginSwitcherLoopBack == 0x04040404);
	BISPASSERT(pVNet2Path4to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(2==pVNet2Path4to2->m_nVirNetNo);

	//不变
	pTPortS1_2toS4_2 = DTSVNMGetPort(0x01010101,2);
	BISPASSERT(NULL!=pTPortS1_2toS4_2);
	BISPASSERT(!pTPortS1_2toS4_2->m_bIsOutPort);
	BISPASSERT(pTPortS1_2toS4_2->m_bIsAcitve);
	BISPASSERT(pTPortS1_2toS4_2->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_2toS4_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS1_2toS4_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS1_2toS4_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS1_2toS4_2->m_vectPathID.size());	
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[0]);
	//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[1]);

	//-----------------------------------------------------------------------------
	//路径列表
	//减1
	BISPASSERT(3==DTSVNMGetPathCount());
	//VNet1
	// 	BISPASSERT(pVNet1Path3to1->m_dwBeginSwitcherLoopBack == 0x03030303);
	// 	BISPASSERT(pVNet1Path3to1->m_dwEndSwitcherLoopBack == 0x01010101);
	// 	BISPASSERT(pVNet1Path3to1->m_nVirNetNo == 1);
	// 	BISPASSERT(3==pVNet1Path3to1->m_vectLabelSwitch.size());

// 	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
// 	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
// 	BISPASSERT(pVNet1Path3to2->m_nVirNetNo == 1);
// 	BISPASSERT(2==pVNet1Path3to2->m_vectLabelSwitch.size());

	// 	BISPASSERT(pVNet1Path2to1->m_dwBeginSwitcherLoopBack == 0x02020202);
	// 	BISPASSERT(pVNet1Path2to1->m_dwEndSwitcherLoopBack == 0x01010101);
	// 	BISPASSERT(pVNet1Path2to1->m_nVirNetNo == 1);
	// 	BISPASSERT(2==pVNet1Path2to1->m_vectLabelSwitch.size());

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
	//外部调用 //删除VNet1的S2_1端口
	//中继端口删除2次，出口删除2次
	BISPASSERT(8==nLogcountaft-nLogcountBef);
		
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-8);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(1==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-7);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-6);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x03030303==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-5);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-4);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(2==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_TRUNK_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-3);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-2);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND==pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(1==pCallParam->m_nVirNetNo);
	BISPASSERT(1==pCallParam->m_nVNetLabel);
	BISPASSERT(0x02020202==pCallParam->m_dwSwitcherLoopBack);
	BISPASSERT(1==pCallParam->m_nPortIndex);
	BISPASSERT(VNM_FT_ACT_OUT_PORT_DEL==pCallParam->m_nFTAction);

	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);

	return 0;
}

int DTSVNMTestVNet_1StatsOneOutPortDelPortTest7()
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
//	LPDTSVNMPath pVNet1Path2to1,pVNet1Path3to2,pVNet1Path3to1;
	LPDTSVNMPath pVNet2Path4to2,pVNet2Path3to2,pVNet2Path3to4;
	//
//	int nLogcountBef,nLogcountaft;
	int nRet;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";

	//////////////////////////////////////////////////////////////////////////
	//执行
	//删除VNet1的S3_1端口
	DTSVNMTestVNetStatsOneOutPortDelPort(1,0x03030303,1);
	
	//----------------------------------------------------------------------------
	//虚网列表
	pVirNet_1 = DTSVNMGetVirNetByNo(1);
	BISPASSERT(NULL!=pVirNet_1);
	BISPASSERT(pVirNet_1->m_nVirNetNo==1);
	BISPASSERT(pVirNet_1->m_nVirLabelRef==1);
	BISPASSERT(DTS_VNM_STATS_NONE == pVirNet_1->m_nStats);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),1);
	BISPASSERT(0==strcmp(szTmp,pVirNet_1->m_szVirNetName));
	//减少
	BISPASSERT(0==pVirNet_1->m_mapOutPort.size());
	BISPASSERT(0==pVirNet_1->m_mapOutPort.count(0x01010101));
	BISPASSERT(0==pVirNet_1->m_mapOutPort.count(0x02020202));
	BISPASSERT(0==pVirNet_1->m_mapOutPort.count(0x03030303));

	//	BISPASSERT(1==pVirNet_1->m_mapOutPort[0x01010101]);
	//	BISPASSERT(1==pVirNet_1->m_mapOutPort[0x02020202]);
	//	BISPASSERT(1==pVirNet_1->m_mapOutPort[0x03030303]);


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
	//0x04040404 3个端口不变
	pOPortS4_1 = DTSVNMGetPort(0x04040404,1);
	BISPASSERT(NULL!=pOPortS4_1);
	BISPASSERT(pOPortS4_1->m_bIsOutPort);
	BISPASSERT(pOPortS4_1->m_bIsAcitve);
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

	pTPortS4_2toS1_2 = DTSVNMGetPort(0x04040404,2);
	BISPASSERT(NULL!=pTPortS4_2toS1_2);
	BISPASSERT(!pTPortS4_2toS1_2->m_bIsOutPort);
	BISPASSERT(pTPortS4_2toS1_2->m_bIsAcitve);
	BISPASSERT(pTPortS4_2toS1_2->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_2toS1_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS4_2toS1_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS4_2toS1_2->m_vectPathID.size());
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS4_2toS1_2->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_nVirNetNo==2);

	pTPortS4_3toS3_3 = DTSVNMGetPort(0x04040404,3);
	BISPASSERT(NULL!=pTPortS4_3toS3_3);
	BISPASSERT(!pTPortS4_3toS3_3->m_bIsOutPort);
	BISPASSERT(pTPortS4_3toS3_3->m_bIsAcitve);
	BISPASSERT(pTPortS4_3toS3_3->m_dwSwitcherLoopBack == pSwitcher4->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS4_3toS3_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS4_3toS3_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS4_3toS3_3->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS4_3toS3_3->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to4->m_nPathID == pTPortS4_3toS3_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to4->m_nVirNetNo==2);

	//0x03030303 4个端口
	//不变
	pOPortS3_5 = DTSVNMGetPort(0x03030303,5);
	BISPASSERT(NULL!=pOPortS3_5);	
	BISPASSERT(pOPortS3_5->m_bIsOutPort);
	BISPASSERT(pOPortS3_5->m_bIsAcitve);
	BISPASSERT(pOPortS3_5->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_5->m_nVirNetNo == 2);
	BISPASSERT(1==pOPortS3_5->m_mapLabelReffrence.size());
	BISPASSERT(2==pOPortS3_5->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(2==pOPortS3_5->m_vectPathID.size());
	//	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS3_5->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to4->m_nPathID == pOPortS3_5->m_vectPathID[1]);
	pVNet2Path3to2 = DTSVNMGetPathByID(pOPortS3_5->m_vectPathID[0]);
	BISPASSERT(NULL!=pVNet2Path3to2);
	BISPASSERT(2==pVNet2Path3to2->m_nVirNetNo);

	//S3_1应该无Label引用和Path列表
	//Deactive
	pOPortS3_1 = DTSVNMGetPort(0x03030303,1);
	BISPASSERT(NULL!=pOPortS3_1);	
	BISPASSERT(pOPortS3_1->m_bIsOutPort);
	BISPASSERT(!pOPortS3_1->m_bIsAcitve);
	BISPASSERT(pOPortS3_1->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS3_1->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pOPortS3_1->m_mapLabelReffrence.size());
	BISPASSERT(0==pOPortS3_1->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(0==pOPortS3_1->m_vectPathID.size());
	//	BISPASSERT(pVNet2Path3to2->m_nPathID == pOPortS3_1->m_vectPathID[1]);
	//	pVNet1Path3to2 = DTSVNMGetPath(pOPortS3_1->m_vectPathID[0]);
	//	BISPASSERT(NULL!=pVNet1Path3to2);
	// 	BISPASSERT(1==pVNet1Path3to2->m_nVirNetNo);
	// 	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
	// 	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
	// 	pVNet1Path3to1 = DTSVNMGetPath(pOPortS3_1->m_vectPathID[0]);
	// 	BISPASSERT(NULL!=pVNet1Path3to1);
	//	BISPASSERT(1==pVNet1Path3to1->m_nVirNetNo);

	//变化，只剩下VNet_2的数据	
	pTPortS3_2toS2_2 = DTSVNMGetPort(0x03030303,2);
	BISPASSERT(NULL!=pTPortS3_2toS2_2);
	BISPASSERT(!pTPortS3_2toS2_2->m_bIsOutPort);
	BISPASSERT(pTPortS3_2toS2_2->m_bIsAcitve);
	BISPASSERT(pTPortS3_2toS2_2->m_dwSwitcherLoopBack == pSwitcher3->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS3_2toS2_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	//减1
	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	//减1
	//	BISPASSERT(1==pTPortS3_2toS2_2->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	//减1
	BISPASSERT(1==pTPortS3_2toS2_2->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);
	//	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);
	//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS3_2toS2_2->m_vectPathID[0]);

	//不变
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
	//变化
	pOPortS2_1 = DTSVNMGetPort(0x02020202,1);
	BISPASSERT(NULL!=pOPortS2_1);
	BISPASSERT(pOPortS2_1->m_bIsOutPort);
	BISPASSERT(!pOPortS2_1->m_bIsAcitve);
	BISPASSERT(pOPortS2_1->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS2_1->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pOPortS2_1->m_mapLabelReffrence.size());
	//减1
	//	BISPASSERT(1==pOPortS2_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	//减1
	BISPASSERT(0==pOPortS2_1->m_vectPathID.size());
	// 	pVNet1Path3to2 = DTSVNMGetPath(pOPortS2_1->m_vectPathID[0]);
	// 	BISPASSERT(NULL!=pVNet1Path3to2);
	// 	BISPASSERT(1==pVNet1Path3to2->m_nVirNetNo);
	// 	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
	// 	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
	// 	pVNet1Path2to1 = DTSVNMGetPath(pOPortS2_1->m_vectPathID[0]);
	//	BISPASSERT(NULL!=pVNet1Path2to1);
	//	BISPASSERT(1==pVNet1Path2to1->m_nVirNetNo);
	//	BISPASSERT(pPath2to1->m_nPathID == pOPortS2_1->m_vectPathID[0]);

	//不变
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

	//不变
	pTPortS2_3toS1_3 = DTSVNMGetPort(0x02020202,3);
	BISPASSERT(NULL!=pTPortS2_3toS1_3);
	BISPASSERT(!pTPortS2_3toS1_3->m_bIsOutPort);
	BISPASSERT(pTPortS2_3toS1_3->m_bIsAcitve);
	BISPASSERT(pTPortS2_3toS1_3->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_3toS1_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS2_3toS1_3->m_mapLabelReffrence.size());
	//	BISPASSERT(2==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pTPortS2_3toS1_3->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(1==pTPortS2_3toS1_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	//影响3-1、2-1、4-2 3条路径
	//减2
	BISPASSERT(1==pTPortS2_3toS1_3->m_vectPathID.size());
	//	pVNet1Path3to1 = 
	// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[1]);
	// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS2_3toS1_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_dwBeginSwitcherLoopBack == 0x04040404);
	BISPASSERT(pVNet2Path4to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(2==pVNet2Path4to2->m_nVirNetNo);

	//变化,只剩VNet2的数据
	pTPortS2_2toS3_2 = DTSVNMGetPort(0x02020202,2);
	BISPASSERT(NULL!=pTPortS2_2toS3_2);
	BISPASSERT(!pTPortS2_2toS3_2->m_bIsOutPort);
	BISPASSERT(pTPortS2_2toS3_2->m_bIsAcitve);
	BISPASSERT(pTPortS2_2toS3_2->m_dwSwitcherLoopBack == pSwitcher2->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS2_2toS3_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS2_2toS3_2->m_mapLabelReffrence.size());
	//减1
	BISPASSERT(0==pTPortS2_2toS3_2->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(1==pTPortS2_2toS3_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	//减1
	BISPASSERT(1==pTPortS2_2toS3_2->m_vectPathID.size());
	BISPASSERT(pVNet2Path3to2->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[0]);
	BISPASSERT(pVNet2Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
	BISPASSERT(pVNet2Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(2==pVNet2Path3to2->m_nVirNetNo);
	// 	BISPASSERT(pVNet1Path3to2->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[0]);
	// 	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
	// 	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
	// 	BISPASSERT(1==pVNet1Path3to2->m_nVirNetNo);

	//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS2_2toS3_2->m_vectPathID[0]);

	//0x01010101 4个端口
	//Deactive
	pOPortS1_1 = DTSVNMGetPort(0x01010101,1);
	BISPASSERT(NULL!=pOPortS1_1);
	BISPASSERT(pOPortS1_1->m_bIsOutPort);
	BISPASSERT(!pOPortS1_1->m_bIsAcitve);
	BISPASSERT(pOPortS1_1->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pOPortS1_1->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(0==pOPortS1_1->m_mapLabelReffrence.size());
	BISPASSERT(0==pOPortS1_1->m_vectPathID.size());	
	// 	BISPASSERT(2==pOPortS1_1->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	// 	BISPASSERT(2==pOPortS1_1->m_vectPathID.size());	
	// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pOPortS1_1->m_vectPathID[0]);
	// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pOPortS1_1->m_vectPathID[1]);

	//不变
	pOPortS1_4 = DTSVNMGetPort(0x01010101,4);
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

	//不变
	pTPortS1_3toS2_3 = DTSVNMGetPort(0x01010101,3);
	BISPASSERT(NULL!=pTPortS1_3toS2_3);
	BISPASSERT(!pTPortS1_3toS2_3->m_bIsOutPort);
	BISPASSERT(pTPortS1_3toS2_3->m_bIsAcitve);
	BISPASSERT(pTPortS1_3toS2_3->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_3toS2_3->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS1_3toS2_3->m_mapLabelReffrence.size());
	//	BISPASSERT(2==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet_1->m_nVirLabel]);
	BISPASSERT(0==pTPortS1_3toS2_3->m_mapLabelReffrence.count(pVirNet_1->m_nVirLabelRef));
	BISPASSERT(1==pTPortS1_3toS2_3->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS1_3toS2_3->m_vectPathID.size());	
	// 	BISPASSERT(pVNet1Path2to1->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[0]);
	// 	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[1]);
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS1_3toS2_3->m_vectPathID[0]);
	BISPASSERT(pVNet2Path4to2->m_dwBeginSwitcherLoopBack == 0x04040404);
	BISPASSERT(pVNet2Path4to2->m_dwEndSwitcherLoopBack == 0x02020202);
	BISPASSERT(2==pVNet2Path4to2->m_nVirNetNo);

	//不变
	pTPortS1_2toS4_2 = DTSVNMGetPort(0x01010101,2);
	BISPASSERT(NULL!=pTPortS1_2toS4_2);
	BISPASSERT(!pTPortS1_2toS4_2->m_bIsOutPort);
	BISPASSERT(pTPortS1_2toS4_2->m_bIsAcitve);
	BISPASSERT(pTPortS1_2toS4_2->m_dwSwitcherLoopBack == pSwitcher1->m_dwSwitcherLoopBack);
	BISPASSERT(pTPortS1_2toS4_2->m_nVirNetNo == DTS_VNET_NO_NULL);
	BISPASSERT(1==pTPortS1_2toS4_2->m_mapLabelReffrence.size());
	BISPASSERT(1==pTPortS1_2toS4_2->m_mapLabelReffrence[pVirNet_2->m_nVirLabelRef]);
	BISPASSERT(1==pTPortS1_2toS4_2->m_vectPathID.size());	
	BISPASSERT(pVNet2Path4to2->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[0]);
	//	BISPASSERT(pVNet1Path3to1->m_nPathID == pTPortS1_2toS4_2->m_vectPathID[1]);
	
	//-----------------------------------------------------------------------------
	//路径列表
	//减1
	BISPASSERT(3==DTSVNMGetPathCount());
	//VNet1
// 	BISPASSERT(pVNet1Path3to1->m_dwBeginSwitcherLoopBack == 0x03030303);
// 	BISPASSERT(pVNet1Path3to1->m_dwEndSwitcherLoopBack == 0x01010101);
// 	BISPASSERT(pVNet1Path3to1->m_nVirNetNo == 1);
// 	BISPASSERT(3==pVNet1Path3to1->m_vectLabelSwitch.size());

// 	BISPASSERT(pVNet1Path3to2->m_dwBeginSwitcherLoopBack == 0x03030303);
// 	BISPASSERT(pVNet1Path3to2->m_dwEndSwitcherLoopBack == 0x02020202);
// 	BISPASSERT(pVNet1Path3to2->m_nVirNetNo == 1);
// 	BISPASSERT(2==pVNet1Path3to2->m_vectLabelSwitch.size());

// 	BISPASSERT(pVNet1Path2to1->m_dwBeginSwitcherLoopBack == 0x02020202);
// 	BISPASSERT(pVNet1Path2to1->m_dwEndSwitcherLoopBack == 0x01010101);
// 	BISPASSERT(pVNet1Path2to1->m_nVirNetNo == 1);
// 	BISPASSERT(2==pVNet1Path2to1->m_vectLabelSwitch.size());

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

	return 0;
}


#endif //DTS_TEST



