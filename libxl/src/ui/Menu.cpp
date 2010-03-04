#include <assert.h>
// #include <atlbase.h>
// #include <atltypes.h>
#include "../../include/ui/Menu.h"
#include "../../include/ui/CtrlTarget.h"
#include "../../include/ui/Gdi.h"
#include "../../include/ui/ResMgr.h"
#include "../../include/utilities.h"

namespace {

class CMenuItemControl : public xl::ui::CControl {
	xl::ui::CMenuItem                             &m_item;
	xl::ui::CMenuCustomizer                       *m_pCustomizer;
	bool                                           m_hover;
public:
	CMenuItemControl (xl::ui::CMenuItem &item, xl::ui::CMenuCustomizer *pCustomizer)
		: m_item(item)
		, m_pCustomizer(pCustomizer)
		, m_hover(false)
	{
		assert(m_pCustomizer != NULL);
		setStyle(_T("margin:0px;padding:0px;"));
	}

	virtual ~CMenuItemControl () {

	}

	virtual void drawMe (HDC hdc) {
		assert(m_pCustomizer != NULL);
		m_pCustomizer->drawItem(hdc, getClientRect(), m_item, m_hover);
	}

	virtual void onLButtonDown (CPoint /*pt*/, xl::uint /*key*/) {
		if (m_item.disable() || m_item.getType() == xl::ui::CMenuItem::TYPE_SEPERATE) {
			return;
		}
		_Capture(true);
	}

	virtual void onLButtonUp (CPoint pt, xl::uint /*key*/) {
		if (m_item.disable() || m_item.getType() == xl::ui::CMenuItem::TYPE_SEPERATE) {
			return;
		}
		_Capture(false);
		if (m_rect.PtInRect(pt)) {
			assert(_GetTarget() != NULL);
			_GetTarget()->onCommand(m_item.getId(), shared_from_this());
		}
	}

	virtual void onMouseIn (CPoint /*pt*/) {
		m_hover = true;
		invalidate();
	}

	virtual void onMouseOut (CPoint /*pt*/) {
		m_hover = false;
		invalidate();
	}
};

class CMenuControl 
	: public xl::ui::CControl
	, xl::ui::CCtrlTarget
{
	xl::ui::CMenu::MenuItems                      *m_pItems;
	xl::ui::CMenuCustomizer                       *m_pCustomizer;

	void _CreateChildren () {
		assert(m_pItems != NULL);
		int y = 0;
		xl::tchar style[128];
		assert(_GetMainCtrl());
		HWND hWnd = _GetMainCtrl()->getHWND();
		HDC hdc = ::GetDC(hWnd);
		xl::ui::CMenu::MenuItems::iterator it, end = m_pItems->end();
		for (it = m_pItems->begin(); it != end; ++ it) {
			xl::ui::CControlPtr child;
			CMenuItemControl *pItemCtrl = new CMenuItemControl(*it, m_pCustomizer);
			child.reset(pItemCtrl);
			insertChild(child);
			CSize sz = m_pCustomizer->getItemSize(hdc, *it);
			y += sz.cy;
			_stprintf_s(style, 128, _T("width:fill;height:%dpx;"), sz.cy);
			child->setStyle(style);
		}
		::ReleaseDC(hWnd, hdc);
	}

public:
	CMenuControl (xl::ui::CMenu::MenuItems *pItems, xl::ui::CMenuCustomizer *pCustomizer) 
		: xl::ui::CControl(0)
		, m_pItems(pItems)
		, m_pCustomizer(pCustomizer)
	{
		assert(m_pCustomizer != NULL);
		assert(m_pItems != NULL);
		setStyle(_T("width:fill;height:fill;px:left;py:top;margin:0;padding:0;background-color:#ffffff;"));
		m_pCustomizer->setMenuControlStyle(this);		
	}

	CSize reportSize (HDC hdc) {
		assert(m_pCustomizer != NULL);

		int w = 0, h = 0;
		xl::ui::CMenu::MenuItems::iterator it, end = m_pItems->end();
		for (it = m_pItems->begin(); it != end; ++ it) {
			CSize szItem = m_pCustomizer->getItemSize(hdc, *it);
			if (w < szItem.cx) {
				w = szItem.cx;
			}
			h += szItem.cy;
		}

		CRect rc = getClientRect();
		w += m_rect.Width() - rc.Width();
		h += m_rect.Height() - rc.Height();

		return CSize(w, h);
	}


	virtual void onAttach () {
		_CreateChildren();
	}

	virtual void onMouseOutChild (CPoint pt) {
		if (!m_rect.PtInRect(pt)) {
			_SetTimer(250);
		}
	}

	virtual void onMouseOut (CPoint pt) {
		if (!m_rect.PtInRect(pt)) {
			_SetTimer(250);
		}
	}

	virtual void onTimer (xl::uint /*id*/) {
		assert(_GetMainCtrl() && _GetMainCtrl()->getWindow());
		CPoint pt;
		::GetCursorPos(&pt);
		_GetMainCtrl()->getWindow()->ScreenToClient(&pt);
		if (!m_rect.PtInRect(pt)) {
			assert(_GetTarget() != NULL);
			_GetTarget()->onCommand(0, shared_from_this());
		}
	}
};

}


