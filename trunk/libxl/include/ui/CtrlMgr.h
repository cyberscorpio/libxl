#ifndef XL_UI_CTRL_MGR_H
#define XL_UI_CTRL_MGR_H
#include <vector>
#include <atlbase.h>
#include <atlwin.h>
#include "Ctrls.h"


namespace xl {
	namespace ui {

class CCtrlMgr
{
protected:
	typedef std::vector<CControlPtr>                    CControlContainer;
	typedef CControlContainer::iterator                 CControlIter;
	typedef CControlContainer::const_iterator           CControlConstIter;
	typedef CControlContainer::reverse_iterator         CControlIterR;
	typedef CControlContainer::const_reverse_iterator   CControlConstIterR;

	ATL::CWindow         *m_pWindow;
	CControlContainer     m_controls;

	/**
	 * inner properties
	 */
	bool m_captured;
	CControlPtr m_ctrlHover;

	//////////////////////////////////////////////////////////////////////////
	// protected methods
	CControlPtr _GetControlByPoint (CPoint pt);

public:
	CCtrlMgr (ATL::CWindow *);
	virtual ~CCtrlMgr ();

	bool insertControl (CControlPtr ctrl);
	bool removeControl (CControlPtr ctrl);
	bool removeControl (uint id);

	// void draw ();
	void invalidateControl (CControlPtr ctrl = CControlPtr());

	BEGIN_MSG_MAP(CCtrlMgr)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};


	} // ui
} // xl


#endif
