#include "../libxl/include/ui/gdi.h"
#include "../libxl/include/ui/ResMgr.h"
#include "../libxl/include/ui/CtrlButton.h"
#include "../libxl/include/ui/CtrlSlider.h"
#include "../libxl/include/ui/Menu.h"
#include "../libxl/include/ui/DIBSection.h"
#include "../libxl/include/ui/Bitmap.h"
#include "MainWindow.h"
#include "resource.h"

#define ID_VIEW 99
#define ID_SLIDER 98

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
	CFloat () : xl::ui::CCtrlSlider(0, 0, 0) {
		m_id = ID_SLIDER;
		setStyle(_T("position:left bottom; padding:0; height:32; width:600; float:true; margin:0 auto 20;opacity:25;"));
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
		if (disable) {
			return;
		}
		setStyle(_T("opacity:100"));
		_GetMainCtrl()->invalidateControl(shared_from_this());
	}

	virtual void onMouseOut (CPoint pt) {
		if (disable) {
			return;
		}
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
	CPoint m_pt;
	xl::ui::CDIBSectionPtr m_dib;
public:

	CView () 
		: m_dib(xl::ui::CDIBSection::createDIBSection(333, 333))
	{
		m_id = ID_VIEW;
		setStyle(_T("margin:5; background-color: #323232; px:left; py:top; width:fill; height:fill; "));

		int w = m_dib->getWidth();
		int h = m_dib->getHeight();
		for (int y = 0; y < h; ++ y) {
			unsigned char *data = (unsigned char *)m_dib->getLine(y);
			if (y % 2) {
				for (int x = 0; x < w; ++ x) {
					*data ++ = 0xff;
					*data ++ = 0xff;
					*data ++ = 0xff;
				}
			} else {
				for (int x = 0; x < w; ++ x) {
					data += 2;
					*data ++ = 0xff;
				}
			}
		}

		m_dib = m_dib->clone();
		m_dib = m_dib->cloneAndResize(400, 200, xl::ui::CDIBSection::RT_BICUBIC);
	}

	virtual void onAttach () {
		xl::ui::CCtrlButton *pButton = new xl::ui::CCtrlImageButton(1);
		TCHAR buf[256];
		_stprintf_s(buf, 256, _T("button-image: %d #ff00ff;button-image-text-padding:4"), IDB_BMP5);
 		pButton->setStyle(buf);
 		_stprintf_s(buf, 256, _T("imagebutton-image: %d %d %d #ff00ff"), IDB_BMP1, IDB_BMP2, IDB_BMP3);
 		pButton->setStyle(buf);
		xl::ui::CControlPtr button (pButton);
		button->setStyle(_T("margin:10;width:100;height:40;border:0;font-weight:bold;font-size:20px;"));
		// button->setStyle(_T("opacity:50"));
		pButton->setStyle(_T("border:1 #00ff00;background-color:#ffff00;"));
		insertChild(button);

		pButton->setText(_T("Prompt"));
		pButton = new xl::ui::CCtrlImageButton(2, IDB_BMP1, IDB_BMP2, IDB_BMP3, true);
		button.reset(pButton);
		button->setStyle(_T("margin:10;width:100;height:40;border:0;")); 
		pButton->setText(_T("Hide"));
		insertChild(button);

		pButton = new xl::ui::CCtrlImageButton(3, IDB_BMP1, IDB_BMP2, IDB_BMP3, true);
		button.reset(pButton);
		button->setStyle(_T("margin:10;width:100;height:40;border:0;")); 
		pButton->setText(_T("Disable"));
		insertChild(button);

		pButton = new xl::ui::CCtrlImageButton(4, IDB_BMP1, IDB_BMP2, IDB_BMP3, true);
		button.reset(pButton);
		button->setStyle(_T("margin:10;width:100;height:40;border:0;")); 
		pButton->setText(_T("Enlarge"));
		insertChild(button);

		pButton = new xl::ui::CCtrlImageButton(5, IDB_BMP4, IDB_BMP4, IDB_BMP4, true, RGB(255, 0, 255));
		button.reset(pButton);
		button->setStyle(_T("margin:10;width:40;height:40;border:0;")); 
		insertChild(button);
	}

	virtual void onMouseIn (CPoint pt) {
		m_pt = pt;
		_GetMainCtrl()->invalidateControl(shared_from_this());
	}

	virtual void onMouseOut (CPoint pt) {
		_GetMainCtrl()->invalidateControl(shared_from_this());
	}

	virtual void onMouseMove (CPoint pt, xl::uint) {
		if (m_pt == pt) {
			return;
		}
		m_pt = pt;
		_GetMainCtrl()->invalidateControl(shared_from_this());
	}

	virtual void onLButtonDown (CPoint pt, xl::uint) {
		_Capture(true);
	}

	virtual void onLButtonUp (CPoint pt, xl::uint) {
		_Capture(false);
		invalidate();
	}

	virtual void onRButtonDown (CPoint pt, xl::uint) {
	}

	virtual void onRButtonUp (CPoint pt, xl::uint) {
		CPoint ptScreen = pt;
		_GetMainCtrl()->getWindow()->ClientToScreen(&ptScreen);
		xl::ui::CMenu menu(_GetMainCtrl()->getHWND());
		menu.addItem(1, _T("Copy"));
		menu.addImageItem(2, _T("Paste"), IDB_BMP5, true);
		menu.addItem(3, _T("Cut"));
		menu.addSeperate();
		menu.addImageItem(4, _T("Exit"), IDB_BMP5);
		xl::uint id = menu.show(ptScreen);
		if (id != 0) {
			if (id == 4) {
				_GetMainCtrl()->exit();
				return;
			}
			xl::tchar buf[128];
			_stprintf_s(buf, 128, _T("you select menu: %d"), id);
			::MessageBox(_GetMainCtrl()->getHWND(), buf, _T(""), 0);
		}
	}

	virtual void onLostCapture () {
		_GetMainCtrl()->invalidateControl(shared_from_this());
	}

	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc (hdc);
		CRect rc = getClientRect();

		xl::ui::CDC mdc;
		mdc.CreateCompatibleDC(hdc);
		m_dib->attachToDC(mdc);
		int w = m_dib->getWidth();
		int h = m_dib->getHeight();
		int x = rc.left + (rc.Width() - w) / 2;
		int y = rc.top + (rc.Height() - h) / 2;
		if (x < rc.left) {
			x = rc.left;
		}
		if (y < rc.top) {
			y = rc.top;
		}
		if (x + w > rc.right) {
			w = rc.right - x;
		}
		if (y + h > rc.bottom) {
			h = rc.bottom - y;
		}
		dc.BitBlt(x, y, w, h, mdc, 0, 0, SRCCOPY);

		m_dib->detachFromDC(mdc);

		if (_GetMainCtrl()->getHoverCtrl() == shared_from_this()) {
			TCHAR buf[1024];
			_stprintf_s(buf, 1024, _T("Mouse: %d - %d"), m_pt.x - m_rect.left, m_pt.y - m_rect.top);
			dc.drawTransparentText(buf, -1, rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}

		xl::ui::CResMgr *pResMgr = xl::ui::CResMgr::getInstance();
		xl::ui::CBitmapPtr bmp1 = pResMgr->getBitmap(IDB_BMP1)
			, bmp2 = pResMgr->getTransBitmap(IDB_BMP2, RGB(255, 0, 255))
			, bmp3 = pResMgr->getTransBitmap(IDB_BMP3, RGB(255, 0, 255), true)
			, bmp4 = pResMgr->getTransBitmap(IDB_BMP3, RGB(255, 0, 255), false);
		rc = getClientRect();
		w = bmp1->getWidth();
		h = bmp1->getHeight();
		x = rc.left + 20;
		y = rc.bottom - h - 20;
		bmp1->draw(hdc, x, y, w, h, 0, 0);

		x += w + 10;
		w = bmp2->getWidth();
		h = bmp2->getHeight();
		bmp2->draw(hdc, x, y, w * 3 / 4, h * 3 / 4, 0, 0, w, h);

		x += w * 3 / 4 + 10;
		w = bmp3->getWidth();
		h = bmp3->getHeight();
		bmp3->draw(hdc, x, y, w * 2, h * 2, 0, 0, w, h);

		x += w * 2 + 10;
		w = bmp4->getWidth();
		h = bmp4->getHeight();
		bmp4->draw(hdc, x, y, w * 2, h * 2, 0, 0, w, h);
	}
};

