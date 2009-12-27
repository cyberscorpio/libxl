/**
 * @brief wrapper for file system relative calls
 */
#ifndef XL_FS_H
#define XL_FS_H
#include "common.h"
#include "string.h"

namespace xl {

__int64 file_get_length(FILE *);

/**
 * file_put_contents and file_get_contents are borrowed from PHP, they are very handy functions.
 */
/**
 * @brief write the total std::string into the file.
 * @return the count of bytes written. -1 indicate an error.
 */
int file_put_contents(const tstring &filename, const std::string &data);

/**
 * @brief read the total file contents into a std::string.
 * @param offset if set offset, read the file content from offset.
 * @note don't get content of a very large file.
 */
bool file_get_contents(const tstring &filename, std::string &data, size_t offset = 0);


}


#endif
