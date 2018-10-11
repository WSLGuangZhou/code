/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   17:18
	filename: 	DTSVirtualNetManagerInterface.h
	author:		sl.wu
	
	purpose:	��������ģ������ṩ�ĵ��ýӿ�
*********************************************************************/
#ifndef DTS_VIRTUAL_NET_MANAGER_INTERFACE_H_INCLUDED
#define DTS_VIRTUAL_NET_MANAGER_INTERFACE_H_INCLUDED

//PACK_BEGIN
//�����ṩ�Ľӿں���������ͨ�ö���

//����ģ����
#define DTS_VIRTUAL_NET_MANAGER_NAME		"VirtualNetManager"
#define DTS_VIRTUAL_NET_DEFAULT_PREFIX		"VNet"
#define DTS_VIRTUAL_NET_NAME_PREFIX			"VirNetNamePrefix"

enum DTS_VNM_RTURN_RESULT
{
	VNM_RESULT_EXCEPTION		= 0	,		
	VNM_RESULT_OK					,
	VNM_RESULT_REPEAT				,

	VNM_RESULT_COUNT
};

//////////////////////////////////////////////////////////////////////////
//��ʼ��
int DTSVirtualNetManagerInit();
int DTSVNMGetConfigFileVNetSetting();

//���һ�����������ø�������Լ������
int DTSVNMNewVirNet(unsigned int nVirNetNo,	//������
					unsigned int  nBandWidth);	//����

//ɾ��һ������
int DTSVNMDelVirNet(unsigned int nVirNetNo);

//������һ����������,����ֵDTS_VNM_RTURN_RESULT
int DTSVNMAddOutPort(unsigned int nVirNetNo,		//������
					unsigned int nRouterLoopBack,	//���ڽ�������LoopBack��ַ
					unsigned int nRouterOutPort);	//�������ĳ���������

//ɾ��һ����������
int DTSVNMDelOutPort(unsigned int nVirNetNo,	//������
					unsigned int nRouterLoopBack, //���ڽ�������LoopBack��ַ
					unsigned int nRouterOutPort);	//�������ĳ���������

//��·����
int DTSVNMLinkDown(	unsigned int	dwSwitcherLoopBack,
					unsigned int	nPortIndex,
					unsigned int	dwOppoLoopBack,
					unsigned int	nOppoPortIndex);


#endif	//DTS_VIRTUAL_NET_MANAGER_INTERFACE_H_INCLUDED



