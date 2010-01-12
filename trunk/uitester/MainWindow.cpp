#include "../libxl/include/ui/gdi.h"
#include "MainWindow.h"

class CToolbar : public xl::ui::CControl
{
public:
	CToolbar () {
		// margin.left = margin.right = 5;
		margin.top = 0;
		margin.bottom = 5;
		setStyle(_T("px:left;py:top;height:32;width:fill"));
	}

	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc (hdc);
		dc.FillSolidRect(m_rect, RGB(255,255,192));
	}
};

class CStatusbar : public xl::ui::CControl
{
public:
	CStatusbar () {
		// margin.left = margin.right = 5;
		margin.top = 5;
		margin.bottom = 0;
		setStyle(_T("px:left;py:bottom;height:32;width:fill;"));
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
		margin.left = margin.right = 0;
		margin.top = margin.bottom = 0;
		setStyle(_T("px:left;py:top;width:fill;height:fill;"));
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
		margin.left = 5;
		margin.top = margin.bottom = 5;
		setStyle(_T("width:100; height:fill; px:left; py:top;"));
	}

	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc (hdc);
		dc.FillSolidRect(m_rect, RGB(192,192,255));
	}
};

class CView : public xl::ui::CControl
{
	bool m_hover;
	CPoint m_pt;
public:
	CView () : m_hover(false) {
		margin.left = margin.right = 5;
		margin.top = margin.bottom = 5;
		setStyle(_T("px:left; py:top; width:fill; height:fill;opacity:50;"));
	}

	virtual void onMouseIn (CPoint pt) {
		m_hover = true;
		m_pt = pt;
		m_mgr->invalidateControl(shared_from_this());
	}

	virtual void onMouseOut (CPoint pt) {
		m_hover = false;
		m_mgr->invalidateControl(shared_from_this());
	}

	virtual void onMouseMove (CPoint pt) {
		m_pt = pt;
		m_mgr->invalidateControl(shared_from_this());
	}


	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc (hdc);
		dc.FillSolidRect(m_rect, RGB(192,255,255));
		if (m_hover) {
			TCHAR buf[1024];
			_stprintf_s(buf, 1024, _T("Mouse: %d - %d"), m_pt.x, m_pt.y);
			dc.DrawText(buf, -1, m_rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
	}
};



LRESULT CMainWindow::OnCreate (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	bHandled = false;

	m_ctrl.reset(new xl::ui::CControl(0));
	m_ctrl->init(this);

	m_ctrl->px = xl::ui::PX_LEFT;
	m_ctrl->py = xl::ui::PY_TOP;
	m_ctrl->width = xl::ui::SIZE_FILL;
	m_ctrl->height = xl::ui::SIZE_FILL;

	xl::ui::CControlPtr ctrl(new CToolbar());
	m_ctrl->insertChild(ctrl);

	ctrl.reset(new CStatusbar);
	m_ctrl->insertChild(ctrl);

	xl::ui::CControlPtr client(new CClient());
	m_ctrl->insertChild(client);

	ctrl.reset(new CExplorer());
	client->insertChild(ctrl);

	ctrl.reset(new CExplorer());
	ctrl->setStyle(_T("px:right"));
	client->insertChild(ctrl);

	ctrl.reset(new CView());
	client->insertChild(ctrl);

	ctrl.reset(new CToolbar());
	ctrl->setStyle(_T("float:float; py:bottom; opacity:50; height:120"));
	ctrl->margin.left = 50;
	ctrl->margin.right = 50;
	ctrl->margin.top = 0;
	ctrl->margin.bottom = 10;
	m_ctrl->insertChild(ctrl);

	return TRUE;
}

LRESULT CMainWindow::OnEraseBkGnd (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	return 1;
}

LRESULT CMainWindow::OnPaint (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	xl::ui::CPaintDC dc(m_hWnd);
	m_ctrl->draw(dc.m_hDC);
	return 0;
}

LRESULT CMainWindow::OnSize (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	
	CRect rc;
	GetClientRect(rc);
	m_ctrl->layout(rc);

	Invalidate();

	return 0;
}
