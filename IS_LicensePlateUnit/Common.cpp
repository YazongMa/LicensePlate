#include <time.h>
#include <io.h>
#include <direct.h>

#include "Common/Common.h"
//日志	
#define LOG_FILE_MAX_SIZE					(1024 * 1024 * 100)
#define LOG_INFO_MAX_SIZE					(1024 * 24)


void __stdcall GetPPUPath(TCHAR* pBuf, DWORD dwBufSize)
{
	GetExePath(pBuf, dwBufSize);
	_tcscat(pBuf, _T("\\..\\IS_PrePrecessUnit\\"));
}

void __stdcall GetExePath(TCHAR* pBuf, DWORD dwBufSize)
{
	HMODULE				hDll;
	TCHAR				szPath[MAX_PATH];
	TCHAR*				p;

	//获得EXE句柄
	hDll = GetModuleHandle(NULL);
	GetModuleFileName(hDll, szPath, MAX_PATH);
	p = _tcsrchr(szPath, _T('\\'));
	*p = _T('\0');
	_tcscpy_s(pBuf, dwBufSize, szPath);
}


void __cdecl WriteLogInfo(const TCHAR * pszFileName, const TCHAR * pszLogInfo, ...)
{
	TCHAR		szLogInfoTmp[LOG_INFO_MAX_SIZE];
	TCHAR		szLogInfoFull[LOG_INFO_MAX_SIZE + 64];
	TCHAR		szPath[MAX_PATH];
	TCHAR		szFullPathName[MAX_PATH];
	TCHAR		szLogFolder[MAX_PATH];
	TCHAR		szLogDateFolder[MAX_PATH];
	TCHAR		szDate[16];
	TCHAR		szTime[16];
	SYSTEMTIME	sysTime;
	va_list		args;
	HANDLE		hFile = NULL;
	DWORD		dwFileSize = 0;
	int			nLoop = 1;
	DWORD		dwBytesWritten = 0;
	TCHAR		pszFileNameTmp[MAX_PATH];

	//如果日志信息长度超过了限制，直接返回
	if (pszFileName == NULL || pszFileName[0] == _T('\0') || pszLogInfo == NULL || pszLogInfo[0] == _T('\0')
		|| _tcslen(pszLogInfo) >= LOG_INFO_MAX_SIZE)
		return;

	//加上时间戳
	_tstrdate_s(szDate, 16);
	_tstrtime_s(szTime, 16);

	va_start(args, pszLogInfo);
	_vstprintf_s(szLogInfoTmp, LOG_INFO_MAX_SIZE, pszLogInfo, args);
	va_end(args);

	_stprintf_s(szLogInfoFull, LOG_INFO_MAX_SIZE + 64, _T("%s %s %s\r\n"), szDate, szTime, szLogInfoTmp);


	GetPPUPath(szPath, MAX_PATH);
	_stprintf_s(szLogFolder, MAX_PATH, _T("%s\\log"), szPath);

	GetLocalTime(&sysTime);
	if (_access(szLogFolder, 0))
	{
		_mkdir(szLogFolder);
	}
	_stprintf_s(szLogDateFolder, MAX_PATH, _T("%s\\%04d%02d%02d"), szLogFolder, sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	if (_access(szLogDateFolder, 0))
	{
		_mkdir(szLogDateFolder);
	}

	//确定日志文件路径 需要判断日志长度是否超出限制 若超出则写下一个
	strcpy_s(pszFileNameTmp, MAX_PATH, pszFileName);
	while (1)
	{
		_stprintf_s(szFullPathName, MAX_PATH, _T("%s\\%s.txt"), szLogDateFolder, pszFileNameTmp);

		hFile = CreateFile(szFullPathName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return;
		}
		dwFileSize = GetFileSize(hFile, NULL);
		if (dwFileSize >= LOG_FILE_MAX_SIZE)
		{
			CloseHandle(hFile);

			_stprintf_s(pszFileNameTmp, MAX_PATH, _T("%s_%d"), pszFileName, nLoop++);
		}
		else
		{
			break;
		}
	}

	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, szLogInfoFull, (DWORD)_tcslen(szLogInfoFull) * sizeof(TCHAR), &dwBytesWritten, NULL);
	CloseHandle(hFile);

	//printf("%s\n", szLogInfoFull);
}
