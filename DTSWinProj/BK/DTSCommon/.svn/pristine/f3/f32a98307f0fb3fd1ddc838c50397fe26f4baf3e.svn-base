///////////////////////////////////////////////////////////////////////////////////////////
// Broadband Integrated Switching Platform(BISP) Basic Component
// Copyright(c) 2000-2008 Network Communications Company, 
//						  Haige Communications Group Incorporated Company.
// All rights reserved.
//
// This file is a part of the Broadband Integrated Switching Platform(BISP).
// The code and information is provided "as-is" without
// warranty of any kind, either expressed or implied.
///////////////////////////////////////////////////////////////////////////////////////////
//	文 件 名 : SoftBus.h
//	文件功能 : 软件总线定义
//	作    者 : 谭 北 华
//	创建时间 : 2005年4月22日
//	项目名称 : 
//	备    注 :
//	历史记录 : 2007.12.11 增加了GSB_SendMessageEx、GSB_PostMessageEx、GSB_JobAddEx、GSB_WaitJobAddEx
//						   四个函数，增加了对队列优先级的支持，可最大支持255个优先级处理队列
//			   2008.01.11 增加了GSB_SetJitterTimer、GSB_SetJitterTimerFast、GSB_SetJitterTimerByID
//						   三个函数，支持设置抖动定时器的功能。所谓的抖动定时器是指在固定间隔uElase之内
//						   随机的在指定的nJitterTime范围内抖动触发，一般用于路由协议的Hello包发送功能。
//				2008.03.17 增加GSB_GetTimerLeftSleep函数，用来统计定时器的下次触发时间
//				2008.03.26 修改GSB_LoopMessage和GSB_PeekMessage的返回值处理，增加Timeout的指示
///////////////////////////////////////////////////////////////

#ifndef __SOFTBUS_H_INCLUDED
#define __SOFTBUS_H_INCLUDED



#ifdef __cplusplus
 //软件总线句柄	
 typedef struct _SBHandle{}* SBHANDLE;
 //可变参数的回调函数
 typedef int 	(*GSBFUNCPTR) (LPCSTR lpBuffer,int nLen,...);     /* ptr to function returning int */
 typedef int 	(*GSBTIMERFUNCPTR) (UINT nTimerId,LPCSTR lpBuffer,int nLen,...);     /* ptr to timer function returning int */
 typedef int 	(*GSBFASTFUNCPTR) (...);     /* ptr to function returning int */
 typedef int 	(*GSBFASTTIMERFUNCPTR) (UINT nTimerId,...);     /* ptr to timer function returning int */
#else
 //软件总线句柄	
 typedef struct _SBHandle{DWORD dwFlag;}* SBHANDLE;
 //可变参数的回调函数
 typedef int 	(*GSBFUNCPTR) ();	   /* ptr to function returning int */
 typedef int 	(*GSBTIMERFUNCPTR) ();	   /* ptr to timer function returning int */
 typedef int 	(*GSBFASTFUNCPTR) ();	   /* ptr to timer function returning int */
 typedef int 	(*GSBFASTTIMERFUNCPTR) ();	   /* ptr to timer function returning int */
#endif	

//定义系统相关的事件类型
#ifdef NSL_OS_WINDOWS
#define GSB_MAKE_SYSEVENT_HANDLE(nID) ((HANDLE)(nID))
#elif defined (NSL_OS_VXWORKS)
#define GSB_MAKE_SYSEVENT_HANDLE(nID) ((SEM_ID)(nID))
#elif defined (NSL_OS_LINUX)
#define GSB_MAKE_SYSEVENT_HANDLE(nID) ((sem_t*)(nID))
#endif 

//取得系统相关的事件句柄
#define GSB_GET_EVENT_HANDLE(hBus) GSB_MAKE_SYSEVENT_HANDLE(GSB_GetSystemMsgEvent(hBus))


