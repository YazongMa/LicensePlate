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

//λ�þ��ο���Ϣ
typedef struct
{
	UINT32 		m_nX;		//���Ͻ�X
	UINT32		m_nY;		//���Ͻ�Y
	UINT32		m_nW;		//���
	UINT32 		m_nH;		//�߶�
}plate_rect;


//��Ա��״̬
typedef enum enumClusterState
{
	enumClusterStateUnknown = 0,							//δ֪
	enumClusterStateFirst = 1,								//����
	enumClusterStateStay = 2,								//ͣ��
	enumClusterStateLast = 3,								//�뿪
};


typedef struct __ppu_upload_face_info__
{
	TCHAR		m_szPlateId[INFO_ID_MAX_SIZE];				//����ΨһID
	UINT32		m_nPlateState;								//����״̬ enumClusterState

	BOOL		m_bHasPlate;								//�Ƿ��г�����Ϣ ��Ӧm_sFacePosition
	plate_rect	m_sPlatePosition;							//����λ����Ϣ
	UINT32		m_nPlateTypeCode;							//��������
	TCHAR		m_szPlateNO[INFO_ID_MAX_SIZE];				//���ƺ�

	UINT64 		m_nFrameId;									//֡ID
	UINT32 		m_nFrameWidth;								//֡���
	UINT32 		m_nFrameHeight;								//֡�߶�
	SYSTEMTIME 	m_tFrameTime;								//֡ʱ��

	SYSTEMTIME	m_tCaptureTime;								//�ɼ�ʱ�䣨ppu������
	TCHAR 		m_szUnitID[DEVICE_ID_MAX_SIZE_EX];			//�ɼ���ID ��ֵΪGUID���״ΰ�װ����ʱ��������ע����б��
	TCHAR 		m_szChneID[DEVICE_ID_MAX_SIZE_EX];			//ͨ��ID ��ֵΪGUID���״ΰ�װ����ʱ��������ע����б��
	TCHAR 		m_szChneName[CHANNEL_NAME_MAX_SIZE];		//ͨ������

	UINT32 		m_nPlateSize;								//�������ݴ�С
	UINT32 		m_nPlateOffset;								//��������ƫ��
	TCHAR		m_szPlateExt[FILE_EXT_NAME_MAX_LEN];		//����������չ��

	UINT32 		m_nSceneCompressRate;						//����ѹ����
	UINT32 		m_nSceneSize;								//������С
	UINT32 		m_nSceneOffset;								//��������ƫ��
	TCHAR		m_szSceneExt[FILE_EXT_NAME_MAX_LEN];		//����������չ��

	TCHAR 		m_szStreamTag[DATASTREAM_TAG_MAX_SIZE];		//��������� ���ַ���������� ����������ID,����ID,ʱ��;��
}lpu_upload_info;

//���ƴ���Ԫlpu�ϴ��ĳ��ư�
typedef struct __ppu_upload_face_package__
{
	UINT32					m_nPackageSize;					//������ У��
	UINT32 					m_nNum;							//������Ϣ����
	lpu_upload_info			m_sInfo;						//�̶����ȵĳ�����Ϣ 
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