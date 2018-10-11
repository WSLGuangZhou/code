/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   17:20
	filename: 	DTSVirtualNetManagerPath.cpp
	author:		sl.wu
	
	purpose:	虚网管理模块路径管理
*********************************************************************/
#include "../DTSCommon/DTSCommon.h"
#include "DTSVirtualNetManager.h"
#include "DTSVirtualNetManagerFuncCall.h"
#include "DTSVirtualNetManagerInterface.h"
//#include "DTSVirtualNetManagerLabel.h"
#include "DTSVirtualNetManagerPath.h"

//#include "netCoreCtrlTypeDef.h"

DTSVNMPathMap	l_mapPath;

LOCAL UINT64 DTSVNMGetCookie(LPDTSVNMPath pPath)
{
 	UINT64	n64Ret = 0;
// 	INT16 wBeginRand = BISPRandGetValue( 0,(INT16)pPath->m_dwBeginSwitcherLoopBack);
// 	INT16 wEndRand = BISPRandGetValue( 0,(INT16)pPath->m_dwEndSwitcherLoopBack);
	
	return n64Ret;
}

LOCAL int DTSVNMPathCopy(LPDTSVNMPath pPathSave,LPDTSVNMPath pPath)
{
	BISPASSERT(NULL!=pPath);
	BISPASSERT(NULL!=pPathSave);
	UINT i = 0;
	pPathSave->m_nPathID = pPath->m_nPathID;
	pPathSave->m_nVirNetNo = pPath->m_nVirNetNo;
	pPathSave->m_nSwitcherCount = pPath->m_nSwitcherCount;
	pPathSave->m_dwBeginSwitcherLoopBack = pPath->m_dwBeginSwitcherLoopBack;
	pPathSave->m_nBeginPortIndex = pPath->m_nBeginPortIndex;
	pPathSave->m_dwEndSwitcherLoopBack = pPath->m_dwEndSwitcherLoopBack;
	pPathSave->m_nEndPortIndex = pPath->m_nEndPortIndex;

	pPathSave->m_nBeginOutPWLabel = pPath->m_nBeginOutPWLabel;	
	pPathSave->m_nBeginOutVCLabel = pPath->m_nBeginOutVCLabel;	
	pPathSave->m_nBeginInPWLabel = pPath->m_nBeginInPWLabel;
	pPathSave->m_nBeginInVCLabel = pPath->m_nBeginInVCLabel;

	pPathSave->m_n64Cookie = pPath->m_n64Cookie;

	for(i=0; i<pPath->m_vectLabelSwitch.size(); i++)
	{
		pPathSave->m_vectLabelSwitch.push_back(pPath->m_vectLabelSwitch[i]);
		pPathSave->m_vectLabelSwitch[i].m_pPath = pPathSave;
	}	

	return VNM_RESULT_OK;
}

