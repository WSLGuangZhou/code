/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   17:17
	filename: 	DTSVirtualNetManagerTestFunc.cpp
	author:		sl.wu
	
	purpose:	��������ģ�鵥Ԫ���ԣ�����TEST�����ֱ����Ӳ���
*********************************************************************/
//WIN32
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



LOCAL int DTSVNMTestNewVNet(UINT nVirNetNo,UINT nBandWidth)
{
	int nLogcountBef,nLogcountaft,nRet;
	int	nSwitcherCountBef,nSwitcherCountAft;
	int	nPathCountBef,nPathCountAft;
	LPDTSVirtualNet pVirNet = NULL;
	LPDTSVNMSwitcher pSwitcher = NULL;
	LPDTSVNMPort pPort = NULL;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";

	nSwitcherCountBef = DTSVNMGetSwitcherCount();
	nPathCountBef = DTSVNMGetPathCount();
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL==pVirNet);

	//////////////////////////////////////////////////////////////////////////
	//����
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMNewVirNet(nVirNetNo,nBandWidth);
	BISPASSERT(VNM_RESULT_OK==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	//----------------------------------------------------------------------------
	//�����б�
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(pVirNet->m_nVirNetNo==nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pVirNet->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nBandWidth==nBandWidth);
	BISPASSERT(DTS_VNM_STATS_NONE == pVirNet->m_nStats);
	BISPASSERT(0==pVirNet->m_mapOutPort.size());
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),nVirNetNo);
	BISPASSERT(0==strcmp(szTmp,pVirNet->m_szVirNetName));

	//-----------------------------------------------------------------------------
	//�������б�
	nSwitcherCountAft = DTSVNMGetSwitcherCount();
	BISPASSERT(nSwitcherCountAft==nSwitcherCountBef);

	//-----------------------------------------------------------------------------
	//·���б�
	nPathCountAft = DTSVNMGetPathCount();
	BISPASSERT(nPathCountAft==nPathCountBef);

	//----------------------------------------------------------------------------
	//�ⲿ����
	//���ʹ�������������
	BISPASSERT(0==nLogcountaft-nLogcountBef);
// 	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
// 	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND == pLogEntry->nMainType);
// 	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pDataBuff;
// 	BISPASSERT(VNM_FT_ACT_NEW_VIRNET == pCallParam->m_nFTAction);
// 	BISPASSERT(0==strcmp(pCallParam->m_pBuff,pVirNet->m_szVirNetName));

	return 0;
}



LOCAL int DTSVNMTestVNetStatsNoneDelPort(UINT nVirNetNo,
										UINT nSwitcherLoopBack,
										UINT nSwithcerOutPort)
{
	int nLogcountBef,nLogcountaft,nRet;
//	int nSwitcherCount;
	int	nSwitcherCountBef,nSwitcherCountAft;
	int	nPathCountBef,nPathCountAft;
//	int	nSwitcherPortCountBef,nSwitcherPortCountAft;
	int	nVNetPortCountBef,nVNetPortCountAft;
	LPDTSVirtualNet pVirNet = NULL;
	LPDTSVNMSwitcher pSwitcher = NULL;
	LPDTSVNMPort pPort = NULL;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";

	nSwitcherCountBef = DTSVNMGetSwitcherCount();
	nPathCountBef = DTSVNMGetPathCount();
//	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
//	nSwitcherPortCountBef = (NULL==pSwitcher)?0:pSwitcher->m_mapPort.size();
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	nVNetPortCountBef = (NULL==nVirNetNo)?0:pVirNet->m_mapOutPort.size();

	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMDelOutPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	BISPASSERT(VNM_RESULT_EXCEPTION==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	//----------------------------------------------------------------------------
	//�����б�
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(pVirNet->m_nVirNetNo==nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==nVirNetNo);
	BISPASSERT(DTS_VNM_STATS_NONE == pVirNet->m_nStats);
	nVNetPortCountAft=pVirNet->m_mapOutPort.size();
	BISPASSERT(0==nVNetPortCountAft-nVNetPortCountBef);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),nVirNetNo);
	BISPASSERT(0==strcmp(szTmp,pVirNet->m_szVirNetName));
	BISPASSERT(0==pVirNet->m_mapOutPort.count(nSwitcherLoopBack));
//	BISPASSERT(nSwithcerOutPort==pVirNet->m_mapOutPort[nSwitcherLoopBack]);

	//-----------------------------------------------------------------------------
	//�������б�
	nSwitcherCountAft = DTSVNMGetSwitcherCount();
//	nSwitcherCount = bIsNewSwithcer?1:0;
	BISPASSERT(0==nSwitcherCountAft-nSwitcherCountBef);
// 	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
// 	BISPASSERT(NULL!=pSwitcher);
// 	BISPASSERT(pSwitcher->m_dwSwitcherLoopBack == nSwitcherLoopBack);
// 	nSwitcherPortCountAft = pSwitcher->m_mapPort.size();
// 	BISPASSERT(1==nSwitcherPortCountAft-nSwitcherPortCountBef);

// 	//�˿�
// 	pPort = DTSVNMGetPort(nSwitcherLoopBack,nSwithcerOutPort);
// 	BISPASSERT(NULL!=pPort);
// 	//�˿ں�
// 	BISPASSERT(nSwithcerOutPort==pPort->m_nPortIndex);
// 	//����������
// 	BISPASSERT(pSwitcher==pPort->m_pSwitcher);
// 	//�Ƿ�Ϊ����
// 	BISPASSERT(pPort->m_bIsOutPort);
// 	//����ǳ��ڣ����¼��������
// 	BISPASSERT(pVirNet==pPort->m_pVirtualNet);	
// 	//�Զ˶˿�
// 	BISPASSERT(NULL==pPort->m_pOppoPort);
// 	//�漰��·��PathID�б�
// 	BISPASSERT(0==pPort->m_vectPathID.size());
// 	//��ǩ���ü���
// 	BISPASSERT(0==pPort->m_mapLabelReffrence.size());

	//-----------------------------------------------------------------------------
	//·���б�
	nPathCountAft = DTSVNMGetPathCount();
	BISPASSERT(nPathCountAft==nPathCountBef);
	//	BISPASSERT(0==DTSVNMGetPathCount());

	//----------------------------------------------------------------------------
	//�ⲿ����
	//��ʱ������Ķ˿ڣ����ⲿ����
	BISPASSERT(0==nLogcountaft-nLogcountBef);
	// 	LPDTSTESTUNITLOGENTRY pLogEntry = DTSTestUnitLogGetEntry(nLogcountaft-1);
	// 	LPDTSVNMFUNCCALLPARAM pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pDataBuff;
	// 	BISPASSERT(nVirNetNo,pCallParam->nVirNetNo);
	// 	BISPASSERT(nRouterLoopBack,pCallParam->nRouterLoopBack);
	// 	BISPASSERT(nRouterOutPort,pCallParam->nRouterOutPort);
	return 0;
}

