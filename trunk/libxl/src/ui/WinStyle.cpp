#include <assert.h>

#include "../../include/common.h"
#include "../../include/ui/WinStyle.h"
#include "../../include/string.h"
/**
 * styles
 * px: left | right
 * py: top | bottom
 * float: float | none
 * width: int | "fill"
 * height: int | "fill"
 * opacity: [0-100]
 */

namespace xl {
	namespace ui {

CWinStyle::CWinStyle () {
	reset();
}

CWinStyle::~CWinStyle () {

}

void CWinStyle::reset () {
	margin.left = margin.top = margin.right = margin.bottom = 0;
	padding.left = padding.top = padding.right = padding.bottom = 0;
	px = PX_LEFT;
	py = PY_TOP;
	isfloat = false;
	width = SIZE_FILL;
	height = SIZE_FILL;
	opacity = 100;
}

void CWinStyle::setStyle (const tstring &style) {
	typedef std::vector<tstring>  Parts;
	Parts styles = explode(_T(";"), style);

	for (size_t i = 0; i < styles.size(); ++ i) {
		tstring property = styles[i];
		Parts kv = explode(_T(":"), property);
		assert (kv.size() == 2);
		kv[0].trim();
		kv[1].trim();
		_ParseProperty(kv[0], kv[1]);
	}
}

void CWinStyle::_ParseProperty (const tstring &key, const tstring &value) {
	if (key == _T("width")) {
		width = value == _T("fill") ? SIZE_FILL : _tstoi(value);
	} else if (key == _T("height")) {
		height = value == _T("fill") ? SIZE_FILL : _tstoi(value);
	} else if (key == _T("px")) {
		px = value == _T("left") ? PX_LEFT : (value == _T("right") ? PX_RIGHT : PX_COUNT);
		assert (px != PX_COUNT);
	} else if (key == _T("py")) {
		py = value == _T("top") ? PY_TOP : (value == _T("bottom")) ? PY_BOTTOM : PY_COUNT;
		assert (py != PY_COUNT);
	} else if (key == _T("opacity")) {
		opacity = _tstoi(value);
		assert (opacity <= 100 && opacity >= 0);
	} else if (key == _T("float")) {
		if (value == _T("float")) {
			isfloat = true;
		} else if (value == _T("none")) {
			isfloat = false;
		} else {
			assert (false);
		}
	} else {
		assert (false);
	}
}


	} // ui
} // xl