#ifndef XL_UI_BITMAP_H
#define XL_UI_BITMAP_H
#include <memory>
#include "../common.h"
#include "DIBSection.h"
XL_BEGIN
UI_BEGIN

class CBitmap;
typedef std::tr1::shared_ptr<CBitmap>  CBitmapPtr;

class CBitmap : public CDIBSection {
	COLORREF           m_rgbTrans;
	COLORREF          *m_pTransColor;

public:
	CBitmap ();
	CBitmap (COLORREF colorKey);
	virtual ~CBitmap ();

	void setColorKey (COLORREF colorKey);
	void clearColorKey ();

	void gray ();

	bool load (HBITMAP);
	bool load (int id); // load from resource

	void draw (HDC hdc, int toX, int toY, int toW, int toH, int fromX, int fromY, uint op = SRCCOPY);
	void draw (HDC hdc, int toX, int toY, int toW, int toH, int fromX, int fromY, int fromW, int fromH, uint op = SRCCOPY);
};

UI_END
XL_END
#endif