LOCAL int DTSVNMTestVNetStatsNoneAddPort(UINT nVirNetNo,
										UINT nSwitcherLoopBack,
										UINT nSwithcerOutPort,
										BOOL bIsNewSwithcer)
{
	int nLogcountBef,nLogcountaft,nRet,nSwitcherCount;
	int	nSwitcherCountBef,nSwitcherCountAft;
	int	nPathCountBef,nPathCountAft;
//	int	nSwitcherPortCountBef,nSwitcherPortCountAft;
	int	nVNetPortCountBef,nVNetPortCountAft;
	LPDTSVirtualNet pVirNet = NULL;
	LPDTSVNMSwitcher pSwitcher = NULL;
	LPDTSVNMPort pPort = NULL;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";

	nSwitcherCountBef = DTSVNMGetSwitcherCount();
	nPathCountBef = DTSVNMGetPathCount();
	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
//	nSwitcherPortCountBef = (NULL==pSwitcher)?0:pSwitcher->m_mapPort.size();
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	nVNetPortCountBef = (NULL==nVirNetNo)?0:pVirNet->m_mapOutPort.size();

	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMAddOutPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	BISPASSERT(VNM_RESULT_OK==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	//----------------------------------------------------------------------------
	//�����б�
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(pVirNet->m_nVirNetNo==nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==nVirNetNo);
//	BISPASSERT(pVirNet->m_nBandWidth==nBandWidth);
	BISPASSERT(DTS_VNM_STATS_ONE_OUT_PORT == pVirNet->m_nStats);
//	BISPASSERT(1==pVirNet->m_mapOutPort.size());
	nVNetPortCountAft=pVirNet->m_mapOutPort.size();
	BISPASSERT(1==nVNetPortCountAft-nVNetPortCountBef);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),nVirNetNo);
	BISPASSERT(0==strcmp(szTmp,pVirNet->m_szVirNetName));
	BISPASSERT(1==pVirNet->m_mapOutPort.count(nSwitcherLoopBack));
	BISPASSERT(nSwithcerOutPort==pVirNet->m_mapOutPort[nSwitcherLoopBack]);

	//-----------------------------------------------------------------------------
	//�������б�
	nSwitcherCountAft = DTSVNMGetSwitcherCount();
	nSwitcherCount = bIsNewSwithcer?1:0;
	BISPASSERT(nSwitcherCount==nSwitcherCountAft-nSwitcherCountBef);
//	BISPASSERT(1==DTSVNMGetSwitcherCount());
	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
	BISPASSERT(NULL!=pSwitcher);
	BISPASSERT(pSwitcher->m_dwSwitcherLoopBack == nSwitcherLoopBack);
//	BISPASSERT(2 == pSwitcher->m_mapPort.size());
//	BISPASSERT(1==nSwitcherPortCountAft-nSwitcherPortCountBef);

	//�˿�
	pPort = DTSVNMGetPort(nSwitcherLoopBack,nSwithcerOutPort);
	BISPASSERT(NULL!=pPort);
	//�˿ں�
	BISPASSERT(nSwithcerOutPort==pPort->m_nPortIndex);
	//����������
	BISPASSERT(nSwitcherLoopBack==pPort->m_dwSwitcherLoopBack);
	//�Ƿ�Ϊ����
	BISPASSERT(pPort->m_bIsOutPort);
	BISPASSERT(pPort->m_bIsAcitve);
	//����ǳ��ڣ����¼��������
	BISPASSERT(nVirNetNo==pPort->m_nVirNetNo);	
	//�Զ˶˿�
//	BISPASSERT(NULL==pPort->m_pOppoPort);
	//�漰��·��PathID�б�
	BISPASSERT(0==pPort->m_vectPathID.size());
	//��ǩ���ü���
	BISPASSERT(0==pPort->m_mapLabelReffrence.size());

	//-----------------------------------------------------------------------------
	//·���б�
	nPathCountAft = DTSVNMGetPathCount();
	BISPASSERT(nPathCountAft==nPathCountBef);
//	BISPASSERT(0==DTSVNMGetPathCount());

	//----------------------------------------------------------------------------
	//�ⲿ����
	//֪ͨ����������VNet
	BISPASSERT(2==nLogcountaft-nLogcountBef);
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-2);
	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND == pLogEntry->nMainType);
	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
	BISPASSERT(VNM_FT_ACT_NEW_VIRNET_WITH_PORT == pCallParam->m_nFTAction);
	BISPASSERT(pCallParam->m_dwSwitcherLoopBack == nSwitcherLoopBack);
	BISPASSERT(pCallParam->m_nPortIndex == nSwithcerOutPort);
	BISPASSERT(pCallParam->m_nVirNetNo == nVirNetNo);
	BISPASSERT(0==strcmp(pCallParam->m_pBuff,pVirNet->m_szVirNetName));

	//����FT
	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
	BISPASSERT(pLogEntry->nSendDataBuffLen == sizeof(ofp_pri_vpls_vc));
	ofp_pri_vpls_vc* pOPFMsg = (ofp_pri_vpls_vc*)pLogEntry->pSendDataBuff;
	BISPASSERT(OFP13_VERSION == pOPFMsg->h.ofp_h.bVersion);
	BISPASSERT(OFPT_SET_PRIVATE == pOPFMsg->h.ofp_h.bType);
	BISPASSERT(sizeof(ofp_pri_vpls_vc) == ntohs(pOPFMsg->h.ofp_h.wLength));
	BISPASSERT(0!=pOPFMsg->h.ofp_h.dwXid);		
	BISPASSERT(OFP_PRI_TYPE_VPLS == pOPFMsg->h.bType);
	BISPASSERT(OFP_PRI_SUBTYPE_VPLS_VC == pOPFMsg->h.bSubtype);
	BISPASSERT(OFP_PRI_OPT_INSERT == pOPFMsg->h.bOpt);
	BISPASSERT(0==strcmp(pOPFMsg->alias,pVirNet->m_szVirNetName));
	BISPASSERT(pOPFMsg->dwPort == ntohl(pCallParam->m_nPortIndex));

	return 0;
}


