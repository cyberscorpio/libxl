#ifndef XL_UI_WINSTYLE_H
#define XL_UI_WINSTYLE_H
#include "../common.h"
#include "../string.h"

namespace xl {
	namespace ui {


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
};


const int SIZE_FILL = -1;

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



class CWinStyle {
	tstring style;
public:
	EDGE margin;
	EDGE padding;

	POSITION_X px;
	POSITION_Y py;

	int width;
	int height;

	CWinStyle () {
		margin.left = margin.top = margin.right = margin.bottom = 0;
		padding.left = padding.top = padding.right = padding.bottom = 0;
		px = PX_LEFT;
		py = PY_TOP;
		width = SIZE_FILL;
		height = SIZE_FILL;
	}

	void setStyle (const tstring &style = _T(""));
};




	} // ui
} // xl

#endif
