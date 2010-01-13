#ifndef XL_UI_CTRL_MGR_H
#define XL_UI_CTRL_MGR_H
#include <vector>
#include <atlbase.h>
#include <atlwin.h>
#include "Ctrls.h"


namespace xl {
	namespace ui {

class CCtrlMain : public CControl
{
protected:
	ATL::CWindow         *m_pWindow;

	/**
	 * inner properties
	 */
	bool m_captured;
	CControlPtr m_ctrlHover;

	//////////////////////////////////////////////////////////////////////////
	// protected methods

public:
	CCtrlMain (ATL::CWindow *);
	virtual ~CCtrlMain ();

	CControlPtr getControlByID (uint id);

	void invalidateControl (CControlPtr ctrl = CControlPtr());

	BEGIN_MSG_MAP(CCtrlMain)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};


	} // ui
} // xl


#endif
