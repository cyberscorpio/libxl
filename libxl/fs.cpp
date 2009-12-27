#include <assert.h>
#include <stdio.h>
#include <limits>
#include <utility>
#include "fs.h"

#if (_MSC_VER >= 1500)
#pragma warning (disable:4996)
#endif

namespace xl {

__int64 file_get_length(FILE *file) {
	assert (file != NULL);
	__int64 pos = _ftelli64(file);
	
	fseek(file, 0, SEEK_END);
	__int64 length = _ftelli64(file);
	
	_fseeki64(file, pos, SEEK_SET);

	return length;
}
	
int file_put_contents(const tstring &filename, const std::string &data) {
	FILE *file = _tfopen(filename, _T("w+b"));
	if (!file) {
		return -1;
	}
	int written = (int)fwrite(data.c_str(), 1, data.length(), file);
	fclose(file);
	return written;
}

bool file_get_contents(const tstring &filename, std::string &data, size_t offset) {
	FILE *file = _tfopen(filename, _T("rb"));
	if (!file) {
		return false;
	}

	bool result = false;
	__int64 length = file_get_length(file);
	if (length > 0 && length < std::numeric_limits<int>::max()) {
		int size = (int)length - offset;
		char *p = new char [size];
		if (p) {
			if (offset > 0) {
				fseek(file, offset, SEEK_SET);
			}

			if (fread(p, 1, size, file) == size) {
				std::string tmp(p, size);
				std::swap(tmp, data);
				result = true;
			}
			delete []p;
		}
	} else if (length == 0) {
		result = true;
		data = "";
	} else {
		// error or too large
	}

	fclose(file);
	return result;
}




}