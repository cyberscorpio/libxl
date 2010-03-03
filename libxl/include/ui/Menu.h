#ifndef XL_UI_MENU_H
#define XL_UI_MENU_H
#include <vector>
#include <atlbase.h>
#include <atlwin.h>
#include "../common.h"
#include "../string.h"
#include "CtrlMain.h"
#include "CtrlTarget.h"
XL_BEGIN
UI_BEGIN

class CMenuItem {

	bool               m_disable;
	uint               m_id;
	tstring            m_text;

public:
	CMenuItem (uint id, const tstring &text)
		: m_disable(false)
		, m_id(id)
		, m_text(text)
	{

	}

	uint getId () const {
		return m_id;
	}

	tstring getText () const {
		return m_text;
	}

};

typedef CWinTraits<
                   WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                   WS_EX_TOOLWINDOW
                  > CMenuTraits;

class CMenu
	: public CWindowImplBaseT<ATL::CWindow, CMenuTraits>
	, private xl::ui::CCtrlTarget
{
	typedef ATL::CWindow                                  _TBase;

public:
	DECLARE_WND_CLASS_EX (_T("xl::ui::menu"), 0, COLOR_INFOBK)
	typedef std::vector<CMenuItem>                        MenuItems;

protected:
	uint               m_cmdId;
	CCtrlMainPtr       m_ctrlMain;
	MenuItems          m_items;
	HWND               m_hWndOwner;

	HWND _Create (CRect rc);

public:
	CMenu ();
	virtual ~CMenu ();

	void addItem (CMenuItem &);
	uint show (CPoint ptPosScreen);

	virtual void onCommand (uint id, CControlPtr ctrl);

	BEGIN_MSG_MAP(CMenu)
		{
			CCtrlMain *ctrlMain = (CCtrlMain *)m_ctrlMain.get();
			if (ctrlMain) {
				if(ctrlMain->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult))
					return TRUE; 
			}
		}
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnCreate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

UI_END
XL_END
#endif