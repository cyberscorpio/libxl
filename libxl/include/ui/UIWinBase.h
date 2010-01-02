#ifndef XL_UI_WINBASE_H
#define XL_UI_WINBASE_H
#include <atlbase.h>
#include <atltypes.h>
#include <atlapp.h>
#include <atltypes.h>

namespace xl {
	namespace ui {

enum EDGE {
	TOP,
	RIGHT,
	BOTTOM,
	LEFT
};

template <class T>
class CUIWinBaseT
{
protected:
	int margin[4];
	int padding[4];

public:
	CUIWinBaseT () {
		for (int i = 0; i < 4; ++ i) {
			margin[i] = padding[i] = 0;
		}
	}

	CRect getContentRect () {
		CRect rc;
		T *p = (T *)this;
		p->GetClientRect(rc);
		rc.left += padding[LEFT];
		rc.right -= padding[RIGHT];
		rc.top += padding[TOP];
		rc.bottom -= padding[BOTTOM];
		return rc;
	}
};

	} // namespace ui
} // namespace xl

#endif
