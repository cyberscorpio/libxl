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

class CCtrlMgr;

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
	friend class CCtrlMgr;
	typedef std::vector<CControlPtr>             CControlContainer;
	typedef CControlContainer::iterator          CControlIter;
	typedef CControlContainer::const_iterator    CControlConstIter;

	CCtrlMgr            *m_mgr;
	uint                 m_id;

	CControlWeakPtr      m_parent;
	CControlContainer    m_controls;
	mutable CRect        m_rect;

	void _LayoutChildren () const;


public:
	CControl (uint id = 0);
	virtual ~CControl ();
	bool init (CCtrlMgr *mgr);

	uint getID () const { return m_id; }

	bool insertChild (CControlPtr child);
	void setParent (CControlPtr parent);

	void draw (HDC hdc);

	//////////////////////////////////////////////////////////////////////////
	// virtual
	virtual CRect layout (CRect rc) const;
	virtual void drawMe (HDC hdc);

	virtual void onMouseIn (CPoint pt) {}
	virtual void onMouseOut (CPoint pt) {}
	virtual void onMouseMove (CPoint pt) {}
};
	} // ui
} // xl
#endif
