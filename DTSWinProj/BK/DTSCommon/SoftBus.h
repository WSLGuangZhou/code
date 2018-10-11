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
//	�� �� �� : SoftBus.h
//	�ļ����� : ������߶���
//	��    �� : ̷ �� ��
//	����ʱ�� : 2005��4��22��
//	��Ŀ���� : 
//	��    ע :
//	��ʷ��¼ : 2007.12.11 ������GSB_SendMessageEx��GSB_PostMessageEx��GSB_JobAddEx��GSB_WaitJobAddEx
//						   �ĸ������������˶Զ������ȼ���֧�֣������֧��255�����ȼ��������
//			   2008.01.11 ������GSB_SetJitterTimer��GSB_SetJitterTimerFast��GSB_SetJitterTimerByID
//						   ����������֧�����ö�����ʱ���Ĺ��ܡ���ν�Ķ�����ʱ����ָ�ڹ̶����uElase֮��
//						   �������ָ����nJitterTime��Χ�ڶ���������һ������·��Э���Hello�����͹��ܡ�
//				2008.03.17 ����GSB_GetTimerLeftSleep����������ͳ�ƶ�ʱ�����´δ���ʱ��
//				2008.03.26 �޸�GSB_LoopMessage��GSB_PeekMessage�ķ���ֵ��������Timeout��ָʾ
///////////////////////////////////////////////////////////////

#ifndef __SOFTBUS_H_INCLUDED
#define __SOFTBUS_H_INCLUDED



#ifdef __cplusplus
 //������߾��	
 typedef struct _SBHandle{}* SBHANDLE;
 //�ɱ�����Ļص�����
 typedef int 	(*GSBFUNCPTR) (LPCSTR lpBuffer,int nLen,...);     /* ptr to function returning int */
 typedef int 	(*GSBTIMERFUNCPTR) (UINT nTimerId,LPCSTR lpBuffer,int nLen,...);     /* ptr to timer function returning int */
 typedef int 	(*GSBFASTFUNCPTR) (...);     /* ptr to function returning int */
 typedef int 	(*GSBFASTTIMERFUNCPTR) (UINT nTimerId,...);     /* ptr to timer function returning int */
#else
 //������߾��	
 typedef struct _SBHandle{DWORD dwFlag;}* SBHANDLE;
 //�ɱ�����Ļص�����
 typedef int 	(*GSBFUNCPTR) ();	   /* ptr to function returning int */
 typedef int 	(*GSBTIMERFUNCPTR) ();	   /* ptr to timer function returning int */
 typedef int 	(*GSBFASTFUNCPTR) ();	   /* ptr to timer function returning int */
 typedef int 	(*GSBFASTTIMERFUNCPTR) ();	   /* ptr to timer function returning int */
#endif	

//����ϵͳ��ص��¼�����
#ifdef NSL_OS_WINDOWS
#define GSB_MAKE_SYSEVENT_HANDLE(nID) ((HANDLE)(nID))
#elif defined (NSL_OS_VXWORKS)
#define GSB_MAKE_SYSEVENT_HANDLE(nID) ((SEM_ID)(nID))
#elif defined (NSL_OS_LINUX)
#define GSB_MAKE_SYSEVENT_HANDLE(nID) ((sem_t*)(nID))
#endif 

//ȡ��ϵͳ��ص��¼����
#define GSB_GET_EVENT_HANDLE(hBus) GSB_MAKE_SYSEVENT_HANDLE(GSB_GetSystemMsgEvent(hBus))


//����������ȼ�����(�Ӹߵ���Ϊ0-255��0��ʾ������ȼ���255��ʾ������ȼ�)
#define GSB_PRIORITY_REALTIME	0
#define GSB_PRIORITY_HIGHEST	50
#define GSB_PRIORITY_HIGH		100
#define GSB_PRIORITY_LOW		150
#define GSB_PRIORITY_LOWEST		200
#define GSB_PRIORITY_IDLE		255

