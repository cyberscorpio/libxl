#ifndef XL_UI_CTRL_BUTTON_H
#define XL_UI_CTRL_BUTTON_H
#include "../common.h"
#include "../string.h"
#include "Control.h"
#include "Bitmap.h"

XL_BEGIN
UI_BEGIN

class CCtrlButton : public CControl 
{
protected:
	bool               m_pushAndCapture;
	tstring            m_text;
	int                m_text_image_pading;
	uint               m_imgId;
	bool               m_imgTrans;
	COLORREF           m_imgClrKey;

	void _DrawImageAndText (HDC);

	//////////////////////////////////////////////////////////////////////////
	// protected virtual methods
	virtual void _ParseProperty (const tstring &key, const tstring &value, bool &relayout, bool &redraw);

public:
	CCtrlButton (uint id, const tstring &text = _T(""), uint idImg = 0, bool imgTrans = false, COLORREF clrKey = RGB(255, 0, 255));
	virtual ~CCtrlButton ();
	void setText (const tstring &text);

	//////////////////////////////////////////////////////////////////////////
	// virtual methods
	virtual void drawMe (HDC hdc);

	virtual void onMouseIn (CPoint pt);
	virtual void onMouseOut (CPoint pt);
	virtual void onLostCapture();
	virtual void onLButtonDown (CPoint pt, uint key);
	virtual void onLButtonUp (CPoint pt, uint key);
};

class CCtrlImageButton : public CCtrlButton
{
	uint               m_imageIds[3]; // normal, hover, pushed
	COLORREF           m_colorKey;
	COLORREF          *m_pColorKey;
protected:

	CBitmapPtr _GetImage();

	//////////////////////////////////////////////////////////////////////////
	// protected virtual methods
	virtual void _ParseProperty (const tstring &key, const tstring &value, bool &relayout, bool &redraw);

public:
	CCtrlImageButton (uint id, uint normalId = 0, uint hoverId = 0, uint pushId = 0, bool bitmapTrans = false, COLORREF colorKey = RGB(255, 0, 255));

	virtual void drawMe (HDC hdc);
	virtual void onMouseIn (CPoint pt);
	virtual void onMouseOut (CPoint pt);
	virtual void onLostCapture ();
	virtual void onLButtonDown (CPoint pt, uint key);
	virtual void onLButtonUp (CPoint pt, uint key);
};


UI_END
XL_END

#endif
