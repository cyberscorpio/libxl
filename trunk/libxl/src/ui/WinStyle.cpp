#include "../../include/ui/WinStyle.h"

namespace xl {
	namespace ui {

WinStyle::WinStyle () {
	for (int i = 0; i < 4; ++ i) {
		margin[i] = padding[i] = 0;
	}
}

WinStyle::~WinStyle () {
}


	} // namespace ui
} // namespace xl
