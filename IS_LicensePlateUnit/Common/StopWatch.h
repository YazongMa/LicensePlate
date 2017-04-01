#ifndef __STOPWATCH_H__
#define __STOPWATCH_H__

#include <windows.h>

class CStopWatch
{
public:
	CStopWatch(void)
	{
		memset((void *)&m_liPerfFreq, 0, sizeof(LARGE_INTEGER));
		memset((void *)&m_liPerfStart, 0, sizeof(LARGE_INTEGER));
		QueryPerformanceFrequency(&m_liPerfFreq);
	}
	~CStopWatch(void)   {}
public:
	void Start(void)    {   QueryPerformanceCounter(&m_liPerfStart);    }

	// Returns # of milliseconds since Start was called
	__int64 Now(void) const 
	{
		LARGE_INTEGER   liPerfNow   = {0};

		QueryPerformanceCounter(&liPerfNow);

		return (((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000) / m_liPerfFreq.QuadPart);
	}
private:
	LARGE_INTEGER   m_liPerfFreq;   // Counts per second
	LARGE_INTEGER   m_liPerfStart;  // Starting count
};

#endif