//Ĭ�ϵ����ȼ�������ͼ���Ϊ255
#define GSB_PRIORITY_DEFAULT GSB_PRIORITY_IDLE

//��ʱ����ֵ(GSB_LoopMessage��GSB_PeekMessage�ķ���ֵ)
#define GSB_ERROR		(-1)			//������Ϣ
#define GSB_EXIT		0				//�����յ�EXIT��Ϣ��Ҫ���˳�
#define GSB_OK			1				//����ɹ�
#define GSB_TIMEOUT		2				//����ʱ


#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

typedef int (WINAPI *LPONBUSMESSAGE)(UINT nModuleSrcID,  //������������ģ��ID
									 UINT nModuleDestID, //Ŀ���������ģ��ID
									 UINT nMsg,			 //��Ϣ
									 WPARAM wParam,		 //����1
									 LPARAM lParam,		 //����2
									 LPCSTR lpBuffer,	 //����
									 int nLen,			 //���峤��
									 DWORD dwUserData	 //�Զ�������
									 );


//�µ���Ϣ����(V3)
typedef struct tagGSBMsg
{
	UINT			nModuleSrcID;		//������������ģ��ID,nParam1
	UINT			nModuleDestID;		//Ŀ���������ģ��ID,nParam2
	UINT			nMsg;				//��Ϣ,nParam3
	WPARAM			wParam;				//����1,nParam4
	LPARAM			lParam;				//����2,nParam5
	DWORD			dwPrivate;			//˽������
}GSBMSG,*LPGSBMSG;



//ͨ��ģ��ID
#define BROASTCAST_MODULE_ID (UINT(-1))

//�Ż��������Ϣ����
#define GSB_OPTIMIZ_BUFFER_SIZE 1024L

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_Create
//	�������� : ����������߶���
//				ע��!!!: ��������ڴ���ʱ������GSB_Create����ʱ���̣߳�
//						 ���Դ�Ϊ����������GSB_SendMessage��GSB_WaitJobAdd�Ծ�����
//						 ͬ�����û��첽����
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : SBHANDLE ,������߶����������º����ĵ�һ��������
//	����˵�� : DWORD dwMaxModule, ����ģ�����
//			  DWORD dwMaxBufferSize,ÿ����Ϣ���Ļ����С,
//									ע��V3�汾����������ڷ��ж�ģʽ(SendMessage,PostMessage)�Ѷ���Ϣ��С����������
//									dwMaxBufferSize�����Ǹ�����������������жϴ���ʱ(IntPostMessage,IntJobAdd)
//									���Է��͵������Ϣ����
//									�Ƽ���dwMaxBufferSize��ֵ����ΪGSB_OPTIMIZ_BUFFER_SIZE
///////////////////////////////////////////////////////////////
SBHANDLE WINAPI GSB_Create(DWORD dwMaxModule,
						   DWORD dwMaxBufferSize);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_Destroy
//	�������� : ����������߶���
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : BOOL
//	����˵�� : SBHANDLE hBus,������߶�����
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_Destroy(SBHANDLE hBus);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_SetOwnerTaskId
//	�������� : ������������������߳�(����),������������߳�(����)�ڴ���ʱȡ��������
//				���߳�ΪĬ���̣߳����ô˺�����ı�Ĭ������,���Դ�Ϊ����������GSB_SendMessage��
//				GSB_WaitJobAdd�Ծ�����ͬ�����û��첽����
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��22��
//	�� �� ֵ : BOOL
//	����˵�� : SBHANDLE hBus,,������߶�����
//				           UINT nTaskID ��������ID�����Ϊ0��ʾ���õ�ǰ������ID
///////////////////////////////////////////////////////////////
#ifdef NSL_OS_LINUX
	typedef pthread_t sb_task_id_t;
#else
	typedef UINT sb_task_id_t;
#endif /* NSL_OS_LINUX */

