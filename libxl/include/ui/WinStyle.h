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



class CWinStyle {
public:
};




	} // ui
} // xl

#endif
