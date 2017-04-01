#ifndef __IDENTIFICATION_H__
#define __IDENTIFICATION_H__

#include "Common/Common.h"
#include <vector>

class CLicensePlateUnit : public ThreadHelper
{
public:
	CLicensePlateUnit();
	~CLicensePlateUnit();

	int		Start();
	void	Stop();

	// 继承自ThreadObj的函数重写
	virtual	void run();

private:
	LockHelper			m_Lock;
	std::vector<int>	m_vecChnl;
};


#endif