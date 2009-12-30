#ifndef XL_APPLICATION_H
#define XL_APPLICATION_H
// #define _WTL_NO_CSTRING
#include <atlbase.h>
#include <atlapp.h>


namespace xl {
	namespace ui {


template <class T>
class CApplicationT : public CAppModule {

protected:
	CApplicationT () {}
	~CApplicationT () {}

	CMessageLoop m_msgLoop;

	virtual void preRun (LPCTSTR, int) {
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
		return true;
	}

	void cleanup () {
		Term();
		::CoUninitialize();
	}

	int run (LPCTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT) {
		AddMessageLoop(&m_msgLoop);

		T *p = (T *)this;
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

