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
	float	fA;				// ��Ƭƫת��
	float	fD;				// ��Ƭ������


	_FRect() : x(0), y(0), w(0), h(0), fA(0.0f), fD(0.0f){}
}FRect;

typedef enum
{
	IDCARD				=	1,		/*��ͨOCR��*/
	CARDAPI4			=	2,		/*�������֤��ȡ��*/
	VIDEO				=	3,		/*��Ƶ������*/
	QRCODE				=	4,		/*��ά��ɨ����*/
	FINGERPRINT			=	5,		/*ָ��ʶ����*/
	MAGNETIC			=	6,		/*������ȡ�ࣺ���ա����յ�*/

	ESTIMATE			=	7,		/*���������*/

	OTHER				=	0,		/*����*/
}InstanceTpye;


typedef enum
{
	INIT				=	101,	/*��ʼ��*/
	START				=	102,	/*��ʼ*/
	STOP				=	103,	/*ֹͣ*/
	UNINIT				=	104,	/*����ʼ��*/
	PAUSE				=	105,	/*��ͣ*/
	RESUME				=	106,	/*�ָ�������ͣ״̬�»ָ�*/

	UNKNOW				=	100,	/*δ֪*/
}MethodTpye;


/************************************************************************/
/*  ��Ƶ����, �������ص����ͼ�����˵����										*/
/*  void*			֡���ݽṹ											*/
/*	int��			֡���ݳ���											*/
/*	const TCHAR*��	���صĴ���������Ϣ										*/
/*  int��			���صĴ�����											*/
/*  void*��			�ص�����												*/
/************************************************************************/
typedef void ( __stdcall * Func_Frame)(void*, long*, const char*, int, void*);

typedef struct __FrameParam						
{										
	Func_Frame			pFuncPoint;			// �ص�����ָ��
	void*				pContext;			// �ص���������

	int					nInterval;			// ��Ƶ������(����)
	int					nVideoType;			// ��Ƶ���� 1:usb����ͷ, 2:rtsp�����, 0:δ֪

	int					nCamSeq;			// USB����ͷ���(ͬʱ�ж��usb����ͷ�������, 0ΪĬ��; ��Ҫʹ����������ͷ, ����������ļ����ֹ�����, ����������Ч)
	int					nPixelFlg;			// Ĭ��������ʾ��Ƶ��Ϊ1ʱ��������������������Ч
	int					nCamWidth;			// ʹ��USB����ͷ��ʱ, ��ֵΪ֡�Ŀ��, Ĭ��Ϊ800����
	int					nCamHeight;			// ʹ��USB����ͷ��ʱ, ��ֵΪ֡�ĸ߶�, Ĭ��Ϊ600����
	int					nExposure;			// �ع�������ڣ�Ĭ��Ϊ0�Զ��ع⡣���������ã���ΧΪ-1~-13, ���ع�ǿ�ң�������Ϊ-6���ң���

	int					nFlipFlag;			// ͼ��ת����

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
	int					nFormat;				//֡����
	int					nLength;				//֡��С
	int					nWidth;					//֡���
	int					nHeigth;				//֡�߶�
	unsigned char*		pbyData;				//֡����
	unsigned long long	nId;					//֡��
	char				szDeviceId[32];			//�ɼ��豸ID;
	SYSTEMTIME			sysTime;				//�ɼ�ʱ��
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
