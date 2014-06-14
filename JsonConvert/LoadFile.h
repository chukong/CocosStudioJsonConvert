#ifndef _LOADFILE_H
#define _LOADFILE_H

#include <stdio.h>
#include <windows.h>

// 读取一个数据块
inline void		ReadMemoryData(BYTE* pReadData, int nMaxSize, DWORD& uOffSize, void* pOutData, int nReadSize)
{
	if (uOffSize + nReadSize <= 0 || uOffSize + nReadSize > nMaxSize)
	{
		// 异常抛出
		throw;
	}
	memcpy(pOutData, &pReadData[uOffSize], nReadSize);
	uOffSize += nReadSize;
}

// 取得某一文件大小。
bool GetFileSize( const char *szFile, size_t *p_szFile );
// 加载文件数据。
bool LoadFile( const char *szFile, void *pBuf, size_t szBuf, size_t *p_szLoaded, unsigned int flag = 0 );

#endif