#if 0
#define _WTL_NO_CSTRING
#include <atlstr.h>
#include <atlbase.h>
#include <atlapp.h>

#pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

CAppModule _Module;

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT) {
	CMessageLoop msgLoop;
	_Module.AddMessageLoop(&msgLoop);

	CMainWindow *pMainWindow = CMainWindow::getInstance();
	msgLoop.AddIdleHandler(pMainWindow);
	if (pMainWindow->createWindow() == NULL) {
		ATLTRACE(_T("Main window creation failed!\n"));
		MessageBox(0, _T("Create main window failed!"), _T("error:"), MB_OK);
		_Module.RemoveMessageLoop();
		return 0;
	}

	pMainWindow->ShowWindow(nCmdShow);

	int nRet = msgLoop.Run();

	msgLoop.RemoveIdleHandler(pMainWindow);
	_Module.RemoveMessageLoop();

	return nRet;
}



int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpstrCmdLine, int nCmdShow) {
	TCHAR *lpSemaphoreName = _T("XL::uploader::semaphore");
	HANDLE semaphore = ::CreateSemaphore(NULL, 1, 1, lpSemaphoreName);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(semaphore);

		// HWND hWnd = ::FindWindow(NULL, CMainWindow::getWindowName());
		// if (hWnd != NULL) {
		// ::SendMessage(hWnd, WM_SHOWWINDOW, TRUE, );
		// }
		return 0;
	}

	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT (SUCCEEDED(hRes));


	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES | ICC_TAB_CLASSES);

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT (SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	CloseHandle(semaphore);

	//	FreeImage_DeInitialise();
	return nRet;
}
#endif