//软件总线优先级定义(从高到低为0-255，0表示最高优先级，255表示最低优先级)
#define GSB_PRIORITY_REALTIME	0
#define GSB_PRIORITY_HIGHEST	50
#define GSB_PRIORITY_HIGH		100
#define GSB_PRIORITY_LOW		150
#define GSB_PRIORITY_LOWEST		200
#define GSB_PRIORITY_IDLE		255

//默认的优先级采用最低级别，为255
#define GSB_PRIORITY_DEFAULT GSB_PRIORITY_IDLE

//超时返回值(GSB_LoopMessage和GSB_PeekMessage的返回值)
#define GSB_ERROR		(-1)			//错误消息
#define GSB_EXIT		0				//总线收到EXIT消息，要求退出
#define GSB_OK			1				//处理成功
#define GSB_TIMEOUT		2				//处理超时


#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

typedef int (WINAPI *LPONBUSMESSAGE)(UINT nModuleSrcID,  //请求的软件总线模块ID
									 UINT nModuleDestID, //目标软件总线模块ID
									 UINT nMsg,			 //消息
									 WPARAM wParam,		 //参数1
									 LPARAM lParam,		 //参数2
									 LPCSTR lpBuffer,	 //缓冲
									 int nLen,			 //缓冲长度
									 DWORD dwUserData	 //自定义数据
									 );


//新的消息定义(V3)
typedef struct tagGSBMsg
{
	UINT			nModuleSrcID;		//请求的软件总线模块ID,nParam1
	UINT			nModuleDestID;		//目标软件总线模块ID,nParam2
	UINT			nMsg;				//消息,nParam3
	WPARAM			wParam;				//参数1,nParam4
	LPARAM			lParam;				//参数2,nParam5
	DWORD			dwPrivate;			//私有数据
}GSBMSG,*LPGSBMSG;



//通播模块ID
#define BROASTCAST_MODULE_ID (UINT(-1))

//优化的最大消息长度
#define GSB_OPTIMIZ_BUFFER_SIZE 1024L

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_Create
//	函数功能 : 创建软件总线对象
//				注意!!!: 软件总线在创建时被绑定在GSB_Create调用时的线程，
//						 并以此为依据来处理GSB_SendMessage和GSB_WaitJobAdd以决定是
//						 同步调用或异步调用
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : SBHANDLE ,软件总线对象句柄，以下函数的第一个参数用
//	参数说明 : DWORD dwMaxModule, 最大的模块个数
//			  DWORD dwMaxBufferSize,每条消息最大的缓冲大小,
//									注意V3版本的软件总线在非中断模式(SendMessage,PostMessage)已对消息大小不加限制了
//									dwMaxBufferSize仅仅是给出可以让软件总线中断处理时(IntPostMessage,IntJobAdd)
//									可以发送的最大消息长度
//									推荐把dwMaxBufferSize的值设置为GSB_OPTIMIZ_BUFFER_SIZE
///////////////////////////////////////////////////////////////
SBHANDLE WINAPI GSB_Create(DWORD dwMaxModule,
						   DWORD dwMaxBufferSize);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_Destroy
//	函数功能 : 销毁软件总线对象
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : BOOL
//	参数说明 : SBHANDLE hBus,软件总线对象句柄
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_Destroy(SBHANDLE hBus);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_SetOwnerTaskId
//	函数功能 : 设置软件总线所属的线程(任务),软件总线所属线程(任务)在创建时取创建所在
//				的线程为默认线程，调用此函数会改变默认设置,并以此为依据来处理GSB_SendMessage和
//				GSB_WaitJobAdd以决定是同步调用或异步调用
//	作    者 : 谭 北 华
//	时    间 : 2005年4月22日
//	返 回 值 : BOOL
//	参数说明 : SBHANDLE hBus,,软件总线对象句柄
//				           UINT nTaskID 所属任务ID，如果为0表示采用当前的任务ID
///////////////////////////////////////////////////////////////
#ifdef NSL_OS_LINUX
	typedef pthread_t sb_task_id_t;
#else
	typedef UINT sb_task_id_t;
#endif /* NSL_OS_LINUX */

