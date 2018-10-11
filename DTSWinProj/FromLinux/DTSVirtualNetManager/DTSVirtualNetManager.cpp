/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   17:11
	filename: 	DTSVirtualNetManager.cpp
	author:		sl.wu
	
	purpose:	虚网管理模块需要对外调用接口
*********************************************************************/
#include "../DTSCommon/DTSCommon.h"
#include "DTSVirtualNetManagerFuncCall.h"
#include "DTSVirtualNetManagerInterface.h"
#include "DTSVirtualNetManager.h"
//#include "DTSVirtualNetManagerLabel.h"
#include "DTSVirtualNetManagerPath.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//全局列表
//虚网列表
LOCAL DTSVNMVirNetMap l_mapVirNet;
//交换机列表
LOCAL DTSVNMSwitcherMap	l_mapSwitcher;

//////////////////////////////////////////////////////////////////////////
//测试打印控制
DWORD g_dwDTSVNMDebugOption = 0;
//消息打印
LOCAL char l_arrszVNMDebugMsg[DTS_VNM_MSG_PORT_COUNT][DTS_DEBUG_STRING_LENGTH] = 
{
//	"DTS_VNM_MSG_NONE"				,
	"DTS_VNM_MSG_VIRNET_NEW"		,
	"DTS_VNM_MSG_VIRNET_DEL"		,
	"DTS_VNM_MSG_PORT_ADD"			,
	"DTS_VNM_MSG_PORT_DEL"			,
	"DTS_VNM_MSG_LINK_DOWN"			,
//	"DTS_VNM_MSG_GET_PATH"

//	DTS_VNM_MSG_PORT_COUNT			
};

LOCAL char l_arrszVNMStats[DTS_VNM_STATS_COUNT][DTS_DEBUG_STRING_LENGTH] = 
{
	"DTS_VNM_STATS_NONE"				,
	"DTS_VNM_STATS_ONE_OUT_PORT"		,
	//有两个或以上出口
	"DTS_VNM_STATS_ACTIVE"			
	

//	"DTS_VNM_STATS_COUNT"
};



LOCAL char l_arrszVNMDebugCallFunc[VNM_FUNC_CALL_COUNT][DTS_DEBUG_STRING_LENGTH] = 
{
	"VNM_FUNC_CALL_NULL"					,
	"VNM_FUNC_CALL_TOPO_CONCULATE"			,
	"VNM_FUNC_CALL_FLOWTABLE_SEND"			,
	"VNM_FUNC_CALL_TOPO_UPDATE"				,
	"VNM_FUNC_CALL_MSG_SEND"				,
	//更新打印信息列表

	//	VNM_FUNC_CALL_COUNT
};


LOCAL char l_arrszVNMFTAction[VNM_FLOW_TABLE_ACTION_COUNT][DTS_DEBUG_STRING_LENGTH]=
{
	"VNM_FT_ACT_NONE"						,
	//新建一个虚网
	"VNM_FT_ACT_NEW_VIRNET_WITH_PORT"		,
	//删除一个虚网
	"VNM_FT_ACT_DEL_VIRNET"					,
	//增加一个虚网出口，需要下两个流表对双向流量下发标签控制流表
	"VNM_FT_ACT_OUT_PORT_ADD"				,
	//删除一个虚网出口，需要下两个流表对双向流量下发标签控制流表
	"VNM_FT_ACT_OUT_PORT_DEL"				,
	//增加一个虚网中继端口
	"VNM_FT_ACT_TRUNK_PORT_ADD"				,
	//删除一个虚网中继端口
	"VNM_FT_ACT_TRUNK_PORT_DEL"				,
	//建立中继交换的交换
	"VNM_FT_ACT_SWITCH_ADD"					,
	//删除中继交换机的交换
	"VNM_FT_ACT_SWITCH_DEL"					,
	//虚网Qos设置							
	"VNM_FT_ACT_QOS_SET"					,
	//虚网Qos设置
	"VNM_FT_ACT_QOS_DEL"					

//	VNM_FLOW_TABLE_ACTION_COUNT
};

char* DTSVNMGetFTActionStr(UINT nType)
{
	BISPASSERT(nType<VNM_FLOW_TABLE_ACTION_COUNT);
	return l_arrszVNMFTAction[nType];
}


char* DTSVNMGetFuncTypeStr(UINT nType)
{
	BISPASSERT(nType<VNM_FUNC_CALL_COUNT);
	return l_arrszVNMDebugCallFunc[nType];
}

LOCAL char* DTSVNMGetVirNetStatsStr(LPDTSVirtualNet pVirNet);

LOCAL char l_szVirNetInfo[1024]="";
LOCAL char* DTSVNMGetVirNetInfoStr(LPDTSVirtualNet pVirNet);

LOCAL char l_szSwitcherInfo[512]="";
LOCAL char* DTSVNMGetSwitcherInfoStr(LPDTSVNMSwitcher pSwticher);

LOCAL char l_szPortInfo[128]="";
LOCAL char* DTSVNMGetPortInfoStr(LPDTSVNMPort pPort);


// LOCAL char l_szParam[128]="";
// LOCAL char* DTSGetVNMParamStr(LPDTSVNMParam pPort);

//------------------------------------------------------------------------------------
//新加
LOCAL int DTSVNMAddVirNetInMap(LPDTSVirtualNet pVirNet);
LOCAL int DTSVNMAddSwitcherInMap(LPDTSVNMSwitcher pSwitcher);

//------------------------------------------------------------------------------------
//创建、删除函数
LOCAL LPDTSVirtualNet DTSVNMNewVirNet(LPDTSVNMParam pVNMParam);
LOCAL LPDTSVNMSwitcher DTSVNMNewSwitcher(LPDTSVNMParam pVNMParam);
LOCAL LPDTSVNMPort DTSVNMNewPort(LPDTSVNMParam pVNMParam);
LOCAL int DTSVNMDeactivePortInSwitcher(LPDTSVNMPort pPort);

LOCAL int DTSVNMDelVirNetStatsNone(LPDTSVirtualNet pVirNet);

//------------------------------------------------------------------------------------
//处理函数
LOCAL int DTSVNMAddVirNetOutPort(LPDTSVNMParam pVNMParam);
LOCAL int DTSVNMDelVirNetOutPort(LPDTSVNMParam pVNMParam);

//------------------------------------------------------------------------------------
//流表函数
//发送创建虚网的流表
LOCAL int DTSVNMSendCreateVirNetWithPortFT(LPDTSVirtualNet pVirNet,LPDTSVNMParam pVNMParam);


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//函数实现
//删除一个虚网
LOCAL int DTSVNMDelVirNetStatsNone(LPDTSVirtualNet pVirNet)
{
	BISPASSERT(NULL!=pVirNet);
	l_mapVirNet.erase(pVirNet->m_nVirNetNo);
	delete(pVirNet);
	return VNM_RESULT_OK;
}


//新建一个虚网
LOCAL LPDTSVirtualNet DTSVNMNewVirNet(LPDTSVNMParam pVNMParam)
{
	LPDTSVirtualNet pVirNet = NULL;
	BISPASSERT(NULL!=pVNMParam);
	//创建
	pVirNet = new DTSVirtualNet;
	if (NULL==pVirNet)
	{
		return NULL;
	}
	pVirNet->m_nStats = DTS_VNM_STATS_NONE;
	pVirNet->m_nVirNetNo = pVNMParam->m_nVirNetNo;
	DTSVNMSetVirNetName(pVirNet);	
	pVirNet->m_nVirLabelRef = 0;
	pVirNet->m_nBandWidth = pVNMParam->m_nBandWidth;

	//下发建立虚网的流表
//	DTSVNMSendCreateVirNetFT(pVirNet);

	DTS_VNM_DEBUG_PRINTF(DEBUG_FSM)
		("[FSM][VNM] VirNet(%d) Stats Change To %s\n",
		pVirNet->m_nVirNetNo,
		DTSVNMGetVirNetStatsStr(pVirNet));
	return pVirNet;
}

//新建一个Switcher
LOCAL LPDTSVNMSwitcher DTSVNMNewSwitcher(LPDTSVNMParam pVNMParam)
{
	BISPASSERT(NULL!=pVNMParam);
	LPDTSVNMSwitcher pSwitcher = NULL;
//	pSwitcher = (LPDTSVNMSwitcher)DTS_MALLOC(sizeof(DTSVNMSwitcher));
	pSwitcher = new DTSVNMSwitcher;
	if (NULL==pSwitcher)
	{
		return NULL;
	}
//	memset(pSwitcher,0,sizeof(DTSVNMSwitcher));
	pSwitcher->m_dwSwitcherLoopBack = pVNMParam->m_dwSwitcherLoopBack;
//	pSwitcher->m_mapPort = new DTSVNMPortMap;
	return pSwitcher;
}


LOCAL int DTSVNMDeactivePortInSwitcher(LPDTSVNMPort pPort)
{
	BISPASSERT(NULL!=pPort);
	//暂不删除，只修改Active标志
	pPort->m_bIsAcitve = FALSE;
	pPort->m_nVirNetNo = DTS_VNET_NO_NULL;

// 	LPDTSVNMSwitcher pSwitcher = DTSVNMGetSwitcherByLoopBack(pPort->m_dwSwitcherLoopBack);
// 	if (NULL==pSwitcher)
// 	{
// 		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
// 			("[ERROR] DTSDelPortInSwitcher Port(0x%x)(%d) Cant Find Switcher\n",
// 			pPort->m_dwSwitcherLoopBack,
// 			pPort->m_nPortIndex);
// 		return VNM_RESULT_EXCEPTION;
// 	}
// 	//删除
// 	pSwitcher->m_mapPort.erase(pPort->m_nPortIndex);
// 	delete(pPort);

	return VNM_RESULT_OK;
}


//新建一个Port
LOCAL LPDTSVNMPort DTSVNMNewPort(LPDTSVNMParam pVNMParam)
{
	BISPASSERT(NULL!=pVNMParam);
	LPDTSVNMPort pPort = NULL;
//	pPort = (LPDTSVNMPort)DTS_MALLOC(sizeof(DTSVNMPort));
//	memset(pPort,0,sizeof(DTSVNMPort));
	pPort = new DTSVNMPort;
	if (NULL==pPort)
	{
		return NULL;
	}
	pPort->m_nPortIndex = pVNMParam->m_nPortIndex;
//	pPort->m_pSwitcher = NULL;
	pPort->m_dwSwitcherLoopBack = 0;
	pPort->m_bIsOutPort = FALSE;
	pPort->m_bIsAcitve = FALSE;
//	pPort->m_pVirtualNet = NULL;
	pPort->m_nVirNetNo = DTS_VNET_NO_NULL;
//	pPort->m_pOppoPort = NULL;
	return pPort;
}

LOCAL int DTSVNMAddVirNetInMap(LPDTSVirtualNet pVirNet)
{
	BISPASSERT(NULL!=pVirNet);
	l_mapVirNet[pVirNet->m_nVirNetNo] = pVirNet;
	return VNM_RESULT_OK;
}

