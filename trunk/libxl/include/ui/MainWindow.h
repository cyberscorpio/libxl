#ifndef XL_UI_MAIN_WINDOW_H
#define XL_UI_MAIN_WINDOW_H
#include <assert.h>
#include <vector>
#include <atlbase.h>
#include <atlwin.h>
#include "../common.h"
#include "UIWinBase.h"
#include "CtrlBase.h"

namespace xl {
	namespace ui {

typedef CWinTraits<
                   WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                   WS_EX_APPWINDOW | WS_EX_WINDOWEDGE
                  > CMainWindowTraits; // the default traits

class CCtrlBase;
template <class T, class Traits = CMainWindowTraits>
class CMainWindowT
	: public CWindowImplBaseT<ATL::CWindow, Traits>
	, public CUIWinBaseT<T>
	, public CMessageFilter
	, public CIdleHandler
{
	typedef ATL::CWindow  _TBase;

protected:
	typedef std::vector<CCtrlPtr> _Ctrls;
	typedef _Ctrls::iterator         _CtrlIter;
	_Ctrls    m_ctrls;

	/**
	 * @return the total height it used
	 */
	int _LayoutControls () {
		using xl::ui::EDGE_TOP;
		using xl::ui::EDGE_LEFT;
		using xl::ui::EDGE_BOTTOM;
		using xl::ui::EDGE_RIGHT;
		int total_height = 0;
		CRect rc = getContentRect();
		int line_x = rc.left;
		int line_y = rc.top;
		int max_line_height = -1;
		int line_ctrl_count = 0;
		for (_CtrlIter it = m_ctrls.begin(); it != m_ctrls.end(); ++ it) {
			CCtrlBase *pCtrl = (*it).get();
			assert (pCtrl);
			SIZE sz = pCtrl->reportSize();

			int width = pCtrl->width;
			int height = pCtrl->height;
			int line_height = height + pCtrl->margin[EDGE_TOP] + pCtrl->margin[EDGE_BOTTOM];
			if (line_height > max_line_height) {
				max_line_height = line_height;
			}
			int x = line_x + pCtrl->margin[EDGE_LEFT];
			int y = line_y + pCtrl->margin[EDGE_TOP];
			if (x + width > rc.right && line_ctrl_count > 0) {
				// next line
				line_x = rc.left;
				line_y += max_line_height;
				max_line_height = -1;
				line_ctrl_count = 0;
				-- it;
				continue; // next line
			}
			++ line_ctrl_count;
			CRect rect(x, y, x + width, y + height);
			line_x = x + width + pCtrl->margin[EDGE_RIGHT];

			(*it)->setRect(rect);
		}

		return total_height;
	}

public:
	CMainWindowT (void) {

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
