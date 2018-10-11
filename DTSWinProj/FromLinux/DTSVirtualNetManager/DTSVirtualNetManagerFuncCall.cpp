/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   17:16
	filename: 	DTSVirtualNetManagerFuncCall.cpp
	author:		sl.wu
	
	purpose:	��������ģ����Ҫ������ýӿ�
*********************************************************************/

#include "../DTSCommon/DTSCommon.h"
#include "../DTSCommon/BISPSocket.h"
#include "DTSVirtualNetManagerFuncCall.h"
#include "DTSVirtualNetManagerInterface.h"
#include "DTSVirtualNetManager.h"
#include "DTSVirtualNetManagerPath.h"
#include "DTSOpenFlowCommon.h"
#include "DTSOpenFlowPrivate.h"


#define DTS_VNM__SEND_BUFF_LENGTH			2048
LOCAL BYTE				l_arryBuffSend[DTS_VNM__SEND_BUFF_LENGTH] = {0};
LOCAL SOCKET			l_hSocket;
LOCAL SOCKADDR_IN		l_sockaddr;
#define SWITCH_OF_RECV_PORT			8060
//#define LOCAL_UDP_PORT			52345

int DTSVNMInitSendUDPSocket()
{
#ifdef NSL_OS_WINDOWS
	WSADATA wsd;
	if( WSAStartup(MAKEWORD(2,2),&wsd)!=0 )
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("\n!!!!ERROR TC : WSAStartup Fail\n\n");
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("\n!!!!ERROR TC : Windows Error Code(%d)\n\n",WSAGetLastError());			
		return VNM_RESULT_EXCEPTION;
	}
#endif
	l_hSocket = socket(AF_INET, SOCK_DGRAM,0);
	if (INVALID_SOCKET == l_hSocket)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("!!!![ERROR] DTSVNMInitSendUDPSocket INVALID_SOCKET == l_hSocket\n");
		return VNM_RESULT_EXCEPTION;
	}
	
//	int nSockOption;
// 	if (SOCKET_ERROR == SOCK_IOCTRL(pUDP->m_hOppositeSocket,
// 		FIONBIO,
// 		&nSockOption))
		
	return VNM_RESULT_OK;
}


LOCAL uint32_t l_nXid = 1000000;
LOCAL uint32_t DTSVNMMakeOFPXid()
{
	return l_nXid++;
}

LOCAL int DTSVNMMakeVNetOPFMsg(tagDTSVNMFuncCallParam* pParam,BYTE* pBuff,int nLen,BYTE bOpt)
{
	ofp_pri_vpls* pOPFMsg = (ofp_pri_vpls*)pBuff;
	pOPFMsg->h.ofp_h.bType = OFPT_SET_PRIVATE;
	pOPFMsg->h.ofp_h.wLength = htons(sizeof(ofp_pri_vpls));
	pOPFMsg->h.bType = OFP_PRI_TYPE_VPLS;
	pOPFMsg->h.bSubtype = OFP_PRI_SUBTYPE_VPLS;
	pOPFMsg->h.bOpt = bOpt;
	strcpy(pOPFMsg->alias,pParam->m_pBuff);
	
	return VNM_RESULT_OK;
}

//��Ե����������
LOCAL int DTSVNMMakeOutPortOPFMsg(tagDTSVNMFuncCallParam* pParam,BYTE* pBuff,int nLen,BYTE bOpt)
{
	ofp_pri_vpls_vc*  pOPFMsg = (ofp_pri_vpls_vc* )pBuff;
	pOPFMsg->h.ofp_h.bType = OFPT_SET_PRIVATE;
	pOPFMsg->h.ofp_h.wLength = htons(sizeof(ofp_pri_vpls_vc));
	pOPFMsg->h.bType = OFP_PRI_TYPE_VPLS;
	pOPFMsg->h.bSubtype = OFP_PRI_SUBTYPE_VPLS_VC;
	pOPFMsg->h.bOpt = bOpt;	
	strcpy(pOPFMsg->alias,pParam->m_pBuff);
	pOPFMsg->dwPort = htonl(pParam->m_nPortIndex);
	
	return VNM_RESULT_OK;
}

