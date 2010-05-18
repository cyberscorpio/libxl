#include <iostream>
#include "../libxl/include/Registry.h"

//////////////////////////////////////////////////////////////////////////
// compile:
// cl -c /EHsc Registry.cpp
// link Registry.obj ..\Release\libxl.lib


#ifdef IN_IDE
int test_registry(int argc, xl::tchar **argv) {
#else
int main(int argc, char **argv) {
#endif
	if (!xl::registerProgid(_T("test_registry.1.0"), _T("open"))) {
		std::cout << "register failed!" << std::endl;
	} else {
		std::cout << "register succeed!" << std::endl;
	}
	return 0;
}
