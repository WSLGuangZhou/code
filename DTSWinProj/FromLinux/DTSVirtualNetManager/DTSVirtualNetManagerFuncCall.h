/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   17:16
	filename: 	DTSVirtualNetManagerFuncCall.h
	author:		sl.wu
	
	purpose:	虚网管理模块需要对外调用接口
*********************************************************************/
#ifndef DTS_VIRTUAL_NET_MANAGER_FUNC_CALL_H_INCLUDED
#define DTS_VIRTUAL_NET_MANAGER_FUNC_CALL_H_INCLUDED

//PACK_BEGIN
#include "../DTSCommon/packBegin.h"


enum DTS_VIRTUAL_NET_MANAGER_TYPE
{
	VNM_FUNC_CALL_NULL					= 0	,
	VNM_FUNC_CALL_TOPO_CONCULATE			,
	VNM_FUNC_CALL_FLOWTABLE_SEND			,
	VNM_FUNC_CALL_TOPO_UPDATE				,
		
	//测试用
	VNM_FUNC_CALL_MSG_SEND					,
	//更新打印信息列表

	VNM_FUNC_CALL_COUNT
};


enum DTS_VNM_FLOW_TABEL_ACTION_TYPE
{
	VNM_FT_ACT_NONE						= 0	,
	//新建一个虚网
	VNM_FT_ACT_NEW_VIRNET_WITH_PORT					,
	//删除一个虚网
	VNM_FT_ACT_DEL_VIRNET					,
	//增加一个虚网出口，需要下两个流表对双向流量下发标签控制流表
	VNM_FT_ACT_OUT_PORT_ADD					,
	//删除一个虚网出口，需要下两个流表对双向流量下发标签控制流表
	VNM_FT_ACT_OUT_PORT_DEL					,
	//增加一个虚网中继端口
	VNM_FT_ACT_TRUNK_PORT_ADD				,
	//删除一个虚网中继端口
	VNM_FT_ACT_TRUNK_PORT_DEL				,
	//建立中继交换的交换
	VNM_FT_ACT_SWITCH_ADD					,
	//删除中继交换机的交换
	VNM_FT_ACT_SWITCH_DEL					,
	//虚网Qos设置							
	VNM_FT_ACT_QOS_SET						,
	//虚网Qos设置
	VNM_FT_ACT_QOS_DEL						,

	//同步增加调试打印


	VNM_FLOW_TABLE_ACTION_COUNT
};


enum DTS_VNM_PATH_CONCULATE_TYPE
{
	VNM_CONCULATE_TYPE_FIRST			=	0,
	VNM_CONCULATE_TYPE_AGAIN				,

	VNM_CONCULATE_TYPE_COUNT
};


//////////////////////////////////////////////////////////////////////////
//外部调用统一接口
struct tagDTSVNMFuncCallParam;
int DTSVNMFuncCall(int nFuncType,tagDTSVNMFuncCallParam* pParam); 
int DTSVNMInitSendUDPSocket();
int DTSVNMSendBuff(DWORD dwDstAddr,BYTE* pBuff,int nLen);
WORD DTSVNMMakeOFPMsg(tagDTSVNMFuncCallParam* pParam,BYTE* pBuff,int nLen);

#include "../DTSCommon/packEnd.h"
//PACK_END

#endif


