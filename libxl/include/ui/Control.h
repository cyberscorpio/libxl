#ifndef XL_UI_CONTROL_H
#define XL_UI_CONTROL_H
#include <memory>
#include <vector>
#include <atlbase.h>
#include <atltypes.h>
#include <atlapp.h>
#include "WinStyle.h"

namespace xl {
	namespace ui {

class CCtrlMain;
class CCtrlTarget;

//////////////////////////////////////////////////////////////////////////
// typedef(s)
class CControl;
typedef std::tr1::shared_ptr<CControl>       CControlPtr;
typedef std::tr1::weak_ptr<CControl>         CControlWeakPtr;

typedef std::tr1::shared_ptr<CCtrlTarget>    CCtrlTargetPtr;
typedef CCtrlTarget                          *CCtrlTargetRawPtr;

//////////////////////////////////////////////////////////////////////////
// CControl
class CControl : public CWinStyle
               , public std::tr1::enable_shared_from_this<CControl>
{
protected:
	friend class CCtrlMain;
	typedef std::vector<CControlPtr>             CControlContainer;
	typedef CControlContainer::iterator          CControlIter;
	typedef CControlContainer::const_iterator    CControlConstIter;
	typedef CControlContainer::reverse_iterator  CControlIterR;
	// typedef CControlContainer::const_iterator    CControlConstIter;

	uint                 m_id;

	CControlWeakPtr      m_parent;
	CCtrlTargetRawPtr    m_target;
	CControlContainer    m_controls;
	mutable CRect        m_rect;

	void _LayoutChildren () const;
	CControlPtr _GetControlByPoint (CPoint pt);

	void _SetParent (CControlPtr parent);
	void _SetTarget (CCtrlTargetRawPtr target);

	HFONT _GetFont ();
	void _DrawBorder (HDC hdc);

	bool _SetCapture (bool capture);

	//////////////////////////////////////////////////////////////////////////
	// virtual protected methods
	virtual CCtrlMain* _GetMainCtrl ();

public:
	CControl (uint id = 0);
	virtual ~CControl ();

	uint getID () const { return m_id; }
	CRect getClientRect () const; // exclude the border and padding

	bool insertChild (CControlPtr child);
	CControlPtr getControlByID (uint id);


	//////////////////////////////////////////////////////////////////////////
	// virtual
	virtual CRect layout (CRect rc) const;
	virtual bool isPointIn (CPoint pt) const;
	virtual void draw (HDC hdc, CRect rcClip);
	virtual void drawMe (HDC hdc);

	virtual void onAttach () {}
	virtual void onDetach () {}

	virtual void onMouseIn (CPoint pt) {}
	virtual void onMouseInChild (CPoint pt) {}
	virtual void onMouseOut (CPoint pt) {}
	virtual void onMouseOutChild (CPoint pt) {}
	virtual void onMouseMove (CPoint pt) {}
	virtual void onLButtonDown (CPoint pt) {}
	virtual void onLButtonUp (CPoint pt) {}
	virtual void onRButtonDown (CPoint pt) {}
	virtual void onRButtonUp (CPoint pt) {}
	virtual void onGetCapture () {}
	virtual void onLostCapture () {}
};
	} // ui
} // xl
#endif
