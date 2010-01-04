#ifndef XL_UI_WINBASE_H
#define XL_UI_WINBASE_H
#include <atlbase.h>
#include <atltypes.h>
#include <atlapp.h>
#include <atltypes.h>
#include "WinStyle.h"

namespace xl {
	namespace ui {


template <class T>
class CUIWinBaseT
{
protected:
	WinStyle m_style;

public:
	CUIWinBaseT () {
	}

	CRect getContentRect () {
		CRect rc;
		T *p = (T *)this;
		p->GetClientRect(rc);
		rc.left += padding[EDGE_LEFT];
		rc.right -= padding[EDGE_RIGHT];
		rc.top += padding[EDGE_TOP];
		rc.bottom -= padding[EDGE_BOTTOM];
		return rc;
	}
};

	} // namespace ui
} // namespace xl

#endif
