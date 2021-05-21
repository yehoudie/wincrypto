#ifndef SHARED_FILES_W_H
#define SHARED_FILES_W_H

#include <windows.h>
#include <winternl.h>

#include "Files.h"



#define PATH_SEPARATOR 0x5C



typedef void (*FileCallback)(WCHAR*);
//typedef bool (*Condition)(WCHAR*);



/**
* Find files in directory with specified file_type and call back on each file.
* Links will be followed, unless FileUtil::FOLLOW_LINKS is set to false.
* Hidden directories may be skipped by setting FileUtil::SKIP_HIDDEN_DIRS.
* Hidden files may be skipped by setting FileUtil::SKIP_HIDDEN_FILES.
*
* @param	dir WCHAR* the directory to search
* @param	cb FileCallback the callback(WCHAR*) called on each found file
* @param	types char** A white list of file types to search for
* @param	recursive bool do a "recursive" search including all subdirectories
*/
bool actOnFilesInDir(
	const WCHAR* dir, 
	FileCallback cb, 
	const char** types, 
	bool recursive
);

BOOL getFullPathName(WCHAR* src, WCHAR* full_path, WCHAR** base_name);

/**
 * Make dirs recursively.
 * From: https://gist.github.com/JonathonReinhart/8c0d90191c38af2dcadb102c4e202950
 *
 * @param	dir WCHAR* the dir path
 */
int mkdir_p(const WCHAR* path);

#endif
