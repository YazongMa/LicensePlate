#ifndef _VZ_PLAYER_LIB_H_
#define _VZ_PLAYER_LIB_H_

#ifdef PLAYER_EXPORT
#define PLAYER_PORT __declspec(dllexport)
#else
#define PLAYER_PORT __declspec(dllimport)
#endif

#define VZ_PLAYER_LIB_OK			0
#define VZ_PLAYER_LIB_FAILED		-1
#define VZ_PLAYER_LIB_NOT_SUPPORT	-2

typedef	int VzPlayerHandle;

typedef enum VzPlayerPlayScale
{
	ScaleNone = -1,
	ScaleHalfXXX,		//1/8x
	ScaleHalfXX,		//1/4x
	ScaleHalfX,			//1/2x
	ScaleNomal,			//1x
	ScaleDoubleX,		//2x
	ScaleDoubleXX,		//4x
	ScaleDoubleXXX,		//8x
	ScaleNum			//���õĸ���
}
VzPlayerPlayScale;

typedef struct VzFileInfo
{
	LONG timeTotalInMS;	//��Ƶ��ʱ��
	LONG framesTotal;	//��֡��
}
VzFileInfo;

typedef void (CALLBACK *VzPlayer_CallBackPlayTime)(LONG lCurrentTimeInMS, void *clientData);
typedef void (CALLBACK *VzPlayer_CallBackFramePos)(LONG lCurrentFramePos, void *clientData);
typedef void (CALLBACK *VzPlayer_CallBackPlayEnded)(void *clientData);
typedef void (CALLBACK *VzPlayer_CallBackDecodeFrame)(unsigned char *pY, unsigned char *pU, unsigned char *pV,
													  int width, int height, int pitchY, int pitchU, int pitchV,
													  void *clientData);
typedef enum VzPlayerCreateType
{
	Type_FilePlayer,		//��ͳAVI�ļ�������
	Type_SegmentPlayer,		//��AVI�ļ��Ķβ�����
	Type_FFPlayer,			//ʹ��FFMPEG����Ӧ��ǿ
	Type_ARPlayer			//����������Ϣ�����ܴ����
}
VzPlayerCreateType;

typedef enum VZAR_MODEL
{
	VZAR_NULL,			//��Ч��
	VZAR_TARGETS,		//����Ŀ���
	VZAR_SKIP_FRAME,	//ʡ����Ч֡
	VZAR_CONCENTRATE	//Ŀ��Ũ��
}
VZAR_MODEL;

//�������ݵ�����ģʽ
typedef enum VZ_DEC_DATA_MODE
{
	VZ_DEC_DATA_INSTANCE,	//ʵ�ʽ������ݣ��������ƣ�Ĭ�ϣ�
	VZ_DEC_DATA_REFERENCE,	//�����������ݣ���������
	VZ_DEC_DATA_COPY,		//�����������ݣ�������
}
VZ_DEC_DATA_MODE;

//����������handle
//���ص�handle����֮�����к����ĵ���
PLAYER_PORT VzPlayerHandle VzPlayer_CreateHandle(VzPlayerCreateType eCreateType);

//�ͷŽ�����handle
PLAYER_PORT void VzPlayer_DeleteHandle(VzPlayerHandle hndlPlayer);

//ָ������
PLAYER_PORT void VzPlayer_SetHWND(VzPlayerHandle hndlPlayer, HWND hWnd);

//���ļ�
PLAYER_PORT int VzPlayer_OpenFile(VzPlayerHandle hndlPlayer, char *pFileName, VzFileInfo *pFileInfo = NULL);

//�ر��ļ�
PLAYER_PORT int VzPlayer_CloseFile(VzPlayerHandle hndlPlayer);

//���ò�����ֹʱ�䣺������Ƶ��ָ����һ��ʱ�䣬һ��������
PLAYER_PORT int VzPlayer_PlaySpanByTimeStamp(VzPlayerHandle hndlPlayer, LONG lTSStart, LONG lTSEnd);

//����
PLAYER_PORT int VzPlayer_Play(VzPlayerHandle hndlPlayer);

//ֹͣ
PLAYER_PORT int VzPlayer_Stop(VzPlayerHandle hndlPlayer);

//��ͣ
PLAYER_PORT int VzPlayer_Pause(VzPlayerHandle hndlPlayer);

//���������ڼ�����ͣ��handle
PLAYER_PORT int VzPlayer_Resume(VzPlayerHandle hndlPlayer);

//���ò����ٶ�1/8x 1/4x 1/2x 1x 2x 4x 8x
PLAYER_PORT int VzPlayer_SetScale(VzPlayerHandle hndlPlayer, VzPlayerPlayScale playScale);

//�õ���ǰ�ļ�����ʱ�䣨��λms��
PLAYER_PORT LONG VzPlayer_GetTotalTime(VzPlayerHandle hndlPlayer);

//�õ���ǰ�ļ�����֡��
PLAYER_PORT LONG VzPlayer_GetTotalFrames(VzPlayerHandle hndlPlayer);

//ת��ָ��ʱ��λ��֮ǰ����Ĺؼ�֡
PLAYER_PORT void VzPlayer_SeekByTime(VzPlayerHandle hndlPlayer, LONG lTimeToSeek);

//ת��ָ��֡����ʹ�Ƿǹؼ�֡
PLAYER_PORT void VzPlayer_SeekToExactFrame(VzPlayerHandle hndlPlayer, LONG lExactFrame);

//�õ���ǰ���ŵ�ʱ��λ�ã���λms��
PLAYER_PORT LONG VzPlayer_GetPlayedTime(VzPlayerHandle hndlPlayer);

//�õ���ǰ���ŵ�֡λ��
PLAYER_PORT LONG VzPlayer_GetPlayedFrames(VzPlayerHandle hndlPlayer);

