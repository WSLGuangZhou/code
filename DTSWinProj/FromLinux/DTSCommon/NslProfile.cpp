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

//#include "StdAfx.h"
#include "DTSCommon.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

//#include <Nsl/Nsl.h>
//#include <Nsl/File/NslProfile.h>

#define KEY_LEN             63
#define SECTION_LEN         31
#define VALUE_LEN           255
#define BUFFER_LEN          256 /* BUFFER_LEN = VALUE_LEN+1 */

#define CHECK_MEM(p)                                                         \
{                                                                            \
    if ((p) == NULL)                                                         \
    {                                                                        \
        printf("file %s, line %d memory NULL! exit(1)\n",__FILE__, __LINE__);\
        /*lint -e(586) */exit(1);                                            \
    }                                                                        \
}

// 文本行分析类型
enum Nsl_ProfileTextLineType
{
	Nsl_ProfileTextLineType_BlankLine = 0,
	Nsl_ProfileTextLineType_SectionLine,
	Nsl_ProfileTextLineType_KeyLine,
	Nsl_ProfileTextLineType_ErrorLine
};

typedef struct tagProfileKey
{
	CHAR name[KEY_LEN+1];
	CHAR value[VALUE_LEN+1];
	struct tagProfileKey *next;
}PROF_KEY; 

typedef struct tagProfileSection
{
	CHAR name[SECTION_LEN+1];
	PROF_KEY *key_list;
	struct tagProfileSection *next;
}PROF_SECTION;

//#define NSL_LOCAL LOCAL

LOCAL int AnalysisTextLine(LPCSTR pszText);
LOCAL void GetSectionName(LPCSTR pszText, LPSTR section_name);
LOCAL void GetKeyInfo(LPCSTR pszText, LPSTR key_name, LPSTR key_value);
LOCAL void FreeProfile(PROF_SECTION *pProfileInfo);
LOCAL PROF_SECTION* LoadProfile(LPCSTR filename);
LOCAL void SaveProfile(LPCSTR filename, PROF_SECTION* pProfileInfo);
// NSL_LOCAL bool GetProfileStringPrivate(LPCSTR filename, LPCSTR section, 
// 							 LPCSTR key, LPSTR pszRetStr, DWORD dwSize);
LOCAL bool GetProfileStringPrivateFast(LPCSTR filename, LPCSTR section,
								 LPCSTR key, LPSTR pszRetStr, DWORD dwSize);


#ifdef NSL_COMPILER_MSVC9
#pragma warning(disable : 4996)
#endif // NSL_COMPILER_MSVC9

/*******************************************************************************
函 数 名 : AnalysisTextLine
函数功能 : 分析读入行的类型
作    者 : sl.cai
时    间 : 08/17/2009 
返 回 值 : int - Nsl_ProfileTextLineType
参数说明 : pszText待分析一行字符串
调    用 : NULL
备    注 : 较为严格的语法
*******************************************************************************/
LOCAL
int AnalysisTextLine(LPCSTR const pszText)
{
	int const iLen = (int)strlen(pszText);
    int iResult = Nsl_ProfileTextLineType_ErrorLine;

    if ((0 == iLen) || (pszText[0] == (TCHAR)';'))  
    { /* line start with ';' is considered as comment line */
        iResult = Nsl_ProfileTextLineType_BlankLine;
    }
    else if ((pszText[0] == (TCHAR)'[') && (pszText[iLen-1] == (TCHAR)']'))
    {
        iResult =  Nsl_ProfileTextLineType_SectionLine;
    }
    else
    {
        const CHAR* const p1 = strchr(pszText, '=');
        if ((NULL != p1) && /*(p1- (char *)pszText >= 1)*/(pszText != p1)) 
        { /* 不允许'='出现在首位，即key string为空 */
            iResult = Nsl_ProfileTextLineType_KeyLine;
        }
    }
    return iResult;
}

/*******************************************************************************
函 数 名 : GetSectionName
函数功能 : 从Section行中拷贝出Section字符串
作    者 : sl.cai
时    间 : 08/17/2009 
返 回 值 : NULL
参数说明 : pszText - 读入的待提取行
           section_name - 提取section字符串
调    用 : NULL
备    注 : NULL
*******************************************************************************/
LOCAL
void GetSectionName(LPCSTR const pszText, LPSTR section_name)
{
	/* 拷贝'['和']'之间字符串 */
	strncpy(section_name, pszText+1, min(strlen(pszText)-2, SECTION_LEN));
	/* 添加结束符 */
	section_name[min(strlen(pszText)-2, SECTION_LEN)] = '\0';
}

