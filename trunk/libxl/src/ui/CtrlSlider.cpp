#include <assert.h>
#include "../../include/ui/gdi.h"
#include "../../include/ui/CtrlTarget.h"
#include "../../include/ui/CtrlMain.h"
#include "../../include/ui/CtrlSlider.h"
#include "../../include/ui/ResMgr.h"

namespace xl {
	namespace ui {

CRect CCtrlSlider::_GetThumbRect (int thumbWidth) const {
	CRect rc = getClientRect();
	rc.left += m_curr * (rc.Width() - thumbWidth) / (m_max - m_min);
	rc.right = rc.left + thumbWidth;
	return rc;
}

int CCtrlSlider::_ValueByPoint (CPoint pt, int thumbWidth) const {
	CRect rc = getClientRect();
	int v = m_min + (pt.x - m_mouseOffset - rc.left) * (m_max - m_min) / (rc.Width() - thumbWidth);
	if (v < m_min) {
		v = m_min;
	} else if (v > m_max) {
		v = m_max;
	}
	return v;
}


CCtrlSlider::CCtrlSlider (int _min, int _max, int _curr) 
	: m_min(_min), m_max(_max), m_curr(_curr)
	, m_hover(false), m_push(false)
	, m_barHeight(2)
	, m_thumbWidth(16)
	, m_mouseOffset(0)
{
	assert (m_curr >= m_min && m_curr <= m_max);
}

CCtrlSlider::~CCtrlSlider() {

}


void CCtrlSlider::drawMe (HDC hdc) {
	CRect rc = getClientRect();
	CRect rcBar = rc;
	rcBar.top += (rcBar.Height() - m_barHeight) / 2;
	rcBar.bottom = rcBar.top + m_barHeight;
	CDCHandle dc(hdc);
	dc.FillSolidRect(rcBar, RGB(127,127,127));

	rc = _GetThumbRect(m_thumbWidth);
	if (m_hover) {
		dc.FillSolidRect(rc, RGB(0, 0, 0));
	} else {
		dc.FillSolidRect(rc, RGB(96, 96, 96));
	}

	_DrawBorder(hdc);
}

void CCtrlSlider::onMouseIn (CPoint pt) {

}

void CCtrlSlider::onMouseOut (CPoint pt) {

}

void CCtrlSlider::onMouseMove (CPoint pt) {
	CRect rc = _GetThumbRect(m_thumbWidth);
	if (m_push) {
		int v = _ValueByPoint(pt, m_thumbWidth);
		if (v != m_curr) {
			m_curr = v;
			invalidate();
			m_target->onSlider(m_id, m_min, m_max, m_curr, true, shared_from_this());
		}
	} else {
		bool inThumb = rc.PtInRect(pt) ? true : false;
		if (inThumb != m_hover) {
			m_hover = inThumb;
			invalidate();
		}
	}
}

void CCtrlSlider::onLostCapture() {
	if (m_hover || m_push) {
		invalidate();
	}
	m_hover = false;
	m_push = false;
	m_mouseOffset = 0;
}

void CCtrlSlider::onLButtonDown (CPoint pt) {
	CRect rc = _GetThumbRect(m_thumbWidth);
	if (!rc.PtInRect(pt)) {
		int v = _ValueByPoint(pt, m_thumbWidth);
		if (v != m_curr) {
			m_curr = v;
			m_target->onSlider(m_id, m_min, m_max, m_curr, false, shared_from_this());
			invalidate();
		}
		return;
	} else {
		assert(!m_push);
		m_push = true;
		m_mouseOffset = pt.x - rc.left;
		_SetCapture(true);
		invalidate();
	}
}

void CCtrlSlider::onLButtonUp (CPoint pt) {
	if (m_push) {
		m_push = false;
		m_mouseOffset = 0;
		_SetCapture(false);
		m_target->onSlider(m_id, m_min, m_max, m_curr, false, shared_from_this());
		invalidate();
	}
}



	} // ui
} // xl