BOOL WINAPI GSB_SetOwnerTaskId(SBHANDLE hBus,
                               sb_task_id_t nTaskID);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_RegisterModule
//	�������� : ע��ģ�鲢�ҽӵ����������
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : BOOL
//	����˵�� : SBHANDLE hBus,������߶���
//			 UINT nModuleID,ģ��ID
//			 LPONBUSMESSAGE lpfnCallback,ģ������¼��Ļص�����
//		     DWORD dwUserData �Զ�������(���¼��ص�ʱ������)
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_RegisterModule(SBHANDLE hBus,
							   UINT nModuleID,
							   LPONBUSMESSAGE lpfnCallback,
							   DWORD dwUserData);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_UnregisterModule
//	�������� : ��ע��ģ��,ʹ����������߲�����
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : BOOL
//	����˵�� : SBHANDLE hBus,������߶���
//			   UINT nModuleID ģ��ID
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_UnregisterModule(SBHANDLE hBus,
								 UINT nModuleID);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_PostMessage
//	�������� : Ͷ��һ���첽��Ϣ��������߶��в����Ϸ���(����������ȼ�255)
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : BOOL
//	����˵�� : SBHANDLE hBus,������߶���
//				         UINT nModuleSrcID,������Ϣ��ģ��ID
//				         UINT nModuleDestID,������Ϣ��ģ��ID
//				         UINT nMsg,�Է����յ�����ϢID
//				         WPARAM wParam,�Զ������
//				         LPARAM lParam,�Զ������
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
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
//	�� �� �� : GSB_PostMessageEx
//	�������� : Ͷ��һ���첽��Ϣ��������߶��в����Ϸ���
//				�˺�����GSB_PostMessage��������ǿ�汾���ṩ�˶������ȼ�����
//	��    �� : ̷ �� ��
//	ʱ    �� : 2007��12��
//	�� �� ֵ : BOOL
//	����˵�� : SBHANDLE hBus,
//	����˵�� : SBHANDLE hBus,������߶���
//				         UINT nModuleSrcID,������Ϣ��ģ��ID
//				         UINT nModuleDestID,������Ϣ��ģ��ID
//				         UINT nMsg,�Է����յ�����ϢID
//				         WPARAM wParam,�Զ������
//				         LPARAM lParam,�Զ������
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
//				         UINT nPriority,��Ϣ�ڶ����е����ȼ�(0-255)
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
//	�� �� �� : GSB_PostQuitMessage
//	�������� : ����һ���������ѭ�����˳�����
//	��    �� : ̷ �� ��
//	ʱ    �� : 2007��4��14��
//	�� �� ֵ : BOOL
//	����˵�� : SBHANDLE hBus
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_PostQuitMessage(SBHANDLE hBus);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_PostDelayMessage
//	�������� : Ͷ��һ���첽��Ϣ��������߶��в����Ϸ��أ��첽��Ϣ������ʱָ��ʱ���
//				�Զ���ӵ�������߶���
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : UINT ��ʱ��ID,Ψһ��ʶһ����ʱ�¼�����Ϊ����GSB_KillTimerʱ�Ĳ���
//	����˵�� : SBHANDLE hBus,������߶���
//				         UINT nModuleSrcID,������Ϣ��ģ��ID
//				         UINT nModuleDestID,������Ϣ��ģ��ID
//				         UINT nMsg,�Է����յ�����ϢID
//				         WPARAM wParam,�Զ������
//				         LPARAM lParam,�Զ������
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen, Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
//				         DWORD dwDelayTime ��ָ������ʱʱ�䣬�Ժ���Ϊ��λ
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
//	�� �� �� : GSB_SendMessage
//	�������� : Ͷ��һ��ͬ����Ϣ��������߶��У��ȴ���Ϣ��������Ϻ󷵻�
//				��������ߺ�����������ڵ��߳���ͬһ�߳�(����)����������ֱ�ӵ���
//				��������ߺ�����������ڵ��߳��ǲ�ͬ�߳�(����)��������������
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : int  Ϊ�Է���������ķ���ֵ
//	����˵�� : SBHANDLE hBus,������߶���
//				         UINT nModuleSrcID,������Ϣ��ģ��ID
//				         UINT nModuleDestID,������Ϣ��ģ��ID
//				         UINT nMsg,�Է����յ�����ϢID
//				         WPARAM wParam,�Զ������
//				         LPARAM lParam,�Զ������
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen, Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
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
//	�� �� �� : GSB_SendMessageEx
//	�������� : Ͷ��һ��ͬ����Ϣ��������߶��У��ȴ���Ϣ��������Ϻ󷵻�
//				��������ߺ�����������ڵ��߳���ͬһ�߳�(����)����������ֱ�ӵ���
//				��������ߺ�����������ڵ��߳��ǲ�ͬ�߳�(����)��������������
//				�˺�����GSB_SendMessage��������ǿ�汾���ṩ�˶������ȼ�����
//	��    �� : ̷ �� ��
//	ʱ    �� : 2007��12��
//	�� �� ֵ : int  Ϊ�Է���������ķ���ֵ
//	����˵�� : SBHANDLE hBus,������߶���
//				         UINT nModuleSrcID,������Ϣ��ģ��ID
//				         UINT nModuleDestID,������Ϣ��ģ��ID
//				         UINT nMsg,�Է����յ�����ϢID
//				         WPARAM wParam,�Զ������
//				         LPARAM lParam,�Զ������
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen, Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
//				         UINT nPriority ,��Ϣ�ڶ����е����ȼ�(0-255)
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
//	�� �� �� : GSB_LoopMessage
//	�������� : �ȴ�������һ����Ϣ,���������ѭ������
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : int,ȡֵΪGSB_ERROR,GSB_EXIT,GSB_OK,GSB_TIMEOUT�е�һ��
//	����˵�� : SBHANDLE hBus,������߶���
//				         DWORD dwWaitTime,����ĳ�ʱʱ�䣬����
///////////////////////////////////////////////////////////////
int WINAPI GSB_LoopMessage(SBHANDLE hBus,
							DWORD dwWaitTime);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_PeekMessage