//���ٷֱ����ò���λ�ã�0~100��
PLAYER_PORT int VzPlayer_SetPlayPos(VzPlayerHandle hndlPlayer, float fPlayPos);

//��ȡ����λ�õİٷֱȣ�0~100��
PLAYER_PORT float VzPlayer_GetPlayPos(VzPlayerHandle hndlPlayer);

//��ʾ����ͣ����һ֡����
PLAYER_PORT void VzPlayer_ShowNextFrame(VzPlayerHandle hndlPlayer);

//���ò���ֹͣʱ����Ҫˢ����Ļ�õ���ɫ
//pRGB Ϊunsigned char rgb[3]���׵�ַ
PLAYER_PORT void VzPlayer_SetStaticScreen(VzPlayerHandle hndlPlayer, unsigned char *pRGB);

//���ļ�����ֹͣʱ���ػ洰��Ϊ��ɫ������ʹ��VzPlayer_SetStaticScreen�������ػ���ɫ
//ע�⣺�ú�������һֱ���ã��ڲ��Ļ��Ʊ�֤��ֻ����δ����ʱ����Ч�����Բ���Ӱ���������ŵ�Ч��
PLAYER_PORT void VzPlayer_DrawStatic(VzPlayerHandle hndlPlayer);

//����ʱ�������Ļص������������ṩ����ʱ��ķ���
PLAYER_PORT void VzPlayer_SetPlayTimeCallBack(VzPlayerHandle hndlPlayer, 
											  VzPlayer_CallBackPlayTime cbFuncPlayTime, void *clientData);
PLAYER_PORT void VzPlayer_SetFramePosCallBack(VzPlayerHandle hndlPlayer,
											   VzPlayer_CallBackFramePos cbFuncFramePos, void *clientData);
//���ý������ݻص��Ŀ���ģʽ
PLAYER_PORT int VzPlayer_SetDecodeFrameCopyMode(VzPlayerHandle hndlPlayer, VZ_DEC_DATA_MODE eCopyMode);

//���ý������ݻص�
PLAYER_PORT void VzPlayer_SetDecodeFrameCallBack(VzPlayerHandle hndlPlayer,
											   VzPlayer_CallBackDecodeFrame cbFuncDecodeFrame, void *clientData);
//�����ļ����Ž����Ļص�����
PLAYER_PORT void VzPlayer_SetPlayEndCallBack(VzPlayerHandle hndlPlayer,
								 VzPlayer_CallBackPlayEnded cbFuncPlayEnded, void *clientData);

//========AR���������============
//����AR����������ǿģʽ
PLAYER_PORT int VzPlayer_SetARModel(VzPlayerHandle hndlPlayer, VZAR_MODEL eModel);

//����AR��������Ũ��ģʽ�µ���������Ǳ������ݻ���ȫ������
PLAYER_PORT int VzPlayer_SetAROutputAlarmOnly(VzPlayerHandle hndlPlayer, int bAlarmOnly);

//����AR�������ڻ���Ŀ��ģʽ���Ƿ���ʾ������Ϣ
#define VZAR_INTELL_DRAW_NULL		0x0
#define VZAR_INTELL_DRAW_TARGET		0x1
#define VZAR_INTELL_DRAW_TRAJECTOR	0x2
#define VZAR_INTELL_DRAW_RULE		0x4
PLAYER_PORT int VzPlayer_SetARDrawIntellInfo(VzPlayerHandle hndlPlayer, int bitsIntellDraw);

//����AR��������Ũ��ģʽ�µı���
PLAYER_PORT int VzPlayer_SetARBackground(VzPlayerHandle hndlPlayer, unsigned char *pBufRGB, int width, int height);

//����AR��������Ũ��ģʽ�µ�ѡ��Ŀ���λ��
//����Ϊ�ٷ���
PLAYER_PORT int VzPlayer_ARCheckSelection(VzPlayerHandle hndlPlayer, unsigned short usPosX100, unsigned short usPosY100);

//ȡ��ѡ��
PLAYER_PORT int VzPlayer_ARCancelSelection(VzPlayerHandle hndlPlayer);

//��ȡѡ���Ŀ���֡λ��
PLAYER_PORT int VzPlayer_ARGetSelection(VzPlayerHandle hndlPlayer,
										  unsigned int *pUTimeStamp,
										  unsigned int *pUFrameNum);
//============Ԥ�����=============
PLAYER_PORT void VzPlayer_SetPreviewHWND(VzPlayerHandle hndlPlayer, HWND hWnd);

PLAYER_PORT int VzPlayer_PreviewFromSpecifyFrame(VzPlayerHandle hndlPlayer, LONG lExactFrame,
												  LONG lAhead = 30, LONG lDelay = 30, BOOL bRepeat = TRUE);
PLAYER_PORT void VzPlayer_StopPreview();

//Ƭ�β��������

typedef struct VzTM
{
	int sec;
	int min;
	int hour;
	int mday;
	int mon;
	int year;
}
VzTM;

typedef struct VzTMSeg
{
	VzTM tmStart;
	VzTM tmEnd;
}
VzTMSeg;

//������Ƶ�ļ�ɨ��·��
PLAYER_PORT int VzPlayer_ScanDir(VzPlayerHandle hndlPlayer, const char *pPath);

//��ȡ��Ƶ�ļ�ɨ����
PLAYER_PORT int VzPlayer_GetScanResult(VzPlayerHandle hndlPlayer, VzTMSeg *pTMSeg, unsigned int &countTMSeg);

//���ļ��İ�ʱ�������ò���λ��
PLAYER_PORT int VzPlayer_Seek(VzPlayerHandle hndlPlayer, VzTM &tm);

#endif
