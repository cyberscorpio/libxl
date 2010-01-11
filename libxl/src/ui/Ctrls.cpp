#include <assert.h>
#include "../../include/common.h"
#include "../../include/ui/Ctrls.h"
#include "../../include/ui/CtrlMgr.h"
#include "../../include/ui/gdi.h"

namespace xl {
	namespace ui {

void CControl::_LayoutChildren () const {
	CRect rc = m_rect;
	rc.left += padding.left;
	rc.top += padding.top;
	rc.right -= padding.right;
	rc.bottom -= padding.bottom;

	for (CControlConstIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
		rc = (*it)->layout(rc);
	}
}


CControl::CControl (uint id, CCtrlMgr *mgr) : m_id(id), m_mgr(mgr) {
	if (m_id == 0) {
		m_id = (uint)this; // this should be unique ?
	}

	if (m_mgr) {
		m_mgr->insertControl(CControlPtr(this));
	}
}

CControl::~CControl () {

}


bool CControl::insertChild (CControlPtr ctrl) {
	// TODO: Set the parent, and target
	assert (m_mgr != NULL);
	if (!m_mgr->insertControl(ctrl)) {
		return false;
	}

	m_controls.push_back(ctrl);
	_LayoutChildren();
	return true;
}

void CControl::draw (HDC hdc) {

	if (m_rect.Width() <= 0 || m_rect.Height() <= 0) {
		return;
	}

	CRect rc = m_rect;
	CMemoryDC mdc(hdc, rc);

	drawMe(mdc.m_hDC);
	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
		(*it)->draw(mdc.m_hDC);
	}

	if (opacity != 100) {
		mdc.m_paintWhenDestroy = false;
		CDCHandle dc(hdc);
		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255 * opacity / 100, 0};
		dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(), mdc, rc.left, rc.top, rc.Width(), rc.Height(), bf);
	}
}

CRect CControl::layout (CRect rc) const {
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