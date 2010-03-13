#include <iostream>
#include "../libxl/include/fs.h"

//////////////////////////////////////////////////////////////////////////
// compile:
// cl -c /EHsc fs.cpp
// link fs.obj ..\Release\libxl.lib


#ifdef IN_IDE
int test_fs(int argc, xl::tchar **argv) {
#else
int main(int argc, char **argv) {
#endif
	if (argc != 2) {
		std::cout << "usage: fs <file>" << std::endl;
		return -1;
	}

	xl::tchar *filename = argv[1];
	if (!xl::file_exists(filename)) {
		std::cout << filename << " doesn't exists" << std::endl;
		return -1;
	}

	int length = (int)xl::file_get_length(filename);
	std::cout << "length of " << filename << " is " << length << " bytes" << std::endl;

	std::string data;
	xl::file_get_contents(filename, data);

	xl::tstring tempname = filename;
	tempname += _T(".temp");
	xl::file_put_contents(tempname, data);

	std::string temp;
	xl::file_get_contents(tempname, temp);

	_tunlink(tempname);

	if (data.length() != temp.length()
	    || memcmp(data.c_str(), temp.c_str(), data.length()) != 0
	   ) {
		std::cout << "put and get make an error (can't match)!" << std::endl;		
	} else {
		std::cout << "put and get matched, and ALL are OK!" << std::endl;
	}

	xl::tstring fullname = xl::file_get_absolute_name(filename);
	std::cout << "fullname of " << filename << " is:" << std::endl
		<< "\t" << fullname << std::endl;

	xl::tstring directory = xl::file_get_directory(filename);
	std::cout << "directory of " << filename << " is:" << std::endl
		<< "\t" << directory << std::endl;

	return 0;
}
