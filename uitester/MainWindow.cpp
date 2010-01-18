#include "../libxl/include/ui/gdi.h"
#include "../libxl/include/ui/ResMgr.h"
#include "../libxl/include/ui/CtrlButton.h"
#include "../libxl/include/ui/CtrlSlider.h"
#include "MainWindow.h"
#include "resource.h"

class CToolbar : public xl::ui::CControl
{
public:
	CToolbar () {
		setStyle(_T("px:left;py:top;height:32;width:fill;margin:0 0 5"));
	}

	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc (hdc);
		dc.FillSolidRect(m_rect, RGB(255,255,192));
	}
};

class CFloat : public xl::ui::CCtrlSlider
{
public:
	CFloat () : xl::ui::CCtrlSlider(0, 100, 50) {
		setStyle(_T("px:left;py:bottom;height:40;width:480;float:true;margin:0 auto 20;opacity:25;"));
	}

// 	virtual void drawMe (HDC hdc) {
// 		xl::ui::CDCHandle dc (hdc);
// 		xl::ui::CResMgr *pResMgr = xl::ui::CResMgr::getInstance();
// 
// 		if (opacity != 100)
// 		{
// 			dc.FillSolidRect(m_rect, RGB(127,127,127));
// 		}
// 		HFONT hFont = pResMgr->getSysFont(40);
// 		HFONT oldFont = dc.SelectFont(hFont);
// 
// 		dc.drawTransparentText(_T("xl::ui is COOL!"), -1, getClientRect(), DT_SINGLELINE | DT_VCENTER | DT_CENTER);
// 
// 		dc.SelectFont(oldFont);
// 		_DrawBorder(hdc);
// 	}

	virtual void onMouseIn (CPoint pt) {
		setStyle(_T("opacity:100"));
		_GetMainCtrl()->invalidateControl(shared_from_this());
	}

	virtual void onMouseOut (CPoint pt) {
		setStyle(_T("opacity:25"));
		_GetMainCtrl()->invalidateControl(shared_from_this());
	}
};



class CStatusbar : public xl::ui::CControl
{
public:
	CStatusbar () {
		setStyle(_T("px:left;py:bottom;height:32;width:fill;margin:5 0 0"));
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
		setStyle(_T("px:left;py:top;width:fill;height:fill;margin:0"));
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
		setStyle(_T("width:100; height:fill; px:left; py:top;margin:5 0 5 5"));
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
		setStyle(_T("px:left; py:top; width:fill; height:fill;margin:5"));
	}

	virtual void onAttach () {
		xl::ui::CCtrlButton *pButton = new xl::ui::CCtrlImageButton(1, IDB_PNG1, IDB_PNG2, IDB_PNG3);
		pButton->setImg(IDB_PNG5);
		pButton->setTextImagePadding(6);
		xl::ui::CControlPtr button (pButton);
		button->setStyle(_T("margin:10;width:100;height:40;border:0;font-weight:bold;"));// opacity:50;
		insertChild(button);
		
		pButton->setText(_T("Prompt"));
		pButton = new xl::ui::CCtrlImageButton(2, IDB_PNG1, IDB_PNG2, IDB_PNG3);
		button.reset(pButton);
		button->setStyle(_T("margin:10;width:100;height:40;border:0;")); 
		pButton->setText(_T("Hide"));
		insertChild(button);

		pButton = new xl::ui::CCtrlImageButton(3, IDB_PNG1, IDB_PNG2, IDB_PNG3);
		button.reset(pButton);
		button->setStyle(_T("margin:10;width:100;height:40;border:0;")); 
		pButton->setText(_T("Disable"));
		insertChild(button);

		pButton = new xl::ui::CCtrlImageButton(4, IDB_PNG1, IDB_PNG2, IDB_PNG3);
		button.reset(pButton);
		button->setStyle(_T("margin:10;width:100;height:40;border:0;")); 
		pButton->setText(_T("Enlarge"));
		insertChild(button);

		pButton = new xl::ui::CCtrlImageButton(5, IDB_PNG4, IDB_PNG4, IDB_PNG4);
		button.reset(pButton);
		button->setStyle(_T("margin:10;width:40;height:40;border:0;")); 
		insertChild(button);
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

	virtual void onRButtonDown (CPoint pt) {
		_SetCapture(true);
	}

	virtual void onRButtonUp (CPoint pt) {
		_SetCapture(false);
	}

	virtual void onLostCapture () {
		m_hover = false;
		_GetMainCtrl()->invalidateControl(shared_from_this());
	}

	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc (hdc);
		dc.FillSolidRect(m_rect, RGB(0,192,255));
		if (m_hover) {
			TCHAR buf[1024];
			_stprintf_s(buf, 1024, _T("Mouse: %d - %d"), m_pt.x - m_rect.left, m_pt.y - m_rect.top);
			dc.DrawText(buf, -1, getClientRect(), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
	}
};


void CMainWindow::onCommand (xl::uint id, xl::ui::CControlPtr ctrl) {
	xl::ui::CCtrlButton *button = (xl::ui::CCtrlButton *)ctrl.get();
	if (id == 1) {
		MessageBox(_T("You click button 1"), _T("OK"));
	} else if (id == 2) {
		xl::ui::CControlPtr button_1 = m_ctrl->getControlByID(1);
		if (!button_1->display) {
			button_1->setStyle(_T("display:true"));
			button_1 = m_ctrl->getControlByID(2);
			button->setText(_T("Display"));
		} else {
			button_1->setStyle(_T("display:false"));
			button_1 = m_ctrl->getControlByID(2);
			button->setText(_T("Hide"));
		}
	} else if (id == 3) {
		xl::ui::CControlPtr ctrl = m_ctrl->getControlByID(1);
		if (!ctrl->disable) {
			ctrl->setStyle(_T("disable:true"));
			ctrl = m_ctrl->getControlByID(3);
			button->setText(_T("Enable"));
		} else {
			ctrl->setStyle(_T("disable:false"));
			ctrl = m_ctrl->getControlByID(3);
			button->setText(_T("Disable"));
		}
	} else if (id == 4) {
		static bool larged = true;
		larged = !larged;
		xl::ui::CControlPtr ctrl = m_ctrl->getControlByID(1);
		if (!larged) {
			ctrl->setStyle(_T("width:120"));
			ctrl = m_ctrl->getControlByID(4);
			button->setText(_T("Shrink"));
		} else {
			ctrl->setStyle(_T("width:80"));
			ctrl = m_ctrl->getControlByID(4);
			button->setText(_T("Enlarge"));
		}
	}
}


LRESULT CMainWindow::OnCreate (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	bHandled = false;

	assert (m_ctrl != NULL);
	m_ctrl->setStyle(_T("px:left;py:top;width:fill;height:fill;"));

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
