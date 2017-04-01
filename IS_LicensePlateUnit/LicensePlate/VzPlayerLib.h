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
	ScaleNum			//可用的个数
}
VzPlayerPlayScale;

typedef struct VzFileInfo
{
	LONG timeTotalInMS;	//视频的时长
	LONG framesTotal;	//总帧数
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
	Type_FilePlayer,		//传统AVI文件播放器
	Type_SegmentPlayer,		//多AVI文件的段播放器
	Type_FFPlayer,			//使用FFMPEG的适应性强
	Type_ARPlayer			//叠加智能信息的智能处理的
}
VzPlayerCreateType;

typedef enum VZAR_MODEL
{
	VZAR_NULL,			//无效果
	VZAR_TARGETS,		//绘制目标框
	VZAR_SKIP_FRAME,	//省略无效帧
	VZAR_CONCENTRATE	//目标浓缩
}
VZAR_MODEL;

//解码数据的作用模式
typedef enum VZ_DEC_DATA_MODE
{
	VZ_DEC_DATA_INSTANCE,	//实际解码数据，并将绘制（默认）
	VZ_DEC_DATA_REFERENCE,	//拷贝解码数据，并将绘制
	VZ_DEC_DATA_COPY,		//拷贝解码数据，不绘制
}
VZ_DEC_DATA_MODE;

//建立播放器handle
//返回的handle用于之后所有函数的调用
PLAYER_PORT VzPlayerHandle VzPlayer_CreateHandle(VzPlayerCreateType eCreateType);

//释放建立的handle
PLAYER_PORT void VzPlayer_DeleteHandle(VzPlayerHandle hndlPlayer);

//指定窗口
PLAYER_PORT void VzPlayer_SetHWND(VzPlayerHandle hndlPlayer, HWND hWnd);

//打开文件
PLAYER_PORT int VzPlayer_OpenFile(VzPlayerHandle hndlPlayer, char *pFileName, VzFileInfo *pFileInfo = NULL);

//关闭文件
PLAYER_PORT int VzPlayer_CloseFile(VzPlayerHandle hndlPlayer);

//设置播放起止时间：播放视频的指定的一段时间，一次性作用
PLAYER_PORT int VzPlayer_PlaySpanByTimeStamp(VzPlayerHandle hndlPlayer, LONG lTSStart, LONG lTSEnd);

//播放
PLAYER_PORT int VzPlayer_Play(VzPlayerHandle hndlPlayer);

//停止
PLAYER_PORT int VzPlayer_Stop(VzPlayerHandle hndlPlayer);

//暂停
PLAYER_PORT int VzPlayer_Pause(VzPlayerHandle hndlPlayer);

//继续：用于继续暂停的handle
PLAYER_PORT int VzPlayer_Resume(VzPlayerHandle hndlPlayer);

//设置播放速度1/8x 1/4x 1/2x 1x 2x 4x 8x
PLAYER_PORT int VzPlayer_SetScale(VzPlayerHandle hndlPlayer, VzPlayerPlayScale playScale);

//得到当前文件的总时间（单位ms）
PLAYER_PORT LONG VzPlayer_GetTotalTime(VzPlayerHandle hndlPlayer);

//得到当前文件的总帧数
PLAYER_PORT LONG VzPlayer_GetTotalFrames(VzPlayerHandle hndlPlayer);

//转到指定时间位置之前最近的关键帧
PLAYER_PORT void VzPlayer_SeekByTime(VzPlayerHandle hndlPlayer, LONG lTimeToSeek);

//转到指定帧，即使是非关键帧
PLAYER_PORT void VzPlayer_SeekToExactFrame(VzPlayerHandle hndlPlayer, LONG lExactFrame);

//得到当前播放的时间位置（单位ms）
PLAYER_PORT LONG VzPlayer_GetPlayedTime(VzPlayerHandle hndlPlayer);

//得到当前播放的帧位置
PLAYER_PORT LONG VzPlayer_GetPlayedFrames(VzPlayerHandle hndlPlayer);

//按百分比设置播放位置（0~100）
PLAYER_PORT int VzPlayer_SetPlayPos(VzPlayerHandle hndlPlayer, float fPlayPos);

//获取播放位置的百分比（0~100）
PLAYER_PORT float VzPlayer_GetPlayPos(VzPlayerHandle hndlPlayer);

//显示并暂停在下一帧画面
PLAYER_PORT void VzPlayer_ShowNextFrame(VzPlayerHandle hndlPlayer);

