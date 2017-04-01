
#include "Common/StopWatch.h"
#include "NetClient/NetClient.h"
#include "LicensePlate/LicenseProc.h"
#include "common/Common.h"
#include "ParamProc.h"
#include "VideoProc.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <opencv2/opencv.hpp>

#include "LicensePlateUnit.h"

#include "tlhelp32.h"
#include "Psapi.h"

#pragma comment(lib, "Psapi.lib") 


typedef struct __videoType
{
	CVideoProc*		video;
	int				chnlId;
}videoType;


static std::vector<videoType> g_vecVideoData;

void __stdcall	func_Video(void* pFrame, long* pLen, const char* szErrMsg, int nRet, void* pCtx);
int  __stdcall	getLPMID();

static int		g_nCurSkip = 0;
static __int64  g_i64FaceIdx = 0;

CLicensePlateUnit::CLicensePlateUnit()
{
}


CLicensePlateUnit::~CLicensePlateUnit()
{
}

int 
CLicensePlateUnit::Start()
{
	LOGFMTA("服务程序开始启动, 线程[%lld], 信号量[%X], 对象[%X]", _hThreadID, _hSemHelper, this);
	try
	{
		CParamProc::Instance()->init(m_vecChnl);
		CNetClient::Instance()->init();
		CLicenseProc::Instance()->init();


		for (int i = 0; i < m_vecChnl.size(); ++i)
		{
			videoType videoData = { 0 };
			videoData.chnlId = m_vecChnl[i];
			videoData.video = new CVideoProc(videoData.chnlId, m_vecChnl.size());
			videoData.video->start();

			g_vecVideoData.push_back(videoData);
		}

		CNetClient::Instance()->start();
		CLicenseProc::Instance()->start();
	}
	catch (const CIsException& e)
	{
		LOGFMTD("CLicensePlateUnit::Start 服务程序启动异常, 异常信息[%s], 异常代码[%d]", 
			e.errorInfo(), e.errorCode());
		return 0;
	}
	LOGFMTA("服务程序启动成功, 线程[%lld], 信号量[%X], 对象[%X]", _hThreadID, _hSemHelper, this);

	return getLPMID();
}


void
CLicensePlateUnit::Stop()
{
	try
	{
		for (int i = 0; i < g_vecVideoData.size(); ++i)
		{
			videoType videoData = g_vecVideoData[i];
			videoData.video->stop();
			delete videoData.video;
			videoData.video = NULL;
		}
		g_vecVideoData.clear();

		CLicenseProc::Instance()->stop();
		CNetClient::Instance()->stop();
	}
	catch (...)
	{
		LOGFMTD("CLicensePlateUnit::Stop 服务程序退出异常");
		return;
	}
	LOGFMTA("服务程序已正常退出, 线程[%lld], 信号量[%X], 对象[%X]", _hThreadID, &_hSemHelper, this);
}


void
CLicensePlateUnit::run()
{
	bool	bExit = false;
	LOGFMTA("主业务线程启动成功, 线程[%lld], 信号量[%X], 对象[%X]", _hThreadID, &_hSemHelper, this);

	while(1)
	{
		if(true == wait(1000) || bExit == true)
			break;

		m_Lock.lock();
		//...
		m_Lock.unLock();
	}

	LOGFMTA("主业务线程已正常退出, 线程[%lld], 信号量[%X], 对象[%X]", _hThreadID, &_hSemHelper, this);
	return;
}


void __stdcall func_Video(void* pCVFrame, long* nLen, const char* szErrMsg, int nRet, void* pCtx)
{
	CLicensePlateUnit* pMgr = static_cast<CLicensePlateUnit*>(pCtx);
}


int  __stdcall	getLPMID()
{
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapShot == 0 || hSnapShot == INVALID_HANDLE_VALUE)
		return -1;
	
	BOOL bRet = Process32First(hSnapShot, &pe32);
	while (bRet)
	{
		if (_tcsicmp(pe32.szExeFile, _T("IS_LicensePlateMgr.exe")) == 0)
			return pe32.th32ProcessID;
		bRet = Process32Next(hSnapShot, &pe32);
	}

	CloseHandle(hSnapShot);
	return -1;
}