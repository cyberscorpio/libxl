#include <assert.h>
#include "../../include/common.h"
#include "../../include/ui/Ctrls.h"
#include "../../include/ui/gdi.h"

namespace xl {
	namespace ui {

void CControl::_LayoutChildren () {
	CRect rc = m_rect;
	rc.left += padding.left;
	rc.top += padding.top;
	rc.right -= padding.right;
	rc.bottom -= padding.bottom;

	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
		rc = (*it)->layout(rc);
	}
}


CControl::CControl () {

}

CControl::~CControl () {

}


void CControl::insertChild (CControlPtr ctrl) {
	m_controls.push_back(ctrl);
	_LayoutChildren();
}

void CControl::draw (HDC hdc) {
	drawMe(hdc);

	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
		(*it)->draw(hdc);
	}
}

CRect CControl::layout (CRect rc) {
	int x, y, width, height;
	CRect rcRemain = rc;

	// width
	if (this->width == SIZE_FILL) {
		width = rc.Width() - margin.left - margin.right;
	} else {
		width = this->width + margin.left + margin.right;
		if (width > rc.Width()) {
			width = rc.Width();
		}
	}

	// height
	if (this->height == SIZE_FILL) {
		height = rc.Height() - margin.top - margin.bottom;
	} else {
		height = this->height + margin.top + margin.bottom;
		if (height > rc.Height()) {
			height = rc.Height();
		}
	}

	// x
	switch (px) {
	case PX_LEFT:
		x = rc.left + margin.left;
		rcRemain.left = x + width + margin.right;
		break;
	case PX_RIGHT:
		x = rc.right - margin.right - width;
		rcRemain.right = x - margin.left;
		break;
	default:
		assert (false);
	}

	// y
	switch (py) {
	case PY_TOP:
		y = rc.top + margin.top;
		if (this->width == SIZE_FILL) {
			rcRemain.top = y + height + margin.bottom;
		}
		break;
	case PY_BOTTOM:
		y = rc.bottom - margin.bottom - height;
		if (this->width == SIZE_FILL) {
			rcRemain.bottom = y - margin.top;
		}
		break;
	default:
		assert (false);
	}

	m_rect.left = x;
	m_rect.top = y;
	m_rect.right = x + width;
	m_rect.bottom = y + height;

	_LayoutChildren();

	// adjust rcRemain
	if (this->width == SIZE_FILL) {
		rcRemain.left = rc.left;
		rcRemain.right = rc.right;
// 		switch (py) {
// 		case PY_TOP:
// 			rcRemain.top = rc.top + margin.top + height + margin.bottom;
// 			rcRemain.bottom = rc.bottom;
// 			break;
// 		case PY_BOTTOM:
// 			rcRemain.top = rc.top;
// 			rcRemain.bottom = rc.bottom - margin.top - height - margin.bottom;
// 			break;
// 		default:
// 			assert (false);
// 			break;
// 		}
	}

	return rcRemain;
}

void CControl::drawMe (HDC hdc) {
	CDCHandle dc(hdc);
	dc.FillSolidRect(m_rect, ::GetSysColor(COLOR_WINDOW));
}



	} // ui
} // xl