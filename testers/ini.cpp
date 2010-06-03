#ifndef IN_IDE
#define UNICODE
#define _UNICODE
#endif
#include "iostream"
#include "../libxl/include/ini.h"

#ifdef IN_IDE
int test_ini(int argc, char **argv) {
	xl::tstring ts = _T("0804.ini");
#else
int main(int argc, char **argv) {
	if (argc != 2) {
		std::cout << "useage: ini <xxx.ini>" << std::endl;
		return -1;
	}

	xl::tstring ts = xl::s2ts(argv[1]);
#endif
	xl::CIni ini(ts);

	const xl::tstring sct = _T("");
	for (auto it = ini.begin(sct); it != ini.end(sct); ++ it) {
		xl::string k = xl::ts2s(it->first);
		xl::string v = xl::ts2s(it->second);
		std::cout << k << "=" << v << std::endl;
	}

	ini.set(_T("person"), _T("name"), _T("cyberscorpio"));
	ini.write();
	return 0;
}

