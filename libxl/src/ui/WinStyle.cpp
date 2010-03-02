#include <assert.h>

#include "../../include/common.h"
#include "../../include/ui/WinStyle.h"
#include "../../include/string.h"
/**
 * styles
 * margin: int[ int[ int[ int]]]
 * padding: int[ int[ int[ int]]]
 * border: int[ #hex*6[ style]]
 * background: none
 * background-color: #hex*6
 * background-image-id: int type[ fill | left | right | center | repeat[ fill | top | bottom | center | repeat]]
 * background-image-url: url[ fill | left | right | center | repeat[ fill | top | bottom | center | repeat]]
 * px: left | right
 * py: top | bottom
 * position: left | right top | bottom
 * float: true | false | none
 * width: int | "fill"
 * height: int | "fill"
 * font-weight: normal | bold
 * font-size: int
 * color: #hex*6
 * opacity: [0-100]
 * display: true | false | none
 * disable: true | false
 */
/**
 * The rules of layout the controls:
 * - Layout is based on "line"
 * - All float elements MUST be the last child of its parent, otherwise it will be covered by it siblings
 * - All controls which width is SIZE_FILL must be the last one of the "line".
 * - All controls which height is SIZE_FILL must after the controls which py = BOTTOM
 */

#pragma warning (disable : 4996)

XL_BEGIN
UI_BEGIN

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
	_Reset();
}

CWinStyle::~CWinStyle () {

}

void CWinStyle::_Reset () {
	margin.left = margin.top = margin.right = margin.bottom = 0;
	padding.left = padding.top = padding.right = padding.bottom = 0;
	border.reset();
	background.reset();
	px = PX_LEFT;
	py = PY_TOP;
	display = true;
	disable = false;
	isfloat = false;
	width = SIZE_FILL;
	height = SIZE_FILL;
	fontsize = 0;
	fontweight = FONTW_NORMAL;
	color = RGB(0,0,0);
	opacity = 100;
}

void CWinStyle::_SetStyle (tstring style, bool &relayout, bool &redraw) {
	relayout = redraw = false;
	style.trim();
	ExplodeT<TCHAR>::ValueT styles = explode(_T(";"), style);

	for (size_t i = 0; i < styles.size(); ++ i) {
		tstring property = styles[i];
		ExplodeT<TCHAR>::ValueT kv = explode(_T(":"), property);
		assert (kv.size() == 2);
		kv[0].trim();
		while (kv[1].replace(_T("  "), _T(" ")) > 0)
			;
		kv[1].trim();
		_ParseProperty(kv[0], kv[1], relayout, redraw);
	}
}

void CWinStyle::_ParsePosition (tstring value) {
	value.trim();
	ExplodeT<TCHAR>::ValueT values = explode(_T(" "), value);
	assert(values.size() == 2);
	values[0].trim();
	values[1].trim();

	px = values[0] == _T("left") ? PX_LEFT : (values[0] == _T("right") ? PX_RIGHT : PX_COUNT);
	py = values[1] == _T("top") ? PY_TOP : (values[1] == _T("bottom") ? PY_BOTTOM : PY_COUNT);
}