//设置播放停止时，将要刷新屏幕用的颜色
//pRGB 为unsigned char rgb[3]的首地址
PLAYER_PORT void VzPlayer_SetStaticScreen(VzPlayerHandle hndlPlayer, unsigned char *pRGB);

//当文件播放停止时，重绘窗口为黑色，或者使用VzPlayer_SetStaticScreen来设置重绘颜色
//注意：该函数可以一直调用，内部的机制保证了只有在未播放时才有效，所以不会影响正常播放的效果
PLAYER_PORT void VzPlayer_DrawStatic(VzPlayerHandle hndlPlayer);

//设置时间跳动的回调函数，用于提供播放时间的反馈
PLAYER_PORT void VzPlayer_SetPlayTimeCallBack(VzPlayerHandle hndlPlayer, 
											  VzPlayer_CallBackPlayTime cbFuncPlayTime, void *clientData);
PLAYER_PORT void VzPlayer_SetFramePosCallBack(VzPlayerHandle hndlPlayer,
											   VzPlayer_CallBackFramePos cbFuncFramePos, void *clientData);
//设置解码数据回调的拷贝模式
PLAYER_PORT int VzPlayer_SetDecodeFrameCopyMode(VzPlayerHandle hndlPlayer, VZ_DEC_DATA_MODE eCopyMode);

//设置解码数据回调
PLAYER_PORT void VzPlayer_SetDecodeFrameCallBack(VzPlayerHandle hndlPlayer,
											   VzPlayer_CallBackDecodeFrame cbFuncDecodeFrame, void *clientData);
//设置文件播放结束的回调函数
PLAYER_PORT void VzPlayer_SetPlayEndCallBack(VzPlayerHandle hndlPlayer,
								 VzPlayer_CallBackPlayEnded cbFuncPlayEnded, void *clientData);

//========AR播放器相关============
//设置AR播放器的增强模式
PLAYER_PORT int VzPlayer_SetARModel(VzPlayerHandle hndlPlayer, VZAR_MODEL eModel);

//设置AR播放器在浓缩模式下的输出内容是报警内容或是全部内容
PLAYER_PORT int VzPlayer_SetAROutputAlarmOnly(VzPlayerHandle hndlPlayer, int bAlarmOnly);

//设置AR播放器在绘制目标模式下是否显示智能信息
#define VZAR_INTELL_DRAW_NULL		0x0
#define VZAR_INTELL_DRAW_TARGET		0x1
#define VZAR_INTELL_DRAW_TRAJECTOR	0x2
#define VZAR_INTELL_DRAW_RULE		0x4
PLAYER_PORT int VzPlayer_SetARDrawIntellInfo(VzPlayerHandle hndlPlayer, int bitsIntellDraw);

//设置AR播放器在浓缩模式下的背景
PLAYER_PORT int VzPlayer_SetARBackground(VzPlayerHandle hndlPlayer, unsigned char *pBufRGB, int width, int height);

//设置AR播放器在浓缩模式下的选择目标的位置
//参数为百分数
PLAYER_PORT int VzPlayer_ARCheckSelection(VzPlayerHandle hndlPlayer, unsigned short usPosX100, unsigned short usPosY100);

//取消选择
PLAYER_PORT int VzPlayer_ARCancelSelection(VzPlayerHandle hndlPlayer);

//获取选择的目标的帧位置
PLAYER_PORT int VzPlayer_ARGetSelection(VzPlayerHandle hndlPlayer,
										  unsigned int *pUTimeStamp,
										  unsigned int *pUFrameNum);
//============预览相关=============
PLAYER_PORT void VzPlayer_SetPreviewHWND(VzPlayerHandle hndlPlayer, HWND hWnd);

PLAYER_PORT int VzPlayer_PreviewFromSpecifyFrame(VzPlayerHandle hndlPlayer, LONG lExactFrame,
												  LONG lAhead = 30, LONG lDelay = 30, BOOL bRepeat = TRUE);
PLAYER_PORT void VzPlayer_StopPreview();

//片段播放器相关

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

//设置视频文件扫描路径
PLAYER_PORT int VzPlayer_ScanDir(VzPlayerHandle hndlPlayer, const char *pPath);

//获取视频文件扫描结果
PLAYER_PORT int VzPlayer_GetScanResult(VzPlayerHandle hndlPlayer, VzTMSeg *pTMSeg, unsigned int &countTMSeg);

//跨文件的按时间来设置播放位置
PLAYER_PORT int VzPlayer_Seek(VzPlayerHandle hndlPlayer, VzTM &tm);

#endif