int DTSVNMTestVNetStatsOneOutPortDelPort(UINT nVirNetNo,
											UINT nSwitcherLoopBack,
											UINT nSwithcerOutPort)
{
	int nLogcountBef,nLogcountaft,nRet;
//	int	nSwitcherCount;
	int	nSwitcherCountBef,nSwitcherCountAft;
	int	nPathCountBef,nPathCountAft;
	int	nSwitcherPortCountBef,nSwitcherPortCountAft;
	int	nVNetPortCountBef,nVNetPortCountAft;
	LPDTSVirtualNet pVirNet = NULL;
	LPDTSVNMSwitcher pSwitcher = NULL;
	LPDTSVNMPort pPort = NULL;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";

	nSwitcherCountBef = DTSVNMGetSwitcherCount();
	nPathCountBef = DTSVNMGetPathCount();
	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
	nSwitcherPortCountBef = (NULL==pSwitcher)?0:pSwitcher->m_mapPort.size();
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	nVNetPortCountBef = (NULL==nVirNetNo)?0:pVirNet->m_mapOutPort.size();

	//////////////////////////////////////////////////////////////////////////
	//ִ��
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMDelOutPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	BISPASSERT(VNM_RESULT_OK==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	//----------------------------------------------------------------------------
	//�����б�
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(pVirNet->m_nVirNetNo==nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==nVirNetNo);
//	BISPASSERT(pVirNet->m_nBandWidth==nBandWidth);
	BISPASSERT(DTS_VNM_STATS_NONE == pVirNet->m_nStats);
//	BISPASSERT(1==pVirNet->m_mapOutPort.size());
	nVNetPortCountAft=pVirNet->m_mapOutPort.size();
	BISPASSERT(-1==nVNetPortCountAft-nVNetPortCountBef);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),nVirNetNo);
	BISPASSERT(0==strcmp(szTmp,pVirNet->m_szVirNetName));
	BISPASSERT(0==pVirNet->m_mapOutPort.count(nSwitcherLoopBack));
//	BISPASSERT(nSwithcerOutPort==pVirNet->m_mapOutPort[nSwitcherLoopBack]);

	//-----------------------------------------------------------------------------
	//�������б�
	nSwitcherCountAft = DTSVNMGetSwitcherCount();
//	nSwitcherCount = bIsNewSwithcer?1:0;
	BISPASSERT(0==nSwitcherCountAft-nSwitcherCountBef);
	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
	BISPASSERT(NULL!=pSwitcher);
	BISPASSERT(pSwitcher->m_dwSwitcherLoopBack == nSwitcherLoopBack);
	nSwitcherPortCountAft = pSwitcher->m_mapPort.size();
	BISPASSERT(0==nSwitcherPortCountAft-nSwitcherPortCountBef);

	//�˿�
	pPort = DTSVNMGetPort(nSwitcherLoopBack,nSwithcerOutPort);
	BISPASSERT(NULL!=pPort);
	BISPASSERT(!pPort->m_bIsAcitve);
// 	//�˿ں�
// 	BISPASSERT(nSwithcerOutPort==pPort->m_nPortIndex);
// 	//����������
// 	BISPASSERT(pSwitcher==pPort->m_pSwitcher);
// 	//�Ƿ�Ϊ����
// 	BISPASSERT(pPort->m_bIsOutPort);
// 	//����ǳ��ڣ����¼��������
// 	BISPASSERT(pVirNet==pPort->m_pVirtualNet);	
// 	//�Զ˶˿�
// 	BISPASSERT(NULL==pPort->m_pOppoPort);
// 	//�漰��·��PathID�б�
// 	BISPASSERT(0==pPort->m_vectPathID.size());
// 	//��ǩ���ü���
// 	BISPASSERT(0==pPort->m_mapLabelReffrence.size());

	//-----------------------------------------------------------------------------
	//·���б�
	nPathCountAft = DTSVNMGetPathCount();
	BISPASSERT(nPathCountAft==nPathCountBef);
	//	BISPASSERT(0==DTSVNMGetPathCount());

	//----------------------------------------------------------------------------
	//�ⲿ����
	//֪ͨ���н�����ɾ��VNet
	BISPASSERT(DTSVNMGetSwitcherCount()*2==nLogcountaft-nLogcountBef);
	int i ;
	for (i=nLogcountBef;i<nLogcountaft;i+=2)
	{
		pLogEntry = DTSTestUnitMDLLogGetEntry(i);
		BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND == pLogEntry->nMainType);
		pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pParamBuff;
		BISPASSERT(pCallParam->m_nVirNetNo == pVirNet->m_nVirNetNo);
		BISPASSERT(VNM_FT_ACT_DEL_VIRNET == pCallParam->m_nFTAction);
