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

// �ı��з�������
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
�� �� �� : AnalysisTextLine
�������� : ���������е�����
��    �� : sl.cai
ʱ    �� : 08/17/2009 
�� �� ֵ : int - Nsl_ProfileTextLineType
����˵�� : pszText������һ���ַ���
��    �� : NULL
��    ע : ��Ϊ�ϸ���﷨
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
        { /* ������'='��������λ����key stringΪ�� */
            iResult = Nsl_ProfileTextLineType_KeyLine;
        }
    }
    return iResult;
}

/*******************************************************************************
�� �� �� : GetSectionName
�������� : ��Section���п�����Section�ַ���
��    �� : sl.cai
ʱ    �� : 08/17/2009 
�� �� ֵ : NULL
����˵�� : pszText - ����Ĵ���ȡ��
           section_name - ��ȡsection�ַ���
��    �� : NULL
��    ע : NULL
*******************************************************************************/
LOCAL
void GetSectionName(LPCSTR const pszText, LPSTR section_name)
{
	/* ����'['��']'֮���ַ��� */
	strncpy(section_name, pszText+1, min(strlen(pszText)-2, SECTION_LEN));
	/* ��ӽ����� */
	section_name[min(strlen(pszText)-2, SECTION_LEN)] = '\0';
}

/*******************************************************************************
�� �� �� : GetKeyInfo
�������� : ��Key���п�����Key�ַ�����Key Value�ַ���
��    �� : sl.cai
ʱ    �� : 08/17/2009 
�� �� ֵ : NULL
����˵�� : pszText - ����Ĵ���ȡ��
           key_name - ��ȡkey�ַ���
           key_value - ��ȡvalue�ַ���   
��    �� : NULL
��    ע : NULL
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
�� �� �� : FreeProfile
�������� : �ͷ�����ռ�
��    �� : sl.cai
ʱ    �� : 08/17/2009 
�� �� ֵ : NULL
����˵�� : pProfileInfo - �ͷŶ���
��    �� : NULL
��    ע : NULL
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
�� �� �� : LoadProfile
�������� : ���ļ���ת������PROF_SECTION����
��    �� : sl.cai
ʱ    �� : 08/17/2009 
�� �� ֵ : PROF_SECTION - ���ɵ��ļ���Ϣ
����˵�� : filename - �ļ���
��    �� : NULL
��    ע : NULL
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
�� �� �� : SaveProfile
�������� : ����Profile
��    �� : sl.cai
ʱ    �� : 08/17/2009 
�� �� ֵ : NULL
����˵�� : filename - �ļ���
           pProfileInfo - ��������ļ���Ϣ
��    �� : NULL
��    ע : NULL
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
				/* section֮����������Ϊ�ָ� */
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
�� �� �� : GetProfileStringPrivate
�������� : �ж��Ƿ����ƥ����ַ���
��    �� : sl.cai
ʱ    �� : 08/17/2009 
�� �� ֵ : bool
����˵�� : filename - �ļ���
           section - section��
           key - key��
           pszRetStr - value��
		   dwSize - pszRetStr����ռ䳤��
��    �� : NULL
��    ע : ��
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
�� �� �� : GetProfileStringPrivateFast
�������� : �ж��Ƿ����ƥ����ַ���
��    �� : sl.cai
ʱ    �� : 09/14/2009 
�� �� ֵ : bool
����˵�� : filename - �ļ���
           section - section��
           key - key��
           pszRetStr - value��
		   dwSize - pszRetStr����ռ䳤��
��    �� : NULL
��    ע : ��������PROF_SECTION��ֱ�Ӳ�ѯ��������β�ѯƥ��
*******************************************************************************/
LOCAL
bool GetProfileStringPrivateFast(LPCSTR const filename,
								 LPCSTR const section,
								 LPCSTR const key,
								 LPSTR   const pszRetStr,
								 DWORD   const dwSize)
{
	/* ��ǰ�Ƿ��Ѿ��ҵ�ƥ���Section */
	bool   l_bIsSectionFind = false;
	/* section���� */
	size_t const nSectionLen= strlen(section);
	/* key���� */
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
						if ( l_bIsSectionFind ) /* �Ѿ��ҵ���Ӧ��section */
						{
							/* key��������ַ�'=' */
							if ((0 == strncmp(key, buff, nKeyLen)) && ('=' == buff[nKeyLen]))
							{
								/* �ж�Ԥ�����д��string�ռ��Ƿ��㹻 */
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

