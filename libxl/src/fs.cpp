#include <assert.h>
#include <stdio.h>
#include <limits>
#include <utility>
#include <windows.h>
#ifdef max // <windows.h> sucks
#undef max
#undef min
#endif
#include "../include/fs.h"

#if (_MSC_VER >= 1500)
#pragma warning (disable:4996)
#endif

//////////////////////////////////////////////////////////////////////////
// static variables
static const char dirsepA = '\\';
static const wchar_t dirsepW = '\\';


//////////////////////////////////////////////////////////////////////////
// static functions
static bool _file_get_contents(FILE *file, std::string &data, size_t offset) {
	assert (file != NULL);

	bool result = false;
	__int64 length = xl::file_get_length(file);
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

	return result;
}


namespace xl {

bool file_existsA (const string &filename) {
	FILE *file = fopen(filename, "r");
	if (file != NULL) {
		fclose(file);
	}
	return file != NULL;
}

bool file_existsW (const wstring &filename) {
	FILE *file = _wfopen(filename, L"r");
	if (file != NULL) {
		fclose(file);
	}
	return file != NULL;
}


string file_get_absolute_nameA (const string &filename) {
	assert (*filename != '.'); // don't support ../ and ./
	bool relative = true;
	if (filename.length() >= 2 && filename.at(1) == ':') {
		relative = false;
	}
	string a = filename;
	if (relative) {
		char d[MAX_PATH];
		::GetCurrentDirectoryA(MAX_PATH, d);
		a = d;
		a += '\\';
		a += filename;
	}
	return a;
#ifndef WIN32
#error implement it!
#endif
}

wstring file_get_absolute_nameW (const wstring &filename) {
	assert (*filename != L'.'); // don't support ../ and ./
	bool relative = true;
	if (filename.length() >= 2 && filename.at(1) == L':') {
		relative = false;
	}
	wstring a = filename;
	if (relative) {
		wchar_t d[MAX_PATH];
		::GetCurrentDirectoryW(MAX_PATH, d);
		a = d;
		a += L'\\';
		a += filename;
	}
	return a;
#ifndef WIN32
#error implement it!
#endif
}



/**
 * @brief return the file length
 * @param file returned by fopen()
 * @return the file length
 */
__int64 file_get_length(FILE *file) {
	assert (file != NULL);
	__int64 pos = _ftelli64(file);
	
	fseek(file, 0, SEEK_END);
	__int64 length = _ftelli64(file);
	
	_fseeki64(file, pos, SEEK_SET);

	return length;
}

__int64 file_get_lengthA (const string &filename) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		return -1;
	}
	__int64 length = file_get_length(file);
	fclose(file);
	return length;
}

__int64 file_get_lengthW (const wstring &filename) {
	FILE *file = _wfopen(filename, L"r");
	if (!file) {
		return -1;
	}
	__int64 length = file_get_length(file);
	fclose(file);
	return length;
}


string file_get_directoryA (const string &filename, bool mustExist) {
	assert (!mustExist || file_existsA(filename));
	string fullname = file_get_absolute_nameA(filename);
	size_t offset = fullname.find_last_of(dirsepA);
	if (offset == fullname.npos) {
		offset = fullname.find_last_of('/');
	}
	string d;
	if (offset != fullname.npos) {
		d = fullname.substr(0, offset);
		assert (!mustExist || (GetFileAttributesA(fullname) & FILE_ATTRIBUTE_DIRECTORY));
	}
	return d;
}

wstring file_get_directoryW (const wstring &filename, bool mustExist) {
	assert (!mustExist || file_existsW(filename));
	wstring fullname = file_get_absolute_nameW(filename);
	size_t offset = fullname.find_last_of(dirsepW);
	if (offset == fullname.npos) {
		offset = fullname.find_last_of(L'/');
	}
	wstring d;
	if (offset != fullname.npos) {
		d = fullname.substr(0, offset);
		assert (!mustExist || (GetFileAttributesW(fullname) & FILE_ATTRIBUTE_DIRECTORY));
	}
	return d;
}


/**
 * @brief put the whole content of data into the file indicated by filename.
 * @note all content in the file will be destroyed.
 */
// ascii version
int file_put_contentsA(const string &filename, const std::string &data) {
	FILE *file = fopen(filename, "w+b");
	if (!file) {
		return -1;
	}
	int written = (int)fwrite(data.c_str(), 1, data.length(), file);
	fclose(file);
	return written;
}

// wchar_t version
int file_put_contentsW(const wstring &filename, const std::string &data) {
	FILE *file = _wfopen(filename, L"w+b");
	if (!file) {
		return -1;
	}
	int written = (int)fwrite(data.c_str(), 1, data.length(), file);
	fclose(file);
	return written;
}


bool file_get_contentsA (const string &filename, std::string &data, size_t offset) {
	FILE *file = fopen(filename, "rb");
	if (!file) {
		return false;
	}

	bool result = _file_get_contents(file, data, offset);

	fclose(file);

	return result;
}

bool file_get_contentsW (const wstring &filename, std::string &data, size_t offset) {
	FILE *file = _wfopen(filename, L"rb");
	if (!file) {
		return false;
	}

	bool result = _file_get_contents(file, data, offset);

	fclose(file);

	return result;
}




}
