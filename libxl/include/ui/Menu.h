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

//////////////////////////////////////////////////////////////////////////
// CMenuItem

class CMenuItem {

public:
	enum TYPE {
		TYPE_TEXT,
		TYPE_SEPERATE,
		TYPE_COUNT
	};

private:
	TYPE               m_type;
	bool               m_disable;
	uint               m_id;
	tstring            m_text;
	ushort             m_imgId;
	COLORREF           m_clrKey;

public:
	CMenuItem (uint id, const tstring &text, ushort imgId = 0, bool disable = false, TYPE type = TYPE_TEXT)
		: m_type(type)
		, m_disable(disable)
		, m_id(id)
		, m_text(text)
		, m_imgId(imgId)
		, m_clrKey(RGB(255,0,255))
	{

	}

	TYPE getType () const {
		return m_type;
	}

	bool disable () const {
		return m_disable;
	}

	uint getId () const {
		return m_id;
	}

	tstring getText () const {
		return m_text;
	}

	ushort getImageId () const {
		return m_imgId;
	}

	void setColorKey (COLORREF colorKey) {
		m_clrKey = colorKey;
	}

	COLORREF getColorKey () const {
		return m_clrKey;
	}
};


//////////////////////////////////////////////////////////////////////////
// CMenuCustomizer

class CMenuCustomizer {
	static const uint TEXT_INDENT = 4;
	static const uint TEXT_PADDING = 4;
	static const uint IMAGE_WIDTH = 16;
	static const uint IMAGE_HEIGHT = 16;
	static const uint IMAGE_PADDING = 4;
public:
	CMenuCustomizer ();
	virtual ~CMenuCustomizer ();

	virtual void setMenuControlStyle (xl::ui::CControl *ctrlMenu);
	virtual CSize getItemSize (HDC, xl::ui::CMenuItem &);
	virtual void drawItem (HDC, CRect rcItem, xl::ui::CMenuItem &, bool hover);
};


//////////////////////////////////////////////////////////////////////////
// CMenu

typedef CWinTraits<
                   WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                   WS_EX_TOPMOST | WS_EX_NOACTIVATE// | WS_EX_TOOLWINDOW
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
	HWND               m_hWndOwner;
	CMenuCustomizer   *m_pCustomizer;
	uint               m_cmdId;
	CCtrlMainPtr       m_ctrlMain;
	MenuItems          m_items;

	CMenuCustomizer    m_defaultCustomizer;
	bool               m_initializing;

	HWND _Create (CRect rc);

public:
	CMenu (HWND hWndOwner, CMenuCustomizer * = NULL);
	virtual ~CMenu ();

	void addItem (uint id, const xl::tstring &text, bool disable = false);
	void addImageItem (uint id, const xl::tstring &text, xl::ushort imgId, bool disable = false);
	void addSeperate ();
	uint show (CPoint ptPosScreen);

	// receive command from the menu item
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