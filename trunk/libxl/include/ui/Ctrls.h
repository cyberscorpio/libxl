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

//////////////////////////////////////////////////////////////////////////
// typedef(s)
class CControl;
typedef std::tr1::shared_ptr<CControl>    CControlPtr;
typedef std::vector<CControlPtr>          CControlContainer;
typedef CControlContainer::iterator       CControlIter;

//////////////////////////////////////////////////////////////////////////
// CControl
class CControl : public CWinStyle
{
protected:
	CControlContainer    m_controls;
	CRect                m_rect;

	void _LayoutChildren ();

public:
	CControl ();
	virtual ~CControl ();

	void insertChild (CControlPtr);
	void draw (HDC hdc);

	//////////////////////////////////////////////////////////////////////////
	// virtual
	virtual CRect layout (CRect rc);
	virtual void drawMe (HDC hdc);
};
	} // ui
} // xl
#endif
