#ifndef __VIDEOPROC_H__
#define __VIDEOPROC_H__

#include "Common\Common.h"
#include "DataStruct.h"

class CVideoProc : public ThreadHelper
{
public:
	CVideoProc(int channel_id, int channel_num);
	virtual ~CVideoProc();

	virtual void run();

	int		init(Func_Frame pFunc_Frame, void* pCtx);
	void	pause();
	void	resume();

private:
	int		check(char* url, char* path, int type);

private:
	Func_Frame	m_pFuncFrame;
	void*		m_pContext;

	int			m_nVideoWidth;
	int			m_nVideoHeight;

	int			m_nChnlID;
	char		m_nChnlName[100];
	static int	m_nChnlNum;
};

#endif