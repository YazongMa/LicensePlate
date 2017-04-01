#pragma once

#include <iostream>
#include <string> 
using namespace std;

//#include "network.h"
#include "Common/Common.h"
#include "math.h"
#include "RabbitMQClient.h"
#include "ParamProc.h"

typedef UINT32	(__stdcall * func_RMQSC_Init)(RMQSendInitConfig *pInitCfg, int nLogLevel);
typedef void	(__stdcall * func_RMQSC_Uninit)();
typedef UINT32	(__stdcall * func_RMQSC_SendData)(BYTE *pbyData, UINT32 nDataSize);
typedef BOOL	(__stdcall * func_RMQSC_TestConn)(TCHAR *pszIP, UINT32 nPort, TCHAR *pszHostName, TCHAR *pszHostPwd);
typedef unsigned char uchar;

//位置矩形框信息
typedef struct
{
	UINT32 		m_nX;		//左上角X
	UINT32		m_nY;		//左上角Y
	UINT32		m_nW;		//宽度
	UINT32 		m_nH;		//高度
}plate_rect;


//人员类状态
typedef enum enumClusterState
{
	enumClusterStateUnknown = 0,							//未知
	enumClusterStateFirst = 1,								//初入
	enumClusterStateStay = 2,								//停留
	enumClusterStateLast = 3,								//离开
};


typedef struct __ppu_upload_face_info__
{
	TCHAR		m_szPlateId[INFO_ID_MAX_SIZE];				//车牌唯一ID
	UINT32		m_nPlateState;								//车牌状态 enumClusterState

	BOOL		m_bHasPlate;								//是否有车牌信息 对应m_sFacePosition
	plate_rect	m_sPlatePosition;							//车牌位置信息
	UINT32		m_nPlateTypeCode;							//车牌类型
	TCHAR		m_szPlateNO[INFO_ID_MAX_SIZE];				//车牌号

	UINT64 		m_nFrameId;									//帧ID
	UINT32 		m_nFrameWidth;								//帧宽度
	UINT32 		m_nFrameHeight;								//帧高度
	SYSTEMTIME 	m_tFrameTime;								//帧时间

	SYSTEMTIME	m_tCaptureTime;								//采集时间（ppu产生）
	TCHAR 		m_szUnitID[DEVICE_ID_MAX_SIZE_EX];			//采集点ID 该值为GUID是首次安装服务时产生并在注册表中标记
	TCHAR 		m_szChneID[DEVICE_ID_MAX_SIZE_EX];			//通道ID 该值为GUID是首次安装服务时产生并在注册表中标记
	TCHAR 		m_szChneName[CHANNEL_NAME_MAX_SIZE];		//通道名称

	UINT32 		m_nPlateSize;								//车牌数据大小
	UINT32 		m_nPlateOffset;								//车牌数据偏移
	TCHAR		m_szPlateExt[FILE_EXT_NAME_MAX_LEN];		//车牌数据拓展名

	UINT32 		m_nSceneCompressRate;						//场景压缩率
	UINT32 		m_nSceneSize;								//场景大小
	UINT32 		m_nSceneOffset;								//场景数据偏移
	TCHAR		m_szSceneExt[FILE_EXT_NAME_MAX_LEN];		//场景数据拓展名

	TCHAR 		m_szStreamTag[DATASTREAM_TAG_MAX_SIZE];		//数据流标记 在字符串后面加上 “服务类型ID,服务ID,时间;”
}lpu_upload_info;

//车牌处理单元lpu上传的车牌包
typedef struct __ppu_upload_face_package__
{
	UINT32					m_nPackageSize;					//包长度 校验
	UINT32 					m_nNum;							//车牌信息数量
	lpu_upload_info			m_sInfo;						//固定长度的车牌信息 
}lpu_upload_package;


class CNetClient : public ThreadHelper
{
public:
	static CNetClient* Instance();
	virtual void run();

	bool	init();
	bool	uninit();
	
	void	push(uchar* const data);

private:
	CNetClient();
	~CNetClient();

	bool	SendCmdUploadFaceEx(void *pPerfectFaceObject_, int nPerfectFaceObject);
	long	SendDataCacheFaceEx(long& lUpCacheSucceed, long& lUpCacheFailed);

	static HMODULE					m_hSendNode;
	static func_RMQSC_Init			m_func_RMQSC_Init;
	static func_RMQSC_Uninit		m_func_RMQSC_Uninit;
	static func_RMQSC_SendData		m_func_RMQSC_SendData;
	static func_RMQSC_TestConn		m_func_RMQSC_TestConn;

private:
	int					m_nRmqLogLevel;
	int					m_nSendID;
	char				m_chDeviceID[DEVICEID_SIZE];
	char				m_chExePath[MAX_PATH];
};