//		BISPASSERT(pCallParam->m_dwSwitcherLoopBack == nSwitcherLoopBack);
		BISPASSERT(0==strcmp(pCallParam->m_pBuff,pVirNet->m_szVirNetName));

		//����FT
		pLogEntry = DTSTestUnitMDLLogGetEntry(i+1);
		BISPASSERT(VNM_FUNC_CALL_MSG_SEND == pLogEntry->nMainType);
		BISPASSERT(pLogEntry->nSendDataBuffLen == sizeof(ofp_pri_vpls));
		ofp_pri_vpls* pOPFMsg = (ofp_pri_vpls*)pLogEntry->pSendDataBuff;
		BISPASSERT(OFP13_VERSION == pOPFMsg->h.ofp_h.bVersion);
		BISPASSERT(OFPT_SET_PRIVATE == pOPFMsg->h.ofp_h.bType);
		BISPASSERT(sizeof(ofp_pri_vpls) == ntohs(pOPFMsg->h.ofp_h.wLength));
		BISPASSERT(0!=pOPFMsg->h.ofp_h.dwXid);
		BISPASSERT(OFP_PRI_TYPE_VPLS == pOPFMsg->h.bType);
		BISPASSERT(OFP_PRI_SUBTYPE_VPLS == pOPFMsg->h.bSubtype);
		BISPASSERT(OFP_PRI_OPT_REMOVE == pOPFMsg->h.bOpt);
		BISPASSERT(0==strcmp(pOPFMsg->alias,pVirNet->m_szVirNetName));
//		BISPASSERT(pOPFMsg->dwPort == ntohl(pCallParam->m_nPortIndex));
	}
	return 0;
}



LOCAL int DTSTestVNMStatsOneOutPortDelPortNotExist(UINT nVirNetNo,
												UINT nSwitcherLoopBack,
												UINT nSwithcerOutPort,
												BOOL bIsSwitcherExist)


{
	int nLogcountBef,nLogcountaft,nRet;
	//	int nSwitcherCount;
	int	nSwitcherCountBef,nSwitcherCountAft;
	int	nPathCountBef,nPathCountAft;
	int	nSwitcherPortCountBef,nSwitcherPortCountAft;
	int	nVNetPortCountBef,nVNetPortCountAft;
	UINT nOldVNetStats;
	LPDTSVirtualNet pVirNet = NULL;
	LPDTSVNMSwitcher pSwitcher = NULL;
	LPDTSVNMPort pPort = NULL;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";

	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	nOldVNetStats = pVirNet->m_nStats;
	//	memcpy(&OldVirNet,pVirNet,sizeof(DTSVirtualNet));
	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
	if (bIsSwitcherExist)
	{
		BISPASSERT(NULL!=pSwitcher);
	}	
	//	memcpy(&OldSwitcher,pSwitcher,sizeof(DTSVNMSwitcher));
	// 	pPort = DTSVNMGetPort(nSwitcherLoopBack,nSwithcerOutPort);
	// 	BISPASSERT(NULL!=pPort);
	//	memcpy(&OldPort,pPort,sizeof(DTSVNMPort));

	nSwitcherCountBef = DTSVNMGetSwitcherCount();
	nPathCountBef = DTSVNMGetPathCount();
	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
	nSwitcherPortCountBef = (NULL==pSwitcher)?0:pSwitcher->m_mapPort.size();
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	nVNetPortCountBef = (NULL==nVirNetNo)?0:pVirNet->m_mapOutPort.size();

	//////////////////////////////////////////////////////////////////////////
	//ִ��
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMDelOutPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	BISPASSERT(VNM_RESULT_OK!=nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	//----------------------------------------------------------------------------
	//�����б�
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(pVirNet->m_nVirNetNo==nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==nVirNetNo);
	BISPASSERT(nOldVNetStats == pVirNet->m_nStats);
	//	BISPASSERT(DTS_VNM_STATS_NONE == pVirNet->m_nStats);
	nVNetPortCountAft=pVirNet->m_mapOutPort.size();
	BISPASSERT(0==nVNetPortCountAft-nVNetPortCountBef);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),nVirNetNo);
	BISPASSERT(0==strcmp(szTmp,pVirNet->m_szVirNetName));
	// 	nRet = bIsPortAlreadyInVNet?1:0;
	// 	BISPASSERT(nRet==pVirNet->m_mapOutPort.count(nSwitcherLoopBack));
	//	BISPASSERT(nSwithcerOutPort==pVirNet->m_mapOutPort[nSwitcherLoopBack]);
	
	//-----------------------------------------------------------------------------
	//�������б�
	nSwitcherCountAft = DTSVNMGetSwitcherCount();
	//	nSwitcherCount = bIsNewSwithcer?1:0;
	BISPASSERT(0==nSwitcherCountAft-nSwitcherCountBef);
	if (bIsSwitcherExist)
	{
		pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
		BISPASSERT(NULL!=pSwitcher);
		BISPASSERT(pSwitcher->m_dwSwitcherLoopBack == nSwitcherLoopBack);
		nSwitcherPortCountAft = pSwitcher->m_mapPort.size();
		BISPASSERT(0==nSwitcherPortCountAft-nSwitcherPortCountBef);
	}
	
	//-----------------------------------------------------------------------------
	//·���б�
	nPathCountAft = DTSVNMGetPathCount();
	BISPASSERT(nPathCountAft==nPathCountBef);
	//	BISPASSERT(0==DTSVNMGetPathCount());

	//----------------------------------------------------------------------------
	//�ⲿ����
	//��ʱ������Ķ˿ڣ����ⲿ����
	BISPASSERT(0==nLogcountaft-nLogcountBef);
	
	return 0;
}