BOOL WINAPI GSB_SetOwnerTaskId(SBHANDLE hBus,
                               sb_task_id_t nTaskID);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_RegisterModule
//	函数功能 : 注册模块并挂接到软件总线上
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : BOOL
//	参数说明 : SBHANDLE hBus,软件总线对象
//			 UINT nModuleID,模块ID
//			 LPONBUSMESSAGE lpfnCallback,模块接收事件的回调函数
//		     DWORD dwUserData 自定义数据(在事件回调时传回来)
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_RegisterModule(SBHANDLE hBus,
							   UINT nModuleID,
							   LPONBUSMESSAGE lpfnCallback,
							   DWORD dwUserData);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_UnregisterModule
//	函数功能 : 反注册模块,使其与软件总线不关联
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : BOOL
//	参数说明 : SBHANDLE hBus,软件总线对象
//			   UINT nModuleID 模块ID
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_UnregisterModule(SBHANDLE hBus,
								 UINT nModuleID);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_PostMessage
//	函数功能 : 投递一条异步消息到软件总线队列并马上返回(采用最低优先级255)
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : BOOL
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         UINT nModuleSrcID,发送消息的模块ID
//				         UINT nModuleDestID,接收消息的模块ID
//				         UINT nMsg,对方接收到的消息ID
//				         WPARAM wParam,自定义参数
//				         LPARAM lParam,自定义参数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen 要复制传递的缓冲长度，为-1表示字符串长度
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_PostMessage(SBHANDLE hBus,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,
							LPCSTR lpBuffer,
							int nLen);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_PostMessageEx
//	函数功能 : 投递一条异步消息到软件总线队列并马上返回
//				此函数是GSB_PostMessage函数的增强版本，提供了队列优先级功能
//	作    者 : 谭 北 华
//	时    间 : 2007年12月
//	返 回 值 : BOOL
//	参数说明 : SBHANDLE hBus,
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         UINT nModuleSrcID,发送消息的模块ID
//				         UINT nModuleDestID,接收消息的模块ID
//				         UINT nMsg,对方接收到的消息ID
//				         WPARAM wParam,自定义参数
//				         LPARAM lParam,自定义参数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen 要复制传递的缓冲长度，为-1表示字符串长度
//				         UINT nPriority,消息在队列中的优先级(0-255)
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_PostMessageEx(SBHANDLE hBus,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,
							LPCSTR lpBuffer,
							int nLen,
							UINT nPriority);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_PostQuitMessage
//	函数功能 : 发送一条软件总线循环的退出命令
//	作    者 : 谭 北 华
//	时    间 : 2007年4月14日
//	返 回 值 : BOOL
//	参数说明 : SBHANDLE hBus
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_PostQuitMessage(SBHANDLE hBus);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_PostDelayMessage
//	函数功能 : 投递一条异步消息到软件总线队列并马上返回，异步消息将在延时指定时间后
//				自动添加到软件总线队列
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : UINT 定时器ID,唯一标识一个定时事件，作为参数GSB_KillTimer时的参数
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         UINT nModuleSrcID,发送消息的模块ID
//				         UINT nModuleDestID,接收消息的模块ID
//				         UINT nMsg,对方接收到的消息ID
//				         WPARAM wParam,自定义参数
//				         LPARAM lParam,自定义参数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen, 要复制传递的缓冲长度，为-1表示字符串长度
//				         DWORD dwDelayTime ，指定的延时时间，以毫秒为单位
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_PostDelayMessage(SBHANDLE hBus,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,
							LPCSTR lpBuffer,
							int nLen,
							DWORD dwDelayTime);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_SendMessage
//	函数功能 : 投递一条同步消息到软件总线队列，等待消息被处理完毕后返回
//				如果调用者和软件总线所在的线程是同一线程(任务)，将会引起直接调用
//				如果调用者和软件总线所在的线程是不同线程(任务)，将会引起阻塞
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : int  为对方函数处理的返回值
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         UINT nModuleSrcID,发送消息的模块ID
//				         UINT nModuleDestID,接收消息的模块ID
//				         UINT nMsg,对方接收到的消息ID
//				         WPARAM wParam,自定义参数
//				         LPARAM lParam,自定义参数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen, 要复制传递的缓冲长度，为-1表示字符串长度
///////////////////////////////////////////////////////////////
int  WINAPI GSB_SendMessage(SBHANDLE hBus,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,
							LPCSTR lpBuffer,
							int nLen);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_SendMessageEx
