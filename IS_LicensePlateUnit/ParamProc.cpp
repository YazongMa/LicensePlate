#include "ParamProc.h"
#include "Common/Common.h"

#include <io.h>
#include <assert.h>

static int	g_nChnlID = 0;
static long g_lClusterID = 0;
static long g_lClusterInterval = 0;


CParamProc* CParamProc::Instance()
{
	static CParamProc param;
	return &param;
}

CParamProc::CParamProc()
{
};

CParamProc::~CParamProc()
{
};

bool CParamProc::init(std::vector<int>& chnls)
{
	if (!GetParamMgr())
		throw CIsException(-1, "调用GetParamMgr失败");
	

	TCHAR szFile[MAX_PATH] = { 0 };
	TCHAR szFind[MAX_PATH] = { 0 };
	TCHAR szCfgPath[MAX_PATH] = { 0 };
	WIN32_FIND_DATA FindFileData = { 0 };
	GetPPUPath(szCfgPath, MAX_PATH);
	_stprintf_s(szCfgPath, MAX_PATH, _T("%s\\config"), szCfgPath);
	_stprintf_s(szFind, MAX_PATH, _T("%s\\0*.ini"), szCfgPath);
	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);

	int chnl_count = 0;
	while (1)
	{
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			memset((void *)szFile, 0, sizeof(TCHAR) * MAX_PATH);
			_stprintf_s(szFile, MAX_PATH, _T("%s\\%s"), szCfgPath, FindFileData.cFileName);
			if (_access(szFile, 0))
			{
				if (!FindNextFile(hFind, &FindFileData))
				{
					break;
				}
				else
				{
					continue;
				}
			}

			TCHAR szChnlID[32] = { 0 };
			memset((void *)szChnlID, 0, sizeof(TCHAR) * 32);
			memcpy_s(szChnlID, 32, FindFileData.cFileName, 3);
			GetParamChnl(szChnlID);
			
			chnls.push_back(atoi(szChnlID));
		}

		if (!FindNextFile(hFind, &FindFileData))
		{
			break;
		}
	}
	FindClose(hFind);

	return true;
}


bool CParamProc::GetParamMgr(ParamMgrType& mgrType)
{
	memcpy((void *)&mgrType, (void *)&m_ParamMgr, sizeof(ParamMgrType));
	return true;
}


bool CParamProc::GetParamChnl(int nChnlID, ParamChnlType& chnlType)
{
	std::map<int, ParamChnlType>::iterator it = m_mParamChnl.find(nChnlID);
	if (it == m_mParamChnl.end())
		return false;

	memcpy((void *)&chnlType, (void *)&it->second, sizeof(ParamChnlType));
	return true;
}



