/**
 * @brief wrapper for file system relative calls
 */
#ifndef XL_FS_H
#define XL_FS_H
#include "common.h"
#include "string.h"

namespace xl {

bool file_existsA (const string &filename);
bool file_existsW (const wstring &filename);
inline bool file_exists (const tstring &filename) {
#ifdef UNICODE
	return file_existsW(filename);
#else
	return file_existsA(filename);
#endif
}

__int64 file_get_length (FILE *);
__int64 file_get_lengthA (const string &filename);
__int64 file_get_lengthW (const wstring &filename);
inline __int64 file_get_length (const tstring &filename) {
#ifdef UNICODE
	return file_get_lengthW(filename);
#else
	return file_get_lengthA(filename);
#endif
}

//////////////////////////////////////////////////////////////////////////
// file_put_contents and file_get_contents are borrowed from PHP, they are very handy functions.
/**
 * @brief write the total std::string into the file.
 * @return the count of bytes written. -1 indicate an error.
 */
int file_put_contentsA (const string &filename, const std::string &data);
int file_put_contentsW (const wstring &filename, const std::string &data);
inline int file_put_contents (const tstring &filename, const std::string &data) {
#ifdef UNICODE
	return file_put_contentsW(filename, data);
#else
	return file_put_contentsA(filename, data);
#endif
}

/**
 * @brief read the total file contents into a std::string.
 * @param offset if set offset, read the file content from offset.
 * @note don't get content of a very large file.
 */
bool file_get_contentsA (const string &filename, std::string &data, size_t offset = 0);
bool file_get_contentsW (const wstring &filename, std::string &data, size_t offset = 0);
inline bool file_get_contents (const tstring &filename, std::string &data, size_t offset = 0) {
#ifdef UNICODE
	return file_get_contentsW(filename, data, offset);
#else
	return file_get_contentsA(filename, data, offset);
#endif
}


}


#endif
