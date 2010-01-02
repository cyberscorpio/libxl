#ifndef XL_UI_APPLICATION_H
#define XL_UI_APPLICATION_H
// #define _WTL_NO_CSTRING
#include <atlbase.h>
#include <atlapp.h>

#pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

namespace xl {
	namespace ui {


template <class T>
class CApplicationT : public CAppModule {
	CMessageLoop m_msgLoop;
	bool m_initialized;

protected:
	CApplicationT ()
		: m_initialized(false)
	{
	}
	~CApplicationT () {

	}

	virtual HWND createMainWindow (LPCTSTR /* lpstrCmdLine */, int /* nCmdShow */) {
		return NULL;
	}

	virtual void preRun () {
	}

	virtual void postRun () {
	}


public:
	static T* getInstance () {
		static T This;
		return &This;
	}

	bool initialize (HINSTANCE hInst) {
		HRESULT hRes = ::CoInitialize(NULL);
		ATLASSERT (SUCCEEDED(hRes));
		::DefWindowProc(NULL, 0, 0, 0L);
		AtlInitCommonControls(ICC_BAR_CLASSES | ICC_TAB_CLASSES);
		hRes = Init(NULL, hInst);
		ATLASSERT (SUCCEEDED(hRes));
		m_initialized = true;
		return true;
	}

	void cleanup () {
		if (m_initialized) {
			Term();
			::CoUninitialize();
		}
	}

	int run (LPCTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT) {
		if (!m_initialized) {
			AtlInitCommonControls(ICC_BAR_CLASSES | ICC_TAB_CLASSES);
			::MessageBox(0, _T("Call CApplication::initialize() before run()"), NULL, 0);
			return -1;
		}
		AddMessageLoop(&m_msgLoop);

		T *p = (T *)this;
		HWND hWnd = p->createMainWindow(lpstrCmdLine, nCmdShow);
		if (!hWnd) {
			::MessageBox(0, _T("Create main window failed!"), NULL, 0);
			::PostQuitMessage(-1);
		}

		::UpdateWindow(hWnd);
		::ShowWindow(hWnd, nCmdShow);

		p->preRun();
		int nRet = m_msgLoop.Run();
		p->postRun();

		RemoveMessageLoop();

		return nRet;
	}
};

	} // namespace ui
} // namespace xl

#endif

