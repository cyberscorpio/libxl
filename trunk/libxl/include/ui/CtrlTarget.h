#ifndef XL_UI_CTRL_TARGET_H
#define XL_UI_CTRL_TARGET_H
#include "../common.h"
#include "Control.h"

namespace xl {
	namespace ui {

class CCtrlTarget
{
public:
	virtual void onCommand (uint id, CControlPtr ctrl) {}
	virtual void onSlider (uint id, int _min, int _max, int _curr, bool tracking, CControlPtr ctrl) {}
	virtual void onGesture (const tstring &gesture) {}

	virtual void onUserEvent (uint evt, void *param, CControlPtr ctrl) {}
};

	} // ui
} // xl


#endif