bool CParamProc::GetParamMgr()
{
	TCHAR szExePath[MAX_PATH] = {0};
	TCHAR szCfgFile[MAX_PATH] = {0};
	GetPPUPath(szExePath, MAX_PATH);
	_stprintf_s(szCfgFile, MAX_PATH, _T("%s\\config\\config.ini"), szExePath);

	if(_access(szCfgFile, 0) == -1)
	{
		return false;
	}

	TCHAR	chPhysicalId[64] = {0};			//前置处理器物理标识
	TCHAR	chPhysicalName[100] = {0};		//前置处理器名称
	TCHAR	chPhysicalIP[20] = {0};			//前置IP地址

	TCHAR	chDMServerIP[100] = {0};		//DM服务器IP
	TCHAR	chDMServerPort[100] = {0};		//DM服务器Port
	TCHAR	chRMQServerIP[100] = {0};		//RMQServerIP
	TCHAR	chRMQServerPort[100] = {0};		//RMQServerPort

	TCHAR	chIsNetDog[100] = {0};			//是否用网络狗
	TCHAR	chNetDogIP[100] = {0};			//网络狗IP
	TCHAR	chMachineCode[100] = {0};		//机器码
	TCHAR	chAlgorithmSN[100] = {0};		//序列号
	TCHAR	chAlgorithmLicense[100] = {0};	//许可号

	TCHAR	chChannelSum[100] = {0};		//通道总数
	TCHAR	chCurChannelID[100] = {0};		//待操作通道号		

	GetPrivateProfileString(_T("PPU_MGR"), _T("PhysicalId"),		DEFAULT_PysicalId,			chPhysicalId,		64,	 szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("PhysicalName"),		DEFAULT_PreName,			chPhysicalName,		100, szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("PhysicalIP"),		DEFAULT_IPADDR,				chPhysicalIP,		100, szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("DMServerIP"),		DEFAULT_ServerIP,			chDMServerIP,		100, szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("DMServerPort"),		DEFAULT_ServerPort,			chDMServerPort,		100, szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("RMQServerIP"),		DEFAULT_RMQServerIP,		chRMQServerIP,		100, szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("RMQServerPort"),		DEFAULT_RMQServerPort,		chRMQServerPort,	100, szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("IsNetDog"),			DEFAULT_IsNetDog,			chIsNetDog,			100, szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("NetDogIP"),			DEFAULT_NetDogIP,			chNetDogIP,			100, szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("MachineCode"),		DEFAULT_MachineCode,		chMachineCode,		100, szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("AlgorithmSN"),		DEFAULT_AlgorithmSN,		chAlgorithmSN,		100, szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("AlgorithmLicense"),	DEFAULT_AlgorithmLicense,	chAlgorithmLicense, 100, szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("ChannelSum"),		DEFAULT_ChannelSum,			chChannelSum,		100, szCfgFile);
	GetPrivateProfileString(_T("PPU_MGR"), _T("CurChannelID"),		DEFAULT_CurChannelID,		chCurChannelID,		100, szCfgFile);

	strcpy_s(m_ParamMgr.m_chPhysicalId,			100, chPhysicalId);
	strcpy_s(m_ParamMgr.m_chPhysicalName,		100, chPhysicalName);
	strcpy_s(m_ParamMgr.m_chPhysicalIP,			20, chPhysicalIP);

	strcpy_s(m_ParamMgr.m_chDMServerIP,			100, chDMServerIP);
	m_ParamMgr.m_nDMServerPort = atoi(chDMServerPort);

	strcpy_s(m_ParamMgr.m_chRMQServerIP,			100,	chRMQServerIP);
	m_ParamMgr.m_nRMQServerPort = atoi(chRMQServerPort);

	m_ParamMgr.m_bIsNetDog = (atoi(chIsNetDog) == 0 ? false : true);
	strcpy_s(m_ParamMgr.m_chNetDogIP,			100, chNetDogIP);
	strcpy_s(m_ParamMgr.m_chMachineCode,			100, chMachineCode);
	strcpy_s(m_ParamMgr.m_chAlgorithmSN,			100, chAlgorithmSN);
	strcpy_s(m_ParamMgr.m_chAlgorithmLicense,	100, chAlgorithmLicense);

	m_ParamMgr.m_nChannelSum = atoi(chChannelSum);
	strcpy_s(m_ParamMgr.m_chCurChannelID,		100, chCurChannelID);
	m_ParamMgr.m_nPPID = GetPrivateProfileInt(_T("PPU_MGR"), _T("ParentProcID"), 0, szCfgFile);

	return true;
}

