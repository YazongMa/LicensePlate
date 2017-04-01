#include "DataCache.h"
#include "Common/Common.h"

#include <deque>
#include <Algorithm>
#include <functional>
#include <io.h>
#include <assert.h>

using namespace std;

DataCacheContext		*g_pDataCacheContext	= NULL;
int						g_nMaxFaceDataCacheNum	= 0;

bool 	DC_Init(char *pchPath, char *pchChnl, int nMaxDataCacheNum)
{
	g_nMaxFaceDataCacheNum = nMaxDataCacheNum;

	if(_access(pchPath, 0) != 0)
	{
		if(!CreateDirectory(pchPath, NULL))
		{
			LOGFMTT("DC_Init �����ļ���·��ʧ�� GetLastError [%ld]", GetLastError());
			return false;
		}
	}

	if(pchPath == NULL || _access(pchPath, 0) != 0)
	{
		LOGFMTT("DC_Init ����·������\n");
		return false;
	}

	if(g_pDataCacheContext != NULL)
	{
		LOGFMTT("DC_Init �ѳ�ʼ��\n");
		return false;
	}

	TCHAR	szPath[MAX_PATH];
	_stprintf_s(szPath, MAX_PATH, _T("%s\\channel_%s"), pchPath, pchChnl);
	if(_access(szPath, 0) != 0)
	{
		if(!CreateDirectory(szPath, NULL))
		{
			LOGFMTT("DC_Init �������湤��·��ʧ�� GetLastError [%ld]", GetLastError());
			return false;
		}
	}

	g_pDataCacheContext	= new DataCacheContext;
	strcpy_s(g_pDataCacheContext->m_szWorkPath, MAX_PATH, szPath);
	g_pDataCacheContext->m_mapDataIndex.clear();
	g_pDataCacheContext->m_mapDataCache.clear();
	g_pDataCacheContext->m_llDataSeq	= 0;

	//����Ŀ¼�µ����л�������
	TCHAR			szFind[MAX_PATH];
	WIN32_FIND_DATA	FindFileData;
	int				nScanRet	= 0;
	long long		llDataId	= 0;
	CHAR			szDataId[MAX_PATH]	= {0};
	CHAR			szExt[MAX_PATH]		= {0};

	strcpy_s(szFind, MAX_PATH, g_pDataCacheContext->m_szWorkPath);
	strcat_s(szFind, MAX_PATH, _T("//*.*"));

	HANDLE	hFind	= FindFirstFile(szFind, &FindFileData);
    if(INVALID_HANDLE_VALUE == hFind)
	{
		LOGFMTT("DC_Init FindFirstFileʧ��\n");
		goto ErrLine;
	}
	while(TRUE)
    {
		if(FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY || FindFileData.dwFileAttributes != FILE_ATTRIBUTE_TEMPORARY || FindFileData.dwFileAttributes != FILE_ATTRIBUTE_HIDDEN)
		{
			if(FindFileData.cFileName[0] != _T('.'))
			{
				nScanRet	= _stscanf_s(FindFileData.cFileName, "%[^.].%s", szDataId, MAX_PATH, szExt, MAX_PATH);
				llDataId	= atoi(szDataId);
				if(nScanRet == 2 && strcmp(szExt, DATA_CACHE_FILE_EXT) == 0 && strlen(szDataId) == DATA_CACHE_FILE_PADDING_LEN && FindFileData.nFileSizeLow >= DATA_CACHE_FILE_HEADER_LEN)
				{
					g_pDataCacheContext->m_mapDataIndex[llDataId]	= llDataId;
					if(llDataId > g_pDataCacheContext->m_llDataSeq)
					{
						g_pDataCacheContext->m_llDataSeq	= llDataId;
					}
				}
				else
				{
					TCHAR	szMsg[MAX_PATH];
					_stprintf_s(szMsg, MAX_PATH, _T("DC_Init �ļ������ļ����Ȳ����ϸ�ʽ�淶��%s\n"), FindFileData.cFileName);
					LOGFMTT("%s", szMsg);
				}
			}
		}

		if(!FindNextFile(hFind, &FindFileData))
		{
			break;
		}
	}
	FindClose(hFind);

	return true;

ErrLine:

	if(g_pDataCacheContext)
	{
		g_pDataCacheContext->m_mapDataIndex.clear();
		g_pDataCacheContext->m_mapDataCache.clear();
		delete g_pDataCacheContext;
		g_pDataCacheContext	= NULL;
	}

	return false;
}

