#include "VideoProc.h"
#include "ParamProc.h"
//#include "videoInput.h"
#include "Common/StopWatch.h"
#include "LicensePlate/LicenseProc.h"

#include <objbase.h>
#include <opencv2/opencv.hpp>

//static videoInput g_videoInput;
static std::deque<std::string> split_ip(const std::string& strIP);

int	CVideoProc::m_nChnlNum = 0;

CVideoProc::CVideoProc(int channel_id, int channel_num)
:
m_nVideoWidth(0),
m_nVideoHeight(0),
m_nChnlID(channel_id)
{
	m_nChnlNum = channel_num;
	memset(m_nChnlName, 0, sizeof(char) * 100);
}


CVideoProc::~CVideoProc()
{
}


void CVideoProc::run()
{
	LOGFMTA("视频线程启动成功, 线程[%lld], 信号量[%X], 对象[%X]", _hThreadID, &_hSemHelper, this);

	cv::VideoCapture  capture;
	cv::Mat FrameData, FrameData_;

	int		w = 0, h = 0;
	int		urlIndex = CV_CAP_DSHOW;
	char	urlDest[MAX_PATH] = { 0 };

	ParamChnlType chnlType = { 0 };
	CParamProc::Instance()->GetParamChnl(m_nChnlID, chnlType);
	strcpy_s(m_nChnlName, chnlType.m_chChannelName);
	int urlType = check(chnlType.m_chVideoStreamURL, urlDest, UNKNOW_URL);
	if (urlType == ERR_URL)
	{
		LOGFMTA("URL[%s] 参数错误. Usage : ", chnlType.m_chVideoStreamURL);
		LOGFMTA("\tfile:///C:/video.avi\n\tVideoStreamURL rtsp://192.168.110.207:554\n\tIf you use RTSP_URL, make sure the username and password are correct. Otherwise, please ignore\n");

		return;
	}

	if (urlType == RTSP_URL)
	{
		std::deque<std::string> deqip = split_ip(std::string(chnlType.m_chVideoStreamURL));
		sprintf_s(urlDest, MAX_PATH, "rtsp://%s:%s@%s.%s.%s.%s", chnlType.m_chUserName, chnlType.m_chPassWord, deqip[0].c_str(), deqip[1].c_str(), deqip[2].c_str(), deqip[3].c_str());
	}
	else if (urlType == FILE_URL)
	{
		sprintf_s(urlDest, MAX_PATH, chnlType.m_chVideoStreamURL + 8);
	}

	//跳帧处理
	int nSkipFrame = 0;
	switch (m_nChnlNum)
	{
	case 0 :
	case 1 :
		break;
	case 2 :
		nSkipFrame = 2;
		break;
	case 3 :
		nSkipFrame = 3;
		break;
	case 4 :
		nSkipFrame = 4;
		break;
	case 5 :
		nSkipFrame = 5;
		break;
	case 6 :
		nSkipFrame = 6;
		break;
	case 7 :
		nSkipFrame = 7;
		break;
	case 8 :
		nSkipFrame = 8;
		break;
	default:
		LOGFMTA("不支持%d路视频", m_nChnlNum);
		return;
	}

	CStopWatch stopwatch;
	int	nTimeval = 0;
	int nWaitTime = 0;
	unsigned __int64 u64FrameID = 0;
	while (1)
	{
		if (true == wait(nWaitTime))
			break;

		stopwatch.Start();
		if (!capture.isOpened())
		{
			if (urlType == CAMERA_URL)
				capture.open(CV_CAP_DSHOW);
			else
				capture.open(urlDest);
			
			if (urlType == FILE_URL)
				nTimeval = 1000 / capture.get(CV_CAP_PROP_FPS);

			w = capture.get(CV_CAP_PROP_FRAME_WIDTH);
			h = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
			if (w <= 0 || h <= 0)
			{
				w = 800;
				h = 600;
			}
			//capture.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
			//capture.set(CV_CAP_PROP_FPS, CParamProc::m_nFps);
			//capture.set(CV_CAP_PROP_FRAME_WIDTH, CParamProc::m_nCamWidth);
			//capture.set(CV_CAP_PROP_FRAME_HEIGHT, CParamProc::m_nCamHeight);
		}

		++u64FrameID;
		if (u64FrameID >= _UI64_MAX - 10)
			u64FrameID = 0;
		capture >> FrameData;
		if (FrameData.empty())
		{
			if (urlType == FILE_URL)
			{
				capture.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
				LOGFMTE("通道[%d] 视频url[%s]播放结束, 进行循环播放...\n", m_nChnlID, urlDest);
			}

			continue;
		}

		if (u64FrameID % (nSkipFrame + 1) == 0)
		{
			//cv::resize(FrameData, FrameData_, cv::Size(800, 800 * h / w ));
			CLicenseProc::Instance()->push_back(new pushType(FrameData.clone(), m_nChnlID, m_nChnlName));
		}

		//播放视频文件时, 设置时间间隔, 以确保帧率正常
		if (urlType == FILE_URL)
		{
			nWaitTime = nTimeval - stopwatch.Now();
			if (nWaitTime < 0) nWaitTime = 0;
		}
	}


	LOGFMTA("视频线程正常结束, 线程[%lld], 信号量[%X], 对象[%X]", _hThreadID, &_hSemHelper, this);
	return;
}


int	CVideoProc::init(Func_Frame pFunc_Frame, void* pCtx)
{
	m_pFuncFrame	= pFunc_Frame;
	m_pContext		= pCtx;
	
	//videoInput::setComMultiThreaded(true);

	return 0;
}

int CVideoProc::check(char* url, char* path, int type)
{
	if (type == CAMERA_URL)
		return CAMERA_URL;

	if (strlen(url) <= 8 || strlen(url) >= MAX_PATH)
		return ERR_URL;

	char urlDest[MAX_PATH] = {0};

	memset((void *)urlDest, 0, sizeof(char) * MAX_PATH);
	strncpy_s(urlDest, 8, url, 7);
	if (strcmp(urlDest, "rtsp://") == 0)
		return RTSP_URL;

	memset((void *)urlDest, 0, sizeof(char) * MAX_PATH);
	strncpy_s(urlDest, 9, url, 8);
	if (strcmp(urlDest, "file:///") != 0)
		return ERR_URL;

	memset((void *)urlDest, 0, sizeof(char) * MAX_PATH);
	strcpy_s(path, strlen(url) - 7, url + 8);
	FILE* fp = NULL;
	errno_t err = fopen_s(&fp, path, "r");
	if (fp == NULL || err != 0)
		return ERR_URL;
	fclose(fp);

	return FILE_URL;
}


void CVideoProc::pause()
{
}


void CVideoProc::resume()
{
}


std::deque<std::string> split_ip(const std::string& strIP)
{
	std::deque<std::string> deqIP;

	std::string::size_type beg = 7;
	std::string::size_type pos = strIP.find('.');

	while (pos != std::string::npos)
	{
		deqIP.push_back(strIP.substr(beg, pos - beg));
		beg = pos + 1;
		pos = strIP.find('.', beg);
	}

	std::string last;
	for (int i = 0; i < 3; ++i)
	{
		pos = beg + i;
		if (pos >= strIP.length())
			break;

		char c = strIP.at(beg + i);

		if (!isdigit(c))
			break;

		last.push_back(c);
	}
	deqIP.push_back(last);

	return deqIP;
}