XL_BEGIN
UI_BEGIN


//////////////////////////////////////////////////////////////////////////
// CMenuCustomizer
CMenuCustomizer::CMenuCustomizer () {

}

CMenuCustomizer::~CMenuCustomizer () {

}

void CMenuCustomizer::setMenuControlStyle (xl::ui::CControl *control) {
	assert(control != NULL);
	control->setStyle(_T("padding:2;background-color:#c0c0ff"));
}

CSize CMenuCustomizer::getItemSize (HDC hdc, CMenuItem &item) {
	const int minw = 120;
	const int minh = IMAGE_HEIGHT + IMAGE_PADDING * 2;
	if (item.getType() == CMenuItem::TYPE_SEPERATE) {
		return CSize(minw, 3);
	} else {
		xl::ui::CDCHandle dc(hdc);
		CRect rc(0, 0, 0, 0);
		uint fmt = DT_SINGLELINE | DT_LEFT | DT_CALCRECT;
		int h = dc.drawTransparentTextWithDefaultFont(item.getText(), -1, rc, fmt);
		h += TEXT_PADDING * 2;
		int w = rc.Width() + TEXT_INDENT * 2;
		w += (IMAGE_WIDTH + IMAGE_PADDING * 2);
		if (w < minw) {
			w = minw;
		}
		if (h < minh) {
			h = minh;
		}
		return CSize(w, h);
	}
}

