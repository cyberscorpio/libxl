#ifndef XL_UI_CTRL_SLIDER_H
#define XL_UI_CTRL_SLIDER_H
#include "../common.h"
#include "../string.h"
#include "Control.h"

namespace xl {
	namespace ui {

class CCtrlSlider : public CControl
{
protected:
	int m_min;
	int m_max;
	int m_curr; // in [m_min, m_max]

	bool m_hoverThumb;
	bool m_pushAndCapture;
	int m_barHeight;
	int m_thumbWidth;
	int m_mouseOffset;

	CRect _GetThumbRect (int thumbWidth) const;
	int _ValueByPoint (CPoint pt, int thumbWidth) const;

protected:
	//////////////////////////////////////////////////////////////////////////
	// protected virtual methods
	virtual void _ParseProperty (const tstring &key, const tstring &value, bool &relayout, bool &redraw);

// 	virtual void _DrawThumb (HDC);
// 	virtual void _DrawThumbHover (HDC);
// 	virtual void _DrawThumbPush (HDC);

public:
	CCtrlSlider (int _min, int _max, int _curr);
	virtual ~CCtrlSlider ();

	//////////////////////////////////////////////////////////////////////////
	// virtual methods
	virtual void drawMe (HDC hdc);

	virtual void onMouseIn (CPoint pt);
	virtual void onMouseOut (CPoint pt);
	virtual void onMouseMove (CPoint pt, uint key);
	virtual void onLostCapture();
	virtual void onLButtonDown (CPoint pt, uint key);
	virtual void onLButtonUp (CPoint pt, uint key);
};

	} // ui
} // xl

#endif