//��Ե�������м̿�
LOCAL int DTSVNMMakeTrunkPortOPFMsg(tagDTSVNMFuncCallParam* pParam,BYTE* pBuff,int nLen,BYTE bOpt)
{
	ofp_pri_vpls_pw*  pOPFMsg = (ofp_pri_vpls_pw* )pBuff;
	pOPFMsg->h.ofp_h.bType = OFPT_SET_PRIVATE;
	pOPFMsg->h.ofp_h.wLength = htons(sizeof(ofp_pri_vpls_pw));
	pOPFMsg->h.bType = OFP_PRI_TYPE_VPLS;
	pOPFMsg->h.bSubtype = OFP_PRI_SUBTYPE_VPLS_PW;
	pOPFMsg->h.bOpt = bOpt;	

	strcpy(pOPFMsg->alias,pParam->m_pBuff);
	pOPFMsg->wflag = htons(OFP_PRI_VPLS_F_PW_USE_ASYMMETRIC); /* OFP_PRI_VPLS_F_XXX */
	pOPFMsg->wPolicer_id = htons(0); /* Valid, while OFP_PRI_VPLS_F_POLICER_ID_VALID is set */
	pOPFMsg->wResverd = htons(0);
	memcpy(pOPFMsg->mac,pParam->m_arrbOppoMac,DTS_VNM_MAC_LEN);
	
	pOPFMsg->dwOut_pw_label = htonl(pParam->m_nOutPWLabel);
	pOPFMsg->dwOut_vc_label = htonl(pParam->m_nOutVCLabel);
	pOPFMsg->dwPort = htonl(pParam->m_nPortIndex);
	pOPFMsg->dwIn_pw_label = htonl(pParam->m_nInPWLabel); /* Valid, while OFP_PRI_VPLS_F_PW_USE_ASYMMETRIC is set */
	pOPFMsg->dwIn_vc_label = htonl(pParam->m_nInVCLabel); /* Valid, while OFP_PRI_VPLS_F_PW_USE_ASYMMETRIC is set */	
	return VNM_RESULT_OK;
}

