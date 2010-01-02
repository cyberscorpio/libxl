#define _WTL_NO_CSTRING
// #include <d2d1.h>
#include "../libxl/include/ui/Application.h"
#include "MainWindow.h"

class CMyApp : public xl::ui::CApplicationT<CMyApp>
{
	CMainWindow m_wndMain;
public:
	virtual HWND createMainWindow (LPCTSTR, int) {
		return m_wndMain.Create(NULL, 0, _T("UITester"));
	}

	virtual void preRun () {

	}

	virtual void postRun () {

	}
};


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpstrCmdLine, int nCmdShow) {

	CMyApp *pApp = CMyApp::getInstance();
	pApp->initialize(hInstance);

	int nRet = pApp->run(lpstrCmdLine, nCmdShow);

	pApp->cleanup();

	return nRet;
}
