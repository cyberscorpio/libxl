#ifndef XL_UI_CTRL_TARGET_H
#define XL_UI_CTRL_TARGET_H
#include "../common.h"
#include "../string.h"
#include "Control.h"

namespace xl {
	namespace ui {

class CCtrlTarget
{
public:
	virtual void onCommand (uint id, CControlPtr ctrl) {}
	virtual void onSlider (uint id, int _min, int _max, int _curr, bool tracking, CControlPtr ctrl) {}

	virtual tstring onGesture (const tstring &gesture, bool release) { return _T("Unknown"); }

	virtual void onUserEvent (uint evt, void *param, CControlPtr ctrl) {}
};

	} // ui
} // xl


#endif
