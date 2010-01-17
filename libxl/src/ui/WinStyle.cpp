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
 * margin: int[ int[ int[ int]]]
 * padding: int[ int[ int[ int]]]
 * border: int[ #hex*6[ style]]
 * font-weight: normal | bold
 * color: #hex*6
 * opacity: [0-100]
 * disable: true | false
 */
/**
 * The rules of layout the controls:
 * - Layout is based on "line"
 * - All float elements MUST be the last child of its parent, otherwise it will be covered by it siblings
 * - All controls which width is SIZE_FILL must be the last one of the "line".
 * - All controls which height is SIZE_FILL must after the controls which py = BOTTOM
 */

namespace xl {
	namespace ui {


void BORDER::reset () {
	top.reset();
	right.reset();
	bottom.reset();
	left.reset();
}

void BORDER::setWidth (int width, EDGETYPE et) {
	if (et == ET_ALL) {
		top.width = right.width = bottom.width = left.width = width;
	} else if (et == ET_TOP) {
		top.width = width;
	} else if (et == ET_RIGHT) {
		right.width = width;
	} else if (et == ET_BOTTOM) {
		bottom.width = width;
	} else if (et == ET_LEFT) {
		left.width = width;
	} else {
		assert(false);
	}
}

void BORDER::setColor (COLORREF color, EDGETYPE et) {
	if (et == ET_ALL) {
		top.color = right.color = bottom.color = left.color = color;
	} else if (et == ET_TOP) {
		top.color = color;
	} else if (et == ET_RIGHT) {
		right.color = color;
	} else if (et == ET_BOTTOM) {
		bottom.color = color;
	} else if (et == ET_LEFT) {
		left.color = color;
	} else {
		assert(false);
	}
}



//////////////////////////////////////////////////////////////////////////
// WinStyle

CWinStyle::CWinStyle () {
	reset();
}

CWinStyle::~CWinStyle () {

}

void CWinStyle::reset () {
	margin.left = margin.top = margin.right = margin.bottom = 0;
	padding.left = padding.top = padding.right = padding.bottom = 0;
	border.reset();
	px = PX_LEFT;
	py = PY_TOP;
	disabled = false;
	isfloat = false;
	transparent = false;
	width = SIZE_FILL;
	height = SIZE_FILL;
	fontweight = FONTW_NORMAL;
	color = RGB(0,0,0);
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
	tchar tmp[32];
	_stprintf(tmp, _T("%d"), EDGE_AUTO);
	value.replace(_T("auto"), tmp);
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

void CWinStyle::_ParseBorder (tstring key, tstring value) {
	key.trim();
	while (value.replace(_T("  "), _T(" ")) > 0)
		;
	value.trim(_T(" "));
	ExplodeT<TCHAR>::ValueT values = explode(_T(" "), value);
	if (key == _T("border") || key == _T("border-top") || key == _T("border-right")
	    || key == _T("border-bottom") || key == _T("border-left")) // border[-top|-right|-bottom|-left]: int[ color[ style]]
	{
		EDGETYPE et = key == _T("border") ? ET_ALL : 
			(key == _T("border-top") ? ET_TOP : 
			(key == _T("border-right") ? ET_RIGHT :
			(key == _T("border-bottom") ? ET_BOTTOM : ET_LEFT)));
		assert(values.size() > 0);
		values[0].trim();
		border.setWidth(_tstoi(values[0]), et);

		if (values.size() > 1) {
			values[1].trim();
			border.setColor(_ParseColor(values[1]), et);
		}
	} else {
		assert(false);
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
	} else if (key.find(_T("border")) == 0) {
		_ParseBorder(key, value);
	} else if (key == _T("opacity")) {
		opacity = _tstoi(value);
		assert(opacity <= 100 && opacity >= 0);
	} else if (key == _T("font-weight")) {
		if (value == _T("normal")) {
			fontweight = FONTW_NORMAL;
		} else if (value == _T("bold")) {
			fontweight = FONTW_BOLD;
		} else {
			assert(false);
		}
	} else if (key == _T("color")) {
		color = _ParseColor(value);
	} else if (key == _T("disable")) {
		if (value == _T("true")) {
			disabled = true;
		} else if (value == _T("false")) {
			disabled = false;
		} else {
			assert(false);
		}
	} else {
		assert(false);
	}
}


	} // ui
} // xl