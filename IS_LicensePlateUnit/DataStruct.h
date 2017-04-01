
#ifndef __DATASTRUCT_H__
#define __DATASTRUCT_H__

#include "Common/Common.h"
#include "Common/StopWatch.h"
#include <iostream>
#include <deque>
#include <map>


//采集端使用-->帧数据格式
class CFrameInfo
{
public:
	//构造函数
	CFrameInfo(int length, int width, int height);

	//析构函数
	~CFrameInfo();

	//赋值函数
	CFrameInfo& operator=(const CFrameInfo& rh);

	//采集帧间隔
	long long operator-(const CFrameInfo& rh);

	//获取当前时间
	static void getTime(SYSTEMTIME* pTime);

public:
	int					nFormat;				//帧类型
	int					nLength;				//帧大小
	int					nWidth;					//帧宽度
	int					nHeigth;				//帧高度
	unsigned char*		pbyData;				//帧数据
	unsigned long long	nId;					//帧号
	SYSTEMTIME			sysTime;				//采集时间
};

typedef std::deque<CFrameInfo *>					DeqFrameInfo_t;
typedef std::deque<CFrameInfo *>::iterator			DeqFrameInfoItr_t;


//共用-->包结构, 客户端上送人脸结构
typedef struct __FaceInfo
{
	int					nWidth;
	int					nHeight;
	int					nLength;
	int					nOffSet;
	unsigned long long	ullFrameId;

	__FaceInfo():nWidth(0), nHeight(0), nLength(0), nOffSet(0), ullFrameId(0){}
}FaceInfo;


// Shared Memory Frame Begin
#define BUFFER_SIZE_8M      8388608
#define UDPHEADFLAG			0xfe235688
#define MAXFACE				50
#define DEVID_MAX_SIZE		32

typedef struct __RECT
{
	int		left;
	int		top;
	int		width;
	int		height;
}__RECT;

typedef struct __UDPFrameData__
{
	unsigned int	HeadFlag;
	char			m_chDeviceId[DEVID_MAX_SIZE];		//采集设备标识
	SYSTEMTIME		m_ftTime;							//采集时间
	__int64			m_nFrameId;							//帧号
	int				m_nFormat;							//帧数据格式 参考enumFrameFormat
	int				m_nFrameSize;						//帧大小
	int				m_nFrameWidth;						//帧宽度
	int				m_nFrameHeigth;						//帧高度
	int             m_nFaceNum;
	__RECT         FaceRect[MAXFACE];
}UDPFrameData;
// Shared Memory Frame End

#endif
