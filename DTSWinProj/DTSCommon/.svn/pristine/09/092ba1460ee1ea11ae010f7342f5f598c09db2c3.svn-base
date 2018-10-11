/*******************************************************************************
模块名       : NslProfile
文件实现功能 : ini文件格式读写
作者         : sl.cai
版本         : 1.1
--------------------------------------------------------------------------------
备注         : 基于标准c编写
--------------------------------------------------------------------------------
修改记录 : 
日 期        版本   修改人         修改内容 
08/14/2009   1.0    sl.cai           创建
09/14/2009   1.1    sl.cai           添加GetProfileStringPrivateFast，
                                     加快了GetPrivateProfile的读取速度
********************************************************************************

* 版权所有(c) 2009, sl.cai, 保留所有权利

*******************************************************************************/
#ifndef NSL_PROFILE_H__
#define NSL_PROFILE_H__

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
函 数 名 : NslWritePrivateProfileString
函数功能 : 把字符串[String]写入[FileName]。如果没有对应的[FileName]，则创建该文
           件。如果没有对应的[AppName]和[KeyName]，则创建一个新的项。
作    者 : sl.cai
时    间 : 08/14/2009 
返 回 值 : BOOL
参数说明 : lpAppName - pointer to section name
           lpKeyName - pointer to key name
           lpString  - pointer to string to add
           lpFileName- pointer to initialization filename
调    用 : NULL
备    注 : NULL
*******************************************************************************/
BOOL NslWritePrivateProfileString(LPCSTR lpAppName,
								  LPCSTR lpKeyName,
								  LPCSTR lpString,
								  LPCSTR lpFileName);

/*******************************************************************************
函 数 名 : NslWritePrivateProfileInt
函数功能 : 把整数值[Value]写入[FileName]。如果没有对应的[FileName]，则创建该文
           件。如果没有对应的[AppName]和[KeyName]，则创建一个新的项。
作    者 : sl.cai
时    间 : 08/14/2009 
返 回 值 : BOOL
参数说明 : lpAppName - pointer to section name
           lpKeyName - pointer to key name
           iValue    - int32 value to add
           lpFileName- pointer to initialization filename
调    用 : NULL
备    注 : NULL
*******************************************************************************/
BOOL NslWritePrivateProfileInt( LPCSTR lpAppName,
								LPCSTR lpKeyName,
								INT32  iValue,
								LPCSTR lpFileName);

/*******************************************************************************
函 数 名 : NslGetPrivateProfileString
函数功能 : 从[FileName]中读取字符串[ReturnedString]，若不存在对应项，则把
           [Default]拷贝到[ReturnedString]作为替换。
作    者 : sl.cai
时    间 : 08/14/2009 
返 回 值 : BOOL
参数说明 : lpAppName        - pointer to section name
           lpKeyName        - pointer to key name
           lpDefault        - points to default string
           lpReturnedString - points to destination buffer
           nSize            - size of destination buffer
           lpFileName       - pointer to initialization filename
调    用 : NULL
备    注 : NULL
*******************************************************************************/
BOOL NslGetPrivateProfileString(LPCSTR lpAppName,
								LPCSTR lpKeyName,
								LPCSTR lpDefault,
								LPSTR  lpReturnedString,
								DWORD  nSize,
								LPCSTR lpFileName);

/*******************************************************************************
函 数 名 : NslGetPrivateProfileInt
函数功能 : 从[FileName]中读取整值返回，若不存在对应项，则把
           [Default]返回作为替换
作    者 : sl.cai
时    间 : 08/14/2009 
返 回 值 : int
参数说明 : lpAppName        - pointer to section name
           lpKeyName        - pointer to key name
           iDefault         - default value
           lpFileName       - pointer to initialization filename
调    用 : NULL
备    注 : NULL
*******************************************************************************/
INT32 NslGetPrivateProfileInt(LPCSTR lpAppName,
							  LPCSTR lpKeyName,
							  INT32  iDefault,
							  LPCSTR lpFileName);
#ifdef __cplusplus
}
#endif

#endif /* NSL_PROFILE_H__ */