void 	DC_Uninit()
{
	if(g_pDataCacheContext == NULL)
	{
		LOGFMTT("DC_Uninit δ��ʼ��");
		return;
	}

	g_pDataCacheContext->m_mapDataIndex.clear();

	mapDataCacheIt	itCache	= g_pDataCacheContext->m_mapDataCache.begin();
	while(itCache != g_pDataCacheContext->m_mapDataCache.end())
	{
		BYTE	*pCache	= itCache->second;
		delete [] pCache;

		itCache++;
	}
	g_pDataCacheContext->m_mapDataCache.clear();

	
	delete g_pDataCacheContext;
	g_pDataCacheContext	= NULL;

	return;
}

__int64	DC_GetLastData(unsigned char **pbyData, int *pDataSize)
{
	if(g_pDataCacheContext == NULL)
	{
		LOGFMTT("DC_GetLastData δ��ʼ��");
		return -1;
	}

	if(g_pDataCacheContext->m_mapDataIndex.size() == 0)
	{
		LOGFMTT("DC_GetLastData ������");
		return 0;
	}

	*pbyData	= NULL;
	*pDataSize	= 0;

	//��map������ȡ��deque��������
	long long			llRet	= -1;
	deque<long long>	deqIndex;
	mapDataCacheIndexIt	itIndex	= g_pDataCacheContext->m_mapDataIndex.begin();
	while(itIndex != g_pDataCacheContext->m_mapDataIndex.end())
	{
		deqIndex.push_back(itIndex->second);

		itIndex++;
	}
	partial_sort(deqIndex.begin(), deqIndex.begin() + 1, deqIndex.end(), greater<long long>());
	llRet	= deqIndex.front();
	deqIndex.clear();

	BYTE	*pCache			= NULL;
	TCHAR	szFilePath[MAX_PATH];
	HANDLE	hFile			= NULL;
	DWORD	dwFileSize		= 0;
	DWORD	dwFileSizeHi	= 0;
	BOOL	bRet			= FALSE;
	DWORD	dwRead			= 0;

	mapDataCacheIt	itCache	= g_pDataCacheContext->m_mapDataCache.find(llRet);
	if(itCache == g_pDataCacheContext->m_mapDataCache.end())
	{
		//���ļ���ȡ
		_stprintf_s(szFilePath, MAX_PATH, _T("%s\\%020I64d.%s"), g_pDataCacheContext->m_szWorkPath, llRet, DATA_CACHE_FILE_EXT);

		hFile	= CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			OutputDebugString(_T("DC_GetLastData ���ļ�ʧ��\n"));
			llRet	= -1;
			return llRet;
		}
		dwFileSize	= GetFileSize(hFile, &dwFileSizeHi);
		if(dwFileSize >= DATA_CACHE_FILE_HEADER_LEN)
		{
			pCache	= new BYTE[dwFileSize];
			bRet	= ReadFile(hFile, pCache, dwFileSize, &dwRead, NULL);
			if(!bRet || dwFileSize != dwRead)
			{
				OutputDebugString(_T("DC_GetLastData ��ȡ�ļ������쳣\n"));
				llRet	= -1;
				goto ErrLine;
			}
		}
		else
		{
			OutputDebugString(_T("DC_GetLastData ��ȡ���ļ����ݳ����쳣\n"));
			llRet	= -1;
			goto ErrLine;
		}
		
		//���¶�ȡ��������ӵ�������
		g_pDataCacheContext->m_mapDataCache[llRet]	= pCache;
	}
	else
	{
		//�ӻ����ȡ
		pCache	= itCache->second;
	}

	//�ӻ����ȡ
	*pbyData	= pCache + DATA_CACHE_FILE_HEADER_LEN;
	memcpy(pDataSize, (pCache + DATA_CACHE_FILE_DATAID_LEN), sizeof(int));
ErrLine:

	if(hFile)
	{
		CloseHandle(hFile);
		hFile	= NULL;
	}

	return llRet;
}

