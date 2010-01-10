#include "../libxl/include/ui/gdi.h"
#include "MainWindow.h"

class CToolbar : public xl::ui::CControl
{
public:
	CToolbar () {
		height = 32;
		width = xl::ui::SIZE_FILL;
		px = xl::ui::PX_LEFT;
		py = xl::ui::PY_TOP;
		// margin.left = margin.right = 5;
		margin.top = 0;
		margin.bottom = 5;
	}

	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc (hdc);
		dc.FillSolidRect(m_rect, RGB(192,255,192));
	}
};

class CStatusbar : public xl::ui::CControl
{
public:
	CStatusbar () {
		height = 32;
		width = xl::ui::SIZE_FILL;
		px = xl::ui::PX_LEFT;
		py = xl::ui::PY_BOTTOM;
		// margin.left = margin.right = 5;
		margin.top = 5;
		margin.bottom = 0;
	}

	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc (hdc);
		dc.FillSolidRect(m_rect, RGB(192,255,192));
	}
};


class CClient : public xl::ui::CControl
{
public:
	CClient () {
		width = xl::ui::SIZE_FILL;
		height = xl::ui::SIZE_FILL;
		px = xl::ui::PX_LEFT;
		py = xl::ui::PY_TOP;
		margin.left = margin.right = 0;
		margin.top = margin.bottom = 0;
	}

	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc (hdc);
		dc.FillSolidRect(m_rect, RGB(255,255,255));
	}
};


class CExplorer : public xl::ui::CControl
{
public:
	CExplorer () {
		width = 200;
		px = xl::ui::PX_LEFT;
		py = xl::ui::PY_TOP;
		margin.left = 5;
		margin.top = margin.bottom = 5;
	}

	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc (hdc);
		dc.FillSolidRect(m_rect, RGB(192,192,255));
	}
};

class CView : public xl::ui::CControl
{
public:
	CView () {
		width = xl::ui::SIZE_FILL;
		px = xl::ui::PX_LEFT;
		py = xl::ui::PY_TOP;
		margin.left = margin.right = 5;
		margin.top = margin.bottom = 5;
	}
	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc (hdc);
		dc.FillSolidRect(m_rect, RGB(192,255,255));
	}
};



LRESULT CMainWindow::OnCreate (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	bHandled = false;

	m_ctrl.px = xl::ui::PX_LEFT;
	m_ctrl.py = xl::ui::PY_TOP;
	m_ctrl.width = xl::ui::SIZE_FILL;
	m_ctrl.height = xl::ui::SIZE_FILL;

	xl::ui::CControlPtr ctrl(new CToolbar());
	m_ctrl.insertChild(ctrl);

	ctrl.reset(new CStatusbar);
	m_ctrl.insertChild(ctrl);

	xl::ui::CControlPtr client(new CClient());
	m_ctrl.insertChild(client);

	ctrl.reset(new CExplorer());
	client->insertChild(ctrl);

	ctrl.reset(new CView());
	client->insertChild(ctrl);

	return TRUE;
}

LRESULT CMainWindow::OnPaint (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	xl::ui::CPaintDC dc(m_hWnd);
	m_ctrl.draw(dc.m_hDC);
	return 0;
}

LRESULT CMainWindow::OnSize (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	
	CRect rc;
	GetClientRect(rc);
	m_ctrl.layout(rc);

	Invalidate();

	return 0;
}
