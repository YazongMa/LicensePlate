#include "NetClient.h"
#include "ParamProc.h"
#include "DataCache.h"

#include "LicensePlate/LPKernelEx.h"
#include "Common/Common.h"

#include <iostream>
#include <ctime>
#include <Iphlpapi.h>
#include <direct.h>
#include <io.h>
#include <deque>

using namespace std;

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "WS2_32.lib")



HMODULE					CNetClient::m_hSendNode				= NULL;
func_RMQSC_Init			CNetClient::m_func_RMQSC_Init		= NULL;
func_RMQSC_Uninit		CNetClient::m_func_RMQSC_Uninit		= NULL;
func_RMQSC_SendData		CNetClient::m_func_RMQSC_SendData	= NULL;
func_RMQSC_TestConn		CNetClient::m_func_RMQSC_TestConn	= NULL;
	
static	int		g_nLoadFlag		= 0;
static	int		g_nCacheClean	= 0;
static	bool	g_bLocalStore	= false;

static  std::deque<uchar *>		g_dPlateData;
static	LockHelper				g_cDataLock;

CNetClient* CNetClient::Instance()
{
	static CNetClient netclient;
	return &netclient;
}


CNetClient::CNetClient()
:
m_nSendID(1)
{
	memset((void *)m_chDeviceID, 0, sizeof(char) * DEVICEID_SIZE);

#ifdef _DEBUG
	m_nRmqLogLevel = RMQSC_DEBUG_MODE;
#else
	m_nRmqLogLevel = RMQSC_WARNING_MODE;
#endif
}

CNetClient::~CNetClient()
{
	memset((void *)m_chDeviceID, 0, sizeof(char) * DEVICEID_SIZE);
}


void CNetClient::run()
{
	LOGFMTA("网络传输线程开始启动, 线程[%lld], 信号量[%X], 对象[%X]", _hThreadID, &_hSemHelper, this);
	while (1)
	{
		if (true == wait(1))
			break;
		g_cDataLock.lock();
		if (g_dPlateData.empty())
		{
			g_cDataLock.unLock();
			continue;
		}
		uchar* vp_data = g_dPlateData.front();
		g_dPlateData.pop_front();
		g_cDataLock.unLock();

		if (!vp_data) 
			continue;

		// 数据组包处理
		int size = ((lpu_upload_package*)vp_data)->m_nPackageSize;
		int nRet = -1;
		if ((nRet = m_func_RMQSC_SendData(vp_data, size)) != RMQSC_OK)
		{
			LOGFMTT("CNetClient::run RMQSC_SendData failed, nRet = %d", nRet);
		}

		IS_DELETE_ARRAY(vp_data);
	}
	LOGFMTA("网络传输线程已正常退出, 线程[%lld], 信号量[%X], 对象[%X]", _hThreadID, &_hSemHelper, this);
}

bool CNetClient::init()
{
	TCHAR	szDll[MAX_PATH];

	ParamMgrType paramMgr = {0};
	CParamProc::Instance()->GetParamMgr(paramMgr);
	if (g_nLoadFlag == 1)
		return true;

	GetExePath(m_chExePath, MAX_PATH);
	_stprintf_s(szDll, MAX_PATH, _T("%s\\rabbitmq_send_client.dll"), m_chExePath);
	m_hSendNode	= LoadLibrary(szDll);
	if(m_hSendNode == NULL)
	{
		throw CIsException(GetLastError(), "LoadLibrary rabbitmq_send_client.dll失败");
	}

	m_func_RMQSC_Init		= (func_RMQSC_Init)		GetProcAddress(m_hSendNode, "RMQSC_Init");
	m_func_RMQSC_Uninit		= (func_RMQSC_Uninit)	GetProcAddress(m_hSendNode, "RMQSC_Uninit");
	m_func_RMQSC_SendData	= (func_RMQSC_SendData)	GetProcAddress(m_hSendNode, "RMQSC_SendData");
	m_func_RMQSC_TestConn	= (func_RMQSC_TestConn)	GetProcAddress(m_hSendNode, "RMQSC_TestConnection");

	if (m_func_RMQSC_Init == NULL || m_func_RMQSC_Uninit == NULL || m_func_RMQSC_TestConn == NULL || m_func_RMQSC_SendData == NULL)
		throw CIsException(GetLastError(), "GetProcAddress rabbitmq_send_client.dll失败");

	g_nLoadFlag = 1;

	strcpy_s(m_chDeviceID, DEVICEID_SIZE, paramMgr.m_chPhysicalId);

	RMQSendServerConfig svr_cfg = { 0 };
	RMQSendInitConfig snd_cfg = { 0 };

	svr_cfg.nPort = paramMgr.m_nRMQServerPort;
	strcpy_s(svr_cfg.szIP, paramMgr.m_chRMQServerIP);
	strcpy_s(svr_cfg.szHostname, "admin");
	strcpy_s(svr_cfg.szHostpwd, "admin");
	strcpy_s(svr_cfg.szExchangeName, "licensePlateExchange");
	strcpy_s(svr_cfg.szRoutingName, "licensePlateRouting");
	memcpy((void *)&snd_cfg.rmqServer[0], (void *)&svr_cfg, sizeof(svr_cfg));
	snd_cfg.nServerCount = 1;
	snd_cfg.bBroadcast = 0;
	snd_cfg.bDurable = 0;

	int	nRet = m_func_RMQSC_Init(&snd_cfg, m_nRmqLogLevel);
	if (nRet != RMQSC_OK)
		throw CIsException(nRet, "调用RMQSC_Init失败");
	
	LOGFMTT("初始化RabbitMQ成功");

	return true;
}

