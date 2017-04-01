#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <tchar.h>
	#include <time.h>
	#define IS_INSTANCE		HMODULE
	#define PLATFORM		1
	#pragma warning(disable:4099)
#else
	#include <dlfcn.h>
	#include <wchar.h>
	#include <sys/time.h>
	#define IS_API
	#define IS_INSTANCE		void*
	#define PLATFORM		2
	#define BOOL	int
	#define BYTE	unsigned char
	#define WORD	unsigned short
	#define DWORD	unsigned long

	#ifdef _UNICODE
	#define TCHAR	wchar_t
	#else
	#define TCHAR	char
	#endif

typedef struct _SYSTEMTIME {
	unsigned short	wYear;
	unsigned short	wMonth;
	unsigned short	wDayOfWeek;
	unsigned short	wDay;
	unsigned short	wHour;
	unsigned short	wMinute;
	unsigned short	wSecond;
	unsigned short	wMilliseconds;
} SYSTEMTIME;

#endif



//#ifdef _DEBUG
//#include <vld.h>
//#endif

#include "Common/log4z.h"

using namespace zsummer::log4z;

#ifndef _tcscpy
	#ifdef  UNICODE   
		#define _tcscpy     wcscpy  
	#else  
		#define _tcscpy     strcpy  
	#endif 
#endif

#ifndef LONG_MAX
#define LONG_MAX		2147483647
#endif

/* maximum signed 64 bit value */
#ifndef _I64_MAX
#define _I64_MAX      9223372036854775807i64
#endif

/* maximum unsigned 64 bit value */
#ifndef _UI64_MAX
#define _UI64_MAX     0xffffffffffffffffui64
#endif


#define DFT_SIZE	128

/************************************************************************/
typedef struct _FRect
{
	int		x;
	int		y;
	int		w;
	int		h;
	float	fA;				// 照片偏转角
	float	fD;				// 照片清晰度


	_FRect() : x(0), y(0), w(0), h(0), fA(0.0f), fD(0.0f){}
}FRect;

typedef enum
{
	IDCARD				=	1,		/*文通OCR类*/
	CARDAPI4			=	2,		/*普天身份证读取类*/
	VIDEO				=	3,		/*视频解码类*/
	QRCODE				=	4,		/*二维码扫描类*/
	FINGERPRINT			=	5,		/*指纹识别类*/
	MAGNETIC			=	6,		/*磁条读取类：护照、驾照等*/

	ESTIMATE			=	7,		/*人脸检测类*/

	OTHER				=	0,		/*其他*/
}InstanceTpye;


typedef enum
{
	INIT				=	101,	/*初始化*/
	START				=	102,	/*开始*/
	STOP				=	103,	/*停止*/
	UNINIT				=	104,	/*反初始化*/
	PAUSE				=	105,	/*暂停*/
	RESUME				=	106,	/*恢复，在暂停状态下恢复*/

	UNKNOW				=	100,	/*未知*/
}MethodTpye;


/************************************************************************/
/*  视频解码, 人脸检测回调类型及参数说明：										*/
/*  void*			帧数据结构											*/
/*	int：			帧数据长度											*/
/*	const TCHAR*：	返回的错误描述信息										*/
/*  int：			返回的错误码											*/
/*  void*：			回调参数												*/
/************************************************************************/
typedef void ( __stdcall * Func_Frame)(void*, long*, const char*, int, void*);

typedef struct __FrameParam						
{										
	Func_Frame			pFuncPoint;			// 回调函数指针
	void*				pContext;			// 回调函数参数

	int					nInterval;			// 视频解码间隔(毫秒)
	int					nVideoType;			// 视频类型 1:usb摄像头, 2:rtsp摄像机, 0:未知

	int					nCamSeq;			// USB摄像头序号(同时有多个usb摄像头的情况下, 0为默认; 若要使用其他摄像头, 则需从配置文件中手工配置, 重启服务生效)
	int					nPixelFlg;			// 默认像素显示视频，为1时，下面两个参数才能生效
	int					nCamWidth;			// 使用USB摄像头序时, 该值为帧的宽度, 默认为800像素
	int					nCamHeight;			// 使用USB摄像头序时, 该值为帧的高度, 默认为600像素
	int					nExposure;			// 曝光参数调节，默认为0自动曝光。参数可配置（范围为-1~-13, 若曝光强烈，可设置为-6左右），

	int					nFlipFlag;			// 图像翻转设置

	char				szSN[64];			// SN
	char				szLic[64];			// Lic

	__FrameParam() : pFuncPoint(NULL), pContext(NULL), nInterval(40), 
					 nVideoType(1), nCamSeq(0), nPixelFlg(0), 
					 nCamWidth(640), nCamHeight(480), nExposure(0), nFlipFlag(1)
	{
		memset((void *)szSN,	0, sizeof(char) * 64);
		memset((void *)szLic,	0, sizeof(char) * 64);
	}
}FrameParam;

