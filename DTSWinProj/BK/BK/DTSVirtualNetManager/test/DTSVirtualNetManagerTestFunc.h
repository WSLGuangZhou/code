/********************************************************************
	created:	2018/04/09
	created:	9:4:2018   17:17
	filename: 	DTSVirtualNetManagerTestFunc.h
	author:		sl.wu
	
	purpose:	ÐéÍø¹ÜÀíÄ£¿éµ¥Ôª²âÊÔ
*********************************************************************/
#ifndef DTS_VIRTUAL_NET_MANAGER_TEST_FUNC_H_INCLUDED
#define DTS_VIRTUAL_NET_MANAGER_TEST_FUNC_H_INCLUDED

#ifdef DTS_TEST

//PACK_BEGIN
#include "../../DTSCommon/packBegin.h"


#define DTS_VNM_TEST_MAC_0_IN		0xE0
#define DTS_VNM_TEST_MAC_0_OUT		0xF0

//////////////////////////////////////////////////////////////////////////
void DTSVirNetManagerTest(int nParam);
int  DTSVNMCreateTestTopoPath();
struct tagDTSVNMPathStruct;
//tagDTSVNMPathStruct* DTSVNMTestGetPath(DWORD dwSrcLB,DWORD dwDstLB);
extern BOOL l_bTestIsPathConculateSucceed;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int DTSVNMTestVNetStatsOneOutPortAddPortTest1();
int DTSVNMTestVNet_1StatsActiveAddPortTest2();
int DTSVNMTestVNet_2StatsOneOutPortAddPortTest3();
int DTSVNMTestVNet_2StatsActiveAddPortTest4();

//É¾³ý¶Ë¿Ú
int DTSVNMTestVNet_1StatsActiveDelPortTest5();
int DTSVNMTestVNet_1StatsActiveDelPortTest6();
int DTSVNMTestVNet_1StatsOneOutPortDelPortTest7();
int DTSVNMTestVNetStatsOneOutPortDelPort(UINT nVirNetNo,
									UINT nSwitcherLoopBack,
									UINT nSwithcerOutPort);

//Â·¾¶¼ÆËãÊ§°Ü²âÊÔ
int DTSTestVNet_2OneOutPortAddPortPathConculateFailTest9();
int DTSTestVNet_2ActiveAddPortPathConculateFailTest10();

//É¾³ýÒ»¸öÐéÍø
int DTSVNMTestVNet_2StatsAciveDelAllTest8();

//Á´Â·¹ÊÕÏ
int DTSVNMTestLinkDownHaveBackupPathTest11();
int DTSVNMTestLinkDownNoBackupPathTest12();
int DTSVNMTestLinkDownNoBackupPathTest13();

#include "../../DTSCommon/packEnd.h"
//PACK_END


#endif //DTS_TEST
#endif	//DTS_VIRTUAL_NET_MANAGER_TEST_FUNC_H_INCLUDED



