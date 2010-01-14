#include <assert.h>

#include "../../include/common.h"
#include "../../include/ui/WinStyle.h"
#include "../../include/string.h"
/**
 * styles
 * px: left | right
 * py: top | bottom
 * float: true | false | none
 * transparent: true | false | none
 * width: int | "fill"
 * height: int | "fill"
 * opacity: [0-100]
 * margin: 0 0 0 0
 * padding: 0 0 0 0
 * border-width: int
 * border-color: #000000
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
	borderWidth = 0;
	borderColor = RGB(0, 0, 0);
	px = PX_LEFT;
	py = PY_TOP;
	isfloat = false;
	transparent = false;
	width = SIZE_FILL;
	height = SIZE_FILL;
	opacity = 100;
}

void CWinStyle::setStyle (const tstring &style) {
	ExplodeT<TCHAR>::ValueT styles = explode(_T(";"), style);

	for (size_t i = 0; i < styles.size(); ++ i) {
		tstring property = styles[i];
		ExplodeT<TCHAR>::ValueT kv = explode(_T(":"), property);
		assert (kv.size() == 2);
		kv[0].trim();
		kv[1].trim();
		_ParseProperty(kv[0], kv[1]);
	}
}

void CWinStyle::_ParseEdge (tstring value, EDGE &edge) {
	while (value.replace(_T("  "), _T(" ")) > 0)
		;
	value.trim(_T(" "));
	ExplodeT<TCHAR>::ValueT edges = explode(_T(" "), value);
	assert (edges.size() > 0 && edges.size() <= ET_COUNT);
	if (edges.size() == 1) {
		int v = _tstoi(edges[0]);
		edge.top = edge.right = edge.bottom = edge.left = v;
	} else if (edges.size() == 2) {
		edge.top = edge.bottom = _tstoi(edges[ET_TOP]);
		edge.right = edge.left = _tstoi(edges[ET_RIGHT]);
	} else if (edges.size() == 3) {
		edge.top = _tstoi(edges[ET_TOP]);
		edge.right = edge.left = _tstoi(edges[ET_RIGHT]);
		edge.bottom = _tstoi(edges[ET_BOTTOM]);
	} else if (edges.size() == 4) {
		edge.top = _tstoi(edges[ET_TOP]);
		edge.right = _tstoi(edges[ET_RIGHT]);
		edge.bottom = _tstoi(edges[ET_BOTTOM]);
		edge.left = _tstoi(edges[ET_LEFT]);
	} else {
		assert(false);
	}
}

COLORREF CWinStyle::_ParseColor (tstring value) {
	COLORREF color = RGB(0,0,0);
	value.trim();
	assert(value.length() == 7 && value.at(0) == _T('#')); // #ffffff
	if (value.length() == 7 && value.at(0) == _T('#')) {
		tchar tmp[5] = {_T('0'), _T('x'), 0, 0, 0};
		int c[3];
		const tchar *p = value.c_str();
		++ p;
		for (int i = 0; i < 3; ++ i) {
			tmp[2] = *p ++;
			tmp[3] = *p ++;
			tmp[4] = 0;
			_stscanf(tmp, _T("%i"), &c[i]);
		}
		color = RGB(c[0], c[1], c[2]);
	}
	return color;
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
		if (value == _T("true")) {
			isfloat = true;
		} else if (value == _T("false") || value == _T("none")) {
			isfloat = false;
		} else {
			assert(false);
		}
	} else if (key == _T("transparent")) {
		if (value == _T("true")) {
			transparent = true;
		} else if (value == _T("false") || value == _T("none")) {
			transparent = false;
		} else {
			assert(false);
		}
	} else if (key == _T("margin")) {
		_ParseEdge(value, margin);
	} else if (key == _T("padding")) {
		_ParseEdge(value, padding);
	} else if (key == _T("border-width")) {
		borderWidth = _tstoi(value);
		assert(borderWidth >= 0);
	} else if (key == _T("border-color")) {
		borderColor = _ParseColor(value);
	} else {
		assert(false);
	}
}


	} // ui
} // xl