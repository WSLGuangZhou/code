/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   17:16
	filename: 	DTSVirtualNetManagerFuncCall.h
	author:		sl.wu
	
	purpose:	��������ģ����Ҫ������ýӿ�
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
		
	//������
	VNM_FUNC_CALL_MSG_SEND					,
	//���´�ӡ��Ϣ�б�

	VNM_FUNC_CALL_COUNT
};


enum DTS_VNM_FLOW_TABEL_ACTION_TYPE
{
	VNM_FT_ACT_NONE						= 0	,
	//�½�һ������
	VNM_FT_ACT_NEW_VIRNET_WITH_PORT					,
	//ɾ��һ������
	VNM_FT_ACT_DEL_VIRNET					,
	//����һ���������ڣ���Ҫ������������˫�������·���ǩ��������
	VNM_FT_ACT_OUT_PORT_ADD					,
	//ɾ��һ���������ڣ���Ҫ������������˫�������·���ǩ��������
	VNM_FT_ACT_OUT_PORT_DEL					,
	//����һ�������м̶˿�
	VNM_FT_ACT_TRUNK_PORT_ADD				,
	//ɾ��һ�������м̶˿�
	VNM_FT_ACT_TRUNK_PORT_DEL				,
	//�����м̽����Ľ���
	VNM_FT_ACT_SWITCH_ADD					,
	//ɾ���м̽������Ľ���
	VNM_FT_ACT_SWITCH_DEL					,
	//����Qos����							
	VNM_FT_ACT_QOS_SET						,
	//����Qos����
	VNM_FT_ACT_QOS_DEL						,

	//ͬ�����ӵ��Դ�ӡ


	VNM_FLOW_TABLE_ACTION_COUNT
};


enum DTS_VNM_PATH_CONCULATE_TYPE
{
	VNM_CONCULATE_TYPE_FIRST			=	0,
	VNM_CONCULATE_TYPE_AGAIN				,

	VNM_CONCULATE_TYPE_COUNT
};


//////////////////////////////////////////////////////////////////////////
//�ⲿ����ͳһ�ӿ�
struct tagDTSVNMFuncCallParam;
int DTSVNMFuncCall(int nFuncType,tagDTSVNMFuncCallParam* pParam); 
int DTSVNMInitSendUDPSocket();
int DTSVNMSendBuff(DWORD dwDstAddr,BYTE* pBuff,int nLen);
WORD DTSVNMMakeOFPMsg(tagDTSVNMFuncCallParam* pParam,BYTE* pBuff,int nLen);

#include "../DTSCommon/packEnd.h"
//PACK_END

#endif