//	�������� : ��������߶���ȡ��һ����Ϣ���˺�������������
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : int,ȡֵΪGSB_ERROR,GSB_EXIT,GSB_OK,GSB_TIMEOUT�е�һ��
//	����˵�� : SBHANDLE hBus,������߶���
//				         LPGSBMSG lpMsg,�����Ϣ����
//				         DWORD dwWaitTime,�ȴ��ĳ�ʱʱ�䣬����
///////////////////////////////////////////////////////////////
int WINAPI GSB_PeekMessage(SBHANDLE hBus,
							LPGSBMSG lpMsg,
							DWORD dwWaitTime);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_DispatchMessage
//	�������� : �ַ�һ����Ϣ,���������ѭ������
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : int �ַ���������ֵ
//	����˵�� : SBHANDLE hBus,������߶���
//				            LPGSBMSG lpMsg,�����Ϣ����
///////////////////////////////////////////////////////////////
int WINAPI GSB_DispatchMessage(SBHANDLE hBus,
							   LPGSBMSG lpMsg);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_GetMaxBufferSize
//	�������� : ȡ���жϴ��������Է��͵���Ϣ����󳤶�
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��20��
//	�� �� ֵ : DWORD �����жϴ�����(IntPostMessage,IntJobAdd)���Ը��ƴ��ݵ���󻺳峤��
//				ע��V3�汾����������ڷ��ж�ģʽ(SendMessage,PostMessage)�Ѷ���Ϣ��С����������
//				�˷��ؽ��������Ϊ�жϴ�����������Ϣ���ȵĲο�
//	����˵�� : SBHANDLE hBus ������߶���
///////////////////////////////////////////////////////////////
DWORD WINAPI GSB_GetMaxBufferSize(SBHANDLE hBus);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_SetTimer
//	�������� : �趨һ����ʱ��
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : UINT ��ʱ��ID,Ψһ��ʶһ����ʱ�¼�����Ϊ����GSB_KillTimerʱ�Ĳ���
//	����˵�� : SBHANDLE hBus,������߶���
//				         UINT uElapse,��ʱ���
//				         UINT nLoopTimes,��ʱ����,LOOP_FOREVERΪ����ѭ����ʱ
//				         GSBTIMERFUNCPTR lpfnRoutine,��ʱ��ר�ûص�����
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen,Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
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
//	�� �� �� : GSB_SetJitterTimer
//	�������� : ����һ��������ʱ��
//	������� : ����������һ�����ж������ܵĶ�ʱ�����ڶ�ʱ��ͬʱ�����ö�ʱ��
//				��nJitterTime��Χ����һ������Ķ���
//	��    ע : 
//	�� �� ֵ : UINT
//	����˵�� : SBHANDLE hBus,������߶���
//				         UINT uElapse,��ʱ���
//				         UINT nJitterTime,��ʱ������С�����ܳ���50%��uElapse
//				         UINT nLoopTimes,��ʱ����,LOOP_FOREVERΪ����ѭ����ʱ
//				         GSBTIMERFUNCPTR lpfnRoutine,��ʱ��ר�ûص�����
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen,Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
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
//	�� �� �� : GSB_SetTimerFast
//	�������� : �趨һ����ʱ��,���ٺ����ص��汾
//	��    �� : ̷ �� ��
//	ʱ    �� : 2007��12��
//	�� �� ֵ : UINT ��ʱ��ID,Ψһ��ʶһ����ʱ�¼�����Ϊ����GSB_KillTimerʱ�Ĳ���
//	����˵�� : SBHANDLE hBus,������߶���
//				         UINT uElapse,��ʱ���
//				         UINT nLoopTimes,��ʱ����,LOOP_FOREVERΪ����ѭ����ʱ
//				         GSBFASTTIMERFUNCPTR lpfnRoutine,��ʱ��ר�ûص�����,,ע��˺����ص��������帴�ƹ���
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
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
//	�� �� �� : GSB_SetJitterTimerFast
//	�������� : �趨һ��������ʱ��,���ٺ����ص��汾
//	��    �� : ̷ �� ��
//	ʱ    �� : 2007��12��
//	�� �� ֵ : UINT ��ʱ��ID,Ψһ��ʶһ����ʱ�¼�����Ϊ����GSB_KillTimerʱ�Ĳ���
//	����˵�� : SBHANDLE hBus,������߶���
//				         UINT uElapse,��ʱ���
//				         UINT nJitterTime,��ʱ������С�����ܳ���50%��uElapse
//				         UINT nLoopTimes,��ʱ����,LOOP_FOREVERΪ����ѭ����ʱ
//				         GSBFASTTIMERFUNCPTR lpfnRoutine,��ʱ��ר�ûص�����,,ע��˺����ص��������帴�ƹ���
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
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
//	�� �� �� : GSB_SetTimerByID
//	�������� : ��ģ��IDΪ�������ö�ʱ��
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : UINT ��ʱ��ID,Ψһ��ʶһ����ʱ�¼�����Ϊ����GSB_KillTimerʱ�Ĳ���
//	����˵�� : SBHANDLE hBus,������߶���
//				        UINT uElapse,��ʱ���
//				        UINT nLoopTimes,��ʱ����,LOOP_FOREVERΪ����ѭ����ʱ
//				        UINT nModuleDestID,���ն�ʱ��Ϣ��ģ��ID
//				        UINT nMsg,���ն�ʱ����ϢID
//				         WPARAM wParam,�Զ������
//				         LPARAM lParam,�Զ������
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen, Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
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
//	�� �� �� : GSB_SetTimerByID
//	�������� : ��ģ��IDΪ����������ʱ������ʱ��
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : UINT ��ʱ��ID,Ψһ��ʶһ����ʱ�¼�����Ϊ����GSB_KillTimerʱ�Ĳ���
//	����˵�� : SBHANDLE hBus,������߶���
//				        UINT uElapse,��ʱ���
//				         UINT nJitterTime,��ʱ������С�����ܳ���50%��uElapse
//				        UINT nLoopTimes,��ʱ����,LOOP_FOREVERΪ����ѭ����ʱ
//				        UINT nModuleDestID,���ն�ʱ��Ϣ��ģ��ID
//				        UINT nMsg,���ն�ʱ����ϢID
//				         WPARAM wParam,�Զ������
//				         LPARAM lParam,�Զ������
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen, Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
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
//	�� �� �� : GSB_KillTimer
//	�������� : ȡ��ָ���Ķ�ʱ���¼�
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��20��
//	�� �� ֵ : BOOL ,TRUE or FALSE
//	����˵�� : SBHANDLE hBus,������߶���
//				        UINT nTimer UINT ��ʱ��ID,Ψһ��ʶһ����ʱ�¼�,
//									��GSB_SetTimer,GSB_SetTimerByID,GSB_DelayJobAdd,GSB_PostDelayMessage
//									���������е�ĳ�����õķ���ֵ��ʶ
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_KillTimer(SBHANDLE hBus,
						UINT nTimer);