//	函数功能 : 投递一条同步消息到软件总线队列，等待消息被处理完毕后返回
//				如果调用者和软件总线所在的线程是同一线程(任务)，将会引起直接调用
//				如果调用者和软件总线所在的线程是不同线程(任务)，将会引起阻塞
//				此函数是GSB_SendMessage函数的增强版本，提供了队列优先级功能
//	作    者 : 谭 北 华
//	时    间 : 2007年12月
//	返 回 值 : int  为对方函数处理的返回值
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         UINT nModuleSrcID,发送消息的模块ID
//				         UINT nModuleDestID,接收消息的模块ID
//				         UINT nMsg,对方接收到的消息ID
//				         WPARAM wParam,自定义参数
//				         LPARAM lParam,自定义参数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen, 要复制传递的缓冲长度，为-1表示字符串长度
//				         UINT nPriority ,消息在队列中的优先级(0-255)
///////////////////////////////////////////////////////////////
int  WINAPI GSB_SendMessageEx(SBHANDLE hBus,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,
							LPCSTR lpBuffer,
							int nLen,
							UINT nPriority);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_LoopMessage
//	函数功能 : 等待并处理一条消息,供软件总线循环泵用
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : int,取值为GSB_ERROR,GSB_EXIT,GSB_OK,GSB_TIMEOUT中的一个
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         DWORD dwWaitTime,处理的超时时间，毫秒
///////////////////////////////////////////////////////////////
int WINAPI GSB_LoopMessage(SBHANDLE hBus,
							DWORD dwWaitTime);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_PeekMessage
//	函数功能 : 从软件总线队列取出一条消息，此函数会引起阻塞
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : int,取值为GSB_ERROR,GSB_EXIT,GSB_OK,GSB_TIMEOUT中的一个
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         LPGSBMSG lpMsg,软件消息内容
//				         DWORD dwWaitTime,等待的超时时间，毫秒
///////////////////////////////////////////////////////////////
int WINAPI GSB_PeekMessage(SBHANDLE hBus,
							LPGSBMSG lpMsg,
							DWORD dwWaitTime);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_DispatchMessage
//	函数功能 : 分发一条消息,供软件总线循环泵用
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : int 分发函数返回值
//	参数说明 : SBHANDLE hBus,软件总线对象
//				            LPGSBMSG lpMsg,软件消息内容
///////////////////////////////////////////////////////////////
int WINAPI GSB_DispatchMessage(SBHANDLE hBus,
							   LPGSBMSG lpMsg);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_GetMaxBufferSize
//	函数功能 : 取得中断处理函数可以发送的消息的最大长度
//	作    者 : 谭 北 华
//	时    间 : 2005年4月20日
//	返 回 值 : DWORD 返回中断处理函数(IntPostMessage,IntJobAdd)可以复制传递的最大缓冲长度
//				注意V3版本的软件总线在非中断模式(SendMessage,PostMessage)已对消息大小不加限制了
//				此返回结果仅仅作为中断处理函数发送消息长度的参考
//	参数说明 : SBHANDLE hBus 软件总线对象
///////////////////////////////////////////////////////////////
DWORD WINAPI GSB_GetMaxBufferSize(SBHANDLE hBus);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_SetTimer
//	函数功能 : 设定一个定时器
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : UINT 定时器ID,唯一标识一个定时事件，作为参数GSB_KillTimer时的参数
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         UINT uElapse,定时间隔
//				         UINT nLoopTimes,定时次数,LOOP_FOREVER为无限循环定时
//				         GSBTIMERFUNCPTR lpfnRoutine,定时器专用回调函数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen,要复制传递的缓冲长度，为-1表示字符串长度
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_SetTimer(SBHANDLE hBus,
						 UINT uElapse,
						 UINT nLoopTimes,
						 GSBTIMERFUNCPTR lpfnRoutine,
						 LPCSTR lpBuffer,
						 int nLen,
						 int nParam1,
						 int nParam2,
						 int nParam3,
						 int nParam4,
						 int nParam5);



