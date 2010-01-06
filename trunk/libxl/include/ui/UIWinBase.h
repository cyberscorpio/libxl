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
class CUIWinBaseT : public WinStyle
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
		rc.left += padding.left;
		rc.right -= padding.right;
		rc.top += padding.top;
		rc.bottom -= padding.bottom;
		return rc;
	}
};

	} // namespace ui
} // namespace xl

#endif