bool 	DC_DeleteData(long long llDataId, int nRet)
{
	if(g_pDataCacheContext == NULL)
	{
		OutputDebugString(_T("DC_DeleteData δ��ʼ��\n"));
		return false;
	}

	//�������Ƿ����
	mapDataCacheIt	itCache	= g_pDataCacheContext->m_mapDataCache.find(llDataId);
	if(itCache != g_pDataCacheContext->m_mapDataCache.end())
	{
		BYTE	*pCache	= itCache->second;
		delete [] pCache;
		g_pDataCacheContext->m_mapDataCache.erase(itCache);
	}

	if(nRet != 0)
		return true;

	//�������Ƿ����
	mapDataCacheIndexIt	itIndex	= g_pDataCacheContext->m_mapDataIndex.find(llDataId);
	if(itIndex != g_pDataCacheContext->m_mapDataIndex.end())
	{
		g_pDataCacheContext->m_mapDataIndex.erase(itIndex);

		TCHAR	szFilePath[MAX_PATH];
		_stprintf_s(szFilePath, MAX_PATH, _T("%s\\%020I64d.%s"), g_pDataCacheContext->m_szWorkPath, llDataId, DATA_CACHE_FILE_EXT);
		BOOL bRet	= DeleteFile(szFilePath);
		if(!bRet)
		{
			TCHAR	szMsg[MAX_PATH];
			_stprintf_s(szMsg, MAX_PATH, _T("DC_DeleteData DeleteFileʧ�� GetLastError=%d"), GetLastError());
			OutputDebugString(szMsg);
		}
	}
	else
	{
		OutputDebugString(_T("DC_DeleteData �޸�����"));
		return false;
	}

	return true;
}

bool 	DC_AddData(unsigned char *pbyData, int nDataSize)
{
	bool	bRet	= false;

	if(g_pDataCacheContext == NULL)
	{
		OutputDebugString(_T("DC_AddData δ��ʼ��\n"));
		return bRet;
	}

	if(pbyData == NULL || nDataSize <= 0)
	{
		OutputDebugString(_T("DC_AddData ��������\n"));
		return bRet;
	}

	if (g_pDataCacheContext->m_mapDataIndex.size() >= g_nMaxFaceDataCacheNum)
	{
		LOGFMTT("��ǰ���������%d ����!\n", g_pDataCacheContext->m_mapDataIndex.size());
		return bRet;
	}

	BYTE	*pCache	= NULL;
	int		nCache	= nDataSize + DATA_CACHE_FILE_HEADER_LEN;
	pCache	= new BYTE[nCache];
	
	//ȡһ����������ID
	//long long	llDataId	= InterlockedIncrement64(&g_pDataCacheContext->m_llDataSeq);
	g_pDataCacheContext->m_llDataSeq++;
	long long	llDataId	= g_pDataCacheContext->m_llDataSeq;
	
	//��������
	memcpy_s(pCache, DATA_CACHE_FILE_DATAID_LEN, &llDataId, sizeof(long long));
	int test = DATA_CACHE_FILE_HEADER_LEN - DATA_CACHE_FILE_DATAID_LEN;
	int ttt = sizeof(int);
	memcpy_s(pCache + DATA_CACHE_FILE_DATAID_LEN, DATA_CACHE_FILE_HEADER_LEN - DATA_CACHE_FILE_DATAID_LEN, &nDataSize, sizeof(int));
	memcpy_s(pCache + DATA_CACHE_FILE_HEADER_LEN, nCache - DATA_CACHE_FILE_HEADER_LEN, pbyData, nDataSize);

	//д���ļ�
	TCHAR	szFilePath[MAX_PATH];
	HANDLE	hFile			= NULL;
	BOOL	bFileRet		= FALSE;
	DWORD	dwWritten		= 0;
	_stprintf_s(szFilePath, MAX_PATH, _T("%s\\%020I64d.%s"), g_pDataCacheContext->m_szWorkPath, llDataId, DATA_CACHE_FILE_EXT);

	hFile	= CreateFile(szFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString(_T("DC_AddData ���ļ�ʧ��\n"));
		goto ErrLine;
	}
	bFileRet	= WriteFile(hFile, pCache, nCache, &dwWritten, NULL);
	if(!bFileRet || nCache != dwWritten)
	{
		OutputDebugString(_T("DC_AddData д���ļ������쳣\n"));
		goto ErrLine;
	}

	//��ӵ���������ӳ��
	g_pDataCacheContext->m_mapDataIndex[llDataId]	= llDataId;

	bRet	= true;

ErrLine:

	if(hFile)
	{
		CloseHandle(hFile);
		hFile	= NULL;
	}

	if(pCache)
	{
		delete [] pCache;
		pCache	= NULL;
	}

	return bRet;
}

long	DC_GetCacheCnt()
{
	return static_cast<long>(g_pDataCacheContext->m_mapDataIndex.size());
}