/*******************************************************************************
函 数 名 : GetKeyInfo
函数功能 : 从Key行中拷贝出Key字符串和Key Value字符串
作    者 : sl.cai
时    间 : 08/17/2009 
返 回 值 : NULL
参数说明 : pszText - 读入的待提取行
           key_name - 提取key字符串
           key_value - 提取value字符串   
调    用 : NULL
备    注 : NULL
*******************************************************************************/
LOCAL
void GetKeyInfo(LPCSTR pszText, LPSTR key_name, LPSTR key_value)
{
	int key_len = 0;
	int value_len = 0;

	char* p1 = strchr(const_cast<char *>(pszText), '=');

	CHECK_MEM(p1);

	while((pszText < p1) && (key_len < KEY_LEN))
	{
		*key_name = *pszText;
		key_name++;
		pszText++;
		key_len++;
	}
	*key_name = '\0';

	p1++;
	while ((*p1 !='\0') && (value_len < VALUE_LEN))
	{
		*key_value = *p1;
		key_value++;
		p1++;
		value_len++;
	}
	*key_value = '\0';
}

/*******************************************************************************
函 数 名 : FreeProfile
函数功能 : 释放申请空间
作    者 : sl.cai
时    间 : 08/17/2009 
返 回 值 : NULL
参数说明 : pProfileInfo - 释放对象
调    用 : NULL
备    注 : NULL
*******************************************************************************/
LOCAL
void FreeProfile(PROF_SECTION *pProfileInfo)
{
	PROF_SECTION *pCurrentSec = pProfileInfo;
    PROF_SECTION *pTempSec = NULL;
    PROF_KEY *pTempKey = NULL;
    PROF_KEY *pCurrentKey = NULL;

    while (NULL != pCurrentSec)
    {
        pTempSec = pCurrentSec;
        pCurrentKey = pTempSec->key_list;
        pCurrentSec = pCurrentSec->next;
        while (NULL != pCurrentKey)
        {
            pTempKey = pCurrentKey;
            pCurrentKey = pCurrentKey->next;
            free(pTempKey);
        }
        free(pTempSec);
    }
}

/*******************************************************************************
函 数 名 : LoadProfile
函数功能 : 从文件中转载生成PROF_SECTION数据
作    者 : sl.cai
时    间 : 08/17/2009 
返 回 值 : PROF_SECTION - 生成的文件信息
参数说明 : filename - 文件名
调    用 : NULL
备    注 : NULL
*******************************************************************************/
LOCAL
PROF_SECTION* LoadProfile(LPCSTR const filename)
{
	CHAR buff[BUFFER_LEN]={'\0'};
	PROF_SECTION *pResult = NULL;
	PROF_SECTION *pLastSec = NULL;
	PROF_SECTION *pTempSec = NULL;
	PROF_KEY *pTempKey = NULL;
	PROF_KEY *pLastKey = NULL;
	int nStrLen = 0;
	
	FILE *fp = fopen(filename, "r");
	if (fp != NULL)
	{
		while (!feof(fp))
		{
			if (fgets(buff, BUFFER_LEN, fp) != NULL)
			{
				nStrLen = strlen(buff);
				buff[nStrLen -1] = '\0';    /* erase last 0x0a(\n) */
				if(0x0d == buff[nStrLen-2])
				{
					buff[nStrLen-2] = '\0';	/* erase last 0x0d(\r) if file from windows */
				}

				switch (AnalysisTextLine(buff))
				{
					case Nsl_ProfileTextLineType_SectionLine:
						pTempSec = (PROF_SECTION *)malloc(sizeof(PROF_SECTION));
						CHECK_MEM(pTempSec);
						GetSectionName(buff, pTempSec->name);
						pTempSec->next = NULL;
						pTempSec->key_list = NULL;
						if (pResult == NULL)
						{
							pResult = pTempSec;
						}
						
						if (pLastSec != NULL)
						{
							pLastSec->next = pTempSec;
						}
						pLastSec = pTempSec;
						pLastKey = NULL;
						break;
					case Nsl_ProfileTextLineType_KeyLine:
						if (pLastSec != NULL)
						{
							pTempKey = (PROF_KEY *)malloc(sizeof(PROF_KEY));
							CHECK_MEM(pTempKey);
							GetKeyInfo(buff, pTempKey->name, pTempKey->value);
							pTempKey->next = NULL;
							if (pLastKey != NULL)
							{
								pLastKey->next = pTempKey;
							}
							else
							{
								pLastSec->key_list = pTempKey;
							}
							pLastKey = pTempKey;
						}
						break;
					case Nsl_ProfileTextLineType_BlankLine:
						break;
					default:
						printf("profile %s format not support\n", filename);
						FreeProfile(pResult);
						fclose(fp);
						return NULL;
				} /* end switch */
			} /* end if */
		} /* end while */
		fclose(fp);
	} /* end if */
	return pResult;
}

