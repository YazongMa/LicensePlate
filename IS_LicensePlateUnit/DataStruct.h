
#ifndef __DATASTRUCT_H__
#define __DATASTRUCT_H__

#include "Common/Common.h"
#include "Common/StopWatch.h"
#include <iostream>
#include <deque>
#include <map>


//�ɼ���ʹ��-->֡���ݸ�ʽ
class CFrameInfo
{
public:
	//���캯��
	CFrameInfo(int length, int width, int height);

	//��������
	~CFrameInfo();

	//��ֵ����
	CFrameInfo& operator=(const CFrameInfo& rh);

	//�ɼ�֡���
	long long operator-(const CFrameInfo& rh);

	//��ȡ��ǰʱ��
	static void getTime(SYSTEMTIME* pTime);

public:
	int					nFormat;				//֡����
	int					nLength;				//֡��С
	int					nWidth;					//֡���
	int					nHeigth;				//֡�߶�
	unsigned char*		pbyData;				//֡����
	unsigned long long	nId;					//֡��
	SYSTEMTIME			sysTime;				//�ɼ�ʱ��
};

typedef std::deque<CFrameInfo *>					DeqFrameInfo_t;
typedef std::deque<CFrameInfo *>::iterator			DeqFrameInfoItr_t;


//����-->���ṹ, �ͻ������������ṹ
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
	char			m_chDeviceId[DEVID_MAX_SIZE];		//�ɼ��豸��ʶ
	SYSTEMTIME		m_ftTime;							//�ɼ�ʱ��
	__int64			m_nFrameId;							//֡��
	int				m_nFormat;							//֡���ݸ�ʽ �ο�enumFrameFormat
	int				m_nFrameSize;						//֡��С
	int				m_nFrameWidth;						//֡���
	int				m_nFrameHeigth;						//֡�߶�
	int             m_nFaceNum;
	__RECT         FaceRect[MAXFACE];
}UDPFrameData;
// Shared Memory Frame End

#endif
