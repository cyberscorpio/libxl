#include <crtdbg.h>
int test_sharedptr(int argc, char **argv);


int main(int argc, char **argv) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	test_sharedptr(argc, argv);
	return 0;
}

