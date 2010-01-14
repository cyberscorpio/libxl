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

	virtual void onUserEvent (uint evt, void *param, CControlPtr ctrl) {}
};

	} // ui
} // xl


#endif
