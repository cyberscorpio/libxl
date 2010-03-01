#include <assert.h>
#include "../../include/common.h"
#include "../../include/ui/Gdi.h"
#include "../../include/ui/Control.h"
#include "../../include/ui/CtrlMain.h"

#ifndef NDEBUG
static int s_control_counts = 0;
#endif

XL_BEGIN
UI_BEGIN

void CControl::_LayoutChildren () {
	CRect rc = getClientRect();

	for (CControlConstIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
		rc = (*it)->layout(rc);
	}
}

void CControl::_SetParent (CControlPtr parent) {
	if (_GetMainCtrl() != NULL) {
		onDetach();
	}

	m_parent = parent;
	
	if (_GetMainCtrl() != NULL) {
		onAttach();
	}
}

void CControl::_SetTarget (CCtrlTargetRawPtr target) {
	m_target = target;
	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
		(*it)->_SetTarget(target);
	}
}

COLORREF CControl::_GetColor () {
	return disable ? ::GetSysColor(COLOR_GRAYTEXT) : color;
}

HFONT CControl::_GetFont () {
	CResMgr *pResMgr = CResMgr::getInstance();
	uint style = 0;
	if (fontweight == FONTW_BOLD) {
		style |= CResMgr::FS_BOLD;
	}
	return pResMgr->getSysFont(0, style);
}

void CControl::_DrawBorder (HDC hdc) {
	CDCHandle dc(hdc);
	COLORREF gray = RGB(127,127,127);
	
	if (border.top.width > 0) {
		dc.FillSolidRect(m_rect.left, m_rect.top, m_rect.Width(), border.top.width, 
			disable ? gray : border.top.color);
	}
	
	if (border.right.width > 0) {
		dc.FillSolidRect(m_rect.right - border.right.width, m_rect.top, border.right.width, m_rect.Height(),
			disable ? gray : border.right.color);
	}

	if (border.bottom.width > 0) {
		dc.FillSolidRect(m_rect.left, m_rect.bottom - border.bottom.width, m_rect.Width(), border.bottom.width, 
			disable ? gray : border.bottom.color);
	}

	if (border.left.width > 0) {
		dc.FillSolidRect(m_rect.left, m_rect.top, border.left.width, m_rect.Height(), 
			disable ? gray : border.left.color);
	}
}

void CControl::_DrawBackground (HDC hdc) {
	if (background.type == BGT_NONE) {
		return;
	}

	CRect rc = m_rect;
	rc.DeflateRect(border.left.width, border.top.width, border.right.width, border.bottom.width);

	if (background.type == BGT_RGB) {
		CDCHandle dc(hdc);
		dc.FillSolidRect(rc, background.color);
		return;
	}

	assert(false); // to be implement later
}

bool CControl::_Capture (bool capture) {
	CCtrlMain *pCtrlMain = _GetMainCtrl();
	CControlPtr pThis = shared_from_this();
	assert (pCtrlMain);
	CControlPtr ctrlCapture = pCtrlMain->getCaptureCtrl();
	if (capture) {
		if (ctrlCapture != pThis) {
			if (ctrlCapture != NULL) {
				ctrlCapture->onLostCapture();
			}
			return pCtrlMain->_SetCaptureCtrl(pThis);
		} else {
			return true;
		}
	} else {
		if (ctrlCapture == pThis) {
			return pCtrlMain->_SetCaptureCtrl(CControlPtr());
		} else {
			return true;
		}
	}
}

uint CControl::_SetTimer (uint elapse, uint id) {
	CCtrlMain *pCtrlMain = _GetMainCtrl();
	assert(pCtrlMain);
	if (!pCtrlMain) {
		return 0;
	}

	return pCtrlMain->_SetTimer(shared_from_this(), elapse, id);
}

CCtrlMain* CControl::_GetMainCtrl () {
	CControlPtr parent = m_parent.lock();
	if (parent != NULL) {
		return parent->_GetMainCtrl();
	}

	return NULL;
}


CControl::CControl (uint id)
	: m_id(id)
	, m_target(NULL)
	, m_rect(0, 0, 0, 0)
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
	// ATLTRACE (_T("%d control(s) remains\n"), s_control_counts);
#endif
}


CRect CControl::getClientRect () const {
	CRect rc = m_rect;
	rc.top += border.top.width + padding.top;
	rc.right -= border.right.width + padding.right;
	rc.bottom -= border.bottom.width + padding.bottom;
	rc.left += border.left.width + padding.left;
	return rc;
}


bool CControl::insertChild (CControlPtr child) {
	child->_SetParent(shared_from_this());
	child->_SetTarget(m_target);

	m_controls.push_back(child);
	CCtrlMain *pCtrlMain = _GetMainCtrl();
	if (pCtrlMain) {
		pCtrlMain->reLayout();
	}
	return true;
}

