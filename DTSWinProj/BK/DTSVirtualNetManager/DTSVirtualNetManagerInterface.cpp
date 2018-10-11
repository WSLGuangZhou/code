/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   17:16
	filename: 	DTSVirtualNetManagerInterface.cpp
	author:		sl.wu
	
	purpose:	虚网管理模块单元测试
*********************************************************************/
#include "../DTSCommon/DTSCommon.h"
#include "DTSVirtualNetManagerInterface.h"
#include "DTSVirtualNetManagerFuncCall.h"
#include "DTSVirtualNetManager.h"
//#include "DTSVirtualNetManagerLabel.h"

//删除一个虚网
int DTSVNMDelVirNet(unsigned int nVirNetNo)
{
	DTSVNMFUNCCALLPARAM CallParam = {0};
	CallParam.m_nVirNetNo = nVirNetNo;
//	CallParam.m_nBandWidth = nBandWidth;
	return DTSVNMGetMsg(DTS_VNM_MSG_VIRNET_DEL,&CallParam);
}

//链路故障
int DTSVNMLinkDown(	DWORD	dwSwitcherLoopBack,
					UINT	nPortIndex,
					DWORD	dwOppoLoopBack,
					UINT	nOppoPortIndex)
{
	DTSVNMFUNCCALLPARAM CallParam = {0};
	CallParam.m_dwSwitcherLoopBack = dwSwitcherLoopBack;
	CallParam.m_nPortIndex = nPortIndex;
	CallParam.m_dwOppoLoopBack = dwOppoLoopBack;
	CallParam.m_nOppoPortIndex = nOppoPortIndex;
	return DTSVNMGetMsg(DTS_VNM_MSG_LINK_DOWN,&CallParam);
}


int DTSVNMNewVirNet(unsigned int nVirNetNo,	//虚网号
				unsigned int  nBandWidth)	//带宽
{
	DTSVNMFUNCCALLPARAM CallParam = {0};
	CallParam.m_nVirNetNo = nVirNetNo;
	CallParam.m_nBandWidth = nBandWidth;
	return DTSVNMGetMsg(DTS_VNM_MSG_VIRNET_NEW,&CallParam);

}


//向增加一个虚网出口
int DTSVNMAddOutPort(UINT nVirNetNo,			//虚网号
							UINT nRouterLoopBack,	//出口交换机的LoopBack地址
							UINT nRouterOutPort)	//交换机的出口索引号

{
	DTSVNMFUNCCALLPARAM CallParam = {0};
	CallParam.m_nVirNetNo = nVirNetNo;
	CallParam.m_dwSwitcherLoopBack = nRouterLoopBack;
	CallParam.m_nPortIndex = nRouterOutPort;
	return DTSVNMGetMsg(DTS_VNM_MSG_PORT_ADD,&CallParam);

}


//删除一个虚网出口
int DTSVNMDelOutPort(UINT nVirNetNo,	//虚网号
						UINT nRouterLoopBack, //出口交换机的LoopBack地址
						UINT nRouterOutPort)	//交换机的出口索引号
{
	DTSVNMFUNCCALLPARAM CallParam = {0};
	CallParam.m_nVirNetNo = nVirNetNo;
	CallParam.m_dwSwitcherLoopBack = nRouterLoopBack;
	CallParam.m_nPortIndex = nRouterOutPort;
	return DTSVNMGetMsg(DTS_VNM_MSG_PORT_DEL,&CallParam);	
}


//生成或读取配置文件内容
LOCAL int DTSVNMInitConfigFile()
{
	//如果不存在的话，创建条目
	int nRet = DTSMdlInitConfigFile(DTS_VIRTUAL_NET_MANAGER_NAME);
	if (DTS_CONFIG_NOT_EXIST == nRet)
	{
		if (!NslWritePrivateProfileString(DTS_VIRTUAL_NET_MANAGER_NAME,
										DTS_VIRTUAL_NET_NAME_PREFIX,
										DTS_VIRTUAL_NET_DEFAULT_PREFIX,
										DTS_CONFIG_FILE_NAME))
		{
			DTS_PRINTF
				("[ERROR][VNM] Write Ini File DTS_VIRTUAL_NET_DEFAULT_PREFIX Fail\n");
		}
	}
	
	int i = 0;
	//读取Debug_Option
	g_dwDTSVNMDebugOption = 0;
	for (i=0;i<DTS_DEBUG_OPTION_COUNT;i++)
	{
		if (DTS_CONFIG_ENABLE == NslGetPrivateProfileInt(DTS_VIRTUAL_NET_MANAGER_NAME,
														g_szDTSDebugOptionName[i],
														DTS_CONFIG_NOT_EXIST,
														DTS_CONFIG_FILE_NAME))
		{
			g_dwDTSVNMDebugOption |= g_arrydwDTSDebugFlag[i];
		}
	}

	//读取VirNetName前缀
	char szTmp[DTS_VNM_VIRNET_NAME_LENGTH] = "";
	NslGetPrivateProfileString(DTS_VIRTUAL_NET_MANAGER_NAME,
								DTS_VIRTUAL_NET_NAME_PREFIX,
								DTS_VIRTUAL_NET_DEFAULT_PREFIX,
								szTmp,
								DTS_VNM_VIRNET_NAME_LENGTH,
								DTS_CONFIG_FILE_NAME);
	DTSVNMSetVirNetPrefix(szTmp);
	DTS_VNM_DEBUG_PRINTF(DEBUG_ENTRY)
		("[INIT][VNM] Get Config VirNet Name Prefix(%s)\n",
		DTSVNMGetVirNetPrefix());
	return VNM_RESULT_OK;
}



int DTSVirtualNetManagerInit()
{
	//启动总线
	DTSCommonLibInit();

	//生成或读取配置文件内容，目前主要是读取Debug Option
	DTSVNMInitConfigFile();
	DTS_VNM_DEBUG_PRINTF(DEBUG_ENTRY)("[ENTRY]	[VNM] Initial\n");
	DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)("[FLOW]	[VNM] Init Config File\n");

	//初始化标签
	DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)("[FLOW]	[VNM] Init Label\n");
//	DTSVNMInitGetLabelRange();
	//初始化发送Socket
	DTSVNMInitSendUDPSocket();

	return VNM_RESULT_OK;
}


