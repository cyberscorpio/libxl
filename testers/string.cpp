#include <iostream>
#include "../libxl/string.h"


static const char *strings[] = {
	"abcabcabc",
	"abccabcabc",
	"abcabcabc",
	"abcdefghijklmn",
};
static const char *searches[] = {
	"bc",
	"bc",
	"a",
	"abcdefghijklmn",
};
static const char *replaces[] = {
	"de",
	"ab",
	"b",
	"12345",
};
static int counts[] = {
	-1,
	-1,
	2,
	4,
};
static const char *results[] = {
	"adeadeade",
	"aabcaabaab",
	"bbcbbcabc",
	"12345",
};

#ifdef IN_IDE
int test_string(int argc, char **argv) {
#else
int main(int argc, char **argv) {
#endif
	std::cout << "1. test string::replace()..." << std::endl;
	for (int i = 0; i < COUNT_OF(strings); ++ i) {
		xl::string s = strings[i];
		s.replace(searches[i], replaces[i], counts[i]);
		std::cout << "case " << i + 1 << std::endl;
		if (s != results[i]) {
			std::cout << "replace (" << searches[i] << ") to ("
				<< replaces[i] << ") " << counts[i] << " times"
				<< std::endl << "get " << s << std::endl
				<< "but expect: " << results[i] << std::endl;
		} else {
			std::cout << "success!" << std::endl;
		}
	}

	return 0;
}