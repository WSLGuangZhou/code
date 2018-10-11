/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   17:18
	filename: 	DTSVirtualNetManagerInterface.h
	author:		sl.wu
	
	purpose:	虚网管理模块对外提供的调用接口
*********************************************************************/
#ifndef DTS_VIRTUAL_NET_MANAGER_INTERFACE_H_INCLUDED
#define DTS_VIRTUAL_NET_MANAGER_INTERFACE_H_INCLUDED

//PACK_BEGIN
//对外提供的接口函数，采用通用定义

//定义模块名
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
//初始化
int DTSVirtualNetManagerInit();
int DTSVNMGetConfigFileVNetSetting();

//添加一个虚网，配置该虚网的约束条件
int DTSVNMNewVirNet(unsigned int nVirNetNo,	//虚网号
					unsigned int  nBandWidth);	//带宽

//删除一个虚网
int DTSVNMDelVirNet(unsigned int nVirNetNo);

//向增加一个虚网出口,返回值DTS_VNM_RTURN_RESULT
int DTSVNMAddOutPort(unsigned int nVirNetNo,		//虚网号
					unsigned int nRouterLoopBack,	//出口交换机的LoopBack地址
					unsigned int nRouterOutPort);	//交换机的出口索引号

//删除一个虚网出口
int DTSVNMDelOutPort(unsigned int nVirNetNo,	//虚网号
					unsigned int nRouterLoopBack, //出口交换机的LoopBack地址
					unsigned int nRouterOutPort);	//交换机的出口索引号

//链路故障
int DTSVNMLinkDown(	unsigned int	dwSwitcherLoopBack,
					unsigned int	nPortIndex,
					unsigned int	dwOppoLoopBack,
					unsigned int	nOppoPortIndex);


#endif	//DTS_VIRTUAL_NET_MANAGER_INTERFACE_H_INCLUDED



