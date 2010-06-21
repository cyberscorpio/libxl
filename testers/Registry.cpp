#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <iostream>
#include "../libxl/include/Registry.h"

//////////////////////////////////////////////////////////////////////////
// compile:
// cl -c /EHsc Registry.cpp
// link Registry.obj ..\Release\libxl.lib


#ifdef IN_IDE
int test_registry(int argc, char **argv) {
#else
int main(int argc, char **argv) {
#endif
	xl::tstring keyName = _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\foobar2000\\capabilities");
	// xl::tstring keyName = _T("HKCU\\SOFTWARE\\foobar2000");
	xl::tstring valueName = _T("applicationDescription");
	xl::tstring value;
	if (xl::CRegistry::getStringValue(keyName, valueName, value)) {
		xl::string s = xl::ts2s(value);
		std::cout << s << std::endl;
	} else {
		std::cout << "read registry error\n";
	}

	valueName = _T("xxx");
	if (xl::CRegistry::setValue(keyName, valueName, _T("keep it simple and stupid"))) {
		std::cout << "set value success\n";
	} else {
		std::cout << "set value error\n";
	}
	return 0;
}