xl::ui::CControlPtr s_removed;
void CMainWindow::onCommand (xl::uint id, xl::ui::CControlPtr ctrl) {
	xl::ui::CCtrlButton *button = (xl::ui::CCtrlButton *)ctrl.get();
	if (id == 1) {
		if (s_removed == NULL) {
			MessageBox(_T("You click button 1"), _T("OK"));
		} else {
			xl::ui::CControlPtr view = m_ctrlMain->getControlByID(ID_VIEW);
			view->insertChild(s_removed);
			s_removed.reset();
		}
	} else if (id == 2) {
		xl::ui::CControlPtr button_1 = m_ctrlMain->getControlByID(1);
		if (!button_1->display) {
			button_1->setStyle(_T("display:true"));
			button_1 = m_ctrlMain->getControlByID(2);
			button->setText(_T("Display"));
		} else {
			button_1->setStyle(_T("display:false"));
			button_1 = m_ctrlMain->getControlByID(2);
			button->setText(_T("Hide"));
		}
	} else if (id == 3) {
		xl::ui::CControlPtr ctrl = m_ctrlMain->getControlByID(1);
		if (!ctrl->disable) {
			ctrl->setStyle(_T("disable:true"));
			ctrl = m_ctrlMain->getControlByID(3);
			button->setText(_T("Enable"));
		} else {
			ctrl->setStyle(_T("disable:false"));
			ctrl = m_ctrlMain->getControlByID(3);
			button->setText(_T("Disable"));
		}
	} else if (id == 4) {
		static bool larged = true;
		larged = !larged;
		xl::ui::CControlPtr ctrl = m_ctrlMain->getControlByID(1);
		if (!larged) {
			ctrl->setStyle(_T("width:120"));
			ctrl = m_ctrlMain->getControlByID(4);
			button->setText(_T("Shrink"));
		} else {
			ctrl->setStyle(_T("width:80"));
			ctrl = m_ctrlMain->getControlByID(4);
			button->setText(_T("Enlarge"));
		}
	} else if (id == 5) {
		// xl::ui::CControlPtr ctrl = m_ctrlMain->removeChild(5);
		// if (ctrl != NULL) {
		// 	s_removed = ctrl;
		// }
		xl::ui::CControlPtr ctrl = m_ctrlMain->getControlByID(ID_SLIDER);
		ctrl->setStyle(_T("disable:false;slider:0 50 50;"));
	}
}

