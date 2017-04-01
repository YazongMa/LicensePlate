#ifndef __LICENSEPROC_H__
#define __LICENSEPROC_H__

#include "Common/Common.h"
#include <opencv2/opencv.hpp>

typedef struct __pushType
{
	cv::Mat		frame;
	int			chnl_id;
	char		chnl_name[100];

	__pushType(cv::Mat& mat, int id, char* name) : frame(mat), chnl_id(id)
	{
		memset(chnl_name, 0, sizeof(char) * 100);
		memcpy(chnl_name, name, strlen(name));
	}
private:
	__pushType& operator=(const __pushType& r);
}pushType;

class CLicenseProc : public ThreadHelper
{
public:
	static CLicenseProc* Instance();
	virtual void run();

	void	init();
	void	push_back(pushType* pushData);
private:
	CLicenseProc();
	virtual ~CLicenseProc();
};

#endif
