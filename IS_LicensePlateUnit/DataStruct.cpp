
#include "DataStruct.h"
#include <fstream>

//构造函数
CFrameInfo::CFrameInfo(int length, int width, int height) 
	: 
nFormat(3), 
nLength(length), 
nWidth(width), 
nHeigth(height), 
pbyData(NULL), 
nId(0)
{
	getTime(&sysTime);
	if(nLength > 0)
	{
		try
		{
			pbyData = new unsigned char[nLength];
		}
		catch(const std::bad_alloc& e)
		{
			//...
			std::cout << e.what() << std::endl;
		}
		memset((void *)pbyData, 0, nLength * sizeof(unsigned char));
	}
}

//析构函数
CFrameInfo::~CFrameInfo()
{
	if(pbyData)
	{
		delete[] pbyData;
		pbyData = NULL;
	}
}

//赋值函数
CFrameInfo&
CFrameInfo::operator=(const CFrameInfo& rh)
{
	if(this == &rh)
		return *this;

	nFormat	= rh.nFormat;
	nLength = rh.nLength;
	nWidth  = rh.nWidth;
	nHeigth = rh.nHeigth;
	nId		= rh.nId;

	if(pbyData)
	{
		delete[] pbyData;
		pbyData = NULL;
	}

	try
	{
		pbyData = new unsigned char[nLength];
	}
	catch (const std::bad_alloc& e)
	{
		//...
		std::cout << e.what() << std::endl;
	}
	memset((void *)pbyData, 0, sizeof(unsigned char) * nLength);
	memcpy((void *)pbyData, (void *)rh.pbyData, sizeof(unsigned char) * nLength);

	memset((void *)&sysTime, 0, sizeof(SYSTEMTIME));
	memcpy((void *)&sysTime, (void *)&rh.sysTime, sizeof(SYSTEMTIME));
	return *this;
}

//采集帧间隔
long long 
CFrameInfo::operator-(const CFrameInfo& rh)
{
	return nId - rh.nId;
}

//获取当前时间
void
CFrameInfo::getTime(SYSTEMTIME* pTime)
{
	if(!pTime)
		return;
#ifndef _WIN32
	timeval tv;
	gettimeofday(&tv, NULL);

	struct tm* tm		= localtime(&tv.tv_sec);
	pTime->wYear			= tm->tm_year + 1900;
	pTime->wMonth		= tm->tm_mon + 1;
	pTime->wDay			= tm->tm_mday;
	pTime->wDayOfWeek	= tm->tm_wday;
	pTime->wHour			= tm->tm_hour;
	pTime->wMinute		= tm->tm_min;
	pTime->wSecond		= tm->tm_sec;
	pTime->wMilliseconds= tv.tv_usec / 1000;
#else
	::GetLocalTime(pTime);
#endif
}