bool CNetClient::uninit()
{
	if (g_nLoadFlag == 0)
		return true;

	m_func_RMQSC_Uninit();

	FreeLibrary(m_hSendNode);
	m_hSendNode				= NULL;
	m_func_RMQSC_Init		= NULL;
	m_func_RMQSC_Uninit		= NULL;
	m_func_RMQSC_SendData	= NULL;
	m_func_RMQSC_TestConn	= NULL;

	g_nLoadFlag = 0;
	
	return true;
}


//data: 场景jpg格式图像数据
//rslt: 车牌信息
void CNetClient::push(uchar* const data)
{
	g_cDataLock.lock();
	g_dPlateData.push_back(data);
	g_cDataLock.unLock();
}



bool CNetClient::SendCmdUploadFaceEx(void* pPerfectFace, int nNum)
{
	bool	bRet		= true;
	BYTE*	pSendBuf	= NULL;
	int		nPkgSize	= sizeof(lpu_upload_package);
	int		nPos		= sizeof(lpu_upload_package);
	int		nRet		= RMQSC_OK;

	//将发送缓存发送到rabbitmq server
	if(m_func_RMQSC_SendData)
	{
		nRet = m_func_RMQSC_SendData(pSendBuf, nPkgSize);
		if (nRet != RMQSC_OK)
		{
			//写入缓存
			DC_AddData(pSendBuf, nPkgSize);

			LOGFMTT("CNetClient::SendCmdUploadFaceEx func_RMQSC_SendData failed, nRet = %d", nRet);
			bRet = false;
		}
	}
	
	IS_DELETE_ARRAY(pSendBuf);
	return bRet;
}


long CNetClient::SendDataCacheFaceEx(long& lUploadSucceed, long& lUploadFailed)
{
	int		nRet			= RMQSC_OK;
	int		nDataCacheSize	= 0;
	BYTE*	pbyDataCache	= NULL;
	__int64 llDataId		= 0;

	long lCacheCnt = DC_GetCacheCnt();
	if(!m_func_RMQSC_SendData)
	{
		LOGFMTT("CNetClient::SendCmdUploadFaceFromDataCach 加载DLL后m_func_RMQSC_SendData为NULL");
		return -1;
	}

	while(1)
	{
		llDataId = DC_GetLastData((BYTE **)&pbyDataCache, &nDataCacheSize);
		if (0 >= llDataId)
			break;
		if(!g_nCacheClean)
		{
			DC_DeleteData(llDataId, RMQSC_OK);
			continue;
		}

		nRet = m_func_RMQSC_SendData(pbyDataCache, nDataCacheSize);
		if (RMQSC_OK != nRet)
		{
			LOGFMTT("CNetClient::SendCmdUploadFaceFromDataCach 发送缓存失败 DataId：%d ", llDataId);
		}

		if(!DC_DeleteData(llDataId, nRet))
		{
			LOGFMTT("CNetClient::SendCmdUploadFaceFromDataCach 释放缓存失败 DataId：%d nRet[%d]", llDataId, nRet);
		}

		InterlockedIncrement(&lUploadSucceed);
		InterlockedDecrement(&lUploadFailed);
	}

	g_nCacheClean = 1;

	return lCacheCnt;
}
