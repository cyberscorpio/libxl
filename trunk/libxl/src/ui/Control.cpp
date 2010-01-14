#include <assert.h>
#include "../../include/common.h"
#include "../../include/ui/Control.h"
#include "../../include/ui/CtrlMain.h"
#include "../../include/ui/gdi.h"

#ifndef NDEBUG
static int s_control_counts = 0;
#endif

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

CControlPtr CControl::_GetControlByPoint (CPoint pt) {
	for (CControlIterR itr = m_controls.rbegin(); itr != m_controls.rend(); ++ itr) {
		if ((*itr)->m_rect.PtInRect(pt)) {
			CControlPtr ctrl = (*itr)->_GetControlByPoint(pt);
			if (ctrl != NULL) {
				return ctrl;
			} else {
				return *itr;
			}
		}
	}

	return CControlPtr();
}

void CControl::_SetParent (CControlPtr parent) {
	if (m_parent.lock() != NULL) {
		onDetach();
	}
	m_parent = parent;
	onAttach();
}

void CControl::_SetTarget (CCtrlTargetRawPtr target) {
	m_target = target;
	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
		(*it)->_SetTarget(target);
	}
}

void CControl::_DrawBorder (HDC hdc) {
	if (borderWidth == 0) {
		return;
	}

	CDCHandle dc(hdc);
	dc.drawRectangle(m_rect, borderWidth, borderColor);
}

bool CControl::_SetCapture (bool capture) {
	CCtrlMain *pCtrlMain = _GetMainCtrl();
	CControlPtr pThis = shared_from_this();
	assert (pCtrlMain);
	if (capture) {
		if (pCtrlMain->_GetCaptureCtrl() != pThis) {
			return pCtrlMain->_SetCapture(pThis);
		} else {
			return true;
		}
	} else {
		if (pCtrlMain->_GetCaptureCtrl() == pThis) {
			return pCtrlMain->_SetCapture(CControlPtr());
		} else {
			return true;
		}
	}
}

CCtrlMain* CControl::_GetMainCtrl () {
	CControlPtr parent = m_parent.lock();
	if (parent != NULL) {
		return parent->_GetMainCtrl();
	}

	return NULL;
}


CControl::CControl (uint id)
	: m_id (id)
	, m_target (NULL)
{
	if (m_id == 0) {
		m_id = (uint)this; // this should be unique ?
	}

#ifndef NDEBUG
	++ s_control_counts;
#endif
}

CControl::~CControl () {
#ifndef NDEBUG
	-- s_control_counts;
	ATLTRACE (_T("%d control(s) remains\n"), s_control_counts);
#endif
}


bool CControl::insertChild (CControlPtr child) {
	child->_SetParent(shared_from_this());
	child->_SetTarget(m_target);

	m_controls.push_back(child);
	_LayoutChildren();
	return true;
}

CControlPtr CControl::getControlByID (uint id) {
	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
		if ((*it)->getID() == id) {
			return *it;
		}

		CControlPtr ctrl = (*it)->getControlByID(id);
		if (ctrl != NULL) {
			return ctrl;
		}
	}
	return CControlPtr();
}

void CControl::draw (HDC hdc, CRect rcClip) {

	// check paint condition
	if (m_rect.Width() <= 0 || m_rect.Height() <= 0) {
		return;
	}

	if (opacity == 0) {
		return;
	}

	CRect rc = m_rect;
	CRect rcTest = rc;
	if (!rcTest.IntersectRect(rcTest, rcClip)) {
		return;
	}

	HDC hdcPaint = hdc;

	CControlPtr parent = m_parent.lock();
	std::auto_ptr<CMemoryDC> mdc;
	if (parent == NULL || opacity != 100 || transparent) {
		mdc.reset(new CMemoryDC(hdc, rc));
		if (opacity != 100) {
			mdc->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), hdc, m_rect.left, m_rect.top, SRCCOPY);
		}
		hdcPaint = mdc->m_hDC;
	}


	drawMe(hdcPaint);
	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
		(*it)->draw(hdcPaint, rcClip);
	}

	if (opacity != 100) {
		mdc->m_paintWhenDestroy = false;
		CDCHandle dc(hdc);
		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255 * opacity / 100, 0};
		dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(), hdcPaint, rc.left, rc.top, rc.Width(), rc.Height(), bf);
	}
}

CRect CControl::layout (CRect rc) const {
	CControlPtr parent = m_parent.lock();
	int x, y, width, height;
	CRect rcRemain = rc;

	if (isfloat) {
		assert (parent != NULL);
		rc = parent->m_rect;
	}

	// width
	if (this->width == SIZE_FILL) {
		width = rc.Width() - margin.left - margin.right;
	} else {
		width = this->width + padding.left + padding.right;
		if (width > rc.Width()) {
			width = rc.Width();
		}
	}

	// height
	if (this->height == SIZE_FILL) {
		height = rc.Height() - margin.top - margin.bottom;
	} else {
		height = this->height + padding.top + padding.bottom;
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

	if (isfloat) {
		rcRemain = rc;
	}

	return rcRemain;
}

void CControl::drawMe (HDC hdc) {
	CDCHandle dc(hdc);
	dc.FillSolidRect(m_rect, ::GetSysColor(COLOR_WINDOW));
}



	} // ui
} // xl