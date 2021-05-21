//#include <unistd.h>

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "Files.h"



void cropTrailingSlash(WCHAR* path)
{
	size_t n = wcslen(path);
	if (n == 0)
		return;
	if ( path[n-1] == '/' )
		path[n - 1] = 0;
}

size_t getFileSize(WCHAR* path)
{
#if defined(_WIN64)
	struct _stat64 stat_buf;
#elif defined(_WIN32)
	struct _stat32 stat_buf;
#else
	struct stat stat_buf;
#endif
	memset(&stat_buf, 0, sizeof(stat_buf));
	errno = 0;
	int rc = wstat(path, &stat_buf);
	//int errsv = errno;
	if ( rc == 0 )
		return stat_buf.st_size;
	else
		return 0;
}

size_t getFileSizeA(CHAR* path)
{
#if defined(_WIN64)
	struct _stat64 stat_buf;
#else
	struct stat stat_buf;
#endif
	memset(&stat_buf, 0, sizeof(stat_buf));
	errno = 0;
	int rc = stat(path, &stat_buf);
	//int errsv = errno;
	if ( rc == 0 )
		return stat_buf.st_size;
	else
		return 0;
}

bool fileExists(WCHAR* path)
{
#if defined(_WIN64)
	struct _stat64 s;
#elif defined(_WIN32)
	struct _stat32 s;
#else
	struct stat s;
#endif
	if (wstat(path, &s) == 0 )
	{
		if ( s.st_mode & S_IFREG )
			return true;
	}

	return false;
}

bool dirExists(WCHAR* path)
{
#if defined(_WIN64)
	struct _stat64 s;
#elif defined(_WIN32)
	struct _stat32 s;
#else
	struct stat s;
#endif
	if (wstat(path, &s) == 0 )
	{
		if ( s.st_mode & S_IFDIR )
			return true;
	}

	return false;
}

bool checkPath(WCHAR* path, int is_dir)
{
	if (is_dir)
		return dirExists(path);
	else
		return fileExists(path);
}
