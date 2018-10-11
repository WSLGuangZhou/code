/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   17:20
	filename: 	DTSVirtualNetManagerPath.h
	author:		sl.wu
	
	purpose:	虚网管理模块路径管理
*********************************************************************/
#ifndef DTS_VIRTUAL_NET_MANAGER_PATH_H_INCLUDED
#define DTS_VIRTUAL_NET_MANAGER_PATH_H_INCLUDED

#include "DTSVirtualNetManagerPathInterface.h"

//PACK_BEGIN
#include "../DTSCommon/packBegin.h"

typedef map<UINT,LPDTSVNMPath>	DTSVNMPathMap;
typedef DTSVNMPathMap::iterator	DTSVNMPathIter;

//记录一条路径
int DTSVNMAddNewPath(LPDTSVNMPath pPath);
//删除一条路径
int DTSVNMDelPath(UINT nPathID);
//检索
LPDTSVNMPath DTSVNMGetPathByID(UINT nPathID);
LPDTSVNMPath DTSVNMGetPathByVNetSrcDst(UINT nVNetNo,DWORD dwSrc,DWORD dwDst);
//个数
UINT DTSVNMGetPathCount();

//计算VC、PW交换标签，赋值在Path里面
int DTSVNMSetPathLabel(	LPDTSVNMPath pPath);
int DTSVNMSetPathCookieByLabel(	LPDTSVNMPath pPath);

extern DTSVNMPathMap	l_mapPath;

#include "../DTSCommon/packEnd.h"
//PACK_END
	
#endif




