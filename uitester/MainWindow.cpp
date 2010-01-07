#include "../libxl/include/ui/gdi.h"
#include "MainWindow.h"

class CMyCtrl : public xl::ui::CCtrlBase
{
public:
	COLORREF m_rgbBackground;
	int m_index;

	CMyCtrl (int index) : m_index(index) {}

	virtual void draw (HDC hdc) {
		xl::ui::CDCHandle dc(hdc);
		CRect rc = m_rc;

		dc.FillSolidRect(rc, m_rgbBackground);
		TCHAR buf[1024];
		TCHAR *disp = display == DISP_BLOCK ? _T("B") :
			(display == DISP_LEFT ? _T("L") :
			(display == DISP_RIGHT ? _T("R") :
			(display == DISP_CLEAR ? _T("C") : _T("U"))));
		_stprintf_s(buf, 1024, _T("%d - %s"), m_index, disp);
		dc.DrawText(buf, -1, rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
};



LRESULT CMainWindow::OnCreate (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	bHandled = false;
	padding.left = 20;
	padding.right = 20;

	COLORREF colors[] = {
		RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255),
		RGB(255, 255, 0), RGB(0, 255, 255), RGB(255, 0, 255),
		RGB(0, 0, 0), RGB(127, 255, 127), RGB(255, 127, 127),
	};
	for (int i = 0; i < COUNT_OF(colors); ++ i) {
		CMyCtrl *pCtrl = new CMyCtrl(i);
		pCtrl->m_rgbBackground = colors[i];
		pCtrl->display = xl::ui::WinStyle::DISP_LEFT;
		if ((i % 5) == 0) {
			pCtrl->display = xl::ui::WinStyle::DISP_RIGHT;
		} else if ((i % 5) == 1) {
			pCtrl->display = xl::ui::WinStyle::DISP_BLOCK;
		}
		m_ctrls.push_back(xl::ui::CCtrlPtr(pCtrl));
	}
	_LayoutControls();

	return TRUE;
}

LRESULT CMainWindow::OnPaint (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	xl::ui::CPaintDC dc(m_hWnd);
	CRect rc = getContentRect();
	// dc.drawLine(rc.left, rc.bottom / 2, rc.right, rc.bottom / 2);
	for (_CtrlIter it = m_ctrls.begin(); it != m_ctrls.end(); ++ it) {
		if ((*it)->display != WinStyle::DISP_NONE) {
			(*it)->draw(dc.m_hDC);
		}
	}
	return 0;
}

LRESULT CMainWindow::OnSize (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {

	_LayoutControls();
	InvalidateRect(NULL);

	return 0;
}