LOCAL int DTSVNMAddSwitcherInMap(LPDTSVNMSwitcher pSwitcher)
{
	BISPASSERT(NULL!=pSwitcher);
	l_mapSwitcher[pSwitcher->m_dwSwitcherLoopBack] = pSwitcher;
	return VNM_RESULT_OK;
}

//检索端口
LPDTSVNMPort DTSVNMGetPort(DWORD dwSwitcherLoopBack,UINT nPortIndex)
{
	LPDTSVNMSwitcher pSwitcher =  DTSVNMGetSwitcherByLoopBack(dwSwitcherLoopBack);
	if (NULL==pSwitcher)
	{
		return NULL;
	}
	if (0==pSwitcher->m_mapPort.count(nPortIndex))
	{
		return NULL;
	}
	DTSVNMPortIter iterPort = pSwitcher->m_mapPort.find(nPortIndex);
	return iterPort->second;
	
}

//检索VirNet
LPDTSVirtualNet DTSVNMGetVirNetByNo(UINT nVirNetNo)
{
// 	DTSVNMVirNetIter	iterVirNet;
// 
// 	iterVirNet = l_mapVirNet.find(nVirNetNo);
// 	if (iterVirNet == l_mapVirNet.end())
// 	{
// 		return NULL;
// 	}
// 
// 	return iterVirNet->second;

	if (0==l_mapVirNet.count(nVirNetNo))
	{
		return NULL;
	}

	return l_mapVirNet[nVirNetNo];
}


//检索Switcher
LPDTSVNMSwitcher DTSVNMGetSwitcherByLoopBack(DWORD dwLoopBack)
{
	DTSVNMSwitcherIter	iterSwitcher;
	iterSwitcher = l_mapSwitcher.find(dwLoopBack);
	if (iterSwitcher==l_mapSwitcher.end())
	{
		return NULL;
	}
	return iterSwitcher->second;
}


//新加端口冲突判断，返回通过、失败、重复添加
LOCAL int DTSAddOutPortConflictCheck(LPDTSVNMParam pVNMParam)
{
	BISPASSERT(NULL!=pVNMParam);
	LPDTSVirtualNet pVirNet = NULL;
	LPDTSVNMSwitcher pSwither = NULL;
	LPDTSVNMPort pPort = NULL;
	DTSVNMPortIter		iterPort;
//	BOOL bIsSwitcherExist = FALSE;
	int nRet;
	
	//按目前设计，需要判断该端口是否已经属于另一个虚网，该交换机是否已经有另一个端口属于该虚网
	pSwither = DTSVNMGetSwitcherByLoopBack(pVNMParam->m_dwSwitcherLoopBack);
	if(NULL==pSwither)
	{
		//此前无此交换机配置
		return VNM_RESULT_OK;
	}

	//该交换机是否已经有另一个端口属于该虚网
	for (iterPort = pSwither->m_mapPort.begin();
		iterPort != pSwither->m_mapPort.end();
		iterPort++)
	{
		pPort = iterPort->second;	
		if(!pPort->m_bIsAcitve)
		{
			//非Active状态的端口忽略
			continue;
		}
//		pVirNet = (LPDTSVirtualNet)pPort->m_pVirtualNet;
		pVirNet = DTSVNMGetVirNetByNo(pPort->m_nVirNetNo);
		if ((NULL!=pVirNet)&&(pVirNet->m_nVirNetNo == pVNMParam->m_nVirNetNo))
		{
			nRet = (pPort->m_nPortIndex==pVNMParam->m_nPortIndex)?VNM_RESULT_REPEAT:VNM_RESULT_EXCEPTION;
			return nRet;
		}
	}
	
	//检索端口
	iterPort = pSwither->m_mapPort.find(pVNMParam->m_nPortIndex);
	if ((iterPort == pSwither->m_mapPort.end())||(!iterPort->second->m_bIsAcitve))
	{
		//此前无此端口配置
		return VNM_RESULT_OK;
	}
	
	pPort = iterPort->second;	
	if (!pPort->m_bIsOutPort)
	{
		//如果该端口是中继端口，则返回失败
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] Port(0x%x)(%d) is a trunk Port\n",
			pVNMParam->m_dwSwitcherLoopBack,pPort->m_nPortIndex);
		return VNM_RESULT_EXCEPTION;
	}
	else
	{
//		BISPASSERT(NULL!=pPort->m_pVirtualNet);
		//判断该端口是否已经属于另一个虚网
//		pVirNet = (LPDTSVirtualNet)pPort->m_pVirtualNet;
		pVirNet = DTSVNMGetVirNetByNo(pPort->m_nVirNetNo);
		BISPASSERT(NULL!=pVirNet);		
		if (pVNMParam->m_nVirNetNo == pVirNet->m_nVirNetNo)
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("[WARNING] Port(0x%x)(%d) is already in VirNet(%d)\n",
				pVNMParam->m_dwSwitcherLoopBack,
				pPort->m_nPortIndex,
				pVirNet->m_nVirNetNo);
			return VNM_RESULT_REPEAT;
		}
		else
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("[ERROR] Port(0x%x)(%d) already has in another VirNet(%d)\n",
				pVNMParam->m_dwSwitcherLoopBack,
				pPort->m_nPortIndex,
				pVirNet->m_nVirNetNo);
			return VNM_RESULT_EXCEPTION;
		}
	}
	return VNM_RESULT_OK;
}

//交换机是否还有端口属于该虚网
// LOCAL BOOL DTSVNMIsSwitcherInVNet(LPDTSVirtualNet pVirNet,
// 								LPDTSVNMSwitcher pSwitcher)
// {
// 	BISPASSERT(NULL!=pVirNet);
// 	BISPASSERT(NULL!=pSwitcher);
// 
// 	return FALSE;
// }

LOCAL LPDTSVNMSwitcher DTSVNMAddVirNetOutPortUpdateSwitcherMap(LPDTSVirtualNet pVirNet,
																LPDTSVNMParam pVNMParam)
{
	LPDTSVNMSwitcher pSwitcher = NULL;
	LPDTSVNMPort	 pPort = NULL;
	DTSVNMPortIter	 iterPort;

	//查找
	pSwitcher = DTSVNMGetSwitcherByLoopBack(pVNMParam->m_dwSwitcherLoopBack);
	//新建Switcher
	if (NULL==pSwitcher)
	{
		pSwitcher = DTSVNMNewSwitcher(pVNMParam);
		if (NULL==pSwitcher)
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("Create pSwither(0x%x) Fail\n",pVNMParam->m_dwSwitcherLoopBack);
			return NULL;
		}
		else
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)
				("Create pSwither(0x%x) Succeed\n",pVNMParam->m_dwSwitcherLoopBack);
		}

		//加入Map
		DTSVNMAddSwitcherInMap(pSwitcher);
		pSwitcher->m_nIndex = DTSVNMGetSwitcherCount();
	}

	//赋值
	pSwitcher->m_dwSwitcherLoopBack = pVNMParam->m_dwSwitcherLoopBack;
	//新增端口，经过前面的冲突检查，此端口配置应该不存在
	iterPort = pSwitcher->m_mapPort.find(pVNMParam->m_nPortIndex);
	if (iterPort == pSwitcher->m_mapPort.end())
	{
		pPort = DTSVNMNewPort(pVNMParam);
	}
	else
	{
		pPort = iterPort->second;
	}
	
	if (NULL==pPort)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("DTSVNMAddVirNetOutPortUpdateSwitcherMap Get pPort(0x%x)(%d) Fail\n",
			pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_nPortIndex);
		//暂不回收Switch
		return NULL;
	}
	
	//赋值端口
	pPort->m_bIsOutPort = TRUE;
	pPort->m_bIsAcitve = TRUE;
	pPort->m_nPortIndex = pVNMParam->m_nPortIndex;
	DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)
		("DTSVNMAddVirNetOutPortUpdateSwitcherMap Port(0x%x)(%d)->m_pSwitcher = pSwitcher\n",
		pSwitcher->m_dwSwitcherLoopBack,pPort->m_nPortIndex);
//	pPort->m_pSwitcher = pSwitcher;
	pPort->m_dwSwitcherLoopBack = pSwitcher->m_dwSwitcherLoopBack;
	pPort->m_nVirNetNo=pVirNet->m_nVirNetNo;
//	pPort->m_pVirtualNet = pVirNet;

	//加入端口列表
	pSwitcher->m_mapPort.insert(pair<UINT,LPDTSVNMPort>(pVNMParam->m_nPortIndex,pPort));
	return pSwitcher;
}


LOCAL LPDTSVirtualNet DTSVNMAddVirNetUpdateVirNetMap(LPDTSVNMParam pVNMParam)
{
	//更新VirNetMap
	LPDTSVirtualNet		pVirNet = NULL;
//	UINT nNewLabel=0;
	//查找VirNet
	pVirNet = DTSVNMGetVirNetByNo(pVNMParam->m_nVirNetNo);
	//未找到则创建新的
	if (NULL!=pVirNet)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] DTSVNMAddVirNetUpdateVirNetMap VirNet(%d) Already Exist\n",
			pVNMParam->m_nVirNetNo);
		return NULL;
	}
	else 
	{
// 		//先分配标签，再创建
// 		if (VNM_RESULT_OK!=DTSVNMMallocNewLabel(pVNMParam->m_nVirNetNo,&nNewLabel))
// 		{
// 			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
// 				("New VirNet(%d) Malloc Label Fail\n",pVNMParam->m_nVirNetNo);
// 			return NULL;
// 		}
// 		else
// 		{
// 			DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)
// 				("New VirNet(%d) Malloc Label(%d)\n",pVNMParam->m_nVirNetNo,nNewLabel);
// 		}

		//创建新的VirNet
		pVirNet = DTSVNMNewVirNet(pVNMParam);
		if (NULL==pVirNet)
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("Create VirNet(%d) Fail\n",pVNMParam->m_nVirNetNo);
			return NULL;
		}
		else
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)
				("Create VirNet(%d) Succeed\n",pVNMParam->m_nVirNetNo);
		}
		//加入VirNetMap
		DTSVNMAddVirNetInMap(pVirNet);
	}

	//赋值
	pVirNet->m_nVirLabelRef = pVNMParam->m_nVirNetNo;
	pVirNet->m_nVirNetNo = pVNMParam->m_nVirNetNo;
	pVirNet->m_nBandWidth = pVNMParam->m_nBandWidth;

//	pVirNet->m_mapOutPort[pVNMParam->m_dwSwitcherLoopBack] = pVNMParam->m_nPortIndex;

	return pVirNet;
}


