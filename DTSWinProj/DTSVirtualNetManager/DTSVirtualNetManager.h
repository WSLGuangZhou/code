/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   17:15
	filename: 	DTSVirtualNetManager.h
	author:		sl.wu
	
	purpose:	��������ģ��
*********************************************************************/

#ifndef DTS_VIRTUAL_NET_MANAGER_H_INCLUDED
#define DTS_VIRTUAL_NET_MANAGER_H_INCLUDED

#include <map>
#include <vector>

using namespace std;

//PACK_BEGIN
#include "../DTSCommon/packBegin.h"

//////////////////////////////////////////////////////////////////////////
//��������ģ��Debug��ӡ����
extern DWORD g_dwDTSVNMDebugOption;
#define DTS_VNM_DEBUG_PRINTF(nOption)	if((nOption)&g_dwDTSVNMDebugOption) DTS_PRINTF

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//���ڽṹ����
#define DTS_VNET_NO_NULL		(UINT)-1
//���������ļ��������������
#define DTS_VNET_MAX_CONFIG			32	
//���������ļ���������������������
#define DTS_VNET_MAX_OUT_PORT		32		
#define DTS_VNM_MAC_LEN			6
#define DTS_VNM_INVALID_MAC		0xFF
typedef struct tagDTSVirtualNetManagerPortStruct
{
// 	tagDTSVirtualNetManagerOutPortStruct*	m_pNextNode;
// 	tagDTSVirtualNetManagerOutPortStruct*	m_pPrevNode;

	//����������
	DWORD	m_dwSwitcherLoopBack;
	//�˿ں�
	UINT	m_nPortIndex;
 	//����������
// 	void*	m_pSwitcher;
	//�Ƿ�Ϊ����
	BOOL	m_bIsOutPort;
	//���ÿ��������Qos_ID��ֻ������Ϊ���ڵĶ˿�
	WORD	m_wPolicer_id;
	//�Ƿ�ΪActive������ǳ��ڴ���Active�����������κ�VNet�����ǹ�������
	BOOL	m_bIsAcitve;
	//����ǳ��ڣ����¼��������
	UINT	m_nVirNetNo;
//	void*	m_pVirtualNet;	
	//�Զ˶˿ڡ���ʱû��
//	tagDTSVirtualNetManagerOutPortStruct*	m_pOppoPort;
	//�漰��·��PathID�б�
	vector<UINT>	m_vectPathID;
	
	//��ǩ���ü���
	map<UINT,UINT>	m_mapLabelReffrence;

}*LPDTSVNMPort,DTSVNMPort,DTSVNMPORT,*LPDTSVNMPORT;
typedef map<UINT,LPDTSVNMPort>		DTSVNMPortMap;
typedef DTSVNMPortMap::iterator		DTSVNMPortIter;
typedef map<UINT,UINT>::iterator	DTSVNMLabelReffIter;
//typedef CFastListT<DTSVNMPort>	DTSVNMPortList;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//�������ṹ����
typedef struct tagDTSVirtualNetManagerSwitherStruct
{
// 	tagDTSVirtualNetManagerSwitherStruct*	m_pNextNode;
// 	tagDTSVirtualNetManagerSwitherStruct*	m_pPrevNode;

	//LoopBackAddress��ַ
	DWORD			m_dwSwitcherLoopBack;
	UINT			m_nIndex;
	
// 	//�������������������������б���
// 	void* m_pVirtualNet;	
// 	//�ڸ�������Ψһ����Index�����������������б���
// 	UINT  m_nOutPortIndex;
	
	//���ڶ˿��б�
	DTSVNMPortMap	m_mapPort;

}*LPDTSVNMSwitcher,DTSVNMSwitcher,DTSVNMSWITCHER,*LPDTSVNMSWITCHER;
//typedef CFastListT<DTSVNMSwither>	DTSVNMSwitcherList;
typedef map<DWORD , LPDTSVNMSwitcher>	DTSVNMSwitcherMap;
typedef DTSVNMSwitcherMap::iterator		DTSVNMSwitcherIter;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//�����ṹ����
#define DTS_VNM_VIRNET_NAME_LENGTH				16
typedef map<DWORD,UINT> DTSVNMVirNetOutPortMap;	
typedef DTSVNMVirNetOutPortMap::iterator DTSVNMVirNetOutPortIter; 
typedef struct tagDTSVirtualNetStruct
{
	//״̬
	UINT	m_nStats;
	//������
	UINT	m_nVirNetNo;
	//��������
	char	m_szVirNetName[DTS_VNM_VIRNET_NAME_LENGTH];
	//����������ǩ
	UINT	m_nVirLabelRef;
	//����Ҫ��
	UINT	m_nBandWidth;
	//���ڶ˿ڣ�һ������ͬһ��������ֻ��һ������
	DTSVNMVirNetOutPortMap	m_mapOutPort;
	
	//�м̶˿ڣ��ݲ�ʹ�ã����˱仯ʱ�ĸ���̫����
//	DTSVNMVirNetOutPortMap	m_mapTrunkPort;
	
}DTSVirtualNet,*LPDTSVirtualNet;
typedef map<UINT , LPDTSVirtualNet>	DTSVNMVirNetMap;
typedef DTSVNMVirNetMap::iterator DTSVNMVirNetIter;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//�������ýṹ�壬������úͱ��ⲿ���ù���
#define DTS_VNM_PARAM_BUFF_LEN		16

