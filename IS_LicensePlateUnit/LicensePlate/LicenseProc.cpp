#include "LicensePlate/LicenseProc.h"
#include "LicensePlate/LPKernelEx.h"
#include "LicensePlate/VzPlayerLib.h"
#include "NetClient/NetClient.h"
#include "ParamProc.h"


#include <map>
#include <deque>
#include <opencv2/opencv.hpp>


typedef std::deque<pushType *>								deqMat_t;
typedef std::deque<pushType *>::iterator					deqMatItr_t;

typedef std::map<int, std::vector<std::string>>				mapPlate_t;
typedef std::map<int, std::vector<std::string>>::iterator	mapPlateItr_t;

static LockHelper	g_lockFrames;
static deqMat_t		g_deqFrames;
static mapPlate_t	g_mapPlate;
static char			g_szPysicalID[64] = {0};

static int			GetReliableResult(TH_PlateResult* pResult, int nPlateNum, int nChannel);
static void			plate_pkg(const std::vector<uchar>& scence, const std::vector<uchar>& plate, const TH_PlateResult& result, int id, char* name);

CLicenseProc* CLicenseProc::Instance()
{
	static CLicenseProc license;
	return &license;
}


CLicenseProc::CLicenseProc()
{
}


CLicenseProc::~CLicenseProc()
{
	LPR_UninitEx(1);
}


void CLicenseProc::init()
{
	ParamMgrType mgrData = { 0 };
	CParamProc::Instance()->GetParamMgr(mgrData);
	strcpy_s(g_szPysicalID, mgrData.m_chPhysicalId);

	BOOL Ret = 0;
	if ((Ret = LPR_SetImageFormat(FALSE, FALSE, ImageFormatBGR, FALSE, 60, 400, TRUE, FALSE, TRUE, 1) != 1))
		throw CIsException(Ret, "Call LPR_SetImageFormat Failed");
	if ((Ret = LPR_InitEx(1)) != 1)
		throw CIsException(Ret, "Call LPR_InitEx Failed");
}


void CLicenseProc::push_back(pushType* pushData)
{
	g_lockFrames.lock();
	if (g_deqFrames.size() >= 100)
	{
		pushType* data = g_deqFrames.front();
		g_deqFrames.pop_front();

		data->frame.release();
		IS_DELETE(data);
	}
	g_deqFrames.push_back(pushData);
	g_lockFrames.unLock();
}


void CLicenseProc::run()
{
	LOGFMTA("车牌检测线程启动成功, 线程[%lld], 信号量[%X], 对象[%X]", _hThreadID, &_hSemHelper, this);

	std::vector<uchar> lplate(512 * 1024);
	std::vector<uchar> scence(1024 * 1024);
	std::vector<int> params;
	params.push_back(cv::IMWRITE_JPEG_QUALITY);
	params.push_back(70);
	pushType* data = NULL;

	while (1)
	{
		if (true == wait(0))
			break;
		g_lockFrames.lock();
		if (g_deqFrames.empty())
		{
			g_lockFrames.unLock();
			continue;
		}
		data = g_deqFrames.front();
		g_deqFrames.pop_front();
		g_lockFrames.unLock();

		int nReliableRecoNum = 0, nRecogNum = 0, Ret = 0;
		TH_RECT range = { 0 };
		TH_PlateResult result[10] = { 0 };
		TH_PlateResultImage iresult_reliable[10] = {0};

		Ret = LPR_RGB888Ex(data->frame.data, data->frame.cols, data->frame.rows, result, nRecogNum, &range, 1);
		if (nRecogNum >= 2)
		{
			LOGFMTE("通道[%d] 当前帧车牌个数为[%d]", data->chnl_id, nRecogNum);
		}
		//Ret = LPR_GetReliableResult(iresult_reliable, nReliableRecoNum, 1);
		Ret = GetReliableResult(result, nRecogNum, data->chnl_id);

		//将场景压缩成jpg, 质量为70
		scence.clear();
		cv::imencode(".jpg", data->frame, scence, params);
		for (int j = 0; j < Ret; ++j)
		{
			if (result[j].reserved[0] == 1)
				continue;

			int& l = result[j].rcLocation.left;
			int& t = result[j].rcLocation.top;
			int& w = result[j].rcLocation.right;
			int& h = result[j].rcLocation.bottom;
			l = l > 0 ? l : 0;
			t = t > 0 ? t : 0;
			w = w < data->frame.cols ? w : data->frame.cols;
			h = h < data->frame.rows ? h : data->frame.rows;

			LOGFMTD("通道[%d] 车牌[%s] 可信度[%d], [%d,%d,%d,%d]", data->chnl_id, result[j].license, result[j].nConfidence, l, t, w - l, h - t);

			cv::Rect roi(l, t, w - l, h - t);
			if (0 <= roi.x && 0 <= roi.width && roi.x + roi.width <= data->frame.cols &&
				0 <= roi.y && 0 <= roi.height && roi.y + roi.height <= data->frame.rows)
			{
				lplate.clear();
				cv::imencode(".jpg", cv::Mat(data->frame, roi), lplate);

				plate_pkg(scence, lplate, result[j], data->chnl_id, data->chnl_name);
			}
			else
			{
				LOGFMTA("通道[%d] [%s][%s], Roi[%d,%d,%d,%d]错误, 当前数据未上传", data->chnl_id, result[j].license, result[j].color, l, t, w - l, h - t);
			}
		}

		data->frame.release();
		IS_DELETE(data);
	}

	g_lockFrames.lock();
	while (!g_deqFrames.empty())
	{
		data = g_deqFrames.front();
		g_deqFrames.pop_front();

		data->frame.release();
		IS_DELETE(data);
	}
	g_lockFrames.unLock();

	LOGFMTA("车牌检测线程正常结束, 线程[%lld], 信号量[%X], 对象[%X]", _hThreadID, &_hSemHelper, this);
}