LOCAL int DTSVNMMakeSwitchOPFMsg(tagDTSVNMFuncCallParam* pParam,BYTE* pBuff,int nLen,BYTE bOpt)
{
	WORD  wTmpLen = 0;
//	WORD  wPackLen = 0;
	BYTE* pbOffSet = NULL;
	ofp_flow_mod*  pOPFMsg = (ofp_flow_mod* )pBuff;
	pOPFMsg->header.bType = OFPT_FLOW_MOD;
	pOPFMsg->n64Cookie = pParam->m_n64Cookie;/* Opaque controller-issued identifier. */
    pOPFMsg->n64Cookie_mask = 0;     /* Mask used to restrict the cookie bits
                                     that must match when the command is
                                     OFPFC_MODIFY* or OFPFC_DELETE*. A value
                                     of 0 indicates no restriction. */
    pOPFMsg->bTable_id = 0;          /* ID of the table to put the flow in.
                                     For OFPFC_DELETE_* commands, OFPTT_ALL
                                     can also be used to delete matching
                                     flows from all tables. */
	//OFPFC_ADD��OFPFC_DELETE
	pOPFMsg->bCommand = bOpt;								/* One of OFPFC_*. */
    pOPFMsg->wIdle_timeout = htons(OFP_FLOW_PERMANENT);			/* Idle time before discarding (seconds). */
    pOPFMsg->wHard_timeout = htons(OFP_FLOW_PERMANENT);			/* Max time before discarding (seconds). */
    pOPFMsg->wPriority = htons(OFP_DEFAULT_PRIORITY);           /* Priority level of flow entry. */
	pOPFMsg->dwBuffer_id = htonl(OFP_NO_BUFFER);           /* Buffered packet to apply to, or
															OFP_NO_BUFFER.
															Not meaningful for OFPFC_DELETE*. */
    pOPFMsg->dwOut_port = htonl(pParam->m_nOppoPortIndex);   /* For OFPFC_DELETE* commands, require
															 matching entries to include this as an
															 output port.  A value of OFPP_ANY
															 indicates no restriction. */
    pOPFMsg->dwOut_group = htonl(OFPG_ANY);           /* For OFPFC_DELETE* commands, require
														 matching entries to include this as an
														 output group.  A value of OFPG_ANY
														 indicates no restriction. */
    pOPFMsg->wFlags = htons(0);               /* Bitmap of OFPFF_* flags. */
    // uint8_t pad[2]; Ϊ0
	
//	wPackLen = sizeof(ofp_flow_mod) - sizeof(ofp_match);

	//match������Label
	pbOffSet = (BYTE*)&pOPFMsg->match;
	pOPFMsg->match.wType = htons(OFPMT_OXM);
	//ͷ4�ֽ� + ����8�ֽڵ�TLV
	pOPFMsg->match.wLength = htons(4+8*2); 
	DWORD* pdwTmp = (DWORD*)pOPFMsg->match.pad;
	*pdwTmp = htonl(OXM_OF_MPLS_LABEL);
	*(pdwTmp+1) = htonl(pParam->m_nInPWLabel);
	*(pdwTmp+2) = htonl(OXM_OF_MPLS_LABEL);
	*(pdwTmp+3) = htonl(pParam->m_nInVCLabel);
	//����match����ʵ�ʶ��䳤��
	wTmpLen = (ntohs(pOPFMsg->match.wLength) + 7)/8*8;
	pbOffSet += wTmpLen;

//	wPackLen += wTmpLen;

	//ofp_instruction_actions
	ofp_instruction_actions* pInstruction = (ofp_instruction_actions*)pbOffSet;
	pInstruction->wType = htons(OFPIT_APPLY_ACTIONS);              /* One of OFPIT_*_ACTIONS */
	/* Length of this struct in bytes. */
//	uint8_t pad[4];             /* Align to 64-bits */
	pbOffSet += sizeof(ofp_instruction_actions);	

	//ofp_action_output
	//OFPAT_OUTPUT
	ofp_action_output* pActionOutput = (ofp_action_output*)pbOffSet;

	pActionOutput->wType = htons(OFPAT_OUTPUT);                  /* OFPAT_OUTPUT. */
	pActionOutput->wLen	= htons(sizeof(ofp_action_output));      /* Length is 16. */
	pActionOutput->dwPort = htonl(pParam->m_nOppoPortIndex);     /* Output port. */
	pActionOutput->wMax_len = htons(OFPCML_NO_BUFFER);           /* Max length to send to controller. */
//	uint8_t pad[6];                 /* Pad to 64 bits. */
	pbOffSet += sizeof(ofp_action_output);
	
//	wPackLen += sizeof(ofp_action_output);

	//ofp_action_set_field
	//	OFPAT_SET_FIELD
	//	OXM_OF_ETH_DST
	ofp_action_set_field* pActionSetField = (ofp_action_set_field*)pbOffSet;
	pActionSetField->wType = htons(OFPAT_SET_FIELD);  
	pdwTmp = (DWORD*)&pActionSetField->field[0];
	*pdwTmp = htonl(OXM_OF_ETH_DST);
	pbOffSet = (BYTE*)(pdwTmp+1);
	memcpy(pbOffSet,pParam->m_arrbOppoMac,DTS_VNM_MAC_LEN);
	//��������λ��
	pbOffSet+=DTS_VNM_MAC_LEN;
	wTmpLen = (WORD)(pbOffSet - (BYTE*)&pActionSetField->wType);
	//���㲹����Ľṹ�峤��
	pActionSetField->wLen = htons((wTmpLen + 7)/8*8);
	pbOffSet = (BYTE*)&pActionSetField->wType + ntohs(pActionSetField->wLen);
	//ͷ8�ֽ� + ����Action
	pInstruction->wLen = htons(sizeof(ofp_instruction_actions)+ntohs(pActionOutput->wLen)+ntohs(pActionSetField->wLen));
	//��ֵ�����ܳ���
	pOPFMsg->header.wLength = htons((WORD)(pbOffSet - pBuff));
	
	return VNM_RESULT_OK;
}


