#include <assert.h>
#include "../../include/ui/Gdi.h"
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
	, m_hoverThumb(false), m_pushAndCapture(false)
	, m_barHeight(4)
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
	// dc.FillSolidRect(rcBar, RGB(127,127,127));
	dc.Draw3dRect(rcBar, RGB(127,127,127), RGB(212,212,212));

	rc = _GetThumbRect(m_thumbWidth);
	if (m_hoverThumb) {
		dc.FillSolidRect(rc, RGB(0, 0, 0));
	} else {
		dc.FillSolidRect(rc, RGB(96, 96, 96));
	}
}

void CCtrlSlider::onMouseIn (CPoint pt) {

}

void CCtrlSlider::onMouseOut (CPoint pt) {

}

void CCtrlSlider::onMouseMove (CPoint pt, uint key) {
	CRect rc = _GetThumbRect(m_thumbWidth);
	if (m_pushAndCapture) {
		int v = _ValueByPoint(pt, m_thumbWidth);
		if (v != m_curr) {
			m_curr = v;
			invalidate();
			m_target->onSlider(m_id, m_min, m_max, m_curr, true, shared_from_this());
		}
	} else {
		bool inThumb = rc.PtInRect(pt) ? true : false;
		if (inThumb != m_hoverThumb) {
			m_hoverThumb = inThumb;
			invalidate();
		}
	}
}

void CCtrlSlider::onLostCapture() {
	if (m_hoverThumb || m_pushAndCapture) {
		invalidate();
	}
	m_hoverThumb = false;
	m_pushAndCapture = false;
	m_mouseOffset = 0;
}

void CCtrlSlider::onLButtonDown (CPoint pt, uint key) {
	CRect rc = _GetThumbRect(m_thumbWidth);
	if (!rc.PtInRect(pt)) {
		int v = _ValueByPoint(pt, m_thumbWidth);
		if (v != m_curr) {
			m_curr = v;
			m_target->onSlider(m_id, m_min, m_max, m_curr, false, shared_from_this());
			invalidate();
		}
	} else {
		assert(!m_pushAndCapture);
		m_pushAndCapture = true;
		m_mouseOffset = pt.x - rc.left;
		_Capture(true);
		invalidate();
	}
}

void CCtrlSlider::onLButtonUp (CPoint pt, uint key) {
	if (m_pushAndCapture) {
		m_pushAndCapture = false;
		m_mouseOffset = 0;
		_Capture(false);
		m_target->onSlider(m_id, m_min, m_max, m_curr, false, shared_from_this());
		invalidate();
	}
}



	} // ui
} // xl