///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_SetJitterTimer
//	函数功能 : 设置一个抖动定时器
//	处理过程 : 本函数设置一个带有抖动功能的定时器，在定时的同时可以让定时器
//				在nJitterTime范围内作一个随机的抖动
//	备    注 : 
//	返 回 值 : UINT
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         UINT uElapse,定时间隔
//				         UINT nJitterTime,延时抖动大小，不能超过50%的uElapse
//				         UINT nLoopTimes,定时次数,LOOP_FOREVER为无限循环定时
//				         GSBTIMERFUNCPTR lpfnRoutine,定时器专用回调函数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen,要复制传递的缓冲长度，为-1表示字符串长度
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_SetJitterTimer(SBHANDLE hBus,
						 UINT uElapse,
						 UINT nJitterTime,
						 UINT nLoopTimes,
						 GSBTIMERFUNCPTR lpfnRoutine,
						 LPCSTR lpBuffer,
						 int nLen,
						 int nParam1,
						 int nParam2,
						 int nParam3,
						 int nParam4,
						 int nParam5);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_SetTimerFast
//	函数功能 : 设定一个定时器,快速函数回调版本
//	作    者 : 谭 北 华
//	时    间 : 2007年12月
//	返 回 值 : UINT 定时器ID,唯一标识一个定时事件，作为参数GSB_KillTimer时的参数
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         UINT uElapse,定时间隔
//				         UINT nLoopTimes,定时次数,LOOP_FOREVER为无限循环定时
//				         GSBFASTTIMERFUNCPTR lpfnRoutine,定时器专用回调函数,,注意此函数回调不带缓冲复制功能
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_SetTimerFast(SBHANDLE hBus,
						 UINT uElapse,
						 UINT nLoopTimes,
						 GSBFASTTIMERFUNCPTR lpfnRoutine,
						 int nParam1,
						 int nParam2,
						 int nParam3,
						 int nParam4,
						 int nParam5);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_SetJitterTimerFast
//	函数功能 : 设定一个抖动定时器,快速函数回调版本
//	作    者 : 谭 北 华
//	时    间 : 2007年12月
//	返 回 值 : UINT 定时器ID,唯一标识一个定时事件，作为参数GSB_KillTimer时的参数
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         UINT uElapse,定时间隔
//				         UINT nJitterTime,延时抖动大小，不能超过50%的uElapse
//				         UINT nLoopTimes,定时次数,LOOP_FOREVER为无限循环定时
//				         GSBFASTTIMERFUNCPTR lpfnRoutine,定时器专用回调函数,,注意此函数回调不带缓冲复制功能
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_SetJitterTimerFast(SBHANDLE hBus,
						 UINT uElapse,
						 UINT nJitterTime,
						 UINT nLoopTimes,
						 GSBFASTTIMERFUNCPTR lpfnRoutine,
						 int nParam1,
						 int nParam2,
						 int nParam3,
						 int nParam4,
						 int nParam5);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_SetTimerByID
//	函数功能 : 以模块ID为对象设置定时器
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : UINT 定时器ID,唯一标识一个定时事件，作为参数GSB_KillTimer时的参数
//	参数说明 : SBHANDLE hBus,软件总线对象
//				        UINT uElapse,定时间隔
//				        UINT nLoopTimes,定时次数,LOOP_FOREVER为无限循环定时
//				        UINT nModuleDestID,接收定时消息的模块ID
//				        UINT nMsg,接收定时的消息ID
//				         WPARAM wParam,自定义参数
//				         LPARAM lParam,自定义参数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen, 要复制传递的缓冲长度，为-1表示字符串长度
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_SetTimerByID(SBHANDLE hBus,
						UINT uElapse,
						UINT nLoopTimes,
						UINT nModuleDestID,
						UINT nMsg,
						WPARAM wParam,
						LPARAM lParam,
						LPCSTR lpBuffer,
						int nLen);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_SetTimerByID