bool CParamProc::GetParamChnl(TCHAR* szChnlID)
{
	ParamChnlType chnlType = { 0 };
	TCHAR szExePath[MAX_PATH] = {0};
	TCHAR szCfgFile[MAX_PATH] = {0};
	GetPPUPath(szExePath, MAX_PATH);
	_stprintf_s(szCfgFile, MAX_PATH, _T("%s\\config\\%s.ini"), szExePath, szChnlID);

	if(_access(szCfgFile, 0) == -1)
	{
		return false;
	}

	TCHAR chStartFlag[100]			=	{0};
	TCHAR chChannelID[100]          =	{0};
	TCHAR chChannelName[100]        =	{0};
	TCHAR chUserName[100]           =	{0};
	TCHAR chPassWord[100]           =	{0};
	TCHAR chVideoStreamURL[300]     =	{0};
	TCHAR chTopLeftX[100]           =	{0};
	TCHAR chTopLeftY[100]           =	{0};
	TCHAR chWidth[100]              =	{0};
	TCHAR chHeight[100]             =	{0};
	TCHAR chFaceMinSize[100]        =	{0};
	TCHAR chFaceMaxSize[100]        =	{0};
	TCHAR chClusterFaceMaxNum[100]  =	{0};
	TCHAR chUpLoadFaceMaxNum[100]   =	{0};
	TCHAR chAngle[100]              =	{0};
	TCHAR chDetectInterval[100]     =	{0};
	TCHAR chDetectFaceMaxNum[100]   =	{0};
	TCHAR chFaceCacheMaxNum[100]    =	{0};
	TCHAR chDetectThreadNum[100]    =	{0};
	TCHAR chLocalStoreFlag[100]     =	{0};
	TCHAR chScaleWidth[100]			=	{0};
	TCHAR chScaleHeight[100]	    =	{0};
	TCHAR chGPUFlag[100]		    =	{0};
	TCHAR chImageQuality[100]	    =	{0};
	TCHAR chUpLoadInterVal[100]	    =	{0};

	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("StartFlag")        , DEFAULT_IsProcess,					chStartFlag        	 , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("ChannelID")        , DEFAULT_IPCId,						chChannelID          , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("ChannelName")      , DEFAULT_Name,						chChannelName        , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("UserName")         , DEFAULT_LoginUsername, 			chUserName           , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("PassWord")         , DEFAULT_LoginPassword, 			chPassWord           , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("VideoStreamURL")   , DEFAULT_VideoStreamURI,			chVideoStreamURL     , 300, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("TopLeftX")         , DEFAULT_XPosition,					chTopLeftX           , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("TopLeftY")         , DEFAULT_YPosition,					chTopLeftY           , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("Width")            , DEFAULT_Width,						chWidth              , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("Height")           , DEFAULT_Height,					chHeight             , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("FaceMinSize")      , DEFAULT_FaceMinSize,				chFaceMinSize        , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("FaceMaxSize")      , DEFAULT_FaceMaxSize,				chFaceMaxSize        , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("ClusterFaceMaxNum"), DEFAULT_ClusterFaceMaxNum,			chClusterFaceMaxNum  , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("UpLoadFaceMaxNum") , DEFAULT_SelectFaces, 				chUpLoadFaceMaxNum   , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("Angle")            , DEFAULT_Angle, 					chAngle              , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("DetectInterval")   , DEFAULT_DetectInterval,			chDetectInterval     , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("DetectFaceMaxNum") , DEFAULT_DetectFaceMaxNum, 			chDetectFaceMaxNum   , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("FaceCacheMaxNum")  , DEFAULT_MaxFaceDataCacheNum, 		chFaceCacheMaxNum    , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("DetectThreadNum")  , DEFAULT_EvaluationProcThraedNum,	chDetectThreadNum    , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("LocalStoreFlag")   , DEFAULT_IsStorageLocal,			chLocalStoreFlag     , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("ScaleWidth")	   , DEFAULT_ScaleWidth,				chScaleWidth		 , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("ScaleHeight")      , DEFAULT_ScaleHeight,		 		chScaleHeight		 , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("GPUFlag")          , DEFAULT_GPUFlag,					chGPUFlag			 , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("ImageQuality")     , DEFAULT_ImageQuality,				chImageQuality		 , 100, szCfgFile);
	GetPrivateProfileString(_T("CHANNEL_CTRL"), _T("UploadInterVal")   , DEFAULT_UploadInterVal,			chUpLoadInterVal	 , 100, szCfgFile);

	chnlType.m_bStartFlag = (atoi(chStartFlag) == 1 ? true : false);
	strcpy_s(chnlType.m_chChannelID        ,100,	chChannelID);
	strcpy_s(chnlType.m_chChannelName      ,100,	chChannelName);
	strcpy_s(chnlType.m_chUserName         ,100,	chUserName);
	strcpy_s(chnlType.m_chPassWord         ,100,	chPassWord);
	strcpy_s(chnlType.m_chVideoStreamURL   ,300,	chVideoStreamURL);
	chnlType.m_nTopLeftX          			 = atoi(chTopLeftX);
	chnlType.m_nTopLeftY          			 = atoi(chTopLeftY);
	chnlType.m_nWidth             			 = atoi(chWidth);
	chnlType.m_nHeight            			 = atoi(chHeight);
	chnlType.m_nFaceMinSize       			 = atoi(chFaceMinSize);
	chnlType.m_nFaceMaxSize       			 = atoi(chFaceMaxSize);
	chnlType.m_nClusterFaceMaxNum 			 = atoi(chClusterFaceMaxNum);
	chnlType.m_nUpLoadFaceMaxNum  			 = atoi(chUpLoadFaceMaxNum);
	chnlType.m_fAngle             			 = static_cast<float>(atof(chAngle));
	chnlType.m_nDetectInterval    			 = atoi(chDetectInterval);
	chnlType.m_nDetectFaceMaxNum  			 = atoi(chDetectFaceMaxNum);
	chnlType.m_nFaceCacheMaxNum   			 = atoi(chFaceCacheMaxNum);
	chnlType.m_nDetectThreadNum   			 = atoi(chDetectThreadNum);
	chnlType.m_bLocalStoreFlag    			 = (atoi(chLocalStoreFlag) == 1 ? true : false);
	chnlType.m_nScaleWidth					 = atoi(chScaleWidth);
	chnlType.m_nScaleHeight					 = atoi(chScaleHeight);
	chnlType.m_nImageQuality				 = atoi(chImageQuality);
	chnlType.m_bGPUFlag    					 = (atoi(chGPUFlag) == 1 ? true : false);
	chnlType.m_nUpLoadInterVal				 = atoi(chUpLoadInterVal);
	chnlType.m_nFrameCacheSize				 = GetPrivateProfileInt(_T("CHANNEL_CTRL"), _T("FrameCacheSize"), 100, szCfgFile);
	chnlType.m_nPriorityFlag				 = GetPrivateProfileInt(_T("CHANNEL_CTRL"), _T("PriorityFlag"),		0, szCfgFile);
	chnlType.m_nEyesDist					 = GetPrivateProfileInt(_T("CHANNEL_CTRL"), _T("EyesDist"),		   30, szCfgFile);	
	chnlType.m_nVideoType					 = GetPrivateProfileInt(_T("CHANNEL_CTRL"), _T("VideoType"),		3, szCfgFile);
	
	m_mParamChnl[atoi(szChnlID)] = chnlType;

	return true;
}