void CWinStyle::_ParseEdge (tstring value, EDGE &edge) {
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

BACKGROUNDIMAGEPOS_X CWinStyle::_ParseBackgroundImagePosX (tstring value) {
	if (value == _T("fill")) {
		return BGIPX_FILL;
	} else if (value == _T("left")) {
		return BGIPX_LEFT;
	} else if (value == _T("right")) {
		return BGIPX_RIGHT;
	} else if (value == _T("center")) {
		return BGIPX_CENTER;
	} else if (value == _T("repeat")) {
		return BGIPX_REPEAT;
	}
	assert(false);
	return BGIPX_FILL;
}

BACKGROUNDIMAGEPOS_Y CWinStyle::_ParseBackgroundImagePosY (tstring value) {
	value.trim();
	if (value == _T("fill")) {
		return BGIPY_FILL;
	} else if (value == _T("top")) {
		return BGIPY_TOP;
	} else if (value == _T("bottom")) {
		return BGIPY_BOTTOM;
	} else if (value == _T("center")) {
		return BGIPY_CENTER;
	} else if (value == _T("repeat")) {
		return BGIPY_REPEAT;
	}
	assert(false);
	return BGIPY_FILL;
}

void CWinStyle::_ParseBackground (tstring key, tstring value) {
	if (key == _T("background")) {
		assert(value == _T("none"));
		background.type = BGT_NONE;
	} else if (key == _T("background-color")) {
		background.type = BGT_RGB;
		background.color = _ParseColor(value);
	} else if (key == _T("background-image-id")) {
		assert(false); // not supported now
		background.type = BGT_IMAGE_ID;
		background.x = BGIPX_FILL;
		background.y = BGIPY_FILL;
		ExplodeT<TCHAR>::ValueT values = explode(_T(" "), value);
		assert(values.size() > 1);
		background.id = _tstoi(values[0]);
		background.url_or_idtype = values[1];
		if (values.size() > 2) {
			background.x = _ParseBackgroundImagePosX(values[2]);
		}
		if (values.size() > 3) {
			background.y = _ParseBackgroundImagePosY(values[3]);
		}

	} else if (key == _T("background-image-url")) {
		assert(false); // not supported now
		background.type = BGT_IMAGE_URL;
		background.x = BGIPX_FILL;
		background.y = BGIPY_FILL;
		ExplodeT<TCHAR>::ValueT values = explode(_T(" "), value);
		assert(values.size() > 0);
		background.url_or_idtype = values[0];
		if (values.size() > 1) {
			background.x = _ParseBackgroundImagePosX(values[1]);
		}
		if (values.size() > 2) {
			background.y = _ParseBackgroundImagePosY(values[2]);
		}

	} else {
		assert(false);
	}
}

void CWinStyle::_ParseProperty (const tstring &key, const tstring &value, bool &relayout, bool &redraw) {
	if (key == _T("width")) {
		int w = value == _T("fill") ? SIZE_FILL : _tstoi(value);
		if (w != width) {
			width = w;
			relayout = true;
			redraw = true;
		}
	} else if (key == _T("height")) {
		int h = value == _T("fill") ? SIZE_FILL : _tstoi(value);
		if (h != height) {
			height = h;
			relayout = true;
			redraw = true;
		}
	} else if (key == _T("position")) {
		POSITION_X _px = px;
		POSITION_Y _py = py;
		_ParsePosition(value);
		if (_px != px || _py != py) {
			relayout = true;
			redraw = true;
		}
	} else if (key == _T("px")) {
		POSITION_X _px = value == _T("left") ? PX_LEFT : (value == _T("right") ? PX_RIGHT : PX_COUNT);
		assert (_px != PX_COUNT);
		if (_px != px) {
			px = _px;
			relayout = true;
			redraw = true;
		}
	} else if (key == _T("py")) {
		POSITION_Y _py = value == _T("top") ? PY_TOP : (value == _T("bottom")) ? PY_BOTTOM : PY_COUNT;
		assert (_py != PY_COUNT);
		if (_py != py) {
			py = _py;
			relayout = true;
			redraw = true;
		}
	} else if (key == _T("float")) {
		bool _isfloat = isfloat;
		if (value == _T("true")) {
			_isfloat = true;
		} else if (value == _T("false") || value == _T("none")) {
			_isfloat = false;
		} else {
			assert(false);
		}
		if (_isfloat != isfloat) {
			isfloat = _isfloat;
			relayout = true;
			redraw = true;
		}
	} else if (key == _T("margin")) {
		_ParseEdge(value, margin);
		relayout = true;
		redraw = true;
	} else if (key == _T("padding")) {
		_ParseEdge(value, padding);
		relayout = true;
		redraw = true;
	} else if (key.find(_T("border")) == 0) {
		_ParseBorder(key, value);
		relayout = true;
		redraw = true;
	} else if (key.find(_T("background")) == 0) {
		_ParseBackground(key, value);
		redraw = true;
	} else if (key == _T("opacity")) {
		opacity = _tstoi(value);
		assert(opacity <= 100 && opacity >= 0);
		redraw = true;
	} else if (key == _T("font-weight")) {
		if (value == _T("normal")) {
			fontweight = FONTW_NORMAL;
		} else if (value == _T("bold")) {
			fontweight = FONTW_BOLD;
		} else {
			assert(false);
		}
		redraw = true;
	} else if (key == _T("font-size")) {
		int fs = _tstoi(value);
		if (fs != fontsize) {
			fontsize = fs;
			redraw = true;
		}
	} else if (key == _T("color")) {
		color = _ParseColor(value);
		redraw = true;
	} else if (key == _T("display")) {
		if (value == _T("true")) {
			display = true;
		} else if (value == _T("false") || value == _T("none")) {
			display = false;
		} else {
			assert(false);
		}
		redraw = true;
	} else if (key == _T("disable")) {
		if (value == _T("true")) {
			disable = true;
		} else if (value == _T("false")) {
			disable = false;
		} else {
			assert(false);
		}
		redraw = true;
	} else {
		assert(false);
	}
}


UI_END
XL_END
