#define _WTL_NO_CSTRING
#include "../libxl/include/ui/Application.h"

#pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


class CMyApp : public xl::ui::CApplicationT<CMyApp>
{
public:
	virtual void preRun () {
		::MessageBox (NULL, _T(":)"), _T("HI"), 0);
	}
};


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpstrCmdLine, int nCmdShow) {

	CMyApp *pApp = CMyApp::getInstance();
	pApp->initialize(hInstance);

	int nRet = pApp->run(lpstrCmdLine, nCmdShow);

	pApp->cleanup();

	return nRet;
}