LOCAL int DTSVNMMakeMacParam(LPDTSVNMParam pParam,
							UINT nLabelSwitchIndex,
							LPDTSVNMPath pPathRest)
{
	BISPASSERT(NULL!=pParam);
	BISPASSERT(NULL!=pPathRest);
//	LPDTSVNMLabelSwitch pLableSwitch = &pPathRest->m_vectLabelSwitch[nLabelSwitchIndex];
	LPDTSVNMLabelSwitch pLableSwitchTmp = NULL;
	//上游对端Mac地址
	if (nLabelSwitchIndex==0)
	{
		memset(pParam->m_arrbInOppoMac,DTS_VNM_INVALID_MAC,DTS_VNM_MAC_LEN);		
	}
	else
	{
		pLableSwitchTmp = &pPathRest->m_vectLabelSwitch[nLabelSwitchIndex-1];
		memcpy(pParam->m_arrbInOppoMac,pLableSwitchTmp->m_arrbOutMac,DTS_VNM_MAC_LEN);
	}
	//下游对端Mac地址
	if (nLabelSwitchIndex==pPathRest->m_vectLabelSwitch.size()-1)
	{
		memset(pParam->m_arrbOutOppoMac,DTS_VNM_INVALID_MAC,DTS_VNM_MAC_LEN);		
	}
	else
	{
		pLableSwitchTmp = &pPathRest->m_vectLabelSwitch[nLabelSwitchIndex+1];
		memcpy(pParam->m_arrbOutOppoMac,pLableSwitchTmp->m_arrbInMac,DTS_VNM_MAC_LEN);
	}
	return VNM_RESULT_OK;
}


LOCAL int DTSVNMAddTrunkSwitchFT(UINT nLabelSwitchIndex,
								LPDTSVirtualNet pVirNet,
								LPDTSVNMPath pPathRest,
								BOOL bIsAdd)
{
	BISPASSERT(NULL!=pPathRest);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(nLabelSwitchIndex < pPathRest->m_vectLabelSwitch.size());
	LPDTSVNMLabelSwitch pLableSwitch = &pPathRest->m_vectLabelSwitch[nLabelSwitchIndex];
	BISPASSERT(NULL!=pLableSwitch);
	DWORD dwSwitcherLoopBack = pLableSwitch->m_dwSwitcherLoopBack;	
	UINT nInPortIndex  = pLableSwitch->m_nInPortIndex;
	UINT nOutPortIndex = pLableSwitch->m_nOutPortIndex;
	DTSVNMParam	VNMFlowTableParam = {0};

	//设置上下游对端Mac地址
	DTSVNMMakeMacParam(&VNMFlowTableParam,nLabelSwitchIndex,pPathRest);	
	//正向交换，下游Mac为出口对端MAC
	memcpy(VNMFlowTableParam.m_arrbOppoMac,VNMFlowTableParam.m_arrbOutOppoMac,DTS_VNM_MAC_LEN);
	//正向交换，在交换中有效的是In标签作为匹配域，Out端口作为Out_put。
	VNMFlowTableParam.m_nInPWLabel = pPathRest->m_nBeginOutPWLabel;
	VNMFlowTableParam.m_nInVCLabel = pPathRest->m_nBeginOutVCLabel;
	//Out无用
	VNMFlowTableParam.m_nOutPWLabel = pPathRest->m_nBeginInPWLabel;	
	VNMFlowTableParam.m_nOutVCLabel = pPathRest->m_nBeginInVCLabel;

	VNMFlowTableParam.m_n64Cookie = pPathRest->m_n64Cookie;
	VNMFlowTableParam.m_nVirNetNo = pVirNet->m_nVirNetNo;
	VNMFlowTableParam.m_dwSwitcherLoopBack = dwSwitcherLoopBack;
	VNMFlowTableParam.m_nPortIndex = nInPortIndex;
	VNMFlowTableParam.m_nOppoPortIndex = nOutPortIndex;
	VNMFlowTableParam.m_nVNetLabel = pVirNet->m_nVirLabelRef;
	VNMFlowTableParam.m_nFTAction = bIsAdd?VNM_FT_ACT_SWITCH_ADD:VNM_FT_ACT_SWITCH_DEL;	

	strcpy(VNMFlowTableParam.m_pBuff,pVirNet->m_szVirNetName);
	if (VNM_RESULT_OK!= DTSVNMFuncCall(VNM_FUNC_CALL_FLOWTABLE_SEND,&VNMFlowTableParam))
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("!!!![WARNING]DTSVNMAddPathPort VirNet(%d) Send Router(0x%x) Port(%d)toPort(%d) Switch FlowTabel\n",
			pVirNet->m_nVirNetNo,
			VNMFlowTableParam.m_dwSwitcherLoopBack,
			VNMFlowTableParam.m_nPortIndex,
			VNMFlowTableParam.m_nOppoPortIndex);					
	}
	//反向交换
//	BYTE arrbMac[DTS_VNM_MAC_LEN] = {0};
	//有效匹配标签
	VNMFlowTableParam.m_nInPWLabel = pPathRest->m_nBeginInPWLabel;
	VNMFlowTableParam.m_nInVCLabel = pPathRest->m_nBeginInVCLabel;
	//Out无用
	VNMFlowTableParam.m_nOutPWLabel = pPathRest->m_nBeginOutPWLabel;	
	VNMFlowTableParam.m_nOutVCLabel = pPathRest->m_nBeginOutVCLabel;
	VNMFlowTableParam.m_nPortIndex = nOutPortIndex;
	VNMFlowTableParam.m_nOppoPortIndex = nInPortIndex;
	//反向交换，下游Mac为入口对端MAC
	memcpy(VNMFlowTableParam.m_arrbOppoMac,VNMFlowTableParam.m_arrbInOppoMac,DTS_VNM_MAC_LEN);
	if (VNM_RESULT_OK!= DTSVNMFuncCall(VNM_FUNC_CALL_FLOWTABLE_SEND,&VNMFlowTableParam))
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("!!!![WARNING]DTSVNMAddPathPort VirNet(%d) Send Router(0x%x) Port(%d)toPort(%d) Switch FlowTabel\n",
			pVirNet->m_nVirNetNo,
			VNMFlowTableParam.m_dwSwitcherLoopBack,
			VNMFlowTableParam.m_nPortIndex,
			VNMFlowTableParam.m_nOppoPortIndex);					
	}
	
	return VNM_RESULT_OK;
}



LOCAL int DTSVNMSendEdgeSwitchPortFT(UINT nLabelSwitchIndex,
									LPDTSVirtualNet pVirNet,
									LPDTSVNMPath pPathRest,
									BOOL bIsUpPort,
									BOOL bIsOutPort,
									BOOL bIsAdd)
{
	BISPASSERT(NULL!=pPathRest);
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(nLabelSwitchIndex < pPathRest->m_vectLabelSwitch.size());
	LPDTSVNMLabelSwitch pLableSwitch = &pPathRest->m_vectLabelSwitch[nLabelSwitchIndex];
	BISPASSERT(NULL!=pLableSwitch);
	DWORD dwSwitcherLoopBack = pLableSwitch->m_dwSwitcherLoopBack;	
	UINT nPortIndex  = bIsUpPort?pLableSwitch->m_nInPortIndex:pLableSwitch->m_nOutPortIndex;
	DTSVNMParam	VNMFlowTableParam = {0};

	//设置上下游对端Mac地址
	DTSVNMMakeMacParam(&VNMFlowTableParam,nLabelSwitchIndex,pPathRest);
	
	if (bIsUpPort) 
	{
		memcpy(VNMFlowTableParam.m_arrbOppoMac,VNMFlowTableParam.m_arrbInOppoMac,DTS_VNM_MAC_LEN);
	}
	else
	{
		memcpy(VNMFlowTableParam.m_arrbOppoMac,VNMFlowTableParam.m_arrbOutOppoMac,DTS_VNM_MAC_LEN);
	}

	//下发端口加入虚网的流表
	if (dwSwitcherLoopBack == pPathRest->m_dwBeginSwitcherLoopBack)
	{//起始节点
		VNMFlowTableParam.m_nOutPWLabel = pPathRest->m_nBeginOutPWLabel;	
		VNMFlowTableParam.m_nOutVCLabel = pPathRest->m_nBeginOutVCLabel;
		VNMFlowTableParam.m_nInPWLabel = pPathRest->m_nBeginInPWLabel;
		VNMFlowTableParam.m_nInVCLabel = pPathRest->m_nBeginInVCLabel;
	}
	else if (dwSwitcherLoopBack ==  pPathRest->m_dwEndSwitcherLoopBack)
	{//终点节点
		VNMFlowTableParam.m_nOutPWLabel = pPathRest->m_nBeginInPWLabel;	
		VNMFlowTableParam.m_nOutVCLabel = pPathRest->m_nBeginInVCLabel;
		VNMFlowTableParam.m_nInPWLabel	= pPathRest->m_nBeginOutPWLabel;
		VNMFlowTableParam.m_nInVCLabel	= pPathRest->m_nBeginOutVCLabel;
	}
	else
	{
		BISPASSERT(0);
	}
	VNMFlowTableParam.m_nVirNetNo = pVirNet->m_nVirNetNo;
	VNMFlowTableParam.m_dwSwitcherLoopBack = dwSwitcherLoopBack;
	VNMFlowTableParam.m_nPortIndex = nPortIndex;
	VNMFlowTableParam.m_nVNetLabel = pVirNet->m_nVirLabelRef;
	if (bIsAdd)
	{
		VNMFlowTableParam.m_nFTAction = bIsOutPort ? VNM_FT_ACT_OUT_PORT_ADD : VNM_FT_ACT_TRUNK_PORT_ADD;
	}
	else
	{
		VNMFlowTableParam.m_nFTAction = bIsOutPort ? VNM_FT_ACT_OUT_PORT_DEL : VNM_FT_ACT_TRUNK_PORT_DEL;
	}
	strcpy(VNMFlowTableParam.m_pBuff,pVirNet->m_szVirNetName);
	if (VNM_RESULT_OK!= DTSVNMFuncCall(VNM_FUNC_CALL_FLOWTABLE_SEND,&VNMFlowTableParam))
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[WARNING]DTSVNMAddPathPort VirNet(%d) Label(%d) Send Add TrunkPort(0x%x)(%d) FlowTabel Fail\n",
			pVirNet->m_nVirNetNo,
			VNMFlowTableParam.m_nVNetLabel,
			VNMFlowTableParam.m_dwSwitcherLoopBack,
			VNMFlowTableParam.m_nPortIndex);					
	}
	return VNM_RESULT_OK;
}


