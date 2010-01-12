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
protected:
	tstring style;

	void _ParseProperty(const tstring &key, const tstring &value);

public:
	EDGE margin;
	EDGE padding;

	POSITION_X px;
	POSITION_Y py;

	bool isfloat;

	int width;
	int height;

	int opacity; // alpha = 100 - opacity

	CWinStyle ();
	virtual ~CWinStyle ();

	void reset ();
	void setStyle (const tstring &style = _T(""));
};




	} // ui
} // xl

#endif
