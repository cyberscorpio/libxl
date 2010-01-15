#ifndef XL_UI_CTRL_BUTTON_H
#define XL_UI_CTRL_BUTTON_H
#include "../common.h"
#include "../string.h"
#include "Control.h"

namespace xl {
	namespace ui {

class CCtrlButton : public CControl 
{
protected:
	bool m_hover;
	bool m_push;
	tstring m_text;

public:
	CCtrlButton (uint id);
	virtual ~CCtrlButton ();
	void setText (const tstring &text);

	//////////////////////////////////////////////////////////////////////////
	// virtual methods
	virtual void drawMe (HDC hdc);

	virtual void onMouseIn (CPoint pt);
	virtual void onMouseOut (CPoint pt);
	virtual void onLostCapture();
	virtual void onLButtonDown (CPoint pt);
	virtual void onLButtonUp (CPoint pt);

	//////////////////////////////////////////////////////////////////////////
	// virtual button methods
	virtual void drawNormal (HDC hdc);
	virtual void drawHover (HDC hdc);
	virtual void drawPush (HDC hdc);
};

class CCtrlImageButton : public CCtrlButton
{
	uint m_idImageNormal;
	uint m_idImageHover;
	uint m_idImagePush;
public:
	CCtrlImageButton (uint id, uint n = 0, uint h = 0, uint p = 0);
	virtual void drawNormal (HDC hdc);
	virtual void drawHover (HDC hdc);
	virtual void drawPush (HDC hdc);
};


	} // ui
} // xl


#endif