typedef struct tagDTSVNMFuncCallParam
{
	UINT	m_nVirNetNo;
	UINT	m_nVNetLabel;
	UINT	m_nBandWidth;
	char	m_pBuff[DTS_VNM_PARAM_BUFF_LEN];
	UINT	m_nBuffLenth;

	DWORD	m_dwSwitcherLoopBack;
	UINT	m_nPortIndex;
			
	DWORD	m_dwOppoLoopBack;
	UINT	m_nOppoPortIndex;

	//QOS ID
	WORD	m_wPolicer_id;
	//�������ڵĶԶ�Mac��ַ
	BYTE	m_arrbInOppoMac[DTS_VNM_MAC_LEN];
	BYTE	m_arrbOutOppoMac[DTS_VNM_MAC_LEN];

	//��Ե������PW����ʱ���õĶԶ�MAC
	BYTE	m_arrbOppoMac[DTS_VNM_MAC_LEN];

	UINT	m_nOutPWLabel;	
	UINT	m_nOutVCLabel;
	UINT	m_nInPWLabel;
	UINT	m_nInVCLabel;
	
	UINT	m_nFTAction;
	char*	m_pReValueBuff;
	UINT	m_nReValueLen;
	UINT64  m_n64Cookie;

}DTSVNMParam,*LPDTSVNMParam,DTSVNMFUNCCALLPARAM,*LPDTSVNMFUNCCALLPARAM;

//�յ��ⲿ���õ���Ϣ����
enum DTS_VNM_RECIEVE_MESSAGE_TYPE
{
//	DTS_VNM_MSG_NONE				,

	DTS_VNM_MSG_VIRNET_NEW			,
	DTS_VNM_MSG_VIRNET_DEL			,
	DTS_VNM_MSG_PORT_ADD			,
	DTS_VNM_MSG_PORT_DEL			,
	DTS_VNM_MSG_LINK_DOWN			,
//	DTS_VNM_MSG_GET_PATH			,
	//�����µ���Ϣ����ͬ�����ӵ��Դ�ӡ�ַ���

	DTS_VNM_MSG_PORT_COUNT			,
};

enum DTS_VNM_STATS_TYPE
{
	//û�г��ڵ�״̬
	DTS_VNM_STATS_NONE				,
	//ֻ��һ�����ڵ�״̬
	DTS_VNM_STATS_ONE_OUT_PORT		,
	//�����������ϳ���
	DTS_VNM_STATS_ACTIVE			,
	//ͬ�����´�ӡ�ַ���

	DTS_VNM_STATS_COUNT
};

//////////////////////////////////////////////////////////////////////////
//�������ƺ���
char* DTSVNMGetVirNetPrefix();
int	  DTSVNMSetVirNetPrefix(char* lpPrefix);
char* DTSVNMGetVirNetName(LPDTSVirtualNet pVirNet);
int	  DTSVNMSetVirNetName(LPDTSVirtualNet pVirNet);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//�յ��ⲿ��������
int DTSVNMGetMsg(UINT nMsgType ,LPDTSVNMParam pVNMParam);

//------------------------------------------------------------------------------------
//�б���������
//����
LPDTSVirtualNet DTSVNMGetVirNetByNo(UINT nVirNetNo);
LPDTSVNMSwitcher DTSVNMGetSwitcherByLoopBack(DWORD dwLoopBack);
LPDTSVNMPort DTSVNMGetPort(DWORD dwLoopBack,UINT nPortIndex);
UINT DTSVNMGetVirNetCount();
UINT DTSVNMGetSwitcherCount();

//------------------------------------------------------------------------------------
//��ӡ����
char* DTSVNMGetFuncTypeStr(UINT nType);
char* DTSVNMGetFTActionStr(UINT nType);

#include "../DTSCommon/packEnd.h"
//PACK_END

#endif	//DTS_VIRTUAL_NET_MANAGER_H_INCLUDED

