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
const int MARGIN_AUTO = std::numeric_limits<int>::max();

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

	void _ParseEdge (tstring value, EDGE &edge);
	COLORREF _ParseColor (tstring value);
	void _ParseProperty (const tstring &key, const tstring &value);

public:
	EDGE margin;
	EDGE padding;
	int borderWidth;
	COLORREF borderColor;

	POSITION_X px;
	POSITION_Y py;

	bool isfloat;
	bool transparent;

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

#ifdef RESTORE_MIN_MAX
#pragma pop_macro ("min")
#pragma pop_macro ("max")
#undef RESTORE_MIN_MAX
#endif

#endif
