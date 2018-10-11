// DTSWinProj.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../DTSCommon/DTSCommon.h"
#include "../DTSCommon/DTSTestUnit.h"
#include "../DTSCommon/DTSTestCommonLib.h"
#include "../DTSCommon/DTSCommonLibFunc.h"
#include "../DTSVirtualNetManager/DTSVirtualNetManagerInterface.h"
#include "../DTSVirtualNetManager/test/DTSVirtualNetManagerTestFunc.h"

TEST(DTS_ALL_MDL, TEST_CASE)
{
//	DTSVirNetManagerTest(0);	
}


int _tmain(int argc, _TCHAR* argv[])
{
	char bTmp;
	int nParam = 123;

	//测试软件总线初始化各模块
 	DTSCommonLibInit();
 	DTSTestCommonLib(0);

	//初始化测试拓扑，配置文件有可能已经有虚网的配置
// 	DTSVNMCreateTestTopoPath();
// 	//初始化虚网模块
// 	DTSVirtualNetManagerInit();
// 	//Test测试用例
// 	DTSVirNetManagerTest(nParam);	

//  testing::InitGoogleTest(&argc, argv);
//  RUN_ALL_TESTS();
		
	scanf("%c",&bTmp);	
	return 0;
}