//	函数功能 : 以模块ID为对象设置延时抖动定时器
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : UINT 定时器ID,唯一标识一个定时事件，作为参数GSB_KillTimer时的参数
//	参数说明 : SBHANDLE hBus,软件总线对象
//				        UINT uElapse,定时间隔
//				         UINT nJitterTime,延时抖动大小，不能超过50%的uElapse
//				        UINT nLoopTimes,定时次数,LOOP_FOREVER为无限循环定时
//				        UINT nModuleDestID,接收定时消息的模块ID
//				        UINT nMsg,接收定时的消息ID
//				         WPARAM wParam,自定义参数
//				         LPARAM lParam,自定义参数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen, 要复制传递的缓冲长度，为-1表示字符串长度
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_SetJitterTimerByID(SBHANDLE hBus,
						UINT uElapse,
						UINT nJitterTime,
						UINT nLoopTimes,
						UINT nModuleDestID,
						UINT nMsg,
						WPARAM wParam,
						LPARAM lParam,
						LPCSTR lpBuffer,
						int nLen);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_KillTimer
//	函数功能 : 取消指定的定时器事件
//	作    者 : 谭 北 华
//	时    间 : 2005年4月20日
//	返 回 值 : BOOL ,TRUE or FALSE
//	参数说明 : SBHANDLE hBus,软件总线对象
//				        UINT nTimer UINT 定时器ID,唯一标识一个定时事件,
//									由GSB_SetTimer,GSB_SetTimerByID,GSB_DelayJobAdd,GSB_PostDelayMessage
//									几个函数中的某个调用的返回值标识
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_KillTimer(SBHANDLE hBus,
						UINT nTimer);


///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_GetTimerLeftSleep
//	函数功能 : 取得当前时间点离下一次定时器被触发的时间间隔
//	处理过程 : 
//	备    注 : 用来判断定时器在何时被触发
//	返 回 值 : UINT 当前时间点离下一次定时器触发的时间(ms)
//	参数说明 : SBHANDLE hBus,
//				        UINT nTimer
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_GetTimerLeftSleep(SBHANDLE hBus,
						UINT nTimer);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_JobAdd