LOCAL int DTSVNMAddPathPort(DWORD dwSwitcherLoopBack,	
							UINT nPortIndex,
							LPDTSVirtualNet pVirNet,
							LPDTSVNMPath pPathRest,
							BOOL bIsOutPort,
							UINT* pnRefCount )
{
	BISPASSERT(NULL!=pVirNet);
//	BISPASSERT(NULL!=pPathPort);
	BISPASSERT(NULL!=pPathRest);
//	BISPASSERT(NULL!=pLabelSwitch);
	DTSVNMParam	VNMFlowTableParam = {0};	
	LPDTSVNMSwitcher pSwitcher = NULL;
	LPDTSVNMPort pPort = NULL;

	DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
		("[FLOW][VNM] DTSVNMAddPathPort VirNet(%d) PathID(%d) Port(0x%x)(%d) IsOutPort(%d)\n",
		pVirNet->m_nVirNetNo,pPathRest->m_nPathID,
		dwSwitcherLoopBack,nPortIndex,bIsOutPort);

	//新建Switcher、Port
	VNMFlowTableParam.m_dwSwitcherLoopBack = dwSwitcherLoopBack;
	VNMFlowTableParam.m_nPortIndex = nPortIndex;
	VNMFlowTableParam.m_nVNetLabel = pVirNet->m_nVirLabelRef;
	pSwitcher = DTSVNMAddVirNetOutPortUpdateSwitcherMap(pVirNet,&VNMFlowTableParam);
	if (NULL == pSwitcher)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[WARNING]DTSVNMAddPathPort VirNet(%d) Label(%d) Port(0x%x)(%d) Get Switcher Fail\n",
			pVirNet->m_nVirNetNo,
			VNMFlowTableParam.m_nVNetLabel,
			VNMFlowTableParam.m_dwSwitcherLoopBack,
			VNMFlowTableParam.m_nPortIndex);					
		return VNM_RESULT_EXCEPTION;
	}
	pPort = DTSVNMGetPort(dwSwitcherLoopBack,nPortIndex);
	if (NULL == pPort)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[WARNING]DTSVNMAddPathPort VirNet(%d) Label(%d) Port(0x%x)(%d) Get Port Fail\n",
			pVirNet->m_nVirNetNo,
			VNMFlowTableParam.m_nVNetLabel,
			VNMFlowTableParam.m_dwSwitcherLoopBack,
			VNMFlowTableParam.m_nPortIndex);
		return VNM_RESULT_EXCEPTION;
	}
	//端口影响的路径
	pPort->m_vectPathID.push_back(pPathRest->m_nPathID);
	pPort->m_bIsOutPort = bIsOutPort;

	//中继端口
	if (!bIsOutPort)
	{
		//中继端口不单独属于任何一个虚网
		pPort->m_nVirNetNo = DTS_VNET_NO_NULL;
//		pPort->m_pVirtualNet = NULL;
	}

	//第一次添加此标签则下发流表
	if (0==pPort->m_mapLabelReffrence.count(pVirNet->m_nVirLabelRef))
	{
		pPort->m_mapLabelReffrence[pVirNet->m_nVirLabelRef] = 0;
	}
	//更新端口标签引用计数
	pPort->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]++;
	*pnRefCount = pPort->m_mapLabelReffrence[pVirNet->m_nVirLabelRef];
	
	return VNM_RESULT_OK;
}

LOCAL int DTSVNMConculateP2PPath(LPDTSVirtualNet pVirNet, LPDTSVNMParam pVNMParam)
{
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(NULL!=pVNMParam);
	LPDTSVNMPath pPathRest = NULL;
//	DTSVNMParam	VNMFlowTableParam = {0};
	BOOL bIsOutPort = FALSE;
	LPDTSVNMLabelSwitch pLabelSwitch = NULL;
//	int nRet = VNM_RESULT_EXCEPTION;
//	LPDTSVNMPort pPathPort = NULL;
	UINT i = 0;
	UINT nRefCount = 0;
	pVNMParam->m_nBandWidth = pVirNet->m_nBandWidth;
#ifndef DTS_TEST
	DTSVNMPath pathTmp;
	pVNMParam->m_pReValueBuff = (char*)&pathTmp;
#endif	

	//计算路径
	if (VNM_RESULT_OK!= DTSVNMFuncCall(VNM_FUNC_CALL_TOPO_CONCULATE,pVNMParam))
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[WARNING]DTSVNMAddVirNetOutPort VirNet(%d) Conculate Path Src(0x%x) Dst(0x%x) Fail\n",
			pVNMParam->m_nVirNetNo,
			pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_dwOppoLoopBack);		
		return VNM_RESULT_EXCEPTION;
	}
	else
	{
		if (NULL == pVNMParam->m_pReValueBuff)
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("[WARNING]DTSVNMAddVirNetOutPort VirNet(%d) Conculate Path Src(0x%x) Dst(0x%x) Result NULL\n",
				pVNMParam->m_nVirNetNo,
				pVNMParam->m_dwSwitcherLoopBack,
				pVNMParam->m_dwOppoLoopBack);	
		}

		//赋值路径其他参数，计算该路径使用的标签
		pPathRest = (LPDTSVNMPath)pVNMParam->m_pReValueBuff;
		pPathRest->m_nVirNetNo = pVirNet->m_nVirNetNo;
		pPathRest->m_dwBeginSwitcherLoopBack = pVNMParam->m_dwSwitcherLoopBack;
		pPathRest->m_nBeginPortIndex = pVNMParam->m_nPortIndex;
		pPathRest->m_dwEndSwitcherLoopBack = pVNMParam->m_dwOppoLoopBack;		
		pPathRest->m_nEndPortIndex = pVNMParam->m_nOppoPortIndex;
		//计算交换标签
		if (VNM_RESULT_OK != DTSVNMSetPathLabel(pPathRest))
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("!!!![ERROR] DTSVNMConculateP2PPath  DTSVNMGetPathLabel Fail\n\n\n");
			return VNM_RESULT_EXCEPTION;
		}
		//根据交换标签生成cookie
		DTSVNMSetPathCookieByLabel(pPathRest);
		//遍历路径
		for (i=0;i<pPathRest->m_vectLabelSwitch.size();i++)
		{
			bIsOutPort = FALSE;
			pLabelSwitch = &(pPathRest->m_vectLabelSwitch[i]);
// 			pLabelSwitch->m_nInPWLabel = pVirNet->m_nVirLabelRef;
// 			pLabelSwitch->m_nOutPWLabel = pVirNet->m_nVirLabelRef;
			if (i!=0)
			{
				pLabelSwitch->m_nInPWLabel = pPathRest->m_nBeginOutPWLabel;
				pLabelSwitch->m_nInVCLabel = pPathRest->m_nBeginOutVCLabel;
				pLabelSwitch->m_nOutPWLabel = pPathRest->m_nBeginInPWLabel;
				pLabelSwitch->m_nOutVCLabel = pPathRest->m_nBeginInVCLabel;
			}
			else
			{
				pLabelSwitch->m_nInPWLabel = pPathRest->m_nBeginInPWLabel;
				pLabelSwitch->m_nInVCLabel = pPathRest->m_nBeginInVCLabel;
				pLabelSwitch->m_nOutPWLabel = pPathRest->m_nBeginOutPWLabel;
				pLabelSwitch->m_nOutVCLabel = pPathRest->m_nBeginOutVCLabel;
			}
			//建立交换要区分起始节点和中继节点的操作

			//起始节点
			if ((pLabelSwitch->m_dwSwitcherLoopBack == pVNMParam->m_dwSwitcherLoopBack))				
			{
				pLabelSwitch->m_nInPortIndex = pVNMParam->m_nPortIndex;						
//				bIsOutPort = TRUE;
				//操作入端口
				DTSVNMAddPathPort(	pLabelSwitch->m_dwSwitcherLoopBack,
									pLabelSwitch->m_nInPortIndex,
									pVirNet,
									pPathRest,
									TRUE,&nRefCount);
				//出口只需要加入一次虚网即可
				if (1==nRefCount)
				{
					DTSVNMSendEdgeSwitchPortFT(	i,
												pVirNet,
												pPathRest,TRUE,
												TRUE,TRUE);
				}
				
				//操作出端口
				DTSVNMAddPathPort(	pLabelSwitch->m_dwSwitcherLoopBack,
									pLabelSwitch->m_nOutPortIndex,
									pVirNet,
									pPathRest,
									FALSE,&nRefCount);

				DTSVNMSendEdgeSwitchPortFT(	i,
											pVirNet,
											pPathRest,FALSE,
											FALSE,TRUE);
				continue;

			}

			//终点节点
			if (pLabelSwitch->m_dwSwitcherLoopBack == pVNMParam->m_dwOppoLoopBack)
			{
				pLabelSwitch->m_nOutPortIndex = pVNMParam->m_nOppoPortIndex;
//				bIsOutPort = TRUE;
				//操作入端口
				DTSVNMAddPathPort(	pLabelSwitch->m_dwSwitcherLoopBack,
									pLabelSwitch->m_nInPortIndex,
									pVirNet,
									pPathRest,
									FALSE,&nRefCount);
				DTSVNMSendEdgeSwitchPortFT(	i,
											pVirNet,
											pPathRest,TRUE,
											FALSE,TRUE);

				//操作出端口
				DTSVNMAddPathPort(	pLabelSwitch->m_dwSwitcherLoopBack,
									pLabelSwitch->m_nOutPortIndex,
									pVirNet,
									pPathRest,
									TRUE,&nRefCount);
				//出口只需要加入一次虚网即可
				if (1==nRefCount)
				{
					DTSVNMSendEdgeSwitchPortFT(	i,
												pVirNet,
												pPathRest,FALSE,
												TRUE,TRUE);
				}
				continue;

			}

			//中继节点
			//操作入端口
			DTSVNMAddPathPort(	pLabelSwitch->m_dwSwitcherLoopBack,
								pLabelSwitch->m_nInPortIndex,
								pVirNet,
								pPathRest,
								FALSE,&nRefCount);

			//操作出端口
			DTSVNMAddPathPort(	pLabelSwitch->m_dwSwitcherLoopBack,
								pLabelSwitch->m_nOutPortIndex,
								pVirNet,
								pPathRest,
								FALSE,&nRefCount);

			//中继交换
			DTSVNMAddTrunkSwitchFT(	i,pVirNet,pPathRest,TRUE);
		}													
	}

// 	pPathRest->m_nBeginPortIndex = pVNMParam->m_nPortIndex;
// 	pPathRest->m_nEndPortIndex = pVNMParam->m_nOppoPortIndex;
// 	pPathRest->m_nVirNetNo = pVirNet->m_nVirNetNo;
	//记录分配成功的路径
	if (VNM_RESULT_OK != DTSVNMAddNewPath(pPathRest))
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[WARNING]DTSVNMAddVirNetOutPort VirNet(%d) Add PathID(%d) Src(0x%x) Dst(0x%x) Fail\n",
			pVNMParam->m_nVirNetNo,
			pPathRest->m_nPathID,
			pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_dwOppoLoopBack);	
		return VNM_RESULT_EXCEPTION;		
	}
	
	return VNM_RESULT_OK;
}


