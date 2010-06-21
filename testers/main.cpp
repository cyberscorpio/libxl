#include <crtdbg.h>
#include <tchar.h>
int test_sharedptr(int argc, char **argv);
int test_ini(int argc, char **argv);
int test_registry(int argc, char **argv);



int main(int argc, char **argv) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// test_sharedptr(argc, argv);
	// test_ini(argc, argv);
	test_registry(argc, argv);
	return 0;
}

