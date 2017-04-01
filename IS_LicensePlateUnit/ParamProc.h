//配置文件 配置文件中每个标签必须有值 暂未做有无值判断
#pragma once

#include <Windows.h>

//前置处理器
#define		DEFAULT_IPADDR						_T("0.0.0.0")
#define		DEFAULT_PysicalId					_T("1")
#define		DEFAULT_PreName						_T("前置处理器")
#define		DEFAULT_PreModel					_T("型号")
#define		DEFAULT_SN							_T("SN")
#define		DEFAULT_PreLocation					_T("安装位置")
#define		DEFAULT_AdminName					_T("admin")
#define		DEFAULT_AdminPhone					_T("1234567890")
#define		DEFAULT_AdminRemark					_T("备注")
#define		DEFAULT_PreVersion					_T("2.0")
#define		DEFAULT_IsStorageLocal				_T("0")
#define		DEFAULT_ClusterFaceMaxNum			_T("2")
#define     DEFAULT_SelectFaces                 _T("2")
#define		DEFAULT_RMQServerIP					_T("192.168.0.1")
#define		DEFAULT_RMQServerPort				_T("5672")
#define		DEFAULT_ServerIP					_T("192.168.0.1")
#define		DEFAULT_ServerPort					_T("3000")
#define		DEFAULT_EvaluationProcThraedNum		_T("6")
#define		DEFAULT_PerfectFaceProcThreadNum	_T("1")
#define		DEFAULT_IsNetDog					_T("1")
#define		DEFAULT_NetDogIP					_T("")
#define		DEFAULT_MachineCode					_T("")
#define		DEFAULT_AlgorithmSN					_T("")
#define		DEFAULT_AlgorithmLicense			_T("")
#define		DEFAULT_ChannelSum					_T("0")
#define		DEFAULT_CurChannelID				_T("")
#define		DEFAULT_UploadInterVal				_T("1")

//VideoDevice
#define		DEFAULT_IPCId						_T("0")
#define		DEFAULT_Name						_T("网络摄像头1")
#define		DEFAULT_Location					_T("0")
#define		DEFAULT_Brand						_T("银晨摄像机")
#define		DEFAULT_Model						_T("模块型号")
#define		DEFAULT_Version						_T("版本型号")
#define		DEFAULT_VideoStreamURI				_T("rtsp://192.168.115.2:554/cam/realmonitor?channel=1&amp;subtype=0&amp;unicast=true&amp;proto=Onvif")
#define		DEFAULT_VideoStreamTransType		_T("1")
#define		DEFAULT_VideoStreamFormat			_T("1")
#define		DEFAULT_LoginUsername				_T("admin")
#define		DEFAULT_LoginPassword				_T("12345")
#define		DEFAULT_IsProcess					_T("1")
#define		DEFAULT_ImageQuality				_T("90")//照片压缩质量

#define		DEFAULT_XPosition					_T("0")
#define		DEFAULT_YPosition					_T("0")
#define		DEFAULT_Width						_T("1920")
#define		DEFAULT_Height						_T("1080")

#define		DEFAULT_DetectInterval				_T("2")
#define		DEFAULT_DetectFaceMaxNum			_T("1")
#define		DEFAULT_FaceMinSize					_T("30")
#define		DEFAULT_FaceMaxSize					_T("300")
#define     DEFAULT_Angle                       _T("5")

#define     DEFAULT_ScaleWidth                  _T("640")//压缩后宽度
#define     DEFAULT_ScaleHeight                 _T("480")//压缩后高度
#define     DEFAULT_GPUFlag						_T("0")//GPU启用状态
#define		DEFAULT_MaxFaceDataCacheNum			_T("10000")

//配置参数1
typedef struct
{
	char		m_chPhysicalId[64];							//前置处理器物理标识
	char		m_chPhysicalName[100];						//前置处理器名称
	char		m_chPhysicalIP[20];							//前置处理器IP地址

	char		m_chDMServerIP[100];						//DM服务器IP
	int			m_nDMServerPort;							//DM服务器Port
	char		m_chRMQServerIP[100];						//RMQServerIP
	int			m_nRMQServerPort;							//RMQServerPort

	bool		m_bIsNetDog;								//是否用网络狗
	char		m_chNetDogIP[100];							//网络狗IP
	char		m_chMachineCode[100];						//机器码
	char		m_chAlgorithmSN[100];						//序列号
	char		m_chAlgorithmLicense[100];					//许可号

	int			m_nChannelSum;								//通道总数
	char		m_chCurChannelID[100];						//待操作通道ID(前置处理器物理标识+通道序号)
	int			m_nPPID;									//父进程ID
}ParamMgrType;

//配置参数2
typedef struct
{
	bool		m_bStartFlag;								//通道禁启标志
	char		m_chChannelID[100];							//通道ID(前置处理器物理标识+通道序号)
	char		m_chChannelName[100];						//通道名称
	char		m_chUserName[100];							//用户名
	char		m_chPassWord[100];							//密码
	char		m_chVideoStreamURL[300];					//视频流URL
	int			m_nVideoType;								//视频类型 (UNKNOW_URL:0, RTSP_URL:1, FILE_URL:2, CAMERA_URL:3)

	int			m_nTopLeftX;								//左上角X
	int			m_nTopLeftY;								//左上角Y
	int			m_nWidth;									//宽度
	int			m_nHeight;									//高度

	int			m_nFaceMinSize;								//检测人脸大小下限
	int			m_nFaceMaxSize;								//检测人脸大小上限
	int			m_nClusterFaceMaxNum;						//类中的人脸最大数
	int			m_nUpLoadFaceMaxNum;						//上传人脸最大数
	float       m_fAngle;									//偏转角度

	int			m_nDetectInterval;							//跳帧数量 0表示不跳帧
	int			m_nDetectFaceMaxNum;						//人脸检测最大数量
	int			m_nFaceCacheMaxNum;							//缓存人脸最大个数
	int			m_nDetectThreadNum;							//检测评价线程数
	bool		m_bLocalStoreFlag;							//是否开启本地存储
	int			m_nScaleWidth;
	int			m_nScaleHeight;
	bool		m_bGPUFlag;
	int			m_nImageQuality;
	int			m_nUpLoadInterVal;
	int			m_nFrameCacheSize;							//帧相关最大缓存个数
	int			m_nEyesDist;								//优先眼距阈值
	int			m_nPriorityFlag;
}ParamChnlType;


#include <map>
#include <vector>

class CParamProc
{
public:
	static  CParamProc* Instance();
	
	bool	init(std::vector<int>& chnls);
	bool	GetParamMgr(ParamMgrType& mgrType);
	bool	GetParamChnl(int nChnlID, ParamChnlType& chnlType);

private:
	CParamProc();
	~CParamProc();

	bool	GetParamMgr();
	bool	GetParamChnl(TCHAR* szChnlID);

private:
	ParamMgrType					m_ParamMgr;		//前置处理管理器参数 默认为config目录下config.ini文件
	std::map<int, ParamChnlType>	m_mParamChnl;	//通道配置参数 默认为config目录下channel_id.ini文件
};