LOCAL int DTSVNMAddNewPortCanculate(LPDTSVNMParam		pVNMParam,
									LPDTSVirtualNet		pVirNet)
									//LPDTSVNMSwitcher	pSwither)
									//LPDTSVNMPort		pPort)
{
	BISPASSERT(NULL!=pVNMParam);
	BISPASSERT(NULL!=pVirNet);
//	BISPASSERT(NULL!=pSwither);
//	BISPASSERT(NULL!=pPort);

	DTSVNMVirNetOutPortIter	iterOutPort;
//	DTSVNMPath	VirNetPathResult = {0};
//	LPDTSVNMPath pPathRest = NULL;
//	DTSVNMParam	VNMFlowTableParam = {0};
//	LPDTSVNMLabelSwitch pLabelSwitch = NULL;
	int nRet = VNM_RESULT_EXCEPTION;
//	LPDTSVNMPort pPathPort = NULL;
//	LPDTSVNMPort pOutPort = NULL;
//	UINT i = 0;
//	BOOL bIsInPort = FALSE;
//	BOOL bIsOutPort = FALSE;

	//对出口下发打标签的和解标签的流表
// 	pVNMParam->m_nFTAction = VNM_FT_ACT_OUT_PORT_ADD;
// 	pVNMParam->m_nVirNetLabel = pVirNet->m_nVirLabel;
// 	strcpy(pVNMParam->m_pBuff,pVirNet->m_szVirNetName);
// 	if (VNM_RESULT_OK!= DTSVNMFuncCall(VNM_FUNC_CALL_FLOWTABLE_SEND,pVNMParam))
// 	{
// 		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
// 			("[WARNING]DTSVNMAddVirNetOutPort VirNet(%d) Send Add OutPort(0x%x)(%d) FlowTabel Fail\n",
// 			pVNMParam->m_nVirNetNo,
// 			pVNMParam->m_dwSwitcherLoopBack,
// 			pVNMParam->m_nPortIndex);		
// 		return VNM_RESULT_EXCEPTION;
// 	}

	for (iterOutPort = pVirNet->m_mapOutPort.begin();
		iterOutPort != pVirNet->m_mapOutPort.end();
		iterOutPort++)
	{//for_mapOutPort
		//遍历原有端口，一对一计算原有路径
		if (iterOutPort->first != pVNMParam->m_dwSwitcherLoopBack)
		{
			pVNMParam->m_dwOppoLoopBack = iterOutPort->first;
			pVNMParam->m_nOppoPortIndex = iterOutPort->second;

 			if(VNM_RESULT_OK == DTSVNMConculateP2PPath(pVirNet,pVNMParam))
			{
				//只要有一条路径分配成功，切换标志位做后续处理
				nRet = VNM_RESULT_OK;
			}
		}
	}//for_mapOutPort
	
	//如果有一条路径计算成功，则将该端口添加到VNet出口列表，则切换至Active状态
	if (VNM_RESULT_OK == nRet)
	{
		pVirNet->m_nStats = DTS_VNM_STATS_ACTIVE;
		pVirNet->m_mapOutPort[pVNMParam->m_dwSwitcherLoopBack] = pVNMParam->m_nPortIndex;
		DTS_VNM_DEBUG_PRINTF(DEBUG_FSM)
			("[FSM][VNM] VirNet(%d) Stats Change To %s\n",
			pVirNet->m_nVirNetNo,
			DTSVNMGetVirNetStatsStr(pVirNet));
	}

	return nRet;
}


LOCAL int DTSVNMSendCreateVirNetWithPortFT(LPDTSVirtualNet pVirNet,LPDTSVNMParam pVNMParam)
{
	DTSVNMParam VNMFlowTableParam = {0};
	VNMFlowTableParam.m_nFTAction = VNM_FT_ACT_NEW_VIRNET_WITH_PORT;
	VNMFlowTableParam.m_nVirNetNo = pVirNet->m_nVirNetNo;
	VNMFlowTableParam.m_dwSwitcherLoopBack = pVNMParam->m_dwSwitcherLoopBack;
	VNMFlowTableParam.m_nPortIndex = pVNMParam->m_nPortIndex;
	strcpy(VNMFlowTableParam.m_pBuff,pVirNet->m_szVirNetName);
	return DTSVNMFuncCall(VNM_FUNC_CALL_FLOWTABLE_SEND,&VNMFlowTableParam);	
}


LOCAL int DTSVNMSendDelVirNetFT(LPDTSVirtualNet pVirNet,LPDTSVNMSwitcher pSwitcher)
{
	DTSVNMParam VNMFlowTableParam = {0};
	VNMFlowTableParam.m_nVirNetNo = pVirNet->m_nVirNetNo;
	VNMFlowTableParam.m_nFTAction = VNM_FT_ACT_DEL_VIRNET;
	VNMFlowTableParam.m_dwSwitcherLoopBack = pSwitcher->m_dwSwitcherLoopBack;
	strcpy(VNMFlowTableParam.m_pBuff,pVirNet->m_szVirNetName);
	return DTSVNMFuncCall(VNM_FUNC_CALL_FLOWTABLE_SEND,&VNMFlowTableParam);	
}


LOCAL int DTSVNMAddVirNetOutPort(LPDTSVNMParam pVNMParam)
{
	BISPASSERT(NULL!=pVNMParam);
	LPDTSVirtualNet		pVirNet = NULL;
	LPDTSVNMSwitcher	pSwither = NULL;
	LPDTSVNMPort	pPort = NULL;
	int nRet = 0;
	pVirNet = DTSVNMGetVirNetByNo(pVNMParam->m_nVirNetNo);
	BOOL bIsSucceed = FALSE;

	if (NULL==pVirNet)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] Add VirNet Port Fail VirNet(%d) Not Exist\n",
			pVNMParam->m_nVirNetNo);
		return VNM_RESULT_EXCEPTION;
	}
		
	//--------------------------------------------------------------------------
	//配置冲突检测，
	//按目前设计，需要判断该端口是否已经属于另一个虚网
	//该交换机是否已经有另一个端口属于该虚网
	nRet = DTSAddOutPortConflictCheck(pVNMParam);
	if (VNM_RESULT_OK != nRet)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)
			("[WARNING]DTSVNMAddVirNetOutPort VNet(%d) Add Port(0x%x)(%d) Conflict Check nRet(%d)\n",
			pVirNet->m_nVirNetNo,pVNMParam->m_dwSwitcherLoopBack,pVNMParam->m_nPortIndex,nRet);
		return nRet;
		//VNM_RESULT_EXCEPTION或者VNM_RESULT_REPEAT
	}

	//此前无端口，则返回，不计算路径
	pSwither = DTSVNMAddVirNetOutPortUpdateSwitcherMap(pVirNet,pVNMParam);
	if (NULL==pSwither)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)
			("[WARNING]DTSVNMAddVirNetOutPort VNet(%d) Add Port(0x%x)(%d) DTSVNMAddVirNetOutPortUpdateSwitcherMap Fail\n",
			pVirNet->m_nVirNetNo,pVNMParam->m_dwSwitcherLoopBack,pVNMParam->m_nPortIndex);
		return VNM_RESULT_EXCEPTION;
	}
	pPort = DTSVNMGetPort(pVNMParam->m_dwSwitcherLoopBack,
						pVNMParam->m_nPortIndex);
	if (NULL==pPort)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)
			("[WARNING]DTSVNMAddVirNetOutPort VNet(%d) Add Port(0x%x)(%d) NULL==pPort\n",
			pVirNet->m_nVirNetNo,pVNMParam->m_dwSwitcherLoopBack,pVNMParam->m_nPortIndex);
		return VNM_RESULT_EXCEPTION;
	}
	//在路径计算成功前，暂时设置为Deactive
	pPort->m_bIsAcitve = FALSE;
	if (DTS_VNM_STATS_NONE == pVirNet->m_nStats)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_FSM)
			("[FSM][VNM] VirNet(%d) Stats Change To %s\n",
			pVirNet->m_nVirNetNo,
			DTSVNMGetVirNetStatsStr(pVirNet));		
		//下发建立虚网的流表
		if (VNM_RESULT_OK == DTSVNMSendCreateVirNetWithPortFT(pVirNet,pVNMParam))
		{
			bIsSucceed = TRUE;
			pVirNet->m_nStats = DTS_VNM_STATS_ONE_OUT_PORT;
		}
	}	
	else
	{
		//在 DTS_VNM_STATS_ONE_OUT_PORT 和 DTS_VNM_STATS_ACTIVE状态下需要计算路径
		//调用路径计算模块，计算与该虚网现有的出口交换机的转发路径
		if (VNM_RESULT_OK == DTSVNMAddNewPortCanculate(pVNMParam,pVirNet))
		{
			bIsSucceed = TRUE;
		}
		else
		{
			bIsSucceed = FALSE;
			DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)
				("[WARNING] VNet(%d) Add Port(0x%x)(%d) Path Canculate Fail\n",
				pVirNet->m_nVirNetNo,pVNMParam->m_dwSwitcherLoopBack,pVNMParam->m_nPortIndex);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	if (bIsSucceed)
	{
		//-------------------------------------------------------------------------
		//更新VirNetMap
		pVirNet->m_mapOutPort.insert(pair<DWORD,UINT>(pVNMParam->m_dwSwitcherLoopBack,
									pVNMParam->m_nPortIndex));

		//-------------------------------------------------------------------------
		//更新Port		
		pPort->m_bIsAcitve = TRUE;
		return VNM_RESULT_OK;
	}
	else
	{
		return VNM_RESULT_EXCEPTION;
	}

}

LOCAL int DTSVNMErasePathPortLabelSwitcher(	LPDTSVirtualNet pVirNet,
											LPDTSVNMPort pPort,
											UINT	nPathID)
{
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(NULL!=pPort);	
//	DTSVNMParam VNMFlowTableParam = {0};
//	UINT i = 0;
	vector<UINT>::iterator iterPathID,iterPathIDMatch;
			
	//降低路径上端口的Label引用计数
	//如果归零则下发流表在VNet中删除该端口（区分出端口或中继端口）
	if (0==pPort->m_mapLabelReffrence.count(pVirNet->m_nVirLabelRef))
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[FLOW] DTSVNMErasePathPortLabelSwitcher pPort(0x%x)(%d) Cant Find Label(%d)\n",
			pPort->m_dwSwitcherLoopBack,
			pPort->m_nPortIndex,
			pVirNet->m_nVirLabelRef);
	//此处不返回失败，出口可能会被重复删除多次
	//	return VNM_RESULT_EXCEPTION;
	}
	else
	{	
		pPort->m_mapLabelReffrence[pVirNet->m_nVirLabelRef]--;
		if (0==pPort->m_mapLabelReffrence[pVirNet->m_nVirLabelRef])
		{
// 			VNMFlowTableParam.m_nVirNetNo = pVirNet->m_nVirNetNo;
// 			VNMFlowTableParam.m_dwSwitcherLoopBack = pPort->m_dwSwitcherLoopBack;
// 			VNMFlowTableParam.m_nPortIndex = pPort->m_nPortIndex;
// 			VNMFlowTableParam.m_nVNetLabel = pVirNet->m_nVirLabelRef;
// 			VNMFlowTableParam.m_nFTAction = pPort->m_bIsOutPort ? VNM_FT_ACT_OUT_PORT_DEL : VNM_FT_ACT_TRUNK_PORT_DEL;
// 			strcpy(VNMFlowTableParam.m_pBuff,pVirNet->m_szVirNetName);
// 			if (VNM_RESULT_OK!= DTSVNMFuncCall(VNM_FUNC_CALL_FLOWTABLE_SEND,&VNMFlowTableParam))
// 			{
// 				DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
// 					("[WARNING]DTSVNMErasePathPortLabelSwitcher VirNet(%d) Label(%d) Send Del Port(0x%x)(%d) FlowTabel Fail\n",
// 					pVirNet->m_nVirNetNo,
// 					VNMFlowTableParam.m_nVNetLabel,
// 					VNMFlowTableParam.m_dwSwitcherLoopBack,
// 					VNMFlowTableParam.m_nPortIndex);					
// 			}
// 			//删除该标签
			pPort->m_mapLabelReffrence.erase(pVirNet->m_nVirLabelRef);
		}
	}
	
	BOOL bIsMatch = FALSE;
	for (iterPathID = pPort->m_vectPathID.begin();
		iterPathID != pPort->m_vectPathID.end();
		iterPathID++)
	{
		if (*iterPathID == nPathID)
		{
			iterPathIDMatch = iterPathID;
			bIsMatch = TRUE;
			break;
		}
	}
	if (bIsMatch)
	{
		pPort->m_vectPathID.erase(iterPathIDMatch);
	}
	
	//中继端口如果已经没有路径引用，应该将该端口置为Deactive
	if ((0==pPort->m_vectPathID.size())&&(!pPort->m_bIsOutPort))
	{
		BISPASSERT(0==pPort->m_mapLabelReffrence.size());
		DTSVNMDeactivePortInSwitcher(pPort);
	}
	
	return VNM_RESULT_OK;
}