void CMenuCustomizer::drawItem (HDC hdc, CRect rc, CMenuItem &item, bool hover) {
	xl::ui::CDCHandle dc(hdc);

	if (item.getType() == CMenuItem::TYPE_SEPERATE) {
		dc.FillSolidRect(rc, (COLORREF)::GetSysColor(COLOR_MENU));
		rc.left += TEXT_INDENT;
		rc.right -= TEXT_INDENT;
		int y = (rc.bottom - rc.top) / 2 + rc.top;
		int x1 = rc.left;
		int x2 = rc.right;
		HPEN pen = ::CreatePen(PS_SOLID, 1, RGB(192,192,192));
		HPEN oldPen = dc.SelectPen(pen);
		dc.drawLine(x1, y, x2, y);
		dc.SelectPen(oldPen);
		::DeleteObject(pen);
	} else {

		COLORREF textColor = (COLORREF)::GetSysColor(COLOR_MENUTEXT);
		COLORREF bgColor = (COLORREF)::GetSysColor(COLOR_MENU);
		if (item.disable()) {
			textColor = (COLORREF)::GetSysColor(COLOR_GRAYTEXT);
		} else if (hover) {
			textColor = (COLORREF)::GetSysColor(COLOR_HIGHLIGHTTEXT);
			bgColor = (COLORREF)::GetSysColor(COLOR_HIGHLIGHT);
		}

		CRect rcText = rc;
		rcText.left += IMAGE_PADDING * 2 + IMAGE_WIDTH;
		rcText.left += TEXT_INDENT;

		dc.FillSolidRect(rc, bgColor);
		COLORREF old = dc.SetTextColor(textColor);
		uint fmt = DT_SINGLELINE | DT_LEFT | DT_VCENTER;
		dc.drawTransparentTextWithDefaultFont(item.getText(), -1, rcText, fmt);
		dc.SetTextColor(old);

		if (item.getImageId() != 0) {
			CResMgr *pResMgr = CResMgr::getInstance();
			CBitmapPtr img = pResMgr->getTransBitmap(item.getImageId(), item.getColorKey(), item.disable());
			assert(img != NULL);
			int x = rc.left + IMAGE_PADDING;
			int y = rc.top + IMAGE_PADDING;
			img->draw(hdc, x, y, IMAGE_WIDTH, IMAGE_HEIGHT, 0, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CMenu

HWND CMenu::_Create(CRect rc)
{
	ATOM atom = GetWndClassInfo().Register(&m_pfnSuperWindowProc);

	DWORD dwStyle = WS_VISIBLE, dwExStyle = 0;
	dwStyle |= GetWndStyle(0);
	dwExStyle |= GetWndExStyle(0);

	return Create(m_hWndOwner, rc, _T("xl::ui::menu"), dwStyle, dwExStyle, (HMENU)0, atom, NULL);
}



CMenu::CMenu (HWND hWndOwner, CMenuCustomizer *pCustomizer)
	: m_hWndOwner(hWndOwner)
	, m_pCustomizer(pCustomizer)
	, m_cmdId(0)
	, m_initializing(true)
{
	if (m_pCustomizer == NULL) {
		m_pCustomizer = &m_defaultCustomizer;
	}

	assert(m_hWndOwner != NULL);
// 	for (int i = 100; i < 110; ++ i) {
// 		xl::tchar buf[32];
// 		_stprintf_s(buf, 32, _T("%dth item"), i);
// 		addItem(i, buf);
// 
// 		if (i % 7 == 0) {
// 			addSeperate();
// 		}
// 	}
}

CMenu::~CMenu () {

}

// void CMenu::addItem (CMenuItem &item) {
// 	m_items.push_back(item);
// }

void CMenu::addItem (uint id, const xl::tstring &text, bool disable) {
	assert(id != 0);
	CMenuItem item(id, text, 0, disable, CMenuItem::TYPE_TEXT);
	m_items.push_back(item);
}

void CMenu::addImageItem (uint id, const xl::tstring &text, xl::ushort imgId, bool disable) {
	assert(id != 0);
	CMenuItem item(id, text, imgId, disable, CMenuItem::TYPE_TEXT);
	m_items.push_back(item);
}

void CMenu::addSeperate () {
	CMenuItem item(0, _T(""), 0, false, CMenuItem::TYPE_SEPERATE);
	m_items.push_back(item);
}


uint CMenu::show (CPoint ptPos) {
	assert(m_items.size() > 0);

	// 1. use the cursor position as the window's position, and it size is 0
	CRect rc(ptPos.x, ptPos.y, ptPos.x, ptPos.y);

	// 2. create the window (resize itself in OnCreate())
	HWND hWnd = _Create(rc);
	if (!hWnd) {
		return 0;
	}
	assert(m_hWnd == hWnd);
	// ::SetForegroundWindow(m_hWndOwner);
	SetFocus();
	m_initializing = false;

	// 3. the message loop
	MSG msg;
	BOOL ret;
	while (m_hWnd != NULL) {
		ret = ::GetMessage(&msg, NULL, 0, 0);
		if (ret == 0 || ret == -1) {
			break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	// 4. return the command id
	return m_cmdId;
}

void CMenu::onCommand (uint id, CControlPtr ctrl) {
	m_cmdId = id;
	DestroyWindow();
}

LRESULT CMenu::OnKillFocus (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	if (!m_initializing) {
		DestroyWindow();
	}
	return 0;
}

LRESULT CMenu::OnCreate (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	m_ctrlMain.reset(new xl::ui::CCtrlMain(this, this));
	CMenuControl *pMenuCtrl = new CMenuControl(&m_items, m_pCustomizer);
	CControlPtr control(pMenuCtrl);
	m_ctrlMain->insertChild(control);

	HDC hdc = GetDC();
	CSize size = pMenuCtrl->reportSize(hdc);
	ReleaseDC(hdc);

	int sw = ::GetSystemMetrics(SM_CXSCREEN);
	int sh = ::GetSystemMetrics(SM_CYSCREEN);
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	int x = rcWindow.left;
	int y = rcWindow.top;
	x -= 8;
	if (x < 0) {
		x = 0;
	}
	y -= 8;
	if (y < 0) {
		y = 0;
	}

	rcWindow.left = x;
	rcWindow.top = y;
	rcWindow.right = x + size.cx;
	rcWindow.bottom = y + size.cy;

	if (rcWindow.right > sw) {
		rcWindow.OffsetRect(sw - rcWindow.right, 0);
	}
	if (rcWindow.bottom > sh) {
		rcWindow.OffsetRect(0, sh - rcWindow.bottom);
	}

	SetWindowPos(NULL, rcWindow, SWP_NOZORDER | SWP_NOACTIVATE);

	return 0;
}

LRESULT CMenu::OnDestroy (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	return 0;
}

LRESULT CMenu::OnSize (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	CRect rc;
	GetClientRect(rc);
	assert(m_ctrlMain != NULL);
	m_ctrlMain->layout(rc);

	return 0;
}



UI_END
XL_END