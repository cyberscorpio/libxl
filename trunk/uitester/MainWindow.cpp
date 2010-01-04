#include "../libxl/include/ui/gdi.h"
#include "MainWindow.h"


LRESULT CMainWindow::OnCreate (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	bHandled = false;
	padding[xl::ui::EDGE_LEFT] = 20;
	padding[xl::ui::EDGE_RIGHT] = 20;

	COLORREF colors[] = {
		RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255),
		RGB(255, 255, 0), RGB(0, 255, 255), RGB(255, 0, 255),
		RGB(0, 0, 0), RGB(127, 255, 127), RGB(255, 127, 127),
	};
	for (int i = 0; i < COUNT_OF(colors); ++ i) {
		xl::ui::CCtrlBase *pCtrl = new xl::ui::CCtrlBase();
		pCtrl->m_clrBackground = colors[i];
		m_ctrls.push_back(pCtrl);
	}
	_LayoutControls();

	return TRUE;
}

LRESULT CMainWindow::OnPaint (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	xl::ui::CPaintDC dc(m_hWnd);
	CRect rc = getContentRect();
	// dc.drawLine(rc.left, rc.bottom / 2, rc.right, rc.bottom / 2);
	for (_CtrlIter it = m_ctrls.begin(); it != m_ctrls.end(); ++ it) {
		(*it)->draw(dc.m_hDC);
	}
	return 0;
}

LRESULT CMainWindow::OnSize (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {

	_LayoutControls();
	InvalidateRect(NULL);

	return 0;
}