/*******************************************************************************
函 数 名 : SaveProfile
函数功能 : 保存Profile
作    者 : sl.cai
时    间 : 08/17/2009 
返 回 值 : NULL
参数说明 : filename - 文件名
           pProfileInfo - 待保存的文件信息
调    用 : NULL
备    注 : NULL
*******************************************************************************/
LOCAL
void SaveProfile(LPCSTR const filename, PROF_SECTION* pProfileInfo)
{
	PROF_SECTION* pCurrentSec = pProfileInfo;
	PROF_KEY* pCurrentKey = NULL;
	FILE *fp = fopen(filename, "w");
	if (NULL != fp)
	{
		while (NULL != pCurrentSec)
		{
			if (pCurrentSec != pProfileInfo)
			{
				/* section之间插入空行作为分隔 */
				fprintf(fp,"\n");
			}
			fprintf(fp, "[%s]\n", pCurrentSec->name);
			pCurrentKey = pCurrentSec->key_list;
			while (NULL != pCurrentKey)
			{
				fprintf(fp,"%s=%s\n", pCurrentKey->name, pCurrentKey->value);
				pCurrentKey = pCurrentKey->next;
			}
			pCurrentSec = pCurrentSec->next;
		}
		fclose(fp);
	}
}

/*******************************************************************************
函 数 名 : GetProfileStringPrivate
函数功能 : 判断是否存在匹配的字符串
作    者 : sl.cai
时    间 : 08/17/2009 
返 回 值 : bool
参数说明 : filename - 文件名
           section - section名
           key - key名
           pszRetStr - value名
		   dwSize - pszRetStr分配空间长度
调    用 : NULL
备    注 : 旧
*******************************************************************************/
// LOCAL
// bool GetProfileStringPrivate(LPCSTR const filename,
// 							 LPCSTR const section,
// 							 LPCSTR const key,
// 							 LPSTR   const pszRetStr,
// 							 DWORD   const dwSize)
// {
// 	PROF_SECTION * const pProfile = LoadProfile(filename);
// 	const PROF_SECTION * pTempSec = pProfile;
// 	const PROF_KEY     * pTempKey = NULL;
// 	if(NULL != pProfile)
//     {
//         while (NULL != pTempSec)
//         {
//             if (strcmp(pTempSec->name, section) == 0)
//             {
//                 pTempKey = pTempSec->key_list;
//                 while (NULL != pTempKey)
//                 {
//                     if (strcmp(pTempKey->name, key) == 0)
//                     {
// 						if (strlen(pTempKey->value) > dwSize)
// 						{
// 							FreeProfile(pProfile);
// 							return false;
// 						}
//                         strcpy(pszRetStr, pTempKey->value);
// 						FreeProfile(pProfile);
//                         return true;
//                     }
//                     pTempKey = pTempKey->next;
//                 }
// 				FreeProfile(pProfile);
//                 return false;
//             }
//             pTempSec = pTempSec->next;
//         }
//         FreeProfile(pProfile);
//     }
//     return false;
// }

/*******************************************************************************
函 数 名 : GetProfileStringPrivateFast
函数功能 : 判断是否存在匹配的字符串
作    者 : sl.cai
时    间 : 09/14/2009 
返 回 值 : bool
参数说明 : filename - 文件名
           section - section名
           key - key名
           pszRetStr - value名
		   dwSize - pszRetStr分配空间长度
调    用 : NULL
备    注 : 避免生成PROF_SECTION，直接查询，避免二次查询匹配
*******************************************************************************/
LOCAL
bool GetProfileStringPrivateFast(LPCSTR const filename,
								 LPCSTR const section,
								 LPCSTR const key,
								 LPSTR   const pszRetStr,
								 DWORD   const dwSize)
{
	/* 当前是否已经找到匹配的Section */
	bool   l_bIsSectionFind = false;
	/* section长度 */
	size_t const nSectionLen= strlen(section);
	/* key长度 */
	size_t const nKeyLen    = strlen(key);

	CHAR buff[BUFFER_LEN] = {'\0'};
	
	FILE *fp = fopen(filename, "r");
	if (fp != NULL)
	{
		while (!feof(fp))
		{
			if (fgets(buff, BUFFER_LEN, fp) != NULL)
			{
				buff[strlen(buff)-1] = '\0';    /* erase last 0x0a(\n) */
				switch (AnalysisTextLine(buff))
				{
					case Nsl_ProfileTextLineType_SectionLine:
						if ((0 == strncmp(section, buff+1, nSectionLen)) && 
							(']' == buff[nSectionLen+1]))
						{
							l_bIsSectionFind = true;
						}
						else
						{
							l_bIsSectionFind = false;
						}
						break;
					case Nsl_ProfileTextLineType_KeyLine:
						if ( l_bIsSectionFind ) /* 已经找到对应的section */
						{
							/* key后紧接着字符'=' */
							if ((0 == strncmp(key, buff, nKeyLen)) && ('=' == buff[nKeyLen]))
							{
								/* 判断预分配的写回string空间是否足够 */
								if (((strlen(buff)-nKeyLen)-1) < dwSize)
								{
									strcpy(pszRetStr, buff+nKeyLen+1);
									fclose(fp);
									return true;
								}
								else
								{
									return false;
								}
							}
						}
						break;
					default:
						break;
				} /* end switch */
			} /* end if */
		} /* end while */
		fclose(fp);
	} /* end if */
	return false;
}