LOCAL int DTSVNMDelPathEraseLabelSwitch(LPDTSVNMPath pPath)
{
//	LPDTSVNMPath pPath = DTSVNMGetPath(nPathID);
	BISPASSERT(NULL!=pPath);
	LPDTSVNMPort pPortIn = NULL;
	LPDTSVNMPort pPortOut = NULL;
	
	LPDTSVNMLabelSwitch pLabelSwitcher = NULL;
	LPDTSVirtualNet pVirNet = DTSVNMGetVirNetByNo(pPath->m_nVirNetNo);
	if (NULL==pVirNet)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] DTSVNMDelPathEraseLabelSwitch Path(%d) VNet(%d) not Exist\n",
			pPath->m_nPathID,
			pPath->m_nVirNetNo);
		return VNM_RESULT_EXCEPTION;
	}
	UINT i;
	for (i=0;i<pPath->m_vectLabelSwitch.size();i++)
	{
		pLabelSwitcher = &pPath->m_vectLabelSwitch[i];
		pPortIn = DTSVNMGetPort(pLabelSwitcher->m_dwSwitcherLoopBack,
								pLabelSwitcher->m_nInPortIndex);
		if (NULL==pPortIn)
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("[ERROR] DTSVNMDelPathEraseLabelSwitch Path(%d) Port(0x%x)(%d) not Exist\n",
				pPath->m_nPathID,
				pLabelSwitcher->m_dwSwitcherLoopBack,
				pLabelSwitcher->m_nInPortIndex);
			return VNM_RESULT_EXCEPTION;
		}
		DTSVNMErasePathPortLabelSwitcher(pVirNet,pPortIn,pPath->m_nPathID);

		pPortOut = DTSVNMGetPort(pLabelSwitcher->m_dwSwitcherLoopBack,
								pLabelSwitcher->m_nOutPortIndex);
		if (NULL==pPortIn)
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("[ERROR] DTSVNMDelPathEraseLabelSwitch Path(%d) Port(0x%x)(%d) not Exist\n",
				pPath->m_nPathID,
				pLabelSwitcher->m_dwSwitcherLoopBack,
				pLabelSwitcher->m_nOutPortIndex);
			return VNM_RESULT_EXCEPTION;
		}
		DTSVNMErasePathPortLabelSwitcher(pVirNet,pPortOut,pPath->m_nPathID);

		//按顺序清除交换，删除交换需要区分边缘和中继交换机
		//始发节点
		if (0==i)
		{
			
			if (0==pPortIn->m_mapLabelReffrence.count(pVirNet->m_nVirLabelRef))
			{
				DTSVNMSendEdgeSwitchPortFT(i,pVirNet,pPath,TRUE,TRUE,FALSE);				
			}			

			DTSVNMSendEdgeSwitchPortFT(i,pVirNet,pPath,FALSE,FALSE,FALSE);

			continue;
		}

		//终点节点
		if (pPath->m_vectLabelSwitch.size()-1 == i)
		{
			DTSVNMSendEdgeSwitchPortFT(i,pVirNet,pPath,TRUE,FALSE,FALSE);				

			if (0==pPortOut->m_mapLabelReffrence.count(pVirNet->m_nVirLabelRef))
			{
				DTSVNMSendEdgeSwitchPortFT(i,pVirNet,pPath,FALSE,TRUE,FALSE);
			}			
			continue;
		}

		//中继节点
		DTSVNMAddTrunkSwitchFT(i,pVirNet,pPath,FALSE);
	}

	return VNM_RESULT_OK;
}


LOCAL int DTSVNMVNetStatsActiveDelOutPort(LPDTSVirtualNet pVirNet,LPDTSVNMPort pPort)
{
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(NULL!=pPort);
	UINT i=0;
	LPDTSVNMPath pPath = NULL;
	vector<UINT> vectPath;
	//--调用前已完成pPort和VNet的合法性检测

	//复制一份，在遍历过程中可能被删除
	for (i=0;i<pPort->m_vectPathID.size();i++)
	{
		vectPath.push_back(pPort->m_vectPathID[i]);
	}

	//检索出口Port影响的PathID，顺序删除
	for (i=0;i<vectPath.size();i++)
	{
		//按顺序清除交换、只删除该VNet的Path
		pPath = DTSVNMGetPathByID(vectPath[i]);
		if (NULL==pPath)
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("[ERROR] DTSVNMVNetStatsActiveDelOutPort pPath(%d) Not Exist\n",
				vectPath[i]);
			continue;
		}
		if (pPath->m_nVirNetNo == pVirNet->m_nVirNetNo)
		{
			DTSVNMDelPathEraseLabelSwitch(pPath);
		}
		//删除路径
		DTSVNMDelPath(pPath->m_nPathID);
	}
	
	//VNet，删除出口列表，检查出口端口数量，切换状态
	pVirNet->m_mapOutPort.erase(pPort->m_dwSwitcherLoopBack);
	if (1==pVirNet->m_mapOutPort.size())
	{
		pVirNet->m_nStats = DTS_VNM_STATS_ONE_OUT_PORT;
		DTS_VNM_DEBUG_PRINTF(DEBUG_FSM)
			("[FSM][VNM] VNet(%d) Del Port(0x%x)(%d) change to Stats(%s)\n",
			pVirNet->m_nVirNetNo,
			pPort->m_dwSwitcherLoopBack,pPort->m_nPortIndex,
				DTSVNMGetVirNetStatsStr(pVirNet));
	}
	//将该端口设置为Deactive
	BISPASSERT(0==pPort->m_mapLabelReffrence.size());
	BISPASSERT(0==pPort->m_vectPathID.size());
	DTSVNMDeactivePortInSwitcher(pPort);
	
	return VNM_RESULT_OK;
}

LOCAL int DTSVNMDelVNetInAllSwitcher(LPDTSVirtualNet pVirNet)
{
//	LPDTSVNMSwitcher	pSwitcher = NULL;
	DTSVNMSwitcherIter	iterSwitcher;
	for (iterSwitcher=l_mapSwitcher.begin();
		iterSwitcher!=l_mapSwitcher.end();
		iterSwitcher++)
	{
		//下发流表清除Switcher上的VNet配置
		DTSVNMSendDelVirNetFT(pVirNet,iterSwitcher->second);
	}
	return VNM_RESULT_OK;
}

LOCAL int DTSVNMVNetStatsOneOutPortDelOutPort(LPDTSVirtualNet pVirNet,LPDTSVNMParam pVNMParam)
{
	BISPASSERT(NULL!=pVirNet);
	BISPASSERT(NULL!=pVNMParam);
	LPDTSVNMSwitcher		pSwitcher = NULL;
	LPDTSVNMPort			pPort = NULL;
//	LPDTSVirtualNet			pPortVNet = NULL;
	DTSVNMVirNetOutPortIter iterVNetPort;
	DTSVNMPortIter			iterSwitcherPort;

	//VNet操作
	iterVNetPort = pVirNet->m_mapOutPort.find(pVNMParam->m_dwSwitcherLoopBack);
	if ((iterVNetPort == pVirNet->m_mapOutPort.end())
		||(iterVNetPort->second!=pVNMParam->m_nPortIndex))
	{
		//在虚网中没有找到该出口
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] Del VirNet(%d) Port(0x%x)(%d) Fail Port Not Exist in VNet\n",
			pVNMParam->m_nVirNetNo,
			pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_nPortIndex);
		return VNM_RESULT_EXCEPTION;
	}
	pVirNet->m_mapOutPort.erase(pVNMParam->m_dwSwitcherLoopBack);
	pVirNet->m_nStats = DTS_VNM_STATS_NONE;
	DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
		("[FSM][VNM] VirNet(%d) Change To Stats(%d)(%s)\n",
		pVirNet->m_nVirNetNo,
		pVirNet->m_nStats,
		DTSVNMGetVirNetStatsStr(pVirNet));

	//Switcher操作
	pSwitcher = DTSVNMGetSwitcherByLoopBack(pVNMParam->m_dwSwitcherLoopBack);
	if (NULL==pSwitcher)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)	
			("[ERROR] Del VirNet(%d) Port(0x%x)(%d) Fail Switcher Not Exist\n",
			pVNMParam->m_nVirNetNo,
			pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_nPortIndex);
		return VNM_RESULT_EXCEPTION;
	}
	pPort = DTSVNMGetPort(pVNMParam->m_dwSwitcherLoopBack,
						pVNMParam->m_nPortIndex);
	DTSVNMDeactivePortInSwitcher(pPort);

	//通告所有的Switcher，删除VNet
	return DTSVNMDelVNetInAllSwitcher(pVirNet);
	
// 	pSwitcher = DTSVNMGetSwitcherByLoopBack(pVNMParam->m_dwSwitcherLoopBack);
// 	if (NULL==pSwitcher)
// 	{
// 		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)	
// 			("[ERROR] Del VirNet(%d) Port(0x%x)(%d) Fail Switcher Not Exist\n",
// 			pVNMParam->m_nVirNetNo,
// 			pVNMParam->m_dwSwitcherLoopBack,
// 			pVNMParam->m_nPortIndex);
// 		return VNM_RESULT_EXCEPTION;
// 	}
// 	iterSwitcherPort = pSwitcher->m_mapPort.find(pVNMParam->m_nPortIndex);
// 	if (iterSwitcherPort == pSwitcher->m_mapPort.end())
// 	{
// 		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
// 			("[ERROR] Del VirNet(%d) Port(0x%x)(%d) Fail Port Not Exist in Switcher\n",
// 			pVNMParam->m_nVirNetNo,
// 			pVNMParam->m_dwSwitcherLoopBack,
// 			pVNMParam->m_nPortIndex);
// 		return VNM_RESULT_EXCEPTION;
// 	}
// 	pPort = iterSwitcherPort->second;
// 	pPortVNet = DTSVNMGetVirNetByNo(pPort->m_nVirNetNo);
// 	if ((NULL!=pPortVNet)&&(pPortVNet->m_nVirNetNo == pVNMParam->m_nVirNetNo))
// 	{
// 		//在端口列表中删除
// 		pSwitcher->m_mapPort.erase(pVNMParam->m_nPortIndex);// 		
// 		//回收Port的空间
// 		delete(pPort);
// 		return VNM_RESULT_OK;
// 	}
// 	else
// 	{
// 		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
// 			("[ERROR] Del VNet(%d) Port(0x%x)(%d) Fail Switcher Map Port Not in VNet\n",
// 			pVNMParam->m_nVirNetNo,
// 			pVNMParam->m_dwSwitcherLoopBack,
// 			pVNMParam->m_nPortIndex);
// 		return VNM_RESULT_EXCEPTION;
// 	}
	
}

