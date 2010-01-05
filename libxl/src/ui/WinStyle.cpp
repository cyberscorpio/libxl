#include "../../include/ui/WinStyle.h"

namespace xl {
	namespace ui {

WinStyle::WinStyle () 
	: width (0)
	, height (0)
	, position (WinStyle::PT_NORMAL)
	, display (WinStyle::DISP_BLOCK)
	, xfloat (WinStyle::FLOAT_NONE)
{
	for (int i = 0; i < 4; ++ i) {
		margin[i] = padding[i] = 0;
	}
}

WinStyle::~WinStyle () {
}

void WinStyle::setMargin (int margin, EDGETYPE et) {
	if (et == ET_ALL) {
		this->margin.left = this->margin.right = 
			this->margin.top = this->margin.bottom = margin;
	} else {
		this->margin[et] = margin;
	}
}

void WinStyle::setPadding (int padding, EDGETYPE et) {
	if (et == ET_ALL) {
		this->padding.left = this->padding.right =
			this->padding.top = this->padding.bottom = padding;
	} else {
		this->padding[et] = padding;
	}
}

SIZE WinStyle::reportSize () {
	SIZE sz;
	sz.cx = width == SIZE_AUTO ? SIZE_AUTO : width + margin.left + margin.right;
	sz.cy = height == SIZE_AUTO ? SIZE_AUTO : height + margin.top + margin.bottom;
	return sz;
}




	} // namespace ui
} // namespace xl
