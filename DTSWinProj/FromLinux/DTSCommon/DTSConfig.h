//////////////////////////////////////////////////////////////////////////
//�����ļ�
#ifndef DTS_CONFIG_H_INCLUDED
#define DTS_CONFIG_H_INCLUDED

//�����ļ���
#define DTS_CONFIG_FILE_NAME "DTSConfigFile.ini"

#define DTS_MDL_ALL_NAME			"ALL"
#define DTS_CONFIG_LEVEL_NAME		"CONFIG"


#define DTS_CONFIG_ENABLE			1
#define DTS_CONFIG_DISABLE			0
#define DTS_CONFIG_NOT_EXIST		(UINT)-1

//��ģ��ͨ��ConfileFile���ã�����ļ���û�и�ģ��������򴴽���Ŀ������Ĭ��DebugOption���ر����У�
int DTSMdlInitConfigFile(LPCSTR lpAppName);


#endif

