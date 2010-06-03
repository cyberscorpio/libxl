#ifndef IN_IDE
#define UNICODE
#define _UNICODE
#endif
#include <process.h>
#include <iostream>
#include <memory>
#include <Windows.h>
#include "../libxl/include/tsptr.h"
#include "../libxl/include/utilities.h"


//////////////////////////////////////////////////////////////////////////
// compile:
// cl -c /EHsc sharedptr.cpp
// link sharedptr.obj ..\Release\libxl.lib

struct Data {
	unsigned char buf[100];
	void foo () {
		printf ("hi!!!\n");
	}
};
// typedef std::tr1::shared_ptr<Data>     DataPtr;
typedef xl::ts_shared_ptr<Data>        DataPtr;

CRITICAL_SECTION cs;

DataPtr   data;
const int THREAD_COUNT = 2;
HANDLE threads[THREAD_COUNT];
static unsigned int __stdcall thread_proc (void *) {
	for (int i = 0; i < 0x7ffff; ++ i) {
		// ::EnterCriticalSection(&cs);
		data.reset();
		DataPtr d1(new Data());
		DataPtr d2(d1);
		data = d2;
		DataPtr d3 = d2;
		// data->foo();
		// ::LeaveCriticalSection(&cs);
	}
	return 0;
}

#ifdef IN_IDE
int test_sharedptr(int argc, char **argv) {
#else
int main(int argc, char **argv) {
#endif

	::InitializeCriticalSection(&cs);

	xl::CTimerLogger logger(_T("cost:"));
	for (int i = 0; i < THREAD_COUNT; ++ i) {
		threads[i] = (HANDLE)_beginthreadex(NULL, 0, thread_proc, NULL, 0, NULL);
	}
	::WaitForMultipleObjects(THREAD_COUNT, threads, TRUE, INFINITE);
	logger.log();

	::DeleteCriticalSection(&cs);

	return 0;
}
