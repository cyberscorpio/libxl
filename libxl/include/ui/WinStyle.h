#ifndef XL_UI_WINSTYLE_H
#define XL_UI_WINSTYLE_H
#include <limits>
#include "../common.h"
#include "../string.h"

#ifdef max // <windows.h> defines max & min
#define RESTORE_MIN_MAX
#pragma push_macro ("min")
#pragma push_macro ("max")
#undef max
#undef min
#endif


XL_BEGIN
UI_BEGIN



const int SIZE_FILL = -1;
const int EDGE_AUTO = std::numeric_limits<int>::max();

/**
 *  edge type
 */
enum EDGETYPE {
	ET_TOP,
	ET_RIGHT,
	ET_BOTTOM,
	ET_LEFT,
	ET_ALL,
	ET_COUNT = ET_ALL
};

/**
 * edge, used by margin or padding, and so on
 */
struct EDGE {
	int top;
	int right;
	int bottom;
	int left;

	EDGE (int t = 0, int r = 0, int b = 0, int l = 0)
		: top(t), right(r), bottom(b), left(l) {}
	int width () const {
		return (left == EDGE_AUTO ? 0 : left) + (right == EDGE_AUTO ? 0 : right);
	}
	int height () const {
		return (top == EDGE_AUTO ? 0 : top) + (bottom == EDGE_AUTO ? 0 : bottom);
	}
};

struct BORDEREDGE {
	int width;
	COLORREF color;
	uint style; // not used now

	BORDEREDGE () {
		reset();
	}

	void reset () {
		width = 0;
		color = RGB(0,0,0);
		style = 0;
	}
};

struct BORDER {
	BORDEREDGE top;
	BORDEREDGE right;
	BORDEREDGE bottom;
	BORDEREDGE left;

	void reset ();
	void setWidth (int width, EDGETYPE et = ET_ALL);
	void setColor (COLORREF color, EDGETYPE et = ET_ALL);
	int width () const { return left.width + right.width; }
	int height () const { return top.width + bottom.width; }
};

enum BACKGROUNDTYPE {
	BGT_NONE,
	BGT_RGB,
	BGT_IMAGE_ID,
	BGT_IMAGE_URL,
	BGT_COUNT
};

enum BACKGROUNDIMAGEPOS_X {
	BGIPX_FILL,
	BGIPX_LEFT,
	BGIPX_RIGHT,
	BGIPX_CENTER,
	BGIPX_REPEAT,
	GBIPX_COUNT
};

enum BACKGROUNDIMAGEPOS_Y {
	BGIPY_FILL,
	BGIPY_TOP,
	BGIPY_BOTTOM,
	BGIPY_CENTER,
	BGIPY_REPEAT,
	BGIPY_COUNT
};

struct BACKGROUND {
	BACKGROUNDTYPE type;
	COLORREF color;
	uint id;
	tstring url_or_idtype;
	BACKGROUNDIMAGEPOS_X x;
	BACKGROUNDIMAGEPOS_Y y;

public:
	BACKGROUND () 
		: type(BGT_NONE), color(RGB(0,0,0)), id(0), x(BGIPX_FILL), y(BGIPY_FILL)
	{}

	void reset () {
		type = BGT_NONE;
		color = RGB(0,0,0);
		id = 0;
		x = BGIPX_FILL;
		y = BGIPY_FILL;
	}
};


enum POSITION_X {
	PX_LEFT,
	PX_RIGHT,
	PX_COUNT
};

enum POSITION_Y {
	PY_TOP,
	PY_BOTTOM,
	PY_COUNT
};

enum FONTWEIGHT {
	FONTW_NORMAL,
	FONTW_BOLD,
	FONTW_COUNT
};


class CWinStyle {
protected:
	void _ParsePosition (tstring value);
	void _ParseEdge (tstring value, EDGE &edge);
	COLORREF _ParseColor (tstring value);
	void _ParseBorder (tstring key, tstring value);
	BACKGROUNDIMAGEPOS_X _ParseBackgroundImagePosX (tstring value);
	BACKGROUNDIMAGEPOS_Y _ParseBackgroundImagePosY (tstring value);
	void _ParseBackground (tstring key, tstring value);
	void _Reset ();
	void _SetStyle (tstring style, bool &relayout, bool &redraw);

	/**
	 * the derived class can have its own _ParseProperty()
	 */
	virtual void _ParseProperty (const tstring &key, const tstring &value, bool &relayout, bool &redraw);

public:
	bool display;
	bool disable;
	bool isfloat;

	EDGE margin;
	EDGE padding;
	BORDER border;
	BACKGROUND background;

	POSITION_X px;
	POSITION_Y py;

	int width;
	int height;

	int fontsize;
	FONTWEIGHT fontweight;
	COLORREF color;
	int opacity; // alpha = 100 - opacity

	CWinStyle ();
	virtual ~CWinStyle ();
};




UI_END
XL_END


#ifdef RESTORE_MIN_MAX
#pragma pop_macro ("max")
#pragma pop_macro ("min")
#undef RESTORE_MIN_MAX
#endif

#endif
