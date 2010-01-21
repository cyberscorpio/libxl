#ifndef XL_UI_CTRL_GESTURE_H
#define XL_UI_CTRL_GESTURE_H
#include <vector>
#include "../common.h"
#include "../string.h"
#include "Control.h"

namespace xl {
	namespace ui {

class CCtrlMain;
class CCtrlGesture : public CControl 
{
	CCtrlMain             *m_pCtrlMain;
	int                    m_gestureSensitivity;
	uint                   m_gestureTimeout;
	int                    m_gestureLineWidth;
	std::vector<CPoint>    m_points;
	uint                   m_lastMove;
	tstring                m_gesture;
	bool                   m_isTimeout;

protected:
	virtual void _ParseProperty (const tstring &key, const tstring &value, bool &relayout, bool &redraw);

public:
	CCtrlGesture (CCtrlMain *pCtrlMain);
	virtual ~CCtrlGesture ();

	virtual void onLostCapture();
	virtual void onRButtonDown (CPoint pt, uint key);
	virtual void onRButtonUp (CPoint pt, uint key);
	virtual void onMouseMove (CPoint pt, uint key);
	virtual void onTimer (uint id);
	virtual void drawMe (HDC hdc);
};

typedef std::tr1::shared_ptr<CCtrlGesture>     CCtrlGesturePtr;

	} // ui
} // xl

#endif