LOCAL int DTSVNMVNetStatsActiveDelAll(LPDTSVirtualNet pVirNet)
{
	BISPASSERT(NULL!=pVirNet);
//	LPDTSVNMPort pPort = NULL;
	DTSVNMVirNetOutPortIter iterOutPort;
	DTSVNMParam DelPortParam = {0};
	DelPortParam.m_nVirNetNo = pVirNet->m_nVirNetNo;
	DTSVNMVirNetOutPortMap	mapTmp;


	for (iterOutPort = pVirNet->m_mapOutPort.begin();
		iterOutPort!=pVirNet->m_mapOutPort.end();
		iterOutPort++)
	{
		mapTmp.insert(pair<DWORD,UINT>(iterOutPort->first,iterOutPort->second));
	}
	
	for (iterOutPort = mapTmp.begin();
		iterOutPort!=mapTmp.end();
		iterOutPort++)
	{
// 		pPort = DTSVNMGetPort(iterOutPort->first,iterOutPort->second);
// 		if (NULL==pPort)
// 		{
// 			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
// 				("[ERROR] DTSVNMVNetStatsActiveDelAll VirNet(%d) OutPort(ox%x)(%d) Not Exist\n",
// 				pVirNet->m_nVirNetNo,
// 				iterOutPort->first,
// 				iterOutPort->second);
// 		}
		DelPortParam.m_dwSwitcherLoopBack = iterOutPort->first;
		DelPortParam.m_nPortIndex = iterOutPort->second;
		DTSVNMDelVirNetOutPort(&DelPortParam);
//		DTSVNMVNetStatsActiveDelOutPort(pVirNet,pPort);
	}

	return DTSVNMDelVirNetStatsNone(pVirNet);

//	return VNM_RESULT_OK;
}


LOCAL int DTSVNMDelVNet(LPDTSVNMParam pVNMParam)
{
	BISPASSERT(NULL!=pVNMParam);
	LPDTSVirtualNet		pVirNet = NULL;
	pVirNet = DTSVNMGetVirNetByNo(pVNMParam->m_nVirNetNo);
	if (NULL==pVirNet)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] Del VirNet Port Fail VirNet(%d) Not Exist\n",
			pVNMParam->m_nVirNetNo);
		return VNM_RESULT_EXCEPTION;
	}

	switch(pVirNet->m_nStats)
	{
	case DTS_VNM_STATS_NONE:
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[WARNING]Del VNet(%d) Port(0x%x)(%d) VNet Stats(%s)",
			pVirNet->m_nVirNetNo,
			pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_nPortIndex,
			DTSVNMGetVirNetStatsStr(pVirNet));
		return DTSVNMDelVirNetStatsNone(pVirNet);
		
	case  DTS_VNM_STATS_ONE_OUT_PORT:
		//该函数最后会清除所有交换机上的虚网配置
		DTSVNMVNetStatsOneOutPortDelOutPort(pVirNet,pVNMParam);
		//如果交换机内已无此虚网配置，需要发送流表清除虚网配置及交换关系
		return DTSVNMDelVirNetStatsNone(pVirNet);
//		return VNM_RESULT_OK;

	case DTS_VNM_STATS_ACTIVE:
		//如果交换机内已无此虚网配置，需要发送流表清除虚网配置及交换关系
		return DTSVNMVNetStatsActiveDelAll(pVirNet);
//		return VNM_RESULT_OK;

	default:
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[WARNING][IGNORE] Del VNet(%d) Port(0x%x)(%d) VNet Stats(%d) Ignore",
			pVirNet->m_nVirNetNo,
			pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_nPortIndex,
			pVirNet->m_nStats);
		return VNM_RESULT_OK;
	}

}

//通过检索pPort的某一条路径，判断该链接是否与记录的一致
LOCAL BOOL DTSVNMIsValidLink(LPDTSVNMPort	pPort,
							LPDTSVNMPort	pOppoPort)
{
	BISPASSERT(NULL!=pPort);
	BISPASSERT(NULL!=pOppoPort);

	LPDTSVNMPort pLabelSwitchOutPort = NULL;
	LPDTSVNMPort pLabelSwitchNextInPort = NULL;

	if (0==pPort->m_vectPathID.size())
	{
		return FALSE;
	}
	LPDTSVNMPath pPath = DTSVNMGetPathByID(pPort->m_vectPathID[0]);
	if (NULL==pPath)
	{
		return FALSE;
	}
	UINT i = 0;
//	LPDTSVNMLabelSwitch pLabelSwitch = NULL;
	for (i=0; i<pPath->m_vectLabelSwitch.size()-1;i++)
	{
		pLabelSwitchOutPort = DTSVNMGetPort(pPath->m_vectLabelSwitch[i].m_dwSwitcherLoopBack,
											pPath->m_vectLabelSwitch[i].m_nOutPortIndex);
		BISPASSERT(NULL!=pLabelSwitchOutPort);
		pLabelSwitchNextInPort = DTSVNMGetPort(pPath->m_vectLabelSwitch[i+1].m_dwSwitcherLoopBack,
											pPath->m_vectLabelSwitch[i+1].m_nInPortIndex);
		BISPASSERT(NULL!=pLabelSwitchNextInPort);
		if (((pLabelSwitchOutPort==pPort)&&(pLabelSwitchNextInPort==pOppoPort))
		 ||((pLabelSwitchNextInPort==pPort)&&(pLabelSwitchOutPort==pOppoPort)))
		{
			return TRUE;
		}
	}
	return FALSE;
}


LOCAL int DTSVNMProcLinkDown(LPDTSVNMParam pVNMParam)
{
	BISPASSERT(NULL!=pVNMParam);
	LPDTSVNMPort pPort = DTSVNMGetPort(	pVNMParam->m_dwSwitcherLoopBack,
										pVNMParam->m_nPortIndex);
	//没有虚网路径受影响
	if ((NULL==pPort)||(!pPort->m_bIsAcitve))
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)
			("[FLOW][VNM] DTSVNMLinkDown Port(0x%x)(%d) Not Active\n",
			pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_nPortIndex);
		return VNM_RESULT_OK;
	}

	LPDTSVNMPort pOppoPort = DTSVNMGetPort(	pVNMParam->m_dwOppoLoopBack,
											pVNMParam->m_nOppoPortIndex);
	//对端端口应该存在
	if ((NULL==pOppoPort)||(!pOppoPort->m_bIsAcitve))
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR][VNM] DTSVNMLinkDown Port(0x%x)(%d) Active pOppoPort(0x%x)(%d) Not Active\n",
			pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_nPortIndex,
			pVNMParam->m_dwOppoLoopBack,
			pVNMParam->m_nOppoPortIndex);
		return VNM_RESULT_EXCEPTION;
	}

	//检索某条路径，检查这两个端口是否直连
	if (!DTSVNMIsValidLink(pPort,pOppoPort))
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR][VNM] DTSVNMLinkDown Link Invalid Port(0x%x)(%d) pOppoPort(0x%x)(%d)\n",
			pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_nPortIndex,
			pVNMParam->m_dwOppoLoopBack,
			pVNMParam->m_nOppoPortIndex);
		return VNM_RESULT_EXCEPTION;
	}

	//查询该端口影响的路径，循环删除
	UINT i=0;
	LPDTSVNMPath pPath = NULL;
	vector<UINT> vectPath;
	DTSVNMParam ConculateParam = {0};
	LPDTSVirtualNet pVirNet = NULL;
	//复制一份，在遍历过程中可能被删除
	for (i=0; i<pPort->m_vectPathID.size(); i++)
	{
		vectPath.push_back(pPort->m_vectPathID[i]);
	}

	//检索出口Port影响的PathID，顺序删除
	for (i=0; i<vectPath.size(); i++)
	{
		//按顺序清除交换、只删除该VNet的Path
		pPath = DTSVNMGetPathByID(vectPath[i]);
		if (NULL==pPath)
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("[ERROR] DTSVNMVNetStatsActiveDelOutPort pPath(%d) Not Exist\n",
				vectPath[i]);
			continue;
		}
		pVirNet = DTSVNMGetVirNetByNo(pPath->m_nVirNetNo);
		if (NULL==pVirNet)
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("[ERROR][VNM] DTSVNMProcLinkDown Path(%d) VirNet(%d) not Exist\n",
				pPath->m_nPathID,pPath->m_nVirNetNo);
		}
		DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)
			("[FLOW][VNM] Clean Path(%d) VNet(%d) Src(0x%x) Dst(0x%x)\n",
			pPath->m_nPathID,pPath->m_nVirNetNo,
			pPath->m_dwBeginSwitcherLoopBack,pPath->m_dwEndSwitcherLoopBack);
		DTSVNMDelPathEraseLabelSwitch(pPath);
		//申请重新计算点到点的路径
		ConculateParam.m_nVirNetNo = pVirNet->m_nVirNetNo;
		ConculateParam.m_dwSwitcherLoopBack = pPath->m_dwBeginSwitcherLoopBack;
		ConculateParam.m_nPortIndex = pPath->m_nBeginPortIndex;
		ConculateParam.m_dwOppoLoopBack = pPath->m_dwEndSwitcherLoopBack;
		ConculateParam.m_nOppoPortIndex = pPath->m_nEndPortIndex;
		ConculateParam.m_nBandWidth = pVirNet->m_nBandWidth;
		ConculateParam.m_nFTAction = VNM_CONCULATE_TYPE_AGAIN;	//测试用
		DTSVNMConculateP2PPath(pVirNet,&ConculateParam);
		//删除路径
		DTSVNMDelPath(pPath->m_nPathID);
	}
	return VNM_RESULT_OK;
}