WORD DTSVNMMakeOFPMsg(tagDTSVNMFuncCallParam* pParam,BYTE* pBuff,int nLen)
{
	BISPASSERT(NULL!=pParam);
	BISPASSERT(NULL!=pBuff);
	BISPASSERT(0!=nLen);

	memset(pBuff,0,nLen);
	ofp_header* pOFPHead = (ofp_header*)pBuff;
	pOFPHead->bVersion = OFP13_VERSION;
	pOFPHead->dwXid = htonl(DTSVNMMakeOFPXid());

	BISPASSERT(NULL!=pParam);
	switch(pParam->m_nFTAction)
	{
	//����һ������
	case 	VNM_FT_ACT_NEW_VIRNET_WITH_PORT					:
		//���ڵ�����������������ʱ��Я����һ�����ڣ�ֱ�ӵ������ӽӿ�
		DTSVNMMakeOutPortOPFMsg(pParam,pBuff,nLen,OFP_PRI_OPT_INSERT);
		break;
	//ɾ��һ������
	case 	VNM_FT_ACT_DEL_VIRNET					:
		DTSVNMMakeVNetOPFMsg(pParam,pBuff,nLen,OFP_PRI_OPT_REMOVE);
		break;
	//����һ���������ڣ���Ҫ������������˫�������·���ǩ��������
	case 	VNM_FT_ACT_OUT_PORT_ADD					:
		DTSVNMMakeOutPortOPFMsg(pParam,pBuff,nLen,OFP_PRI_OPT_INSERT);
		break;
	//ɾ��һ���������ڣ���Ҫ������������˫�������·���ǩ��������
	case 	VNM_FT_ACT_OUT_PORT_DEL					:
		DTSVNMMakeOutPortOPFMsg(pParam,pBuff,nLen,OFP_PRI_OPT_REMOVE);
		break;
	//����һ�������м̶˿�
	case 	VNM_FT_ACT_TRUNK_PORT_ADD				:
		DTSVNMMakeTrunkPortOPFMsg(pParam,pBuff,nLen,OFP_PRI_OPT_INSERT);
		break;
	//ɾ��һ�������м̶˿�
	case 	VNM_FT_ACT_TRUNK_PORT_DEL				:
		DTSVNMMakeTrunkPortOPFMsg(pParam,pBuff,nLen,OFP_PRI_OPT_REMOVE);
		break;
	//�����м̽����Ľ���
	case 	VNM_FT_ACT_SWITCH_ADD					:
		DTSVNMMakeSwitchOPFMsg(pParam,pBuff,nLen,OFPFC_ADD);
		break;
	//ɾ���м̽������Ľ���
	case 	VNM_FT_ACT_SWITCH_DEL					:
		DTSVNMMakeSwitchOPFMsg(pParam,pBuff,nLen,OFPFC_DELETE);
		break;
	default:
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[WARNING] DTSVNMTopoConculate Unhandle FTAction(%d)\n",pParam->m_nFTAction);
		pOFPHead->wLength = 0;
	}		

	return ntohs(pOFPHead->wLength);
}





//������ʵ�ⲿ�ӿڣ���Ҫ��Ӧ�Ŀ⣬����DTS_TEST�򿪵�����������stub�еĺ���
#ifndef DTS_TEST
#include <set>
#include <nsl/proto/topology/tplg.h>
#include <netCoreCtrlTypeDef.h>

