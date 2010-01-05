#include "../../include/ui/WinStyle.h"

namespace xl {
	namespace ui {

WinStyle::WinStyle () 
	: width (0)
	, height (0)
{
	for (int i = 0; i < 4; ++ i) {
		margin[i] = padding[i] = 0;
	}
}

WinStyle::~WinStyle () {
}

void WinStyle::setMargin (int margin, EDGE edge) {
	if (edge == EDGE_COUNT) {
		for (int i = 0; i < 4; ++ i) {
			this->margin[i] = margin;
		}
	} else {
		this->margin[edge] = margin;
	}
}

void WinStyle::setPadding (int padding, EDGE edge = EDGE_COUNT) {
	if (edge == EDGE_COUNT) {
		for (int i = 0; i < 4; ++ i) {
			this->padding[i] = padding;
		}
	} else {
		this->padding[edge] = padding;
	}
}

SIZE WinStyle::reportSize () {
	SIZE sz;
	sz.cx = width == SIZE_AUTO ? SIZE_AUTO : width + margin[EDGE_LEFT] + margin[EDGE_RIGHT];
	sz.cy = height == SIZE_AUTO ? SIZE_AUTO : height + margin[EDGE_TOP] + margin[EDGE_BOTTOM];
	return sz;
}




	} // namespace ui
} // namespace xl