int GetReliableResult(TH_PlateResult* pResult, int nPlateNum, int nChannel)
{
	if (nPlateNum <= 0)
		return 0;

	int num = nPlateNum;
	mapPlateItr_t it = g_mapPlate.find(nChannel);
	if (it == g_mapPlate.end())
	{
		std::vector<std::string> vecPlate;
		for (int i = 0; i < nPlateNum; ++i)
		{
			if (pResult[i].nConfidence < 98)
			{
				--num;
				continue;
			}
			vecPlate.push_back(pResult[i].license);
		}
		g_mapPlate[nChannel] = vecPlate;
		return num;
	}
	

	std::vector<std::string>& vecPlate = it->second;
	for (int i = 0; i < nPlateNum; ++i)
	{
		if (pResult[i].nConfidence < 98)
		{
			--num;
			continue;
		}
		bool flag = false;
		std::string strPlate = pResult[i].license;
		for (unsigned int k = 0; k < vecPlate.size(); ++k)
			if (vecPlate[k].compare(strPlate) == 0)
				flag = true;
		if (flag)
			pResult[i].reserved[0] = 1;
		else
			vecPlate.push_back(strPlate);
		
		if (vecPlate.size() >= 3)
			vecPlate.erase(vecPlate.begin());
	}

	return num;
}



void plate_pkg(const std::vector<uchar>& scence, const std::vector<uchar>& plate, const TH_PlateResult& result, int id, char* name)
{
	SYSTEMTIME	stTime;
	lpu_upload_package lpu_pkg = { 0 };
	lpu_pkg.m_nNum = 1;
	lpu_pkg.m_nPackageSize = sizeof(lpu_upload_package) + scence.size() + plate.size();
	
	//车牌唯一ID
	GetLocalTime(&stTime);
	_stprintf_s(lpu_pkg.m_sInfo.m_szPlateId, INFO_ID_MAX_SIZE, _T("%s_%03d_%02d%02d%02d_%s"),
		g_szPysicalID, id,
		stTime.wHour, stTime.wMinute, stTime.wSecond, result.license);

	//是否有车牌信息
	lpu_pkg.m_sInfo.m_bHasPlate = 1;

	//车牌位置信息
	lpu_pkg.m_sInfo.m_sPlatePosition.m_nX = result.rcLocation.left;
	lpu_pkg.m_sInfo.m_sPlatePosition.m_nY = result.rcLocation.top;
	lpu_pkg.m_sInfo.m_sPlatePosition.m_nW = result.rcLocation.right - result.rcLocation.left;
	lpu_pkg.m_sInfo.m_sPlatePosition.m_nH = result.rcLocation.bottom - result.rcLocation.top;

	//车牌类型
	lpu_pkg.m_sInfo.m_nPlateTypeCode = result.nType;

	//采集时间
	GetLocalTime(&lpu_pkg.m_sInfo.m_tCaptureTime);

	//车牌号
	strcpy_s(lpu_pkg.m_sInfo.m_szPlateNO, result.license);

	//采集点ID
	_stprintf_s(lpu_pkg.m_sInfo.m_szUnitID, DEVICE_ID_MAX_SIZE_EX, _T("%s"), g_szPysicalID);

	//通道ID
	_stprintf_s(lpu_pkg.m_sInfo.m_szChneID, DEVICE_ID_MAX_SIZE_EX, _T("%s_%03d"), g_szPysicalID, id);

	//通道名称
	_stprintf_s(lpu_pkg.m_sInfo.m_szChneName, 128, _T("%s"), name);

	//车牌数据大小
	lpu_pkg.m_sInfo.m_nPlateSize = plate.size();

	//车牌数据偏移量
	lpu_pkg.m_sInfo.m_nPlateOffset = sizeof(lpu_upload_package) + scence.size();
	
	//车牌数据拓展名
	strcpy_s(lpu_pkg.m_sInfo.m_szPlateExt, "jpg");

	//场景压缩率
	lpu_pkg.m_sInfo.m_nSceneCompressRate = 70;

	//场景数据大小
	lpu_pkg.m_sInfo.m_nSceneSize = scence.size();

	//场景数据偏移量
	lpu_pkg.m_sInfo.m_nSceneOffset = sizeof(lpu_upload_package);

	//场景数据拓展名
	strcpy_s(lpu_pkg.m_sInfo.m_szSceneExt, "jpg");

	GetLocalTime(&stTime);
	_stprintf_s(lpu_pkg.m_sInfo.m_szStreamTag, DATASTREAM_TAG_MAX_SIZE, _T("0,%s_%03d,%04d-%02d-%02d %02d:%02d:%02d;"), 
		g_szPysicalID, id,
		stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	uchar* ucp_data = new uchar[lpu_pkg.m_nPackageSize];
	memset(ucp_data, 0, sizeof(uchar) * lpu_pkg.m_nPackageSize);

	int offset = 0;
	memcpy(ucp_data + offset, &lpu_pkg, sizeof(lpu_upload_package));
	
	offset += sizeof(lpu_upload_package);
	memcpy(ucp_data + offset, &scence[0], scence.size());

	offset += scence.size();
	memcpy(ucp_data + offset, &plate[0], plate.size());

	CNetClient::Instance()->push(ucp_data);
}