//记录一条路径
int DTSVNMAddNewPath(LPDTSVNMPath pPath)
{
	BISPASSERT(NULL!=pPath);
	DTSVNMPathIter	iterPath;
	LPDTSVNMPath pPathSave;
	UINT i = 0;

	DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)
		("[FLOW][VNM] Add New Path(%d) VNet(%d) Src(0x%x) Dst(0x%x)\n",
		pPath->m_nPathID,pPath->m_nVirNetNo,
		pPath->m_dwBeginSwitcherLoopBack,pPath->m_dwEndSwitcherLoopBack);

	iterPath = l_mapPath.find(pPath->m_nPathID);
	if (iterPath!=l_mapPath.end())
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] DTSVNMAddNewPath Fail PathID(%d) Already Exist\n",pPath->m_nPathID);
		return VNM_RESULT_EXCEPTION;
	}
	else
	{
//		pPathSave = (LPDTSVNMPath)DTS_MALLOC(sizeof(DTSVNMPath));
		pPathSave = new DTSVNMPath;
		if (NULL==pPathSave)
		{
			DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
				("[ERROR] DTSVNMAddNewPath(%d) Memery Malloc Fail \n",pPath->m_nPathID);
			return VNM_RESULT_EXCEPTION;
		}

// 		pPathSave->m_nPathID = pPath->m_nPathID;
// 		pPathSave->m_nVirNetNo = pPath->m_nVirNetNo;
// 		pPathSave->m_nSwitcherCount = pPath->m_nSwitcherCount;
// 		pPathSave->m_dwBeginSwitcherLoopBack = pPath->m_dwBeginSwitcherLoopBack;
// 		pPathSave->m_nBeginPortIndex = pPath->m_nBeginPortIndex;
// 		pPathSave->m_dwEndSwitcherLoopBack = pPath->m_dwEndSwitcherLoopBack;
// 		pPathSave->m_nEndPortIndex = pPath->m_nEndPortIndex;
// 
// 		pPathSave->m_nBeginOutPWLabel = pPath->m_nBeginOutPWLabel;	
// 		pPathSave->m_nBeginOutVCLabel = pPath->m_nBeginOutVCLabel;	
// 		pPathSave->m_nBeginInPWLabel = pPath->m_nBeginInPWLabel;
// 		pPathSave->m_nBeginInVCLabel = pPath->m_nBeginInVCLabel;
// 
// 		pPathSave->m_n64Cookie = pPath->m_n64Cookie;
// 
// 		for(i=0; i<pPath->m_vectLabelSwitch.size(); i++)
// 		{
// 			pPathSave->m_vectLabelSwitch.push_back(pPath->m_vectLabelSwitch[i]);
// 			pPathSave->m_vectLabelSwitch[i].m_pPath = pPathSave;
// 		}	
		DTSVNMPathCopy(pPathSave,pPath);

		l_mapPath[pPath->m_nPathID] = pPathSave;
		return VNM_RESULT_OK;
	}	
}


LPDTSVNMPath DTSVNMGetPathByID(UINT nPathID)
{
	if (0==l_mapPath.count(nPathID))
	{
		return NULL;
	}
	return l_mapPath[nPathID];
}



UINT DTSVNMGetPathCount()
{
	return l_mapPath.size();
}


//删除一条路径
int DTSVNMDelPath(UINT nPathID)
{

	DTSVNMPathIter	iterPath;
	LPDTSVNMPath pPath;
	iterPath = l_mapPath.find(nPathID);	
	if (iterPath==l_mapPath.end())
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] DTSVNMDelPath PathID(%d) Not Exist\n",nPathID);
		return VNM_RESULT_EXCEPTION;
	}
	pPath = iterPath->second;
	DTS_VNM_DEBUG_PRINTF(DEBUG_FLOW)
		("[FLOW][VNM] Del Path(%d) Src(0x%x) Dst(0x%x)\n",
		pPath->m_nPathID,pPath->m_dwBeginSwitcherLoopBack,pPath->m_dwEndSwitcherLoopBack);

	delete(pPath);
	l_mapPath.erase(nPathID);

	return VNM_RESULT_OK;
}


int DTSVNMSetPathCookieByLabel(	LPDTSVNMPath pPath)
{
	BISPASSERT(NULL!=pPath);
	DWORD dwTmp = pPath->m_nBeginOutPWLabel << 16 | pPath->m_nBeginOutVCLabel;
	pPath->m_n64Cookie = dwTmp;
	dwTmp = pPath->m_nBeginInPWLabel << 16 | pPath->m_nBeginInVCLabel;
	pPath->m_n64Cookie = pPath->m_n64Cookie << 32 | dwTmp;
	return VNM_RESULT_OK;
}