LOCAL int DTSTestVNMStatsOneOutPortAddPortConfict(UINT nVirNetNo,
												UINT nSwitcherLoopBack,
												UINT nSwithcerOutPort
												//BOOL bIsPortAlreadyInVNet
												)
{
	int nLogcountBef,nLogcountaft,nRet;
//	int nSwitcherCount;
	int	nSwitcherCountBef,nSwitcherCountAft;
	int	nPathCountBef,nPathCountAft;
	int	nSwitcherPortCountBef,nSwitcherPortCountAft;
	int	nVNetPortCountBef,nVNetPortCountAft;
	UINT nOldVNetStats;
	LPDTSVirtualNet pVirNet = NULL;
	LPDTSVNMSwitcher pSwitcher = NULL;
	LPDTSVNMPort pPort = NULL;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";

	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	nOldVNetStats = pVirNet->m_nStats;
//	memcpy(&OldVirNet,pVirNet,sizeof(DTSVirtualNet));
	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
	BISPASSERT(NULL!=pSwitcher);
//	memcpy(&OldSwitcher,pSwitcher,sizeof(DTSVNMSwitcher));
// 	pPort = DTSVNMGetPort(nSwitcherLoopBack,nSwithcerOutPort);
// 	BISPASSERT(NULL!=pPort);
//	memcpy(&OldPort,pPort,sizeof(DTSVNMPort));

	nSwitcherCountBef = DTSVNMGetSwitcherCount();
	nPathCountBef = DTSVNMGetPathCount();
	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
	nSwitcherPortCountBef = (NULL==pSwitcher)?0:pSwitcher->m_mapPort.size();
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	nVNetPortCountBef = (NULL==nVirNetNo)?0:pVirNet->m_mapOutPort.size();

	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMAddOutPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	BISPASSERT(VNM_RESULT_OK!=nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();
	
	//----------------------------------------------------------------------------
	//�����б�
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(pVirNet->m_nVirNetNo==nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==nVirNetNo);
	BISPASSERT(nOldVNetStats == pVirNet->m_nStats);
//	BISPASSERT(DTS_VNM_STATS_NONE == pVirNet->m_nStats);
	nVNetPortCountAft=pVirNet->m_mapOutPort.size();
	BISPASSERT(0==nVNetPortCountAft-nVNetPortCountBef);
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),nVirNetNo);
	BISPASSERT(0==strcmp(szTmp,pVirNet->m_szVirNetName));
// 	nRet = bIsPortAlreadyInVNet?1:0;
// 	BISPASSERT(nRet==pVirNet->m_mapOutPort.count(nSwitcherLoopBack));
//	BISPASSERT(nSwithcerOutPort==pVirNet->m_mapOutPort[nSwitcherLoopBack]);
	
	//-----------------------------------------------------------------------------
	//�������б�
	nSwitcherCountAft = DTSVNMGetSwitcherCount();
//	nSwitcherCount = bIsNewSwithcer?1:0;
	BISPASSERT(0==nSwitcherCountAft-nSwitcherCountBef);
	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
	BISPASSERT(NULL!=pSwitcher);
	BISPASSERT(pSwitcher->m_dwSwitcherLoopBack == nSwitcherLoopBack);
	nSwitcherPortCountAft = pSwitcher->m_mapPort.size();
	BISPASSERT(0==nSwitcherPortCountAft-nSwitcherPortCountBef);

	//�˿�
// 	pPort = DTSVNMGetPort(nSwitcherLoopBack,nSwithcerOutPort);
// 	BISPASSERT(NULL!=pPort);
// 	//�˿ں�
// 	BISPASSERT(nSwithcerOutPort==pPort->m_nPortIndex);
// 	//����������
// 	BISPASSERT(pSwitcher==pPort->m_pSwitcher);
// 	//�Ƿ�Ϊ����
// 	BISPASSERT(pPort->m_bIsOutPort);
// 	//����ǳ��ڣ����¼��������
// 	BISPASSERT(pVirNet==pPort->m_pVirtualNet);	
// 	//�Զ˶˿�
// 	BISPASSERT(NULL==pPort->m_pOppoPort);
// 	//�漰��·��PathID�б�
// 	BISPASSERT(0==pPort->m_vectPathID.size());
// 	//��ǩ���ü���
// 	BISPASSERT(0==pPort->m_mapLabelReffrence.size());

	//-----------------------------------------------------------------------------
	//·���б�
	nPathCountAft = DTSVNMGetPathCount();
	BISPASSERT(nPathCountAft==nPathCountBef);
	//	BISPASSERT(0==DTSVNMGetPathCount());

	//----------------------------------------------------------------------------
	//�ⲿ����
	//��ʱ������Ķ˿ڣ����ⲿ����
	BISPASSERT(0==nLogcountaft-nLogcountBef);

	return 0;
}





