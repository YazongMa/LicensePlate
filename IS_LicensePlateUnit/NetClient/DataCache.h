#pragma once

#include <map>
#include <Windows.h>

#define	DATA_CACHE_WORKPATH				_T("DataCache")
#define	DATA_CACHE_FILE_EXT				_T("datacache")

//文件命名方式为：20位前缀由0填充的数值+".datacache"
#define	DATA_CACHE_FILE_PADDING_LEN		20

//文件内容的组织方式为long long的DataId+int的DataSize+DataSize的BYTE数据
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

/*功能: 取最新的缓存数据
  返回: >0 数据ID; =0 表示无数据; <0 表示取异常
  注意: 该函数需要与 DC_DeleteData 函数搭配使用; 需要外部控制互斥, 否则返回的数据指针可能被破坏*/
__int64 DC_GetLastData(unsigned char **pbyData, int *pDataSize);

bool 	DC_DeleteData(long long llDataId, int nRet);

bool 	DC_AddData(unsigned char *pbyData, int nDataSize);

long	DC_GetCacheCnt();
