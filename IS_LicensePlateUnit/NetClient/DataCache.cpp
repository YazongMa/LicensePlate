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
			LOGFMTT("DC_Init 创建文件夹路径失败 GetLastError [%ld]", GetLastError());
			return false;
		}
	}

	if(pchPath == NULL || _access(pchPath, 0) != 0)
	{
		LOGFMTT("DC_Init 输入路径错误\n");
		return false;
	}

	if(g_pDataCacheContext != NULL)
	{
		LOGFMTT("DC_Init 已初始化\n");
		return false;
	}

	TCHAR	szPath[MAX_PATH];
	_stprintf_s(szPath, MAX_PATH, _T("%s\\channel_%s"), pchPath, pchChnl);
	if(_access(szPath, 0) != 0)
	{
		if(!CreateDirectory(szPath, NULL))
		{
			LOGFMTT("DC_Init 创建缓存工作路径失败 GetLastError [%ld]", GetLastError());
			return false;
		}
	}

	g_pDataCacheContext	= new DataCacheContext;
	strcpy_s(g_pDataCacheContext->m_szWorkPath, MAX_PATH, szPath);
	g_pDataCacheContext->m_mapDataIndex.clear();
	g_pDataCacheContext->m_mapDataCache.clear();
	g_pDataCacheContext->m_llDataSeq	= 0;

	//加载目录下的所有缓存数据
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
		LOGFMTT("DC_Init FindFirstFile失败\n");
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
					_stprintf_s(szMsg, MAX_PATH, _T("DC_Init 文件名或文件长度不符合格式规范：%s\n"), FindFileData.cFileName);
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
		LOGFMTT("DC_Uninit 未初始化");
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
		LOGFMTT("DC_GetLastData 未初始化");
		return -1;
	}

	if(g_pDataCacheContext->m_mapDataIndex.size() == 0)
	{
		LOGFMTT("DC_GetLastData 无数据");
		return 0;
	}

	*pbyData	= NULL;
	*pDataSize	= 0;

	//将map中数据取到deque中做排序
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
		//从文件读取
		_stprintf_s(szFilePath, MAX_PATH, _T("%s\\%020I64d.%s"), g_pDataCacheContext->m_szWorkPath, llRet, DATA_CACHE_FILE_EXT);

		hFile	= CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			OutputDebugString(_T("DC_GetLastData 打开文件失败\n"));
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
				OutputDebugString(_T("DC_GetLastData 读取文件数据异常\n"));
				llRet	= -1;
				goto ErrLine;
			}
		}
		else
		{
			OutputDebugString(_T("DC_GetLastData 读取的文件数据长度异常\n"));
			llRet	= -1;
			goto ErrLine;
		}
		
		//将新读取的数据添加到缓存中
		g_pDataCacheContext->m_mapDataCache[llRet]	= pCache;
	}
	else
	{
		//从缓存读取
		pCache	= itCache->second;
	}

	//从缓存读取
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
		OutputDebugString(_T("DC_DeleteData 未初始化\n"));
		return false;
	}

	//缓存中是否存在
	mapDataCacheIt	itCache	= g_pDataCacheContext->m_mapDataCache.find(llDataId);
	if(itCache != g_pDataCacheContext->m_mapDataCache.end())
	{
		BYTE	*pCache	= itCache->second;
		delete [] pCache;
		g_pDataCacheContext->m_mapDataCache.erase(itCache);
	}

	if(nRet != 0)
		return true;

	//索引中是否存在
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
			_stprintf_s(szMsg, MAX_PATH, _T("DC_DeleteData DeleteFile失败 GetLastError=%d"), GetLastError());
			OutputDebugString(szMsg);
		}
	}
	else
	{
		OutputDebugString(_T("DC_DeleteData 无该数据"));
		return false;
	}

	return true;
}

bool 	DC_AddData(unsigned char *pbyData, int nDataSize)
{
	bool	bRet	= false;

	if(g_pDataCacheContext == NULL)
	{
		OutputDebugString(_T("DC_AddData 未初始化\n"));
		return bRet;
	}

	if(pbyData == NULL || nDataSize <= 0)
	{
		OutputDebugString(_T("DC_AddData 参数错误\n"));
		return bRet;
	}

	if (g_pDataCacheContext->m_mapDataIndex.size() >= g_nMaxFaceDataCacheNum)
	{
		LOGFMTT("当前缓存个数：%d 已满!\n", g_pDataCacheContext->m_mapDataIndex.size());
		return bRet;
	}

	BYTE	*pCache	= NULL;
	int		nCache	= nDataSize + DATA_CACHE_FILE_HEADER_LEN;
	pCache	= new BYTE[nCache];
	
	//取一个缓存数据ID
	//long long	llDataId	= InterlockedIncrement64(&g_pDataCacheContext->m_llDataSeq);
	g_pDataCacheContext->m_llDataSeq++;
	long long	llDataId	= g_pDataCacheContext->m_llDataSeq;
	
	//拷贝数据
	memcpy_s(pCache, DATA_CACHE_FILE_DATAID_LEN, &llDataId, sizeof(long long));
	int test = DATA_CACHE_FILE_HEADER_LEN - DATA_CACHE_FILE_DATAID_LEN;
	int ttt = sizeof(int);
	memcpy_s(pCache + DATA_CACHE_FILE_DATAID_LEN, DATA_CACHE_FILE_HEADER_LEN - DATA_CACHE_FILE_DATAID_LEN, &nDataSize, sizeof(int));
	memcpy_s(pCache + DATA_CACHE_FILE_HEADER_LEN, nCache - DATA_CACHE_FILE_HEADER_LEN, pbyData, nDataSize);

	//写入文件
	TCHAR	szFilePath[MAX_PATH];
	HANDLE	hFile			= NULL;
	BOOL	bFileRet		= FALSE;
	DWORD	dwWritten		= 0;
	_stprintf_s(szFilePath, MAX_PATH, _T("%s\\%020I64d.%s"), g_pDataCacheContext->m_szWorkPath, llDataId, DATA_CACHE_FILE_EXT);

	hFile	= CreateFile(szFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString(_T("DC_AddData 打开文件失败\n"));
		goto ErrLine;
	}
	bFileRet	= WriteFile(hFile, pCache, nCache, &dwWritten, NULL);
	if(!bFileRet || nCache != dwWritten)
	{
		OutputDebugString(_T("DC_AddData 写入文件数据异常\n"));
		goto ErrLine;
	}

	//添加到缓存数据映射
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