LOCAL int DTSVNMTestDelVNetStatsNone(UINT nVirNetNo)
{
	int nLogcountBef,nLogcountaft,nRet;
	//	int	nSwitcherCount;
	int nVNetCountBef,nVNetCountAft;
	int	nSwitcherCountBef,nSwitcherCountAft;
	int	nPathCountBef,nPathCountAft;
	//	int	nSwitcherPortCountBef,nSwitcherPortCountAft;
	//	int	nVNetPortCountBef,nVNetPortCountAft;
	LPDTSVirtualNet pVirNet = NULL;
	LPDTSVNMSwitcher pSwitcher = NULL;
	LPDTSVNMPort pPort = NULL;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";

	nVNetCountBef = DTSVNMGetVirNetCount();
	nSwitcherCountBef = DTSVNMGetSwitcherCount();
	nPathCountBef = DTSVNMGetPathCount();
	// 	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
	// 	nSwitcherPortCountBef = (NULL==pSwitcher)?0:pSwitcher->m_mapPort.size();
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	//	nVNetPortCountBef = (NULL==nVirNetNo)?0:pVirNet->m_mapOutPort.size();

	//////////////////////////////////////////////////////////////////////////
	//ִ��
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMDelVirNet(nVirNetNo);
	BISPASSERT(VNM_RESULT_OK==nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	//----------------------------------------------------------------------------
	//�����б�
	nVNetCountAft = DTSVNMGetVirNetCount();
	BISPASSERT(-1 == nVNetCountAft - nVNetCountBef);
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL==pVirNet);
	// 	BISPASSERT(pVirNet->m_nVirNetNo==nVirNetNo);
	// 	BISPASSERT(pVirNet->m_nVirLabel==nVirNetNo);
	// 	//	BISPASSERT(pVirNet->m_nBandWidth==nBandWidth);
	// 	BISPASSERT(DTS_VNM_STATS_NONE == pVirNet->m_nStats);
	// 	//	BISPASSERT(1==pVirNet->m_mapOutPort.size());
	// 	nVNetPortCountAft=pVirNet->m_mapOutPort.size();
	// 	BISPASSERT(-1==nVNetPortCountAft-nVNetPortCountBef);
	// 	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),nVirNetNo);
	// 	BISPASSERT(0==strcmp(szTmp,pVirNet->m_szVirNetName));
	// 	BISPASSERT(0==pVirNet->m_mapOutPort.count(nSwitcherLoopBack));
	//	BISPASSERT(nSwithcerOutPort==pVirNet->m_mapOutPort[nSwitcherLoopBack]);

	//-----------------------------------------------------------------------------
	//�������б�
	nSwitcherCountAft = DTSVNMGetSwitcherCount();
	//	nSwitcherCount = bIsNewSwithcer?1:0;
	BISPASSERT(0==nSwitcherCountAft-nSwitcherCountBef);
	// 	pSwitcher = DTSVNMGetSwitcherByLoopBack(nSwitcherLoopBack);
	// 	BISPASSERT(NULL!=pSwitcher);
	// 	BISPASSERT(pSwitcher->m_dwSwitcherLoopBack == nSwitcherLoopBack);
	// 	nSwitcherPortCountAft = pSwitcher->m_mapPort.size();
	// 	BISPASSERT(-1==nSwitcherPortCountAft-nSwitcherPortCountBef);

	//�˿�
	// 	pPort = DTSVNMGetPort(nSwitcherLoopBack,nSwithcerOutPort);
	// 	BISPASSERT(NULL==pPort);
	// 	//�˿ں�
	// 	BISPASSERT(nSwithcerOutPort==pPort->m_nPortIndex);
	// 	//����������
	// 	BISPASSERT(pSwitcher==pPort->m_pSwitcher);
	// 	//�Ƿ�Ϊ����
	// 	BISPASSERT(pPort->m_bIsOutPort);
	// 	//����ǳ��ڣ����¼��������
	// 	BISPASSERT(pVirNet==pPort->m_pVirtualNet);	
	// 	//�Զ˶˿�
	// 	BISPASSERT(NULL==pPort->m_pOppoPort);
	// 	//�漰��·��PathID�б�
	// 	BISPASSERT(0==pPort->m_vectPathID.size());
	// 	//��ǩ���ü���
	// 	BISPASSERT(0==pPort->m_mapLabelReffrence.size());

	//-----------------------------------------------------------------------------
	//·���б�
	nPathCountAft = DTSVNMGetPathCount();
	BISPASSERT(nPathCountAft==nPathCountBef);
	//	BISPASSERT(0==DTSVNMGetPathCount());

	//----------------------------------------------------------------------------
	//�ⲿ����
	//֪ͨ������ɾ��VNet
	BISPASSERT(0==nLogcountaft-nLogcountBef);
	// 	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	// 	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND == pLogEntry->nMainType);
	// 	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pDataBuff;
	// 	BISPASSERT(VNM_FT_ACT_DEL_VIRNET == pCallParam->m_nFTAction);
	// 	BISPASSERT(pCallParam->m_dwSwitcherLoopBack == nSwitcherLoopBack);
	// 	BISPASSERT(0==strcmp(pCallParam->m_pBuff,pVirNet->m_szVirNetName));

	return 0;
}


LOCAL int DTSVNMTestNewVNetRepeat(UINT nVirNetNo,UINT nBandWidth)
{
	int nLogcountBef,nLogcountaft,nRet;
	int nVNetCountBef,nVNetCountAft;
	int	nSwitcherCountBef,nSwitcherCountAft;
	int	nPathCountBef,nPathCountAft;
	LPDTSVirtualNet pVirNet = NULL;
	LPDTSVNMSwitcher pSwitcher = NULL;
	LPDTSVNMPort pPort = NULL;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";

	nVNetCountBef = DTSVNMGetVirNetCount();
	nSwitcherCountBef = DTSVNMGetSwitcherCount();
	nPathCountBef = DTSVNMGetPathCount();
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);

	//////////////////////////////////////////////////////////////////////////
	//����
	DTSTestUnitMDLLogReset();
	nLogcountBef = DTSTestUnitMDLLogGetCount();
	nRet = DTSVNMNewVirNet(nVirNetNo,nBandWidth);
	BISPASSERT(VNM_RESULT_OK!=nRet);
	nLogcountaft = DTSTestUnitMDLLogGetCount();

	//----------------------------------------------------------------------------
	//�����б�
	nVNetCountAft = DTSVNMGetVirNetCount();
	BISPASSERT(nVNetCountAft==nVNetCountBef);
	pVirNet = DTSVNMGetVirNetByNo(nVirNetNo);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(pVirNet->m_nVirNetNo==nVirNetNo);
	BISPASSERT(pVirNet->m_nVirLabelRef==pVirNet->m_nVirNetNo);
	BISPASSERT(pVirNet->m_nBandWidth==nBandWidth);
	BISPASSERT(DTS_VNM_STATS_NONE == pVirNet->m_nStats);
	BISPASSERT(0==pVirNet->m_mapOutPort.size());
	sprintf(szTmp,"%s%d",DTSVNMGetVirNetPrefix(),nVirNetNo);
	BISPASSERT(0==strcmp(szTmp,pVirNet->m_szVirNetName));

	//-----------------------------------------------------------------------------
	//�������б�
	nSwitcherCountAft = DTSVNMGetSwitcherCount();
	BISPASSERT(nSwitcherCountAft==nSwitcherCountBef);

	//-----------------------------------------------------------------------------
	//·���б�
	nPathCountAft = DTSVNMGetPathCount();
	BISPASSERT(nPathCountAft==nPathCountBef);

	//----------------------------------------------------------------------------
	//�ⲿ����
	//���ʹ�������������
	BISPASSERT(0==nLogcountaft-nLogcountBef);
	// 	pLogEntry = DTSTestUnitMDLLogGetEntry(nLogcountaft-1);
	// 	BISPASSERT(VNM_FUNC_CALL_FLOWTABLE_SEND == pLogEntry->nMainType);
	// 	pCallParam = (LPDTSVNMFUNCCALLPARAM)pLogEntry->pDataBuff;
	// 	BISPASSERT(VNM_FT_ACT_NEW_VIRNET == pCallParam->m_nFTAction);
	// 	BISPASSERT(0==strcmp(pCallParam->m_pBuff,pVirNet->m_szVirNetName));

	return 0;
}




