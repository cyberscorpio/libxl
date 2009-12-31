#define _WTL_NO_CSTRING
#include "../libxl/include/ui/Application.h"
#include "../libxl/include/ui/MainWindow.h"


class CMyWindow : public xl::ui::CMainWindowT<CMyWindow>
{
public:
	DECLARE_WND_CLASS_EX(_T("CMyWindow"), 0, COLOR_INFOBK)
};

class CMyApp : public xl::ui::CApplicationT<CMyApp>
{
	CMyWindow m_wndMain;
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
