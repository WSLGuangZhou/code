//////////////////////////////////////////////////////////////////////////
//基础库初始化
#include "DTSCommon.h"
#include "DTSCommonLibFunc.h"

SBHANDLE g_hDTSBusMain = NULL;

LOCAL int SYSAPICALL DTSSoftbusLoop(void* pParam)
{
	SBHANDLE hBus = (NULL==pParam)?g_hDTSBusMain:(SBHANDLE)pParam;

	DTS_PRINTF("DTSSoftbusLoop Begin\n");

	GSB_SetOwnerTaskId(hBus,BISPGETTASKID());

	GSBMSG msg;

	while(GSB_PeekMessage(hBus,&msg,(DWORD)-1))
	{
		GSB_DispatchMessage(hBus,&msg);
	}

	return 0; 
}

LOCAL int DTSActiveSoftBus()
{
	//启动总线
	int nHandle ;
	g_hDTSBusMain = GSB_Create(64,1500);
	if (NULL==g_hDTSBusMain) 
	{
		DTS_PRINTF("GSB_Create Fail \n");
	}
	nHandle = BISPTaskCreate( (BISPTASKPROC)DTSSoftbusLoop,
		NULL,
		80,
		BISP_DEFAULT_STACK_SIZE,
		"tSoBu");

	return 0;
}

int DTSMdlInitConfigFile(LPCSTR lpAppName)
{
	//第一次创建
	if (DTS_CONFIG_NOT_EXIST ==  NslGetPrivateProfileInt(lpAppName,
														DTS_CONFIG_LEVEL_NAME,
														DTS_CONFIG_NOT_EXIST,
														DTS_CONFIG_FILE_NAME))
	{
		int i=0;
		//首次创建模块条目
		if (!NslWritePrivateProfileInt( lpAppName,
			DTS_CONFIG_LEVEL_NAME,
			DTS_CONFIG_ENABLE,
			DTS_CONFIG_FILE_NAME))
		{
			DTS_PRINTF("DTSMdlInitConfigFile Write ConfigFile Fail\n");
			return 0;
		}

		//创建默认DebugFlag
		for(i=0;i<DTS_DEBUG_OPTION_COUNT;i++)
		{
			NslWritePrivateProfileInt( lpAppName,
										g_szDTSDebugOptionName[i],
										DTS_CONFIG_DISABLE,
										DTS_CONFIG_FILE_NAME);
		}

		//默认打开ERROR
		NslWritePrivateProfileInt ( lpAppName,
									g_szDTSDebugOptionName[DTS_DEBUG_OPTION_ERROR],
									DTS_CONFIG_ENABLE,
									DTS_CONFIG_FILE_NAME);

		return DTS_CONFIG_NOT_EXIST;
			

	}
	return 0;
}


LOCAL int DTSCreateIniFile()
{
	
	if (!NslWritePrivateProfileInt( DTS_MDL_ALL_NAME,
									DTS_CONFIG_LEVEL_NAME,
									DTS_CONFIG_ENABLE,
									DTS_CONFIG_FILE_NAME))
	{
		DTS_PRINTF("Create ConfigFile Fail\n");
		return 0;
	}


	// 	if (DTS_CONFIG_NOT_EXIST == )
	// 	{
	// 	}
	// 	DTS_CONFIG_NOT_EXIST ==  NslGetPrivateProfileInt(LPCSTR lpAppName,
	// 		LPCSTR lpKeyName,
	// 		INT32  iDefault,
	// 		LPCSTR lpFileName);
	// 	
	// 		DTS_MDL_ALL_NAME

	return 0;
}


//基础库初始化，启动总线，创建配置文件
int DTSCommonLibInit()
{

	//启动软件总线
	DTSActiveSoftBus();

	//创建配置文件
	DTSCreateIniFile();
	
	return 0;
}


