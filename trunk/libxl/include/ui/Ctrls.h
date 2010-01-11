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

template <class T>
class CCtrlMgr;

//////////////////////////////////////////////////////////////////////////
// typedef(s)
class CControl;
typedef std::tr1::shared_ptr<CControl>       CControlPtr;

//////////////////////////////////////////////////////////////////////////
// CControl
class CControl : public CWinStyle
{
protected:
	typedef std::vector<CControlPtr>             CControlContainer;
	typedef CControlContainer::iterator          CControlIter;
	typedef CControlContainer::const_iterator    CControlConstIter;

	CCtrlMgr            *m_mgr;
	uint                 m_id;
	CControlContainer    m_controls;
	mutable CRect        m_rect;

	void _LayoutChildren () const;


public:
	CControl (uint id = 0, CCtrlMgr *mgr = NULL);
	virtual ~CControl ();

	uint getID () const { return m_id; }

	bool insertChild (CControlPtr);
	void draw (HDC hdc);

	//////////////////////////////////////////////////////////////////////////
	// virtual
	virtual CRect layout (CRect rc) const;
	virtual void drawMe (HDC hdc);
};
	} // ui
} // xl
#endif