LOCAL int DTSVNMTestAll()
{
//	int nLogcountBef,nLogcountaft,nRet;
//	int nMemeryBef,nMemeryAft;
	UINT nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort,nBandWidth;
//	UINT nTrunkPort,nOppoSwitcherLB,nOppoOutPort,nOppoTrunkPort;
//	UINT nTrunkSwitcherLB,nTrunkInPort,nTrunkOutPort;
	
	LPDTSVirtualNet pVirNet = NULL;
	LPDTSVNMSwitcher pSwitcher = NULL;
	LPDTSVNMPort pPort = NULL;
	LPDTSTESTUNITLOGENTRY pLogEntry = NULL;
	LPDTSVNMFUNCCALLPARAM pCallParam = NULL;
	char szTmp[32]="";
	l_bTestIsPathConculateSucceed = TRUE;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//���һ������
	//����ǰ���
	BISPASSERT(0==DTSVNMGetVirNetCount());
	BISPASSERT(0==DTSVNMGetSwitcherCount());
	BISPASSERT(0==DTSVNMGetPathCount());
	//VNet1
	nVirNetNo = 1;
	nBandWidth = 100;
	DTSVNMTestNewVNet(nVirNetNo,nBandWidth);	
	//�ظ��½�
	DTSVNMTestNewVNetRepeat(nVirNetNo,nBandWidth);	
	
	//VNet2
	nVirNetNo = 2;
	nBandWidth = 200;
	DTSVNMTestNewVNet(nVirNetNo,nBandWidth);
	//�ظ��½�
	DTSVNMTestNewVNetRepeat(nVirNetNo,nBandWidth);	

	//VNet3
	nVirNetNo = 3;
	nBandWidth = 300;
	DTSVNMTestNewVNet(nVirNetNo,nBandWidth);
	//�ظ��½�
	DTSVNMTestNewVNetRepeat(nVirNetNo,nBandWidth);	

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//״̬ΪDTS_VNM_STATS_NONE�µĲ���
	//////////////////////////////////////////////////////////////////////////
	//ɾ��VNet
	nVirNetNo = 1;
	DTSVNMTestDelVNetStatsNone(nVirNetNo);
	//���½���VNet1
	nVirNetNo = 1;
	nBandWidth = 100;
	DTSVNMTestNewVNet(nVirNetNo,nBandWidth);	
	
	//////////////////////////////////////////////////////////////////////////
	//Del_Port
	//ɾ��VNet1�����ڵ�һ���˿�1.1.1.1��1�˿�
	nVirNetNo = 1;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 1;
	DTSVNMTestVNetStatsNoneDelPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	
	//ɾ��VNet2�����ڵ�һ���˿�1.1.1.1��1�˿�
	nVirNetNo = 2;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 1;
	DTSVNMTestVNetStatsNoneDelPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);

	//ɾ��VNet3�����ڵ�һ���˿�1.1.1.1��1�˿�
	nVirNetNo = 3;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 1;
	DTSVNMTestVNetStatsNoneDelPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	
	////////////////////////////////////////////////////////////////////////
	//Add_Port
	//��VNet1��ӵ�һ���˿�1.1.1.1��1�˿�
	nVirNetNo = 1;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 1;
	DTSVNMTestVNetStatsNoneAddPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort,TRUE);
	//�ظ����
	DTSTestVNMStatsOneOutPortAddPortConfict(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	
	//��VNet3���һ���˿�1.1.1.1��4�˿�
	nVirNetNo = 3;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 4;
	DTSVNMTestVNetStatsNoneAddPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort,FALSE);
	//�ظ����
	DTSTestVNMStatsOneOutPortAddPortConfict(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	
	//��VNet2��ӵ�һ���˿�2.2.2.2��5�˿�
	nVirNetNo = 2;
	nSwitcherLoopBack = 0x02020202;
	nSwithcerOutPort = 5;
	DTSVNMTestVNetStatsNoneAddPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort,TRUE);
	//�ظ����
	DTSTestVNMStatsOneOutPortAddPortConfict(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//״̬ΪDTS_VNM_STATS_ONE_OUT_PORT�µĲ���
	//Add_Port_Confict
	//-------------------------------------------------------------------------
	//�ظ���������������
	//-------------------------------------------------------------------------
	//ͬһ���˿ڼ�����VNet
	//��VNet3���0x01010101��1�˿�
	nVirNetNo = 3;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 1;
	DTSTestVNMStatsOneOutPortAddPortConfict(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	
	//��VNet1���0x02020202��5�˿�
	nVirNetNo = 1;
	nSwitcherLoopBack = 0x02020202;
	nSwithcerOutPort = 5;
	DTSTestVNMStatsOneOutPortAddPortConfict(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);

	//-------------------------------------------------------------------------
	//ͬһ��Switcher�ж���˿ڼ�����ͬ��VNet
	//0x01010101��4�˿ڼ���VNet1
	nVirNetNo = 1;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 4;
	DTSTestVNMStatsOneOutPortAddPortConfict(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);

	//0x01010101��2�˿ڼ���VNet1
	nVirNetNo = 1;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 2;
	DTSTestVNMStatsOneOutPortAddPortConfict(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	
	//0x01010101��3�˿ڼ���VNet3
	nVirNetNo = 3;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 3;
	DTSTestVNMStatsOneOutPortAddPortConfict(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);

	//0x02020202��1�˿ڼ���VNet2
	nVirNetNo = 2;
	nSwitcherLoopBack = 0x02020202;
	nSwithcerOutPort = 1;
	DTSTestVNMStatsOneOutPortAddPortConfict(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	
	//////////////////////////////////////////////////////////////////////////
	//ɾ�������ڵĶ˿�
	//ɾ��VNet1��0x01010101�ϵ�2�˿�
	nVirNetNo = 1;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 2;
	DTSTestVNMStatsOneOutPortDelPortNotExist(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort,TRUE);
	//ɾ��VNet2��0x05050505�ϵ�2�˿�
	nVirNetNo = 2;
	nSwitcherLoopBack = 0x05050505;
	nSwithcerOutPort = 2;
	DTSTestVNMStatsOneOutPortDelPortNotExist(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort,FALSE);
	//ɾ��VNet3��0x02020202�ϵ�1�˿�
	nVirNetNo = 3;
	nSwitcherLoopBack = 0x02020202;
	nSwithcerOutPort = 1;
	DTSTestVNMStatsOneOutPortDelPortNotExist(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort,TRUE);

	//////////////////////////////////////////////////////////////////////////
	//ɾ���˿�
	//ɾ��VNet1��0x01010101�ϵ�1�˿�
	nVirNetNo = 1;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 1;
	DTSVNMTestVNetStatsOneOutPortDelPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	DTSVNMTestVNetStatsNoneAddPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort,FALSE);

	//ɾ��VNet3��0x01010101�ϵ�4�˿�
	nVirNetNo = 3;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 4;
	DTSVNMTestVNetStatsOneOutPortDelPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	DTSVNMTestVNetStatsNoneAddPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort,FALSE);

	//ɾ��VNet2��0x02020202�ϵ�5�˿�
	nVirNetNo = 2;
	nSwitcherLoopBack = 0x02020202;
	nSwithcerOutPort = 5;
	DTSVNMTestVNetStatsOneOutPortDelPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort);
	DTSVNMTestVNetStatsNoneAddPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort,FALSE);
	
	//////////////////////////////////////////////////////////////////////////
	//Add_Port ��ΪDTS_VNM_STATS_ACTIVE״̬ 
	//VNet1����0x02020202�ϵ�1�˿ڣ�һ��0x01010101[1][3]--[3][1]0x02020202
