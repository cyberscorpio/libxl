#include <process.h>
#include <iostream>
#include <memory>
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////
// compile:
// cl -c /EHsc sharedptr.cpp
// link sharedptr.obj ..\Release\libxl.lib

struct Data {
	unsigned char buf[100];
};
typedef std::tr1::shared_ptr<Data>     DataPtr;

DataPtr   data;
const int THREAD_COUNT = 2;
HANDLE threads[THREAD_COUNT];
static unsigned int __stdcall thread_proc (void *) {
	for (int i = 0; i < 0x7ffff; ++ i) {
		data.reset();
		DataPtr d(new Data());
		data = d;
	}
	return 0;
}

#ifdef IN_IDE
int test_sharedptr(int argc, char **argv) {
#else
int main(int argc, char **argv) {
#endif

	for (int i = 0; i < THREAD_COUNT; ++ i) {
		threads[i] = (HANDLE)_beginthreadex(NULL, 0, thread_proc, NULL, 0, NULL);
	}

	::WaitForMultipleObjects(THREAD_COUNT, threads, TRUE, INFINITE);

	return 0;
}