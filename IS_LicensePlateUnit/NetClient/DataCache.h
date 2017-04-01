#pragma once

#include <map>
#include <Windows.h>

#define	DATA_CACHE_WORKPATH				_T("DataCache")
#define	DATA_CACHE_FILE_EXT				_T("datacache")

//�ļ�������ʽΪ��20λǰ׺��0������ֵ+".datacache"
#define	DATA_CACHE_FILE_PADDING_LEN		20

//�ļ����ݵ���֯��ʽΪlong long��DataId+int��DataSize+DataSize��BYTE����
#define DATA_CACHE_FILE_HEADER_LEN		(sizeof(long long) + sizeof(int))
#define	DATA_CACHE_FILE_DATAID_LEN		sizeof(long long)


typedef std::map<long long, long long>				mapDataCacheIndex;
typedef std::map<long long, long long>::iterator	mapDataCacheIndexIt;
typedef std::map<long long, BYTE *>					mapDataCache;
typedef std::map<long long, BYTE *>::iterator		mapDataCacheIt;

typedef struct
{
	TCHAR				m_szWorkPath[MAX_PATH];
	LONGLONG			m_llDataSeq;
	mapDataCacheIndex	m_mapDataIndex;
	mapDataCache		m_mapDataCache;

}DataCacheContext;

bool 	DC_Init(char *pchPath, char *pchChnl, int nMaxDataCacheNum);

void 	DC_Uninit();

/*����: ȡ���µĻ�������
  ����: >0 ����ID; =0 ��ʾ������; <0 ��ʾȡ�쳣
  ע��: �ú�����Ҫ�� DC_DeleteData ��������ʹ��; ��Ҫ�ⲿ���ƻ���, ���򷵻ص�����ָ����ܱ��ƻ�*/
__int64 DC_GetLastData(unsigned char **pbyData, int *pDataSize);

bool 	DC_DeleteData(long long llDataId, int nRet);

bool 	DC_AddData(unsigned char *pbyData, int nDataSize);

long	DC_GetCacheCnt();
