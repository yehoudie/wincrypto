#include <stdio.h>
#include <direct.h>

#include "debug.h"
#include "filesW.h"
#include "Fifo.h"



bool actOnFilesInDir(const WCHAR* dir, FileCallback cb, const char** types, bool recursive)
{
	debug_winfo(L"actOnFilesInDir(%s)\n", dir);
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAW ffd;
	wchar_t* mask = L"*";
	wchar_t spec[MAX_PATH];
	wchar_t* path;
	Fifo directories;
	bool s = true;
	PFifoEntry entry;
	(void)types;

	//cropTrailingSlash(dir);

	Fifo_init(&directories);
	Fifo_push(&directories, dir, (size_t)wcslen(dir)*2+2);

	while (!Fifo_empty(&directories))
	{
		entry = Fifo_front(&directories);
		path = (WCHAR*)entry->value;

		debug_winfo(L" - path: %ws\n", path);
		memset(spec, 0, MAX_PATH * 2);
		wsprintfW(spec, L"%ws\\%ws", path, mask);
		spec[MAX_PATH - 1] = 0;
		debug_winfo(L" - spec: %s\n", spec);

		hFind = FindFirstFileW(spec, &ffd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			wprintf(L" - ERROR (0x%x): hfinf\n", GetLastError());
			s = false;
			break;
		}
		do
		{
			if (wcscmp(ffd.cFileName, L".") != 0 &&
				wcscmp(ffd.cFileName, L"..") != 0)
			{
				memset(spec, 0, MAX_PATH * 2);
				debug_winfo(L" - - path: %ws\n", path);
				debug_winfo(L" - - ffd.cFileName: %ws\n", ffd.cFileName);
				wsprintfW(spec, L"%ws\\%s", path, ffd.cFileName);
				spec[MAX_PATH - 1] = 0;
				if ( (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && recursive)
				{
					debug_winfo(L" - - dir: %ws\n", spec);
					s = (bool)Fifo_push(&directories, spec, (size_t)wcslen(spec) * 2 + 2);
					debug_winfo(L" - - fifo size: %u\n", s);
					if (s == 0)
					{
						printf("Fifo push error!\n");
						break;
					}
				}
				else
				{
					debug_winfo(L" - - file: %ws\n", spec);
					cb(spec);
				}
			}
		}
		while (FindNextFileW(hFind, &ffd) != 0);

		if (GetLastError() != ERROR_NO_MORE_FILES)
		{
			FindClose(hFind);
			s = true;
			break;
		}

		Fifo_pop_front(&directories);

		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}
	
	Fifo_destroy(&directories);

	return s;
}

BOOL getFullPathName(WCHAR* src, WCHAR* full_path, WCHAR** base_name)
{
	int fpl = GetFullPathNameW(src, MAX_PATH, full_path, base_name);
	if (!fpl)
	{
		wprintf(L"ERROR (0x%lx): Get full path failed for \"%ws\".", GetLastError(), src);
		return FALSE;
	}
	if ( *base_name == NULL && full_path[fpl - 1] == '\\' )
	{
		full_path[fpl - 1] = 0;
	}
	return TRUE;
}

int mkdir_p(const WCHAR* dir)
{
	//		fs::create_directories(dir);
	//BOOL CreateDirectoryA(
	//	LPCSTR                lpPathName,
	//	LPSECURITY_ATTRIBUTES lpSecurityAttributes
	//);
	const WCHAR* path = dir;
	const size_t len = wcslen(path);
	WCHAR _path[MAX_PATH];
	WCHAR* p;
	int errsv;

	errno = 0;

	/* Copy WCHAR* so its mutable */
	if (len > sizeof(_path) - 1)
	{
		errno = ENAMETOOLONG;
		return -1;
	}
	errsv = wcscpy_s(_path, MAX_PATH, path);
	if (errsv != 0)
	{
		wprintf(L"ERROR (0x%x): wcscpy_s!\n", errsv);
		return -1;
	}

	/* Iterate the WCHAR* */
	for (p = _path + 1; *p; p++)
	{
		if (*p == '/')
		{
			/* Temporarily truncate */
			*p = '\0';

			if (_wmkdir(_path) != 0)
			{
				if (errno != EEXIST)
					return -1;
			}

			*p = '/';
		}
	}

	if (_wmkdir(_path) != 0)
	{
		if (errno != EEXIST)
			return -1;
	}

	return 0;
}