//	函数功能 : 事件函数直接回调,为异步函数调用
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : BOOL TRUE or FALSE
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         GSBFUNCPTR lpfnRoutine,回调函数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen,要复制传递的缓冲长度，为-1表示字符串长度
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_JobAdd(SBHANDLE hBus,
								GSBFUNCPTR lpfnRoutine,
								LPCSTR lpBuffer,
								int nLen,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_JobAddEx
//	函数功能 : 事件函数直接回调,为异步函数调用
//			   此函数是GSB_JobAdd函数的增强版本，提供了队列优先级功能				
//	作    者 : 谭 北 华
//	时    间 : 2007年12月
//	返 回 值 : BOOL TRUE or FALSE
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         GSBFUNCPTR lpfnRoutine,回调函数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen,要复制传递的缓冲长度，为-1表示字符串长度
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
//				         UINT nPriority ,消息在队列中的优先级(0-255)
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_JobAddEx(SBHANDLE hBus,
								GSBFUNCPTR lpfnRoutine,
								LPCSTR lpBuffer,
								int nLen,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5,
								UINT nPriority);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_JobAddFast
//	函数功能 : 事件函数快速回调,为异步函数调用
//	作    者 : 谭 北 华
//	时    间 : 2007年12月
//	返 回 值 : BOOL TRUE or FALSE
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         GSBFASTFUNCPTR lpfnRoutine,回调函数,注意此函数回调不带缓冲复制功能
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_JobAddFast(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_JobAddFastEx
//	函数功能 : 事件函数快速回调,为异步函数调用
//			   此函数是GSB_JobAddFast函数的增强版本，提供了队列优先级功能				
//	作    者 : 谭 北 华
//	时    间 : 2007年12月
//	返 回 值 : BOOL TRUE or FALSE
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         GSBFASTFUNCPTR lpfnRoutine,回调函数,注意此函数回调不带缓冲复制功能
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
//				         UINT nPriority ,消息在队列中的优先级(0-255)
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_JobAddFastEx(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5,
								UINT nPriority);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_WaitJobAdd
//	函数功能 : 事件函数直接回调,为同步函数调用
//				此函数等待处理完才返回,返回值为lpfnRoutine的返回值
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : int  lpfnRoutine的返回值
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         GSBFUNCPTR lpfnRoutine,回调函数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen,要复制传递的缓冲长度，为-1表示字符串长度
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
///////////////////////////////////////////////////////////////
int WINAPI GSB_WaitJobAdd(SBHANDLE hBus,
								GSBFUNCPTR lpfnRoutine,
								LPCSTR lpBuffer,
								int nLen,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_WaitJobAdd
//	函数功能 : 事件函数直接回调,为同步函数调用
//				此函数等待处理完才返回,返回值为lpfnRoutine的返回值
//			    此函数是GSB_WaitJobAdd函数的增强版本，提供了队列优先级功能	
//	作    者 : 谭 北 华
//	时    间 : 2007年12月
//	返 回 值 : int  lpfnRoutine的返回值
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         GSBFUNCPTR lpfnRoutine,回调函数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen,要复制传递的缓冲长度，为-1表示字符串长度
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
//				         UINT nPriority ,消息在队列中的优先级(0-255)
///////////////////////////////////////////////////////////////
int WINAPI GSB_WaitJobAddEx(SBHANDLE hBus,
								GSBFUNCPTR lpfnRoutine,
								LPCSTR lpBuffer,
								int nLen,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5,
								UINT nPriority);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_WaitJobAddFast
//	函数功能 : 事件函数快速回调,为同步函数调用
//				此函数等待处理完才返回,返回值为lpfnRoutine的返回值
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : int  lpfnRoutine的返回值
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         GSBFASTFUNCPTR lpfnRoutine,回调函数,注意此函数回调不带缓冲复制功能
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
///////////////////////////////////////////////////////////////
int WINAPI GSB_WaitJobAddFast(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_WaitJobAddFastEx
//	函数功能 : 事件函数快速回调,为同步函数调用
//				此函数等待处理完才返回,返回值为lpfnRoutine的返回值
//			    此函数是GSB_WaitJobAddFast函数的增强版本，提供了队列优先级功能	
//	作    者 : 谭 北 华
//	时    间 : 2007年12月
//	返 回 值 : int  lpfnRoutine的返回值
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         GSBFASTFUNCPTR lpfnRoutine,回调函数,注意此函数回调不带缓冲复制功能
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
//				         UINT nPriority ,消息在队列中的优先级(0-255)
///////////////////////////////////////////////////////////////
int WINAPI GSB_WaitJobAddFastEx(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5,
								UINT nPriority);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_DelayJobAdd
//	函数功能 : 事件函数延时异步回调
//	作    者 : 谭 北 华
//	时    间 : 2005年4月
//	返 回 值 : UINT
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         GSBFUNCPTR lpfnRoutine,回调函数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen,要复制传递的缓冲长度，为-1表示字符串长度
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
//				          DWORD dwDelayTime ,指定的延时时间，以毫秒为单位
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_DelayJobAdd(SBHANDLE hBus,
								GSBFUNCPTR lpfnRoutine,
								LPCSTR lpBuffer,
								int nLen,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5,
								DWORD dwDelayTime);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_DelayJobAdd
//	函数功能 : 事件函数延时异步快速回调
//	作    者 : 谭 北 华
//	时    间 : 2007年12月
//	返 回 值 : UINT
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         GSBFASTFUNCPTR lpfnRoutine,回调函数,注意此函数回调不带缓冲复制功能
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
//				          DWORD dwDelayTime ,指定的延时时间，以毫秒为单位
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_DelayJobAddFast(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5,
								DWORD dwDelayTime);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_KillAllModuleTimer
//	函数功能 : 取消指定模块的定时器事件
//	作    者 : 谭 北 华
//	时    间 : 2006年1月5日
//	返 回 值 : BOOL ,TRUE or FALSE
//	参数说明 : SBHANDLE hBus, 软件总线对象
//				        UINT nModuleId 指定的模块ID
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_KillAllModuleTimer(SBHANDLE hBus,
						UINT nModuleId);


///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_GetSystemMsgEvent
//	函数功能 : 取得与操作系统相关的事件句柄
//	作    者 : 谭 北 华
//	时    间 : 2006年1月5日
//	返 回 值 : UINT ,对于Windows系统，返回的是一个Handle,对VxWorks系统，返回的是SEM_ID
//					请用GSB_MAKE_SYSEVENT_HANDLE宏转换为正确的值
//	参数说明 : SBHANDLE hBus
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_GetSystemMsgEvent(SBHANDLE hBus);


///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_IntPostMessage
//	函数功能 : 可以给中断函数内部调用的PostMessage函数
//				注意：中断函数优先级最高，此函数必须在中断中调用
//	作    者 : 谭 北 华
//	时    间 : 2006年5月25日
//	返 回 值 : BOOL
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         UINT nModuleSrcID,发送消息的模块ID
//				         UINT nModuleDestID,接收消息的模块ID
//				         UINT nMsg,对方接收到的消息ID
//				         WPARAM wParam,自定义参数
//				         LPARAM lParam,自定义参数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen 要复制传递的缓冲长度，为-1表示字符串长度
//								  注意 nLen的大小不要超过GSB_GetMaxBufferSize()返回值
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_IntPostMessage(SBHANDLE hBus,
							UINT nModuleSrcID,
							UINT nModuleDestID,
							UINT nMsg,
							WPARAM wParam,
							LPARAM lParam,
							LPCSTR lpBuffer,
							int nLen);

///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_IntJobAdd
//	函数功能 : 可以给中断函数内部调用的JobAdd函数
//				注意：中断函数优先级最高，此函数必须在中断中调用
//	作    者 : 谭 北 华
//	时    间 : 2006年5月25日
//	返 回 值 : BOOL
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         GSBFUNCPTR lpfnRoutine,回调函数
//				         LPCSTR lpBuffer,要复制传递的缓冲
//				         int nLen,要复制传递的缓冲长度，为-1表示字符串长度
//								  注意 nLen的大小不要超过GSB_GetMaxBufferSize()返回值
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_IntJobAdd(SBHANDLE hBus,
								GSBFUNCPTR lpfnRoutine,
								LPCSTR lpBuffer,
								int nLen,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5);


///////////////////////////////////////////////////////////////
//	函 数 名 : GSB_IntJobAddFast
//	函数功能 : 可以给中断函数内部调用的JobAdd函数
//				注意：中断函数优先级最高，此函数必须在中断中调用
//				此函数为GSB_IntJobAdd的快速分发版本，可以提高在中断中的调用速度
//	作    者 : 谭 北 华
//	时    间 : 2006年5月25日
//	返 回 值 : BOOL
//	参数说明 : SBHANDLE hBus,软件总线对象
//				         GSBFASTFUNCPTR lpfnRoutine,回调函数,注意此函数回调不带缓冲复制功能
//				         int nParam1,传递给回调函数的自定义参数1
//				         int nParam2,传递给回调函数的自定义参数2
//				         int nParam3,传递给回调函数的自定义参数3
//				         int nParam4,传递给回调函数的自定义参数4
//				         int nParam5 传递给回调函数的自定义参数5
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_IntJobAddFast(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5);

#ifdef __cplusplus
};
#endif //__cplusplus


/////////////////////////////////////////////////////////////////////////////////
//软件总线的类封装:
//#include "softbus.inl"


#endif // __SOFTBUS_H_INCLUDED

