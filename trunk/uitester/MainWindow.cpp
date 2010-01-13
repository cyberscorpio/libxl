#include "../libxl/include/ui/gdi.h"
#include "../libxl/include/ui/ResMgr.h"
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

class CFloat : public xl::ui::CControl
{
public:
	CFloat () {
		// margin.left = margin.right = 5;
		margin.top = 0;
		margin.bottom = 50;
		margin.left = margin.right = 50;
		setStyle(_T("px:left;py:bottom;height:120;width:fill;float:float"));
	}

	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc (hdc);
		xl::ui::CResMgr *pResMgr = xl::ui::CResMgr::getInstance();

		if (opacity == 100) {
			// dc.FillSolidRect(m_rect, RGB(255,255,255));
		}
		HFONT hFont = pResMgr->getSysFont(130);
		HFONT oldFont = dc.SelectFont(hFont);

		dc.drawTransparentText(_T("xl::ui is COOL!"), -1, m_rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

		dc.SelectFont(oldFont);
	}
	virtual void onMouseIn (CPoint pt) {
		setStyle(_T("opacity:100"));
		_GetMainCtrl()->invalidateControl(shared_from_this());
	}

	virtual void onMouseOut (CPoint pt) {
		setStyle(_T("opacity:50"));
		_GetMainCtrl()->invalidateControl(shared_from_this());
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
		setStyle(_T("px:left; py:top; width:fill; height:fill;"));
	}

	virtual void onMouseIn (CPoint pt) {
		m_hover = true;
		m_pt = pt;
		_GetMainCtrl()->invalidateControl(shared_from_this());
	}

	virtual void onMouseOut (CPoint pt) {
		m_hover = false;
		_GetMainCtrl()->invalidateControl(shared_from_this());
	}

	virtual void onMouseMove (CPoint pt) {
		m_pt = pt;
		_GetMainCtrl()->invalidateControl(shared_from_this());
	}

	virtual void onLButtonDown (CPoint pt) {
		_SetCapture(true);
	}

	virtual void onLButtonUp (CPoint pt) {
		_SetCapture(false);
	}

	virtual void onLostCapture () {
		m_hover = false;
		_GetMainCtrl()->invalidateControl(shared_from_this());
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

	// m_ctrl.reset(new xl::ui::CCtrlMgr(0));
	// insertControl(m_ctrl);
	assert (m_ctrl != NULL);

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

	ctrl.reset(new CFloat());
	ctrl->setStyle(_T("float:float; py:bottom; opacity:50; height:120"));
	ctrl->margin.left = 50;
	ctrl->margin.right = 50;
	ctrl->margin.top = 0;
	ctrl->margin.bottom = 10;
	m_ctrl->insertChild(ctrl);

	return TRUE;
}

// LRESULT CMainWindow::OnEraseBkGnd (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
// 	return 1;
// }
// 
// LRESULT CMainWindow::OnPaint (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
// 	xl::ui::CPaintDC dc(m_hWnd);
// 	m_ctrl->draw(dc.m_hDC);
// 	return 0;
// }

LRESULT CMainWindow::OnSize (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	
	CRect rc;
	GetClientRect(rc);
	m_ctrl->layout(rc);

	Invalidate();

	return 0;
}
