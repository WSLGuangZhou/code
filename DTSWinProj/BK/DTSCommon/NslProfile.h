/*******************************************************************************
ģ����       : NslProfile
�ļ�ʵ�ֹ��� : ini�ļ���ʽ��д
����         : sl.cai
�汾         : 1.1
--------------------------------------------------------------------------------
��ע         : ���ڱ�׼c��д
--------------------------------------------------------------------------------
�޸ļ�¼ : 
�� ��        �汾   �޸���         �޸����� 
08/14/2009   1.0    sl.cai           ����
09/14/2009   1.1    sl.cai           ���GetProfileStringPrivateFast��
                                     �ӿ���GetPrivateProfile�Ķ�ȡ�ٶ�
********************************************************************************

* ��Ȩ����(c) 2009, sl.cai, ��������Ȩ��

*******************************************************************************/
#ifndef NSL_PROFILE_H__
#define NSL_PROFILE_H__

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
�� �� �� : NslWritePrivateProfileString
�������� : ���ַ���[String]д��[FileName]�����û�ж�Ӧ��[FileName]���򴴽�����
           �������û�ж�Ӧ��[AppName]��[KeyName]���򴴽�һ���µ��
��    �� : sl.cai
ʱ    �� : 08/14/2009 
�� �� ֵ : BOOL
����˵�� : lpAppName - pointer to section name
           lpKeyName - pointer to key name
           lpString  - pointer to string to add
           lpFileName- pointer to initialization filename
��    �� : NULL
��    ע : NULL
*******************************************************************************/
BOOL NslWritePrivateProfileString(LPCSTR lpAppName,
								  LPCSTR lpKeyName,
								  LPCSTR lpString,
								  LPCSTR lpFileName);

/*******************************************************************************
�� �� �� : NslWritePrivateProfileInt
�������� : ������ֵ[Value]д��[FileName]�����û�ж�Ӧ��[FileName]���򴴽�����
           �������û�ж�Ӧ��[AppName]��[KeyName]���򴴽�һ���µ��
��    �� : sl.cai
ʱ    �� : 08/14/2009 
�� �� ֵ : BOOL
����˵�� : lpAppName - pointer to section name
           lpKeyName - pointer to key name
           iValue    - int32 value to add
           lpFileName- pointer to initialization filename
��    �� : NULL
��    ע : NULL
*******************************************************************************/
BOOL NslWritePrivateProfileInt( LPCSTR lpAppName,
								LPCSTR lpKeyName,
								INT32  iValue,
								LPCSTR lpFileName);

/*******************************************************************************
�� �� �� : NslGetPrivateProfileString
�������� : ��[FileName]�ж�ȡ�ַ���[ReturnedString]���������ڶ�Ӧ����
           [Default]������[ReturnedString]��Ϊ�滻��
��    �� : sl.cai
ʱ    �� : 08/14/2009 
�� �� ֵ : BOOL
����˵�� : lpAppName        - pointer to section name
           lpKeyName        - pointer to key name
           lpDefault        - points to default string
           lpReturnedString - points to destination buffer
           nSize            - size of destination buffer
           lpFileName       - pointer to initialization filename
��    �� : NULL
��    ע : NULL
*******************************************************************************/
BOOL NslGetPrivateProfileString(LPCSTR lpAppName,
								LPCSTR lpKeyName,
								LPCSTR lpDefault,
								LPSTR  lpReturnedString,
								DWORD  nSize,
								LPCSTR lpFileName);

/*******************************************************************************
�� �� �� : NslGetPrivateProfileInt
�������� : ��[FileName]�ж�ȡ��ֵ���أ��������ڶ�Ӧ����
           [Default]������Ϊ�滻
��    �� : sl.cai
ʱ    �� : 08/14/2009 
�� �� ֵ : int
����˵�� : lpAppName        - pointer to section name
           lpKeyName        - pointer to key name
           iDefault         - default value
           lpFileName       - pointer to initialization filename
��    �� : NULL
��    ע : NULL
*******************************************************************************/
INT32 NslGetPrivateProfileInt(LPCSTR lpAppName,
							  LPCSTR lpKeyName,
							  INT32  iDefault,
							  LPCSTR lpFileName);
#ifdef __cplusplus
}
#endif

#endif /* NSL_PROFILE_H__ */
