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
	std::vector<CPoint>    m_points;
	uint                   m_lastMove;
	tstring                m_gesture;
	bool                   m_inRButtonUp;
public:
	CCtrlGesture (CCtrlMain *pCtrlMain);
	virtual ~CCtrlGesture ();

	virtual void onLostCapture();
	virtual void onRButtonDown (CPoint pt);
	virtual void onRButtonUp (CPoint pt);
	virtual void onMouseMove (CPoint pt);
	virtual void drawMe (HDC hdc);
};

typedef std::tr1::shared_ptr<CCtrlGesture>     CCtrlGesturePtr;

	} // ui
} // xl

#endif