CControlPtr CControl::removeChild (uint id) {
	CControlPtr ctrl = getControlByID(id);
	if (ctrl != NULL) {
		CCtrlMain *pCtrlMain = _GetMainCtrl();
		if (pCtrlMain) {
			pCtrlMain->_BeforeRemoveCtrl(ctrl);
		}

		CControlPtr parent = ctrl->m_parent.lock();
		assert(parent != NULL);
		bool found = false;
		for (CControlIter it = parent->m_controls.begin(); it != parent->m_controls.end(); ++ it) {
			if ((*it)->getID() == id) {
				found = true;
				parent->m_controls.erase(it);
				break;
			}
		}
		assert(found);
		ctrl->_SetParent(CControlPtr());

		if (pCtrlMain) {
			pCtrlMain->reLayout();
		}
	}

	return ctrl;
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

CControlPtr CControl::getControlByPoint (CPoint pt) {
	if (display && isPointIn(pt)) {
		for (CControlIterR itr = m_controls.rbegin(); itr != m_controls.rend(); ++ itr) {
			if ((*itr)->isPointIn(pt)) {
				CControlPtr ctrl = (*itr)->getControlByPoint(pt);
				if (ctrl != NULL) {
					return ctrl;
				}
			}
		}

		return shared_from_this();
	}

	return CControlPtr();
}

void CControl::invalidate () {
	CCtrlMain *pCtrlMain = _GetMainCtrl();
	if (pCtrlMain) {
		pCtrlMain->invalidateControl(shared_from_this());
	}
}


void CControl::resetStyle () {
	CWinStyle::_Reset();
	CCtrlMain *pCtrlMain = _GetMainCtrl();
	if (pCtrlMain) {
		pCtrlMain->reLayout();
	}
}

void CControl::setStyle (const tstring &style) {
	bool relayout = false, redraw = false;
	CWinStyle::_SetStyle(style, relayout, redraw);
	CCtrlMain *pCtrlMain = _GetMainCtrl();
	if (pCtrlMain) {
		if (relayout) {
			pCtrlMain->reLayout();
		} else if (redraw) {
			pCtrlMain->invalidateControl(shared_from_this());
		}
	}
}


void CControl::draw (HDC hdc, CRect rcClip) {

	// check paint condition
	if (m_rect.Width() <= 0 || m_rect.Height() <= 0) {
		return;
	}

	if (opacity == 0 || !display) {
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
	if (parent == NULL || opacity != 100/* || transparent*/) {
		mdc.reset(new CMemoryDC(hdc, rc));
		if (parent != NULL) {
			mdc->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), hdc, m_rect.left, m_rect.top, SRCCOPY);
		}
		hdcPaint = mdc->m_hDC;
	}

	_DrawBorder(hdcPaint);
	_DrawBackground(hdcPaint);
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

CRect CControl::layout (CRect rc) {
	CControlPtr parent = m_parent.lock();
	int x, y, width, height;
	CRect rcRemain = rc;
	CRect rcOld = m_rect;

	if (isfloat) {
		assert (parent != NULL);
		rc = parent->m_rect;
	}

	if (margin.left == EDGE_AUTO && margin.right == EDGE_AUTO) {
		assert (this->width != SIZE_FILL);
	}
	if (margin.top == EDGE_AUTO && margin.bottom == EDGE_AUTO) {
		assert (this->height != SIZE_FILL);
	}

	// width
	if (this->width == SIZE_FILL) {
		width = rc.Width() - margin.width();
	} else {
		width = this->width + padding.width() + border.width();
		if (width > rc.Width()) {
			width = rc.Width();
		}
	}

	// height
	if (this->height == SIZE_FILL) {
		height = rc.Height() - margin.height();
	} else {
		height = this->height + padding.height() + border.height();
		if (height > rc.Height()) {
			height = rc.Height();
		}
	}

	// x
	if (margin.left == EDGE_AUTO || margin.right == EDGE_AUTO) {
		x = (rc.Width() - width) / 2;
		rcRemain.left = rcRemain.right; // no space remains
	} else {
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
	}

	// y
	if (margin.top == EDGE_AUTO || margin.bottom == EDGE_AUTO) {
		y = (rc.Height() - height) / 2;
		rcRemain.top = rcRemain.bottom; // nospace remains
	} else {
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

	if (rcOld != m_rect) {
		onSize();
	}

	return rcRemain;
}

bool CControl::isPointIn (CPoint pt) const {
	return m_rect.PtInRect(pt) ? true : false;
}

void CControl::drawMe (HDC hdc) {
}



UI_END
XL_END