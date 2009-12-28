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

static const char *trims[] = {
	"   abc   ",
	"abc   ",
	"abc",
	"     abc",
	"       ",
};

static const char *trimresults[] = {
	"abc",
	"abc",
	"abc",
	"abc",
	"",
};

static const char *charlist = "\t&* .";
static const char *multitrims[] = {
	"\t&*abc***.",
	"abc \t.&*",
	"abc",
	"&***abc",
	"&****&\t",
};

static const char *multitrimresults[] = {
	"abc",
	"abc",
	"abc",
	"abc",
	"",
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
			std::cout << "failed! replace (" << searches[i] << ") to ("
				<< replaces[i] << ") " << counts[i] << " times"
				<< std::endl << "get " << s << std::endl
				<< "but expect: " << results[i] << std::endl;
		} else {
			std::cout << "success!" << std::endl;
		}
	}

	std::cout << "2. test trim()..." << std::endl;
	for (int i = 0; i < COUNT_OF(trims); ++ i) {
		xl::string s = trims[i];
		s.trim();
		if (s == trimresults[i]) {
			std::cout << "trim " << trims[i] << " succeed!" << std::endl;
		} else {
			std::cout << "trim " << trims[i] << " failed!" << std::endl;
		}
	}

	std::cout << "3. test multi trim()..." << std::endl;
	for (int i = 0; i < COUNT_OF(multitrims); ++ i) {
		xl::string s = multitrims[i];
		s.trim(charlist);
		if (s == multitrimresults[i]) {
			std::cout << "trim " << multitrims[i] << " succeed!" << std::endl;
		} else {
			std::cout << "trim " << multitrims[i] << " failed!" << std::endl;
		}
	}

	return 0;
}