#ifdef __cplusplus
extern "C" {
#endif

BOOL NslWritePrivateProfileString( LPCSTR const lpAppName, LPCSTR const lpKeyName, 
								   LPCSTR const lpString, LPCSTR const lpFileName )
{
	PROF_SECTION *pProfile = LoadProfile(lpFileName);
	PROF_SECTION *pTempSec = pProfile;
	PROF_KEY     *pTempKey = NULL;
	bool         result    = false;
	while (NULL != pTempSec)
	{
		if (strcmp(pTempSec->name, lpAppName) == 0)
		{
			pTempKey = pTempSec->key_list;
			while (NULL != pTempKey)
			{
				if (strcmp(pTempKey->name, lpKeyName) == 0)
				{
					/* update directly */
					strcpy(pTempKey->value, lpString);
					result = true;
					break;
				}
				pTempKey = pTempKey->next;
			}
			
			/* not found, create the key */
			if (false == result)
			{
				pTempKey = (PROF_KEY *)malloc(sizeof(PROF_KEY));
				CHECK_MEM(pTempKey);
				strcpy(pTempKey->value, lpString);
				strcpy(pTempKey->name, lpKeyName);
				/* insert the new key to the first item*/
				pTempKey->next = pTempSec->key_list;
				pTempSec->key_list = pTempKey;
				result = true;
			}
			break;
		}
		pTempSec = pTempSec->next;
	}
	
	/* section not found, create section and key */
	if (false == result)
	{
		pTempSec = (PROF_SECTION *)malloc(sizeof(PROF_SECTION));
		CHECK_MEM(pTempSec);
		pTempKey = (PROF_KEY *)malloc(sizeof(PROF_KEY));
		CHECK_MEM(pTempKey);
		
		strcpy(pTempKey->value, lpString);
		strcpy(pTempKey->name, lpKeyName);
		pTempKey->next = NULL;
		
		strcpy(pTempSec->name, lpAppName);
		pTempSec->key_list = pTempKey;
		
		pTempSec->next = pProfile;
		pProfile = pTempSec;
	}
	
	SaveProfile(lpFileName, pProfile);
	FreeProfile(pProfile);
	return TRUE;
}

BOOL NslWritePrivateProfileInt( LPCSTR const lpAppName, LPCSTR const lpKeyName, 
							    INT32 const iValue, LPCSTR const lpFileName )
{
    CHAR s[20] = {'\0'};
    sprintf(s, "%d", iValue);
    return NslWritePrivateProfileString(lpAppName, lpKeyName, s, lpFileName);
}

BOOL NslGetPrivateProfileString( LPCSTR const lpAppName, LPCSTR const lpKeyName, 
								 LPCSTR const lpDefault, LPSTR const lpReturnedString, 
								 DWORD const nSize, LPCSTR const lpFileName )
{
    if ( !GetProfileStringPrivateFast(lpFileName, lpAppName, lpKeyName, lpReturnedString, nSize) )
    {
        strcpy(lpReturnedString, lpDefault);
		return FALSE;
    }
	return TRUE;
}

INT32 NslGetPrivateProfileInt( LPCSTR const lpAppName, LPCSTR const lpKeyName, 
							   INT32 const iDefault, LPCSTR const lpFileName )
{
    INT32 iReturn = iDefault;
    CHAR s[10]={'\0'};
    if (GetProfileStringPrivateFast(lpFileName, lpAppName, lpKeyName, s, 10))
    {
        iReturn = atoi(s);
    }
    return iReturn;
}

#ifdef __cplusplus
}
#endif

#ifdef NSL_COMPILER_MSVC9
#pragma warning(default : 4996)
#endif // NSL_COMPILER_MSVC9

