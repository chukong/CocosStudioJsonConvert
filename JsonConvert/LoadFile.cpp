#include "LoadFile.h"

#define READ_BLOCK ( 64 << 10 )		// I每次读取块大小(64KB)
#define MAX_IO_RUN 4				// IOCP数量
#define IOCP_THRESHOLD	( 1 << 20 ) // 使用IOCP阀值(1MB)

// 取得某一文件大小。
bool GetFileSize( const char *szFile, size_t *p_szFile )
{
	if ( NULL == p_szFile || NULL == szFile )
	{
		return false;
	}

	HANDLE hFile = CreateFileA( szFile,
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								0,
								NULL );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		*p_szFile = 0;
		return false;
	}

	LARGE_INTEGER FileSize = { 0 };
	if ( FALSE == ::GetFileSizeEx( hFile, &FileSize ) )
	{
		*p_szFile = 0;
		CloseHandle( hFile );

		return false;
	}
	
	// 内存大小对齐
	LARGE_INTEGER Remaider = { 0 }; 
	Remaider.QuadPart = FileSize.QuadPart & ( READ_BLOCK - 1);
	if ( Remaider.QuadPart > 0 )
	{
		FileSize.QuadPart = FileSize.QuadPart - Remaider.QuadPart + READ_BLOCK;
	}

	*p_szFile = static_cast< size_t >( FileSize.QuadPart );
	CloseHandle( hFile );

	return true;
}

// 加载文件数据。
bool LoadFile( const char *szFile, void *pBuf, size_t szBuf, size_t *p_szLoaded, unsigned int flag/* = 0*/ )
{
	if ( NULL == szFile || NULL == pBuf || 0 == szBuf )
	{
		return false;
	}

	if ( 0 == flag )
	{
		HANDLE hFile = CreateFileA( szFile,
									GENERIC_READ,
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									0,
									NULL );
		
		if ( INVALID_HANDLE_VALUE == hFile )
		{
			if ( NULL != p_szLoaded )
			{
				*p_szLoaded = 0;
			}

			return false;
		}

		DWORD dwRead = 0;
		ReadFile( hFile, pBuf, static_cast< DWORD >( szBuf ), &dwRead, NULL );
		if ( NULL != p_szLoaded )
		{
			*p_szLoaded = static_cast< size_t >( dwRead );
		}
		CloseHandle( hFile );
	}
	else
	{
		HANDLE hFile = CreateFileA( szFile,
									GENERIC_READ,
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
									NULL );
		if ( INVALID_HANDLE_VALUE == hFile )
		{
			return false;
		}

		HANDLE hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
		if ( INVALID_HANDLE_VALUE == hIOCP )
		{
			CloseHandle( hFile );
			return false;
		}

		if ( hIOCP != CreateIoCompletionPort( hFile, hIOCP, 1, 0 ) )
		{
			CloseHandle( hIOCP );
			CloseHandle( hFile );

			return false;
		}

		OVERLAPPED iocp[MAX_IO_RUN];
		memset( iocp ,0,sizeof(iocp));
		LARGE_INTEGER read_offset = { 0 };
		LARGE_INTEGER szFile = { static_cast< LONGLONG >( szBuf ) };
		int read_progress = 0;

		for ( int i = 0; i < MAX_IO_RUN; ++ i )
		{
			read_progress ++;
			PostQueuedCompletionStatus( hIOCP, 0, 1, &iocp[i] );
		}

		BYTE *pBuf2 = reinterpret_cast< BYTE* >( pBuf );
		ULONG_PTR CompletionKey;
		DWORD dwNumBytes;
		OVERLAPPED *p_iocp;
		while ( read_progress > 0 )
		{
			HRESULT hr = GetQueuedCompletionStatus( hIOCP, &dwNumBytes, &CompletionKey, &p_iocp, INFINITE );

			if ( 1 == CompletionKey )
			{
				-- read_progress;
				if ( read_offset.QuadPart < szFile.QuadPart )
				{
					p_iocp->Offset = read_offset.LowPart;
					p_iocp->OffsetHigh = read_offset.HighPart;

					ReadFile( hFile, pBuf2, READ_BLOCK, NULL, p_iocp );
					read_offset.QuadPart += READ_BLOCK;
					pBuf2 += READ_BLOCK;

					++ read_progress;
				}
			}
		}

		CloseHandle( hIOCP );
		CloseHandle( hFile );
	}

	return true;
}