typedef struct __FrameDataStruct
{
	int					nFormat;				//帧类型
	int					nLength;				//帧大小
	int					nWidth;					//帧宽度
	int					nHeigth;				//帧高度
	unsigned char*		pbyData;				//帧数据
	unsigned long long	nId;					//帧号
	char				szDeviceId[32];			//采集设备ID;
	SYSTEMTIME			sysTime;				//采集时间
}FrameDataStruct;


#define MsgReport(BZCode, IsPic, PicTite, PicCode,					\
			MsgCode, GJ, CID, Level, ZJDM)							\
{																	\
	std::ostringstream os;											\
	os	<< "{"														\
		<< "\"BZCode\":"	<< BZCode		<< ","					\
		<< "\"IsPic\":"		<< IsPic		<< ","					\
		<< "\"PicTite\":"	<< PicTite		<< ","					\
		<< "\"PicCode\":"	<< PicCode		<< ","					\
		<< "\"MCode\":"		<< MsgCode		<< ","					\
		<< "\"GJ\":"		<< GJ			<< ","					\
		<< "\"CID\":"		<< CID			<< ","					\
		<< "\"Level\":"		<< Level		<< ","					\
		<< "\"ZJDM\":"		<< ZJDM			<< ""					\
		<< "}";														\
	CNetServer::Instance()->send_msg(os.str());						\
}

#define ErrReport(BZCode, IsPic, PicTite, PicCode,					\
			MsgCode, GJ, CID, Level, ZJDM)							\
{																	\
	std::ostringstream os;											\
	os	<< "{"														\
		<< "\"BZCode\":"	<< BZCode		<< ","					\
		<< "\"IsPic\":"		<< IsPic		<< ","					\
		<< "\"PicTite\":"	<< PicTite		<< ","					\
		<< "\"PicCode\":"	<< PicCode		<< ","					\
		<< "\"MCode\":"		<< MsgCode		<< ","					\
		<< "\"Level\":"		<< Level		<< ","					\
		<< "}";														\
	CNetServer::Instance()->send_msg(os.str());						\
}


/************************************************************************/
class CIsException
{
public:

	inline CIsException(const int nErrCode, char* szErrMess)
	:
	m_nErrCode(nErrCode),
	m_nErrInfo(szErrMess)
	{
	}

	inline CIsException(const CIsException& e)
	:
	m_nErrCode(e.m_nErrCode),
	m_nErrInfo(e.m_nErrInfo)
	{
	}
	virtual ~CIsException(){};

	inline const int	errorCode() const { return m_nErrCode; }
	inline const char*	errorInfo() const { return m_nErrInfo; }

	friend inline std::ostream& operator<<(std::ostream& os, const CIsException& e)
	{
		os << "ErrorCode:[" << e.m_nErrCode << "], ErrorInfo:[" << e.m_nErrInfo << "]";
		return os;
	}

private:

	const int			m_nErrCode;
	const char*			m_nErrInfo;
};
/************************************************************************/

#define		DEVICEID_SIZE					64
#define		INFO_ID_MAX_SIZE				64
#define		DEVICE_ID_MAX_SIZE_EX			64
#define 	PACKAGE_INFO_MAX_NUM			5
#define		CHANNEL_NAME_MAX_SIZE			128
#define		DATASTREAM_TAG_MAX_SIZE			256
#define		FILE_EXT_NAME_MAX_LEN			6


// URL Type
#define UNKNOW_URL							0
#define	RTSP_URL							1
#define	FILE_URL							2
#define	CAMERA_URL							3
#define	ERR_URL								4


#define		IS_FREE(p)			{ if(p) { free(p);			(p)=NULL; } }
#define		IS_DELETE(p)		{ if(p) { delete (p);		(p)=NULL; } }
#define		IS_DELETE_ARRAY(p)	{ if(p) { delete[] (p);		(p)=NULL; } }
#define		IS_CLOSEHANDLE(p)	{ if(p && p != INVALID_HANDLE_VALUE) { CloseHandle(p);	(p)=NULL; } }

void	__stdcall	GetPPUPath(TCHAR* pBuf, DWORD dwBufSize);
void	__stdcall	GetExePath(TCHAR* pBuf, DWORD dwBufSize);
void	__cdecl		WriteLogInfo(const TCHAR * pszFilehName, const TCHAR * pszLogInfo, ...);

#ifdef _DEBUG
#include <vld.h>
#endif

#endif
