//////////////////////////////////////////////////////////////////////////
//基础库初始化
#ifndef DTS_COMMON_FUNC_H_INCLUDED
#define DTS_COMMON_FUNC_H_INCLUDED



//消息总线
extern	SBHANDLE g_hDTSBusMain ;

//主线程必须先调用该函数
//基础库初始化，启动总线，创建配置文件
int DTSCommonLibInit();


#endif	//DTS_COMMON_FUNC_H_INCLUDED

