//�����ļ� �����ļ���ÿ����ǩ������ֵ ��δ������ֵ�ж�
#pragma once

#include <Windows.h>

//ǰ�ô�����
#define		DEFAULT_IPADDR						_T("0.0.0.0")
#define		DEFAULT_PysicalId					_T("1")
#define		DEFAULT_PreName						_T("ǰ�ô�����")
#define		DEFAULT_PreModel					_T("�ͺ�")
#define		DEFAULT_SN							_T("SN")
#define		DEFAULT_PreLocation					_T("��װλ��")
#define		DEFAULT_AdminName					_T("admin")
#define		DEFAULT_AdminPhone					_T("1234567890")
#define		DEFAULT_AdminRemark					_T("��ע")
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
#define		DEFAULT_Name						_T("��������ͷ1")
#define		DEFAULT_Location					_T("0")
#define		DEFAULT_Brand						_T("���������")
#define		DEFAULT_Model						_T("ģ���ͺ�")
#define		DEFAULT_Version						_T("�汾�ͺ�")
#define		DEFAULT_VideoStreamURI				_T("rtsp://192.168.115.2:554/cam/realmonitor?channel=1&amp;subtype=0&amp;unicast=true&amp;proto=Onvif")
#define		DEFAULT_VideoStreamTransType		_T("1")
#define		DEFAULT_VideoStreamFormat			_T("1")
#define		DEFAULT_LoginUsername				_T("admin")
#define		DEFAULT_LoginPassword				_T("12345")
#define		DEFAULT_IsProcess					_T("1")
#define		DEFAULT_ImageQuality				_T("90")//��Ƭѹ������

#define		DEFAULT_XPosition					_T("0")
#define		DEFAULT_YPosition					_T("0")
#define		DEFAULT_Width						_T("1920")
#define		DEFAULT_Height						_T("1080")

#define		DEFAULT_DetectInterval				_T("2")
#define		DEFAULT_DetectFaceMaxNum			_T("1")
#define		DEFAULT_FaceMinSize					_T("30")
#define		DEFAULT_FaceMaxSize					_T("300")
#define     DEFAULT_Angle                       _T("5")

#define     DEFAULT_ScaleWidth                  _T("640")//ѹ������
#define     DEFAULT_ScaleHeight                 _T("480")//ѹ����߶�
#define     DEFAULT_GPUFlag						_T("0")//GPU����״̬
#define		DEFAULT_MaxFaceDataCacheNum			_T("10000")

//���ò���1
typedef struct
{
	char		m_chPhysicalId[64];							//ǰ�ô����������ʶ
	char		m_chPhysicalName[100];						//ǰ�ô���������
	char		m_chPhysicalIP[20];							//ǰ�ô�����IP��ַ

	char		m_chDMServerIP[100];						//DM������IP
	int			m_nDMServerPort;							//DM������Port
	char		m_chRMQServerIP[100];						//RMQServerIP
	int			m_nRMQServerPort;							//RMQServerPort

	bool		m_bIsNetDog;								//�Ƿ������繷
	char		m_chNetDogIP[100];							//���繷IP
	char		m_chMachineCode[100];						//������
	char		m_chAlgorithmSN[100];						//���к�
	char		m_chAlgorithmLicense[100];					//��ɺ�

	int			m_nChannelSum;								//ͨ������
	char		m_chCurChannelID[100];						//������ͨ��ID(ǰ�ô����������ʶ+ͨ�����)
	int			m_nPPID;									//������ID
}ParamMgrType;

//���ò���2
typedef struct
{
	bool		m_bStartFlag;								//ͨ��������־
	char		m_chChannelID[100];							//ͨ��ID(ǰ�ô����������ʶ+ͨ�����)
	char		m_chChannelName[100];						//ͨ������
	char		m_chUserName[100];							//�û���
	char		m_chPassWord[100];							//����
	char		m_chVideoStreamURL[300];					//��Ƶ��URL
	int			m_nVideoType;								//��Ƶ���� (UNKNOW_URL:0, RTSP_URL:1, FILE_URL:2, CAMERA_URL:3)

	int			m_nTopLeftX;								//���Ͻ�X
	int			m_nTopLeftY;								//���Ͻ�Y
	int			m_nWidth;									//���
	int			m_nHeight;									//�߶�

	int			m_nFaceMinSize;								//���������С����
	int			m_nFaceMaxSize;								//���������С����
	int			m_nClusterFaceMaxNum;						//���е����������
	int			m_nUpLoadFaceMaxNum;						//�ϴ����������
	float       m_fAngle;									//ƫת�Ƕ�

	int			m_nDetectInterval;							//��֡���� 0��ʾ����֡
	int			m_nDetectFaceMaxNum;						//��������������
	int			m_nFaceCacheMaxNum;							//��������������
	int			m_nDetectThreadNum;							//��������߳���
	bool		m_bLocalStoreFlag;							//�Ƿ������ش洢
	int			m_nScaleWidth;
	int			m_nScaleHeight;
	bool		m_bGPUFlag;
	int			m_nImageQuality;
	int			m_nUpLoadInterVal;
	int			m_nFrameCacheSize;							//֡�����󻺴����
	int			m_nEyesDist;								//�����۾���ֵ
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
	ParamMgrType					m_ParamMgr;		//ǰ�ô������������ Ĭ��ΪconfigĿ¼��config.ini�ļ�
	std::map<int, ParamChnlType>	m_mParamChnl;	//ͨ�����ò��� Ĭ��ΪconfigĿ¼��channel_id.ini�ļ�
};