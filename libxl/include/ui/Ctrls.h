#ifndef XL_UI_CTRLS_H
#define XL_UI_CTRLS_H
#include <memory>
#include <vector>
#include <atlbase.h>
#include <atltypes.h>
#include <atlapp.h>
#include "WinStyle.h"

namespace xl {
	namespace ui {

class CCtrlMain;

//////////////////////////////////////////////////////////////////////////
// typedef(s)
class CControl;
typedef std::tr1::shared_ptr<CControl>       CControlPtr;
typedef std::tr1::weak_ptr<CControl>         CControlWeakPtr;

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
	CControlContainer    m_controls;
	mutable CRect        m_rect;

	CCtrlMain* _GetMainCtrl ();
	void _LayoutChildren () const;
	CControlPtr _GetControlByPoint (CPoint pt);

	bool _SetCapture (bool capture);

public:
	CControl (uint id = 0);
	virtual ~CControl ();

	uint getID () const { return m_id; }

	bool insertChild (CControlPtr child);
	CControlPtr getControlByID (uint id);
	void setParent (CControlPtr parent);


	//////////////////////////////////////////////////////////////////////////
	// virtual
	virtual CRect layout (CRect rc) const;
	virtual void draw (HDC hdc, CRect rcClip);
	virtual void drawMe (HDC hdc);

	virtual void onMouseIn (CPoint pt) {}
	virtual void onMouseOut (CPoint pt) {}
	virtual void onMouseMove (CPoint pt) {}
	virtual void onLButtonDown (CPoint pt) {}
	virtual void onLButtonUp (CPoint pt) {}
	virtual void onGetCapture () {}
	virtual void onLostCapture () {}
};
	} // ui
} // xl
#endif