// 	nVirNetNo = 1;
// 	nSwitcherLoopBack = 0x02020202;
// 	nSwithcerOutPort = 1;
// 	nTrunkPort = 3;
// 	nOppoSwitcherLB = 0x01010101;
// 	nOppoTrunkPort = 3;
// 	nOppoOutPort = 1;
	DTSVNMTestVNetStatsOneOutPortAddPortTest1();

	//VNet1����0x03030303�ϵ�1�˿ڣ��ɲ��Ե�һ��(R2--R3)������(R1-R4-R3)
	//0x02020202[1][2]--[2][1]0x03030303
	//0x01010101[1][3]---0x02020202[3][2]------[2][1]0x03030303
 	DTSVNMTestVNet_1StatsActiveAddPortTest2();

	//VNet2����0x04040404��2�˿ڣ�ԭ��0x02020202��5�˿�
	//0x04040404[2]---0x01010101[2][3]------[3]0x02020202
	DTSVNMTestVNet_2StatsOneOutPortAddPortTest3();

	//VNet2����0x03030303��5�˿ڣ�ԭ��0x02020202��5�˿ڡ�0x04040404��1�˿�
	//0x03030303[2]--0x02020202[2],0x03030303[3]--0x04040404[3]
	DTSVNMTestVNet_2StatsActiveAddPortTest4();

	//////////////////////////////////////////////////////////////////////////
	//ɾ������
	//ɾ��VNet1��S1_1�˿�
	DTSVNMTestVNet_1StatsActiveDelPortTest5();
	//ɾ��VNet1��S2_1�˿�
	DTSVNMTestVNet_1StatsActiveDelPortTest6();
	//ɾ��VNet1��S3_1�˿�
	DTSVNMTestVNet_1StatsOneOutPortDelPortTest7();

	//����ɾ��VNet_2
	DTSVNMTestVNet_2StatsAciveDelAllTest8();

	//////////////////////////////////////////////////////////////////////////
	//�ؽ�VNet_1
	nVirNetNo = 1;
	nSwitcherLoopBack = 0x01010101;
	nSwithcerOutPort = 1;
	DTSVNMTestVNetStatsNoneAddPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort,FALSE);
	//���VNet1��S2_1�˿�
	pPort = DTSVNMGetPort(0x02020202,3);
	BISPASSERT(0==pPort->m_mapLabelReffrence.size());
	DTSVNMTestVNetStatsOneOutPortAddPortTest1();
	//���VNet1��S3_1�˿�
	DTSVNMTestVNet_1StatsActiveAddPortTest2();
	//�ؽ�VNet_2
	nVirNetNo = 2;
	nBandWidth = 200;
	DTSVNMTestNewVNet(nVirNetNo,nBandWidth);
	nVirNetNo = 2;
	nSwitcherLoopBack = 0x02020202;
	nSwithcerOutPort = 5;
	DTSVNMTestVNetStatsNoneAddPort(nVirNetNo,nSwitcherLoopBack,nSwithcerOutPort,FALSE);

	//����·�����㷵��ʧ�ܵ����
	l_bTestIsPathConculateSucceed = FALSE;
	DTSTestVNet_2OneOutPortAddPortPathConculateFailTest9();
	l_bTestIsPathConculateSucceed = TRUE;

	DTSVNMTestVNet_2StatsOneOutPortAddPortTest3();

	//����·�����㷵��ʧ�ܵ����
	l_bTestIsPathConculateSucceed = FALSE;
	DTSTestVNet_2ActiveAddPortPathConculateFailTest10();
	l_bTestIsPathConculateSucceed = TRUE;

	DTSVNMTestVNet_2StatsActiveAddPortTest4();

	//----------------------------------------------------------------------------
	//������·���Ϻ���Ի�ȡ��һ��·����������·S2_2---S3_2
	DTSVNMTestLinkDownHaveBackupPathTest11();

	//////////////////////////////////////////////////////////////////////////
	//��·����
	//������·���Ϻ��޷������µ�·����������·S2_3---S1_3
	l_bTestIsPathConculateSucceed = FALSE;
	DTSVNMTestLinkDownNoBackupPathTest12();
	//�ڶ���·�����ϣ�������·S3_3---S4_3
	DTSVNMTestLinkDownNoBackupPathTest13();

	l_bTestIsPathConculateSucceed = TRUE;

	return 0; 
}



void DTSVirNetManagerTest(int nParam)
{
	
	DTSVNMTestAll();
	
	return;
}

// TEST(DTS_VIRTUAL_NET_MANAGER_MDL, TEST_CASE)
// {
// 	DTSVNMTestUnitAddOutPort();
// 	DTSVNMTestUnitDelOutPort();	
// }


#endif //DTS_TEST




