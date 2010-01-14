#ifndef XL_UI_MAIN_WINDOW_H
#define XL_UI_MAIN_WINDOW_H
#include <assert.h>
#include <vector>
#include <atlbase.h>
#include <atlwin.h>
#include "../common.h"
#include "CtrlMain.h"
#include "CtrlTarget.h"

namespace xl {
	namespace ui {

typedef CWinTraits<
                   WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                   WS_EX_APPWINDOW | WS_EX_WINDOWEDGE
                  > CMainWindowTraits; // the default traits

template <class T, class Traits = CMainWindowTraits>
class CMainWindowT
	: public CWindowImplBaseT<ATL::CWindow, Traits>
	, public CMessageFilter
	, public CIdleHandler
	, public CCtrlTarget
{
	typedef ATL::CWindow  _TBase;

protected:
	xl::ui::CControlPtr m_ctrl;

public:
	CMainWindowT (void) : m_ctrl(new CCtrlMain(this, this)) {
	}

	virtual ~CMainWindowT (void) {
	}

public:
	DECLARE_WND_CLASS_EX(_T("xl::ui::MainWindowT"), 0, COLOR_WINDOW)

	//////////////////////////////////////////////////////////////////////////
	// virtual 
	virtual BOOL PreTranslateMessage(MSG* pMsg) {
		return FALSE;
	}

	virtual BOOL OnIdle() {
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	// interface


	HWND Create(HWND hWndParent = NULL, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0,
		HMENU hMenu = NULL, LPVOID lpCreateParam = NULL)
	{
		ATOM atom = T::GetWndClassInfo().Register(&m_pfnSuperWindowProc);

		dwStyle |= T::GetWndStyle(0);
		dwExStyle |= T::GetWndExStyle(0);

		if(rect.m_lpRect == NULL) {
			rect.m_lpRect = &_TBase::rcDefault;
		}

		return CWindowImplBaseT<ATL::CWindow, Traits>::Create(hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, hMenu, atom, lpCreateParam);
	}

	BEGIN_MSG_MAP(CMainWindowT)
		{
			CCtrlMain *ctrlMain = (CCtrlMain *)m_ctrl.get();
			if (ctrlMain) {
				if(ctrlMain->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult))
					return TRUE; 
			}
		}
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		return TRUE;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		::PostQuitMessage(0);
		return TRUE;
	}
};

	} // namespace ui
} // namespace xl

#endif
