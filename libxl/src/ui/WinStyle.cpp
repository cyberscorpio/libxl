#include "../../include/ui/WinStyle.h"

namespace xl {
	namespace ui {


void WinStyle::_StyleChanged () {

}


WinStyle::WinStyle () 
	: width (0)
	, height (0)
	, position (WinStyle::PT_NORMAL)
	, display (WinStyle::DISP_BLOCK)
{
}

WinStyle::~WinStyle () {
}

void WinStyle::setMargin (int margin, EDGETYPE et) {
	if (et == ET_ALL) {
		this->margin.left = this->margin.right = 
			this->margin.top = this->margin.bottom = margin;
	} else {
		et == ET_LEFT ? this->margin.left = margin : 
			(et == ET_RIGHT ? this->margin.right = margin :
				(et == ET_TOP ? this->margin.top = margin : 
					this->margin.bottom = margin));
	}
}

void WinStyle::setPadding (int padding, EDGETYPE et) {
	if (et == ET_ALL) {
		this->padding.left = this->padding.right =
			this->padding.top = this->padding.bottom = padding;
	} else {
		et == ET_LEFT ? this->padding.left = padding : 
			(et == ET_RIGHT ? this->padding.right = padding :
				(et == ET_TOP ? this->padding.top = padding : 
					this->padding.bottom = padding));
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