///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_GetTimerLeftSleep
//	�������� : ȡ�õ�ǰʱ�������һ�ζ�ʱ����������ʱ����
//	������� : 
//	��    ע : �����ж϶�ʱ���ں�ʱ������
//	�� �� ֵ : UINT ��ǰʱ�������һ�ζ�ʱ��������ʱ��(ms)
//	����˵�� : SBHANDLE hBus,
//				        UINT nTimer
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_GetTimerLeftSleep(SBHANDLE hBus,
						UINT nTimer);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_JobAdd
//	�������� : �¼�����ֱ�ӻص�,Ϊ�첽��������
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : BOOL TRUE or FALSE
//	����˵�� : SBHANDLE hBus,������߶���
//				         GSBFUNCPTR lpfnRoutine,�ص�����
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen,Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
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
//	�� �� �� : GSB_JobAddEx
//	�������� : �¼�����ֱ�ӻص�,Ϊ�첽��������
//			   �˺�����GSB_JobAdd��������ǿ�汾���ṩ�˶������ȼ�����				
//	��    �� : ̷ �� ��
//	ʱ    �� : 2007��12��
//	�� �� ֵ : BOOL TRUE or FALSE
//	����˵�� : SBHANDLE hBus,������߶���
//				         GSBFUNCPTR lpfnRoutine,�ص�����
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen,Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
//				         UINT nPriority ,��Ϣ�ڶ����е����ȼ�(0-255)
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
//	�� �� �� : GSB_JobAddFast
//	�������� : �¼��������ٻص�,Ϊ�첽��������
//	��    �� : ̷ �� ��
//	ʱ    �� : 2007��12��
//	�� �� ֵ : BOOL TRUE or FALSE
//	����˵�� : SBHANDLE hBus,������߶���
//				         GSBFASTFUNCPTR lpfnRoutine,�ص�����,ע��˺����ص��������帴�ƹ���
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_JobAddFast(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_JobAddFastEx
//	�������� : �¼��������ٻص�,Ϊ�첽��������
//			   �˺�����GSB_JobAddFast��������ǿ�汾���ṩ�˶������ȼ�����				
//	��    �� : ̷ �� ��
//	ʱ    �� : 2007��12��
//	�� �� ֵ : BOOL TRUE or FALSE
//	����˵�� : SBHANDLE hBus,������߶���
//				         GSBFASTFUNCPTR lpfnRoutine,�ص�����,ע��˺����ص��������帴�ƹ���
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
//				         UINT nPriority ,��Ϣ�ڶ����е����ȼ�(0-255)
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
//	�� �� �� : GSB_WaitJobAdd
//	�������� : �¼�����ֱ�ӻص�,Ϊͬ����������
//				�˺����ȴ�������ŷ���,����ֵΪlpfnRoutine�ķ���ֵ
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : int  lpfnRoutine�ķ���ֵ
//	����˵�� : SBHANDLE hBus,������߶���
//				         GSBFUNCPTR lpfnRoutine,�ص�����
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen,Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
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
//	�� �� �� : GSB_WaitJobAdd
//	�������� : �¼�����ֱ�ӻص�,Ϊͬ����������
//				�˺����ȴ�������ŷ���,����ֵΪlpfnRoutine�ķ���ֵ
//			    �˺�����GSB_WaitJobAdd��������ǿ�汾���ṩ�˶������ȼ�����	
//	��    �� : ̷ �� ��
//	ʱ    �� : 2007��12��
//	�� �� ֵ : int  lpfnRoutine�ķ���ֵ
//	����˵�� : SBHANDLE hBus,������߶���
//				         GSBFUNCPTR lpfnRoutine,�ص�����
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen,Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
//				         UINT nPriority ,��Ϣ�ڶ����е����ȼ�(0-255)
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
//	�� �� �� : GSB_WaitJobAddFast
//	�������� : �¼��������ٻص�,Ϊͬ����������
//				�˺����ȴ�������ŷ���,����ֵΪlpfnRoutine�ķ���ֵ
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : int  lpfnRoutine�ķ���ֵ
//	����˵�� : SBHANDLE hBus,������߶���
//				         GSBFASTFUNCPTR lpfnRoutine,�ص�����,ע��˺����ص��������帴�ƹ���
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
///////////////////////////////////////////////////////////////
int WINAPI GSB_WaitJobAddFast(SBHANDLE hBus,
								GSBFASTFUNCPTR lpfnRoutine,
								int nParam1, 
								int nParam2, 
								int nParam3,
								int nParam4, 
								int nParam5);

///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_WaitJobAddFastEx
//	�������� : �¼��������ٻص�,Ϊͬ����������
//				�˺����ȴ�������ŷ���,����ֵΪlpfnRoutine�ķ���ֵ
//			    �˺�����GSB_WaitJobAddFast��������ǿ�汾���ṩ�˶������ȼ�����	
//	��    �� : ̷ �� ��
//	ʱ    �� : 2007��12��
//	�� �� ֵ : int  lpfnRoutine�ķ���ֵ
//	����˵�� : SBHANDLE hBus,������߶���
//				         GSBFASTFUNCPTR lpfnRoutine,�ص�����,ע��˺����ص��������帴�ƹ���
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
//				         UINT nPriority ,��Ϣ�ڶ����е����ȼ�(0-255)
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
//	�� �� �� : GSB_DelayJobAdd
//	�������� : �¼�������ʱ�첽�ص�
//	��    �� : ̷ �� ��
//	ʱ    �� : 2005��4��
//	�� �� ֵ : UINT
//	����˵�� : SBHANDLE hBus,������߶���
//				         GSBFUNCPTR lpfnRoutine,�ص�����
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen,Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
//				          DWORD dwDelayTime ,ָ������ʱʱ�䣬�Ժ���Ϊ��λ
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
//	�� �� �� : GSB_DelayJobAdd
//	�������� : �¼�������ʱ�첽���ٻص�
//	��    �� : ̷ �� ��
//	ʱ    �� : 2007��12��
//	�� �� ֵ : UINT
//	����˵�� : SBHANDLE hBus,������߶���
//				         GSBFASTFUNCPTR lpfnRoutine,�ص�����,ע��˺����ص��������帴�ƹ���
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
//				          DWORD dwDelayTime ,ָ������ʱʱ�䣬�Ժ���Ϊ��λ
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
//	�� �� �� : GSB_KillAllModuleTimer
//	�������� : ȡ��ָ��ģ��Ķ�ʱ���¼�
//	��    �� : ̷ �� ��
//	ʱ    �� : 2006��1��5��
//	�� �� ֵ : BOOL ,TRUE or FALSE
//	����˵�� : SBHANDLE hBus, ������߶���
//				        UINT nModuleId ָ����ģ��ID
///////////////////////////////////////////////////////////////
BOOL WINAPI GSB_KillAllModuleTimer(SBHANDLE hBus,
						UINT nModuleId);


///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_GetSystemMsgEvent
//	�������� : ȡ�������ϵͳ��ص��¼����
//	��    �� : ̷ �� ��
//	ʱ    �� : 2006��1��5��
//	�� �� ֵ : UINT ,����Windowsϵͳ�����ص���һ��Handle,��VxWorksϵͳ�����ص���SEM_ID
//					����GSB_MAKE_SYSEVENT_HANDLE��ת��Ϊ��ȷ��ֵ
//	����˵�� : SBHANDLE hBus
///////////////////////////////////////////////////////////////
UINT WINAPI GSB_GetSystemMsgEvent(SBHANDLE hBus);


///////////////////////////////////////////////////////////////
//	�� �� �� : GSB_IntPostMessage
//	�������� : ���Ը��жϺ����ڲ����õ�PostMessage����
//				ע�⣺�жϺ������ȼ���ߣ��˺����������ж��е���
//	��    �� : ̷ �� ��
//	ʱ    �� : 2006��5��25��
//	�� �� ֵ : BOOL
//	����˵�� : SBHANDLE hBus,������߶���
//				         UINT nModuleSrcID,������Ϣ��ģ��ID
//				         UINT nModuleDestID,������Ϣ��ģ��ID
//				         UINT nMsg,�Է����յ�����ϢID
//				         WPARAM wParam,�Զ������
//				         LPARAM lParam,�Զ������
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
//								  ע�� nLen�Ĵ�С��Ҫ����GSB_GetMaxBufferSize()����ֵ
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
//	�� �� �� : GSB_IntJobAdd
//	�������� : ���Ը��жϺ����ڲ����õ�JobAdd����
//				ע�⣺�жϺ������ȼ���ߣ��˺����������ж��е���
//	��    �� : ̷ �� ��
//	ʱ    �� : 2006��5��25��
//	�� �� ֵ : BOOL
//	����˵�� : SBHANDLE hBus,������߶���
//				         GSBFUNCPTR lpfnRoutine,�ص�����
//				         LPCSTR lpBuffer,Ҫ���ƴ��ݵĻ���
//				         int nLen,Ҫ���ƴ��ݵĻ��峤�ȣ�Ϊ-1��ʾ�ַ�������
//								  ע�� nLen�Ĵ�С��Ҫ����GSB_GetMaxBufferSize()����ֵ
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
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
//	�� �� �� : GSB_IntJobAddFast
//	�������� : ���Ը��жϺ����ڲ����õ�JobAdd����
//				ע�⣺�жϺ������ȼ���ߣ��˺����������ж��е���
//				�˺���ΪGSB_IntJobAdd�Ŀ��ٷַ��汾������������ж��еĵ����ٶ�
//	��    �� : ̷ �� ��
//	ʱ    �� : 2006��5��25��
//	�� �� ֵ : BOOL
//	����˵�� : SBHANDLE hBus,������߶���
//				         GSBFASTFUNCPTR lpfnRoutine,�ص�����,ע��˺����ص��������帴�ƹ���
//				         int nParam1,���ݸ��ص��������Զ������1
//				         int nParam2,���ݸ��ص��������Զ������2
//				         int nParam3,���ݸ��ص��������Զ������3
//				         int nParam4,���ݸ��ص��������Զ������4
//				         int nParam5 ���ݸ��ص��������Զ������5
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
//������ߵ����װ:
//#include "softbus.inl"


#endif // __SOFTBUS_H_INCLUDED

