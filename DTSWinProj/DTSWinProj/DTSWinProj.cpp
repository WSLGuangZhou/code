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

	//����������߳�ʼ����ģ��
 	DTSCommonLibInit();
 	DTSTestCommonLib(0);

	//��ʼ���������ˣ������ļ��п����Ѿ�������������
// 	DTSVNMCreateTestTopoPath();
// 	//��ʼ������ģ��
// 	DTSVirtualNetManagerInit();
// 	//Test��������
// 	DTSVirNetManagerTest(nParam);	

//  testing::InitGoogleTest(&argc, argv);
//  RUN_ALL_TESTS();
		
	scanf("%c",&bTmp);	
	return 0;
}