void CMainWindow::onSlider (xl::uint id, int _min, int _max, int _curr, bool tracking, xl::ui::CControlPtr ctrl) {
	xl::ui::CCtrlMain *pCtrlMain = (xl::ui::CCtrlMain *)m_ctrlMain.get();
	xl::ui::CControlPtr view = pCtrlMain->getControlByID(ID_VIEW);
	CView *pView = (CView *)view.get();
	if (_curr == _max) {
		pView->setStyle(_T("background-color: #ffff00"));
	} else if (_curr == 0) {
		// pView->setStyle(_T("background-color: #00ffff"));
		pView->setStyle(_T("background: none"));
	} else {
		TCHAR buf[64];
		int r = _curr * 31, g = _curr * 23, b = _curr * 107;
		_stprintf_s(buf, 64, _T("background-color: #%02x%02x%02x"), r % 256, g % 256, b % 256);
		pView->setStyle(buf);
	}
}

xl::tstring CMainWindow::onGesture (const xl::tstring &gesture, CPoint ptDown, bool release) {
	if (gesture == _T("RL")) {
		return _T("Next");
	} else if (gesture == _T("LR")) {
		return _T("Prev");
	} else if (gesture == _T("DR")) {
		if (release) {
			::MessageBox(m_hWnd, gesture, _T("Gesture:"), 0);
		}
		return _T("Message Box");
	} else if (gesture == _T("canceled")) {
		return _T("手势已取消");
	} else {
		return xl::ui::CCtrlTarget::onGesture(gesture, ptDown, release);
	}
}


LRESULT CMainWindow::OnCreate (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	bHandled = false;

	if (m_ctrlMain == NULL) {
		m_ctrlMain.reset(new xl::ui::CCtrlMain(this, this));
	}
	m_ctrlMain->setStyle(_T("px:left;py:top;width:fill;height:fill;"));
	m_ctrlMain->enableGesture(true);
	m_ctrlMain->getGestureCtrl()->setStyle(_T("color:#ff0000;background:none;gesture-sensitivity:20"));

	xl::ui::CControlPtr ctrl(new CToolbar());
	m_ctrlMain->insertChild(ctrl);

	ctrl.reset(new CStatusbar);
	m_ctrlMain->insertChild(ctrl);

	xl::ui::CControlPtr client(new CClient());
	m_ctrlMain->insertChild(client);

	ctrl.reset(new CExplorer());
	client->insertChild(ctrl);

	ctrl.reset(new CExplorer());
	ctrl->setStyle(_T("px:right"));
	client->insertChild(ctrl);

	ctrl.reset(new CView());
	client->insertChild(ctrl);

	ctrl.reset(new CFloat());
	m_ctrlMain->insertChild(ctrl);

	return TRUE;
}


LRESULT CMainWindow::OnSize (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	if (m_ctrlMain) {
		CRect rc;
		GetClientRect(rc);
		m_ctrlMain->layout(rc);
	}
	return 0;
}
