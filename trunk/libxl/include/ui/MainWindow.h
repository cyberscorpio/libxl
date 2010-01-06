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
	typedef std::vector<CCtrlPtr>  _Ctrls;
	typedef _Ctrls::iterator       _CtrlIter;
	_Ctrls    m_ctrls;


	/**
	 * @return line height, -1 means the last line
	 */
	int _LayoutLine (_CtrlIter &it, int start_y) {
		int line_height = -1;
		int line_ctrl_count = 0;
		bool no_block = true;
		
		CRect rect = getContentRect();
		int start_x = rect.left;
		int max_x = rect.right;

		_CtrlIter end = m_ctrls.end();
		while (it != end) {
			CCtrlBase *pCtrl = (*it).get();
			WinStyle::DISPLAY display = pCtrl->display;
			EDGE *pMargin = &pCtrl->margin;

			xl::ui::SIZE sz = pCtrl->reportSize();
			if (sz.cx == SIZE_AUTO) {
				// TODO
			} else if (sz.cx == SIZE_FILL) {
				// TODO
			}
			assert (sz.cy >= 0); // TODO: suppport auto / fill height

			if ((line_ctrl_count > 0) && 
			    ((start_x + sz.cx > max_x) || 
			     (display == WinStyle::DISP_CLEAR))) {
				-- it; // it belongs to the next line
				break;
			}

			if (!no_block && display == WinStyle::DISP_BLOCK) {
				-- it;
				break;
			}
			
			// is clear ?
			if (display == WinStyle::DISP_CLEAR) {
				assert (sz.cy >= 0); // clear can't have negative height
				line_height = sz.cy;
				break;
			}

			++ line_ctrl_count;
			// is block or left ?
			if (display == WinStyle::DISP_BLOCK || display == WinStyle::DISP_LEFT) {
				int l = start_x + pMargin->left;
				int t = start_y + pMargin->top;
				int r = start_x + sz.cx - pMargin->right;
				int b = start_y + sz.cy - pMargin->bottom;
				if (r > max_x) {
					r = max_x;
				}
				start_x += sz.cx;
				CRect rc(l, t, r, b);
				pCtrl->setRect(rc);
				if (line_height < sz.cy) {
					line_height = sz.cy;
				}

				if (display == WinStyle::DISP_BLOCK) {
					no_block = false;
					++ it;
					break;
				}
			}

			// right ?
			if (display == WinStyle::DISP_RIGHT) {
				int l = max_x - sz.cx + pMargin->left;
				int t = start_y + pMargin->top;
				int r = max_x - pMargin->right;
				int b = start_y + sz.cy - pMargin->bottom;
				if (l < start_x) {
					l = start_x;
				}
				max_x -= sz.cx;
				CRect rc(l, t, r, b);
				pCtrl->setRect(rc);
				if (line_height < sz.cy) {
					line_height = sz.cy;
				}
			}
			++ it;
		}
		
		return line_height;
	}

	/**
	 * @return the total height it used
	 */
	int _LayoutControls () {
		int total_height = 0;
		CRect rect = getContentRect();
		_CtrlIter it = m_ctrls.begin();
		int start_y = rect.top;
		int line_height = _LayoutLine(it, start_y);
		while (line_height != -1) {
			total_height += line_height;
			start_y += line_height;
			line_height = _LayoutLine(it, start_y);
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
