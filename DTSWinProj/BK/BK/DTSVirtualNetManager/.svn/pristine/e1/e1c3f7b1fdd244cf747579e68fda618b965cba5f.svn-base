/********************************************************************
	created:	2018/05/04	9:46
	filename: 	DTSVirtualNetManagerPathInterface.h
	author:		sl.wu
	
	purpose:	虚网管理模块路径管理
*********************************************************************/

#ifndef DTSVirtualNetManagerPathInterface_h__
#define DTSVirtualNetManagerPathInterface_h__

#include <map>
#include <vector>
//交换
typedef struct tagDTSVNMLabelSwitchStruct
{
	DWORD	m_dwSwitcherLoopBack;
	void*	m_pPath;

	UINT	m_nInPortIndex;
	UINT	m_nOutPortIndex;

	UINT	m_nInPWLabel;
	UINT	m_nInVCLabel;

	UINT	m_nOutPWLabel;
	UINT	m_nOutVCLabel;

	BYTE	m_arrbInMac[DTS_VNM_MAC_LEN];
	BYTE	m_arrbOutMac[DTS_VNM_MAC_LEN];

}DTSVNMLabelSwitch,*LPDTSVNMLabelSwitch;
//typedef map<DWORD,LPDTSVNMLabelSwitch>	DTSVNMLabelSwitchMap;
//typedef vector<DTSVNMLabelSwitch>			DTSVNMLabelSwitchVector;

//路径
typedef struct tagDTSVNMPathStruct
{	
	UINT	m_nPathID;
	UINT	m_nVirNetNo;
	UINT	m_nSwitcherCount;

	DWORD	m_dwBeginSwitcherLoopBack;
	UINT	m_nBeginPortIndex;
	DWORD	m_dwEndSwitcherLoopBack;
	UINT	m_nEndPortIndex;

	UINT	m_nBeginOutPWLabel;	
	UINT	m_nBeginOutVCLabel;

	UINT	m_nBeginInPWLabel;
	UINT	m_nBeginInVCLabel;

	//记录发送交换时的cookie，删除时需要发送相同的cookie，该路径所有交换机共用
	UINT64	m_n64Cookie;

	vector<DTSVNMLabelSwitch>	m_vectLabelSwitch;

}DTSVNMPath,*LPDTSVNMPath;

typedef vector<DTSVNMPath> DTSVNMPathVector;	

DTSVNMPathVector* DTSVirNetManagerGetPathVect(UINT nVirNet);

#endif // DTSVirtualNetManagerPathInterface_h__



