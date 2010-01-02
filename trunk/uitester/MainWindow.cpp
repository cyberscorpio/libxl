#include "../libxl/include/ui/gdi.h"
#include "MainWindow.h"


LRESULT CMainWindow::OnCreate (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	bHandled = false;
	padding[xl::ui::LEFT] = 20;
	padding[xl::ui::RIGHT] = 20;
	return TRUE;
}

LRESULT CMainWindow::OnPaint (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	xl::ui::CPaintDC dc(m_hWnd);
	CRect rc = getContentRect();
	dc.drawLine(rc.left, rc.bottom / 2, rc.right, rc.bottom / 2);
	return 0;
}