int DTSVNMSetPathLabel(	LPDTSVNMPath pPath)
{
	BISPASSERT(NULL!=pPath);
	LPDTSVNMSwitcher pBeginSwitcher;
	LPDTSVNMSwitcher pEndSwitcher;
	pBeginSwitcher = DTSVNMGetSwitcherByLoopBack(pPath->m_dwBeginSwitcherLoopBack);
	pEndSwitcher = DTSVNMGetSwitcherByLoopBack(pPath->m_dwEndSwitcherLoopBack);
	BISPASSERT(NULL!=pBeginSwitcher);
	BISPASSERT(NULL!=pEndSwitcher);
	if (NULL==pBeginSwitcher)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] DTSVNMGetPathLabel BeginSwitcher(0x%x) Not Exist\n",pPath->m_dwBeginSwitcherLoopBack);
		return VNM_RESULT_EXCEPTION;
	}

	if (NULL==pEndSwitcher)
	{
		DTS_VNM_DEBUG_PRINTF(DEBUG_ERROR)
			("[ERROR] DTSVNMGetPathLabel EndSwitcher(0x%x) Not Exist\n",pPath->m_dwEndSwitcherLoopBack);
		return VNM_RESULT_EXCEPTION;
	}

	//PW用于寻找对端交换机，对端交换机索引 * 1000
	pPath->m_nBeginOutPWLabel = pEndSwitcher->m_nIndex*1000;	
	//VC用于给对端区分是哪一条Path，本交换机索引 * 1000 + 对端端口索引号
	pPath->m_nBeginOutVCLabel = pBeginSwitcher->m_nIndex*1000 + pPath->m_nBeginPortIndex;

	//Begin交换机索引*1000
	pPath->m_nBeginInPWLabel = pBeginSwitcher->m_nIndex*1000 ;
	//用于Begin交换机区分哪一条Path，End交换机索引*1000 + End端口号索引
	pPath->m_nBeginInVCLabel = pEndSwitcher->m_nIndex*1000 + pPath->m_nEndPortIndex;	
	
	return VNM_RESULT_OK;
}

LOCAL int DTSVNMGetPathOnWaitJobAdd(LPCSTR	lpBuffer	,
									int		nLen		,
									int		nParam1		,
									int		nParam2	,
									int		nParam3	,
									int		nParam4	,
									int		nParam5	)
{
	DTSVNMPathVector* pVectPath = (DTSVNMPathVector*)nParam2;
	DTSVNMPathIter iterPath;
//	LPDTSVNMLabelSwitch pLabelSwitch;
	LPDTSVNMPath pPath;
	DTSVNMPath pathRet;
	UINT nVirNet = (UINT)nParam1;
//	UINT i=0;
	for (iterPath = l_mapPath.begin(); iterPath!=l_mapPath.end(); iterPath++)		
	{
		pPath = iterPath->second;
		if (nVirNet == pPath->m_nVirNetNo)
		{
			DTSVNMPathCopy(&pathRet,pPath);
			pVectPath->push_back(pathRet);	
		}
	}
	
	return 0;
}

LOCAL DTSVNMPathVector l_mapVect;
DTSVNMPathVector* DTSVirNetManagerGetPathVect(UINT nVirNet)
{
	l_mapVect.clear();

//#ifdef DTS_TEST
// 	DTSVNMGetPathOnWaitJobAdd(NULL,0,
// 							(int)nVirNet,
// 							(int)&l_mapVect,0,0,0);
//#else
	GSB_WaitJobAdd(	g_hDTSBusMain		 ,
					(GSBFUNCPTR)DTSVNMGetPathOnWaitJobAdd,
					NULL,0,(int)nVirNet,(int)&l_mapVect,0,0,0);	
//#endif

	return &l_mapVect;
}

LPDTSVNMPath DTSVNMGetPathByVNetSrcDst(UINT nVNetNo,DWORD dwSrc,DWORD dwDst)
{
	DTSVNMPathVector* pVectPath = DTSVirNetManagerGetPathVect(nVNetNo);
	BISPASSERT(NULL!=pVectPath);
	UINT i=0;
	for (i=0;i<pVectPath->size();i++)
	{
		if ((dwSrc == (*pVectPath)[i].m_dwBeginSwitcherLoopBack)
			&&(dwDst == (*pVectPath)[i].m_dwEndSwitcherLoopBack))
		{
			return &((*pVectPath)[i]);
		}		
	}
	return NULL;
}

