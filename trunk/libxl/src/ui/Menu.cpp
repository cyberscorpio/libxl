#include <assert.h>
// #include <atlbase.h>
// #include <atltypes.h>
#include "../../include/ui/Menu.h"
#include "../../include/ui/CtrlTarget.h"
#include "../../include/ui/Gdi.h"

static const int PADDING = 4;
static const int MINWIDTH = 100;

namespace {

class CMenuItemControl : public xl::ui::CControl {
	xl::ui::CMenuItem m_item;

public:
	CMenuItemControl (xl::ui::CMenuItem item) : m_item(item) {

	}

	virtual ~CMenuItemControl () {

	}

	CSize getItemSize (HDC hdc) const {
		xl::ui::CDCHandle dc(hdc);
		CRect rc = getClientRect();

		xl::uint fmt = DT_SINGLELINE | DT_LEFT | DT_CALCRECT;
		int height = dc.drawTransparentTextWithDefaultFont(m_item.getText(), -1, rc, fmt);
		CSize sz(rc.Width() + PADDING * 2, height + PADDING * 2);
		return sz;
	}

	virtual void drawMe (HDC hdc) {
		xl::ui::CDCHandle dc(hdc);
		CRect rc = getClientRect();
		rc.DeflateRect(PADDING, PADDING, PADDING, PADDING);

		xl::uint fmt = DT_VCENTER | DT_SINGLELINE | DT_LEFT;
		dc.drawTransparentTextWithDefaultFont(m_item.getText(), -1, rc, fmt);
	}

	virtual void onLButtonDown (CPoint pt, xl::uint key) {
		assert(_GetTarget() != NULL);
		_GetTarget()->onCommand(m_item.getId(), shared_from_this());
	}
};

class CMenuControl 
	: public xl::ui::CControl
	, xl::ui::CCtrlTarget
{
	xl::ui::CMenu::MenuItems                      *m_pItems;
	CSize                                          m_size;

	void _CreateChildren () {
		assert(m_pItems != NULL);
		int x = MINWIDTH, y = 0;
		xl::tchar style[128];
		HDC hdc = ::GetDC(NULL);
		xl::ui::CMenu::MenuItems::iterator it, end = m_pItems->end();
		for (it = m_pItems->begin(); it != end; ++ it) {
			xl::ui::CControlPtr child;
			CMenuItemControl *pItemCtrl = new CMenuItemControl(*it);
			child.reset(pItemCtrl);
			insertChild(child);
			CSize sz = pItemCtrl->getItemSize(hdc);
			if (sz.cx > x) {
				x = sz.cx;
			}
			y += sz.cy;
			_stprintf_s(style, 128, _T("width:fill;height:%dpx;"), sz.cy);
			child->setStyle(style);
		}
		::ReleaseDC(NULL, hdc);

		m_size.cx = x;
		m_size.cy = y;
	}

public:
	CMenuControl (xl::ui::CMenu::MenuItems *pItems) 
		: xl::ui::CControl(0)
		, m_pItems(pItems)
	{
		assert(m_pItems != NULL);
		setStyle(_T("width:fill;height:fill;px:left;py:top;margin:0;padding:0;background-color:#ffffff;"));
	}

	CSize getSize () {
		return m_size;
	}

	virtual void onAttach () {
		_CreateChildren();
	}

	virtual void onMouseIn () {
	}

	virtual void onLostCapture () {
	}
};

}


XL_BEGIN
UI_BEGIN


HWND CMenu::_Create(CRect rc)
{
	ATOM atom = GetWndClassInfo().Register(&m_pfnSuperWindowProc);

	DWORD dwStyle = WS_VISIBLE, dwExStyle = 0;
	dwStyle |= GetWndStyle(0);
	dwExStyle |= GetWndExStyle(0);

	return Create(NULL, rc, _T("xl::ui::menu"), dwStyle, dwExStyle, (HMENU)0, atom, NULL);
}



CMenu::CMenu ()
	: m_cmdId(0)
	, m_hWndOwner(::GetFocus())
{
	assert(m_hWndOwner != NULL);
	for (int i = 100; i < 110; ++ i) {
		xl::tchar buf[32];
		_stprintf_s(buf, 32, _T("%dth item"), i);
		CMenuItem item(i, buf);
		addItem(item);
	}
}

CMenu::~CMenu () {

}

void CMenu::addItem (CMenuItem &item) {
	m_items.push_back(item);
}

uint CMenu::show (CPoint ptPos) {
	// 1. calculate the size
	CRect rc(ptPos.x, ptPos.y, ptPos.x, ptPos.y);

	// 2. create the window
	HWND hWnd = _Create(rc);
	if (!hWnd) {
		return 0;
	}
	::SetFocus(hWnd);

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

LRESULT CMenu::OnKillFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	DestroyWindow();
	return 0;
}

LRESULT CMenu::OnCreate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	m_ctrlMain.reset(new xl::ui::CCtrlMain(this, this));
	CMenuControl *pMenuCtrl = new CMenuControl(&m_items);
	xl::ui::CControlPtr control(pMenuCtrl);
	m_ctrlMain->insertChild(control);
	CSize size = pMenuCtrl->getSize();

	int sw = ::GetSystemMetrics(SM_CXSCREEN);
	int sh = ::GetSystemMetrics(SM_CYSCREEN);
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	int x = rcWindow.left;
	int y = rcWindow.top;
	x -= 4;
	y -= 4;

	rcWindow.left = x;
	rcWindow.top = y;
	rcWindow.right = x + size.cx;
	rcWindow.bottom = y + size.cy;

	SetWindowPos(NULL, rcWindow, SWP_NOZORDER);

	return 0;
}

LRESULT CMenu::OnDestroy (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	::SetFocus(m_hWndOwner);
	return 0;
}

LRESULT CMenu::OnSize (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CRect rc;
	GetClientRect(rc);
	m_ctrlMain->layout(rc);
	return 0;
}



UI_END
XL_END