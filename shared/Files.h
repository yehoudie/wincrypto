#ifndef SHARED_FILES_H
#define SHARED_FILES_H

#include <windows.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#if defined(Win64) || defined(_WIN64)
#define fseek(f, o, t) _fseeki64(f, o, t)
#define ftell(s) _ftelli64(s)
#define wstat(p,b) _wstat64(p,b)
#define stat(p,b) _stat64(p,b)
//#define struct stat struct __stat64 
#else
#define wstat(p,b) _wstat32(p,b)
#endif

//bool FOLLOW_LINKS;
//bool SKIP_HIDDEN_DIRS;
//bool SKIP_HIDDEN_FILES;

/*
 * Crop trailing slash of a path.
 *
 * @param	path WCHAR* the path
 */
void cropTrailingSlash(WCHAR* path);

/**
* Get file size the POSIX way in bytes.
* Does not open the file.
* Faster than the fstream method.
*
* @param	path WCHAR* the file source
* @return	uint32_t the file size
*/
size_t getFileSize(WCHAR* file);

/**
* Get file size the POSIX way in bytes.
* Does not open the file.
* Faster than the fstream method.
*
* @param	path CHAR* the file source
* @return	uint32_t the file size
*/
size_t getFileSizeA(CHAR* file);

/**
* Check if a file exists.
*
* @param path
* @return
*/
bool fileExists(WCHAR* path);

/**
 * Check if a dir exists.
 *
 * @param path
 * @return
 */
bool dirExists(WCHAR* path);

bool checkPath(WCHAR* path, int is_dir);

#endif