LOCAL int DTSVNMCallTopoConculate(tagDTSVNMFuncCallParam* pParam)
{
	BISPASSERT(NULL!=pParam);
	LPDTSVNMPath pPathResult = (LPDTSVNMPath)pParam->m_pReValueBuff;
	BISPASSERT(NULL!=pPathResult);
/*
	UINT i = 0;
	S_FwPathReq sFwPathReq;
	S_FwPathRsp sFwPathRsp;
	S_FlowEntryMatch sFlowEntryMatch;
	
	memset(&sFwPathReq, 0, sizeof(S_FwPathReq));
	memset(&sFwPathRsp, 0, sizeof(S_FwPathRsp));
	memset(&sFlowEntryMatch, 0, sizeof(S_FlowEntryMatch));

	sFwPathReq.sourceId = pParam->m_dwSwitcherLoopBack;
	sFwPathReq.destId = pParam->m_dwOppoLoopBack;
	sFwPathReq.sQosInfo.ePriority = P0;
	sFwPathReq.sQosInfo.bandwidth = pParam->m_nBandWidth;
	sFwPathReq.sRestrainCondition.eTransferType  =	NONE;
	
	int nRet;
	nRet = tplg_ons_fw_path((void *)&sFwPathReq, (void *)&sFwPathRsp);
	if (0!=nRet)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("!!!!WARNING tplg_ons_fw_path Fail ret(%d)",nRet);
		return VNM_RESULT_EXCEPTION;
	}

	DTSVNMLabelSwitch LabelSwitch = {0};
	pPathResult->m_nPathID = sFwPathRsp.pathID;
	for (i=0;i<sFwPathRsp.numOfFwDev;i++)
	{
		LabelSwitch.m_dwSwitcherLoopBack = sFwPathRsp.sFwpathDevInfo[i].fwDevId;
		LabelSwitch.m_nInPortIndex = sFwPathRsp.sFwpathDevInfo[i].inPort;
		LabelSwitch.m_nOutPortIndex = sFwPathRsp.sFwpathDevInfo[i].outPort;
		//MAC
		memcpy(LabelSwitch.m_arrbInMac,sFwPathRsp.sFwpathDevInfo[i].inPortMacAddr,DTS_VNM_MAC_LEN);
		memcpy(LabelSwitch.m_arrbOutMac,sFwPathRsp.sFwpathDevInfo[i].outPortMacAddr,DTS_VNM_MAC_LEN);
		pPathResult->m_vectLabelSwitch.push_back(LabelSwitch);
	}

	return VNM_RESULT_OK;
*/
	return VNM_RESULT_EXCEPTION;
}


//////////////////////////////////////////////////////////////////////////
//������ʵ�ⲿ�ӿڣ���Ҫ��Ӧ�Ŀ�
int DTSVNMFuncCall(int nFuncType,tagDTSVNMFuncCallParam* pParam)
{
	DTS_VNM_DEBUG_PRINTF(DEBUG_ENTRY)
		("[ENTRY][VNM] DTSVNMFuncCall FunType(%s)\n",
		DTSVNMGetFuncTypeStr(nFuncType));
	WORD wLen = 0;

	//��ǩ���������㷨
	//VCĿ��Ϊ���ɳ���Router_IP/ID����
	//PW������������
	switch(nFuncType)
	{
	case 	VNM_FUNC_CALL_TOPO_CONCULATE			:
		return DTSVNMCallTopoConculate(pParam);
		break;

	case	VNM_FUNC_CALL_FLOWTABLE_SEND			:
		wLen = DTSVNMMakeOFPMsg(pParam,l_arryBuffSend,DTS_VNM__SEND_BUFF_LENGTH);
		if (0!=wLen)
		{
			return DTSVNMSendBuff(pParam->m_dwSwitcherLoopBack,l_arryBuffSend,(int)wLen);
		}
		else
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("\n\n!!!![ERROR]	DTSVNMMakeOFPMsg return(%d)\n\n",wLen);
			return VNM_RESULT_EXCEPTION;
		}
		break;
//	case	VNM_FUNC_CALL_TOPO_UPDATE				
	default	:
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[WARNING] DTSVNMFuncCall Unhandle FunType(%d)\n",nFuncType);
	}
	

	return VNM_RESULT_OK;
}


int DTSVNMSendBuff(DWORD dwDstAddr,BYTE* pBuff,int nLen)
{
	l_sockaddr.sin_addr.s_addr = htonl(dwDstAddr);
	l_sockaddr.sin_family = AF_INET;
	l_sockaddr.sin_port = htons((unsigned short)SWITCH_OF_RECV_PORT);
	int nSendLen = sendto(	l_hSocket,
		(LPSTR)pBuff,nLen,0,
		(sockaddr*)&l_sockaddr,
		sizeof(sockaddr));
	if (SOCKET_ERROR == nSendLen)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("!!!![ERROR] DTSVNMSendBuff Send to (0x%x) Buff Len(%d) Fail error_code(%d)(%s)\n",
			dwDstAddr,nLen,errno,strerror(errno));
//		return VNM_RESULT_EXCEPTION;
	}

	return VNM_RESULT_OK;
}

#endif //DTS_TEST