//删除一个虚拟网的端口
LOCAL int DTSVNMDelVirNetOutPort(LPDTSVNMParam pVNMParam)
{
	BISPASSERT(NULL!=pVNMParam);
	LPDTSVirtualNet		pVirNet = NULL;
//	LPDTSVNMSwitcher	pSwither = NULL;
	LPDTSVNMPort		pPort = NULL;
	DTSVNMVirNetOutPortIter		iterVNetPort;
	DTSVNMPortIter		iterPort;

	//检查VNet、Port是否存在
	pVirNet = DTSVNMGetVirNetByNo(pVNMParam->m_nVirNetNo);
	if (NULL==pVirNet)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] Del VirNet Port Fail VirNet(%d) Not Exist\n",
			pVNMParam->m_nVirNetNo);
		return VNM_RESULT_EXCEPTION;
	}
	pPort = DTSVNMGetPort(pVNMParam->m_dwSwitcherLoopBack,pVNMParam->m_nPortIndex);
	if(NULL==pPort)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] Del VirNet Port Fail Port(0x%x)(%d) Not Exist\n",
			pVNMParam->m_dwSwitcherLoopBack,pVNMParam->m_nPortIndex);
		return VNM_RESULT_EXCEPTION;
	}
	if ((0==pVirNet->m_mapOutPort.count(pVNMParam->m_dwSwitcherLoopBack))
		||(pVNMParam->m_nPortIndex!=pVirNet->m_mapOutPort[pVNMParam->m_dwSwitcherLoopBack]))
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] Del VirNet Port Fail Port(0x%x)(%d) Not Exist In VNet(%d)\n",
			pVNMParam->m_dwSwitcherLoopBack,pVNMParam->m_nPortIndex,
			pVNMParam->m_nVirNetNo);
		return VNM_RESULT_EXCEPTION;
	}

	switch(pVirNet->m_nStats)
	{
	case DTS_VNM_STATS_NONE:
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[WARNING]Del VNet(%d) Port(0x%x)(%d) VNet Stats(%s)",
			pVirNet->m_nVirNetNo,
			pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_nPortIndex,
			DTSVNMGetVirNetStatsStr(pVirNet));
		return VNM_RESULT_EXCEPTION;

	case  DTS_VNM_STATS_ONE_OUT_PORT:
		return DTSVNMVNetStatsOneOutPortDelOutPort(pVirNet,pVNMParam);

	case DTS_VNM_STATS_ACTIVE:
		return DTSVNMVNetStatsActiveDelOutPort(pVirNet,pPort);

	default:
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[WARNING][IGNORE] Del VNet(%d) Port(0x%x)(%d) VNet Stats(%d) Ignore",
			pVirNet->m_nVirNetNo,
			pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_nPortIndex,
			pVirNet->m_nStats);
		return VNM_RESULT_OK;
	}
}

LOCAL int DTSVNMGetMsgOnJobAdd(LPCSTR	lpBuffer	,
								int		nLen		,
								int		nParam1		,
								int		nParam2	,
								int		nParam3	,
								int		nParam4	,
								int		nParam5	)
{
	LPDTSVNMParam pVNMParam = (LPDTSVNMParam)lpBuffer;
	UINT nMsgType = (UINT)nParam1;
	int* pnRet = (int*)nParam2;
	*pnRet = VNM_RESULT_OK;

	DTS_VNM_DEBUG_PRINTF(DEBUG_ENTRY)
		("\n\n\n[ENTRY][VNM] Recv Msg(%s)\n",l_arrszVNMDebugMsg[nMsgType]);
	BISPASSERT(NULL!=pVNMParam);

	switch (nMsgType)
	{
		// 	case 	DTS_VNM_MSG_NONE				:
		// 		break;

		//向虚网添加一个出口
	case 	DTS_VNM_MSG_PORT_ADD			:
		DTS_VNM_DEBUG_PRINTF(DEBUG_ENTRY)
			("[ENTRY][VNM] Recv Add Port VirNet(%d) Port(0x%x)(%d)\n",
			pVNMParam->m_nVirNetNo,pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_nPortIndex);
		//return DTSVNMAddVirNetOutPort(pVNMParam);
		*pnRet = DTSVNMAddVirNetOutPort(pVNMParam);
		break;

		//删除虚网的一个出口
	case 	DTS_VNM_MSG_PORT_DEL			:
		DTS_VNM_DEBUG_PRINTF(DEBUG_ENTRY)
			("[ENTRY][VNM] Recv Del Port VirNet(%d) Port(0x%x)(%d)\n",
			pVNMParam->m_nVirNetNo,pVNMParam->m_dwSwitcherLoopBack,
			pVNMParam->m_nPortIndex);
		//return DTSVNMDelVirNetOutPort(pVNMParam);
		*pnRet = DTSVNMDelVirNetOutPort(pVNMParam);
		break;

	case	DTS_VNM_MSG_VIRNET_NEW:
		DTS_VNM_DEBUG_PRINTF(DEBUG_ENTRY)
			("[ENTRY][VNM] Recv New VirNet(%d) BandWidth(%d)\n",
			pVNMParam->m_nVirNetNo,pVNMParam->m_nBandWidth);
		if (NULL==DTSVNMAddVirNetUpdateVirNetMap(pVNMParam))
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("[ERROR] Add VirNet(%d) Fail\n",pVNMParam->m_nVirNetNo);
			*pnRet = VNM_RESULT_EXCEPTION;
			break;
			//return VNM_RESULT_EXCEPTION;
		}
		*pnRet = VNM_RESULT_OK;
		break;
		//return VNM_RESULT_OK;

	case DTS_VNM_MSG_VIRNET_DEL:
		DTS_VNM_DEBUG_PRINTF(DEBUG_ENTRY)
			("[ENTRY][VNM] Recv Del VirNet(%d)\n",
			pVNMParam->m_nVirNetNo);
		*pnRet = DTSVNMDelVNet(pVNMParam);
		break;
		//return DTSVNMDelVNet(pVNMParam);

	case DTS_VNM_MSG_LINK_DOWN:
		DTS_VNM_DEBUG_PRINTF(DEBUG_ENTRY)
			("[ENTRY][VNM] Recv Link Down Port(0x%x)(%d) OppoPort(0x%x)(%d)\n",
			pVNMParam->m_dwSwitcherLoopBack,pVNMParam->m_nPortIndex,
			pVNMParam->m_dwOppoLoopBack,pVNMParam->m_nOppoPortIndex);
		*pnRet = DTSVNMProcLinkDown(pVNMParam);
		break;
		//return DTSVNMProcLinkDown(pVNMParam);

	default:
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("\n!!!!WARNING DTSVNMRecvMsg Msg(%d) Ignore\n\n",nMsgType);
		*pnRet = VNM_RESULT_EXCEPTION;
		//return VNM_RESULT_EXCEPTION;	

	}	
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//收到外部调用请求
int DTSVNMGetMsg(UINT nMsgType ,LPDTSVNMParam pVNMParam)
{
	int nRet = VNM_RESULT_OK;
#ifdef DTS_TEST
	DTSVNMGetMsgOnJobAdd((LPCSTR) pVNMParam	,
								sizeof(DTSVNMParam)	,
								(int)nMsgType		,
								(int)&nRet,0,0,0);
#else
	GSB_WaitJobAdd(	g_hDTSBusMain		,
					(GSBFUNCPTR)DTSVNMGetMsgOnJobAdd,
					(LPCSTR)pVNMParam	,
					sizeof(DTSVNMParam)	,
					nMsgType,(int)&nRet,0,0,0);	
#endif

	return nRet;
}

UINT DTSVNMGetVirNetCount()
{
	return l_mapVirNet.size();
}

UINT DTSVNMGetSwitcherCount()
{
	return l_mapSwitcher.size();
}


//////////////////////////////////////////////////////////////////////////
//虚网名称函数
LOCAL char l_szVirNetNamePrefix[DTS_VNM_VIRNET_NAME_LENGTH] = "";
char* DTSVNMGetVirNetPrefix()
{
	return l_szVirNetNamePrefix;
}

int DTSVNMSetVirNetPrefix(char* lpPrefix)
{
	strcpy(l_szVirNetNamePrefix,lpPrefix);
	return VNM_RESULT_OK;
}

int DTSVNMSetVirNetName(LPDTSVirtualNet pVirNet)
{
	BISPASSERT(NULL != pVirNet);
	sprintf(pVirNet->m_szVirNetName,"%s""%d",
			DTSVNMGetVirNetPrefix(),
			pVirNet->m_nVirNetNo);
	return  VNM_RESULT_OK;
}


char* DTSVNMGetVirNetName(LPDTSVirtualNet pVirNet)
{
	BISPASSERT(NULL != pVirNet);

	return pVirNet->m_szVirNetName;
}



LOCAL char* DTSVNMGetVirNetStatsStr(LPDTSVirtualNet pVirNet)
{

	return l_arrszVNMStats[pVirNet->m_nStats];
}

LOCAL char* DTSVNMGetVirNetInfoStr(LPDTSVirtualNet pVirNet)
{
	BISPASSERT(NULL!=pVirNet);
	DTSVNMVirNetOutPortIter		iterPort;
	strcpy(l_szVirNetInfo,"");
	char szTmp[256];
	sprintf(szTmp,"VirNet(%d)(%s) Stats(%s) Out Port Count(%d):\n",
			pVirNet->m_nVirNetNo,
			pVirNet->m_szVirNetName,
			DTSVNMGetVirNetStatsStr(pVirNet),
			pVirNet->m_mapOutPort.size());
	strcat(l_szVirNetInfo,szTmp);
	
	for (iterPort = pVirNet->m_mapOutPort.begin();
		iterPort != pVirNet->m_mapOutPort.end();
		iterPort++)
	{
		sprintf(szTmp,"Out Port(0x%x)(%d)\n",iterPort->first,iterPort->second);
		strcat(l_szVirNetInfo,szTmp);
	}

	return l_szVirNetInfo;
}


LOCAL char* DTSVNMGetSwitcherInfoStr(LPDTSVNMSwitcher pSwitcher)
{
// 	BISPASSERT(NULL!=pSwitcher);
// 	LPDTSVirtualNet pVirNet = (LPDTSVirtualNet)pSwitcher->m_pVirtualNet;
// 	BISPASSERT(NULL!=pVirNet);
// 	strcpy(l_szSwitcherInfo,"");
// 	char szTmp[256];
// 	sprintf(szTmp,"\tSwither LoopBack(0x%x) VirNet(%d) Port Count(%d):\n",
// 			pSwitcher->m_dwSwitcherLoopBack,
// 			pVirNet->m_nVirNetNo,
// 			pSwitcher->m_listVNMPort.GetCount());
// 	strcat(l_szSwitcherInfo,szTmp);
// 	LPDTSVNMPort pPort = NULL;
// 	for (pPort = pSwitcher->m_listVNMPort.GetHead();pPort!=NULL;pPort = pPort->m_pNextNode)
// 	{
// 		strcat(l_szSwitcherInfo,DTSVNMGetPortInfoStr(pPort));
// 	}
 	return l_szSwitcherInfo;
}


LOCAL char* DTSVNMGetPortInfoStr(LPDTSVNMPort pPort)
{
// 	BISPASSERT(NULL!=pPort);
// 	LPDTSVNMSwitcher pSwitcher = (LPDTSVNMSwitcher)pPort->m_pSwitcher;
// 	BISPASSERT(NULL!=pSwitcher);
// 	LPDTSVirtualNet pVirNet = (LPDTSVirtualNet)pSwitcher->m_pVirtualNet;
// 	BISPASSERT(NULL!=pVirNet);
// 	strcpy(l_szPortInfo,"");
// 	char szTmp[256];
// 	sprintf(szTmp,"\t\tVNM PortIndex (%d) Switcher(0x%x) VirNet(%d)\n",
// 			pPort->m_nPortIndex,pSwitcher->m_dwSwitcherLoopBack,pVirNet->m_nVirNetNo);
// 	strcat(l_szPortInfo,szTmp);

	return l_szPortInfo;
}





