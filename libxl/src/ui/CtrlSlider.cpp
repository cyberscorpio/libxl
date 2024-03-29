#include <assert.h>
#include "../../include/ui/Gdi.h"
#include "../../include/ui/CtrlTarget.h"
#include "../../include/ui/CtrlMain.h"
#include "../../include/ui/CtrlSlider.h"
#include "../../include/ui/ResMgr.h"

/**
 * setStyle:
 * slider: int int int (min max curr)
 * thumbnail-min-width: int
 */

XL_BEGIN
UI_BEGIN

static const int THUMB_WIDTH = 16;

int CCtrlSlider::_GetThumbWidth () const {
	CRect rc = getClientRect();
	int range = m_max - m_min;
	if (rc.Width() <= 0 || range <= 0) {
		return 0;
	} else {
		int w = (int)((double)rc.Width() / (double)range + 0.5);
		if (w < m_thumbMinWidth) {
			w = m_thumbMinWidth;
		}
		return w;
	}
}

CRect CCtrlSlider::_GetThumbRect () const {
	CRect rc = getClientRect();
	int range = m_max - m_min;
	if (rc.Width() <= 0 || range <= 0) {
		return CRect(rc.left, rc.top, rc.left, rc.top);
	} else {
		int w = (int)((double)rc.Width() / (double)range + 0.5);
		if (w < m_thumbMinWidth) {
			w = m_thumbMinWidth;
		}

		rc.left += m_curr * (rc.Width() - w) / range;
		rc.right = rc.left + w;
		return rc;
	}
}

int CCtrlSlider::_ValueByPoint (CPoint pt) const {
	CRect rc = getClientRect();
	int v = m_min + (pt.x - m_mouseOffset - rc.left) * (m_max - m_min) / (rc.Width() - _GetThumbWidth());
	if (v < m_min) {
		v = m_min;
	} else if (v > m_max) {
		v = m_max;
	}
	return v;
}

void CCtrlSlider::_ParseProperty (const tstring &key, const tstring &value, bool &relayout, bool &redraw) {
	if (key == _T("slider")) {
		ExplodeT<TCHAR>::ValueT values = explode(_T(" "), value);
		assert(values.size() == 3);
		m_min = _tstoi(values[0]);
		m_max = _tstoi(values[1]);
		m_curr = _tstoi(values[2]);
		assert(m_curr >= m_min && m_curr <= m_max);
		if (m_min == m_max) {
			setStyle(_T("disable:true"));
		}
		redraw = true;
	} else if (key == _T("thumbnail-min-width")) {
		int minWidth = _tstoi(value);
		if (minWidth > 0 && minWidth != m_thumbMinWidth) {
			m_thumbMinWidth = minWidth;
			redraw = true;
		}
	} else {
		CControl::_ParseProperty(key, value, relayout, redraw);
	}
}


CCtrlSlider::CCtrlSlider (int _min, int _max, int _curr) 
	: m_min(_min), m_max(_max), m_curr(_curr)
	, m_hoverThumb(false), m_pushAndCapture(false)
	, m_barHeight(4)
	, m_thumbMinWidth(THUMB_WIDTH)
	, m_mouseOffset(0)
{
	assert(m_curr >= m_min && m_curr <= m_max);
	if (m_min == m_max) {
		// setStyle(_T("disable:true"));
	}
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

	rc = _GetThumbRect();
	if (m_hoverThumb) {
		dc.FillSolidRect(rc, RGB(0, 0, 0));
	} else {
		dc.FillSolidRect(rc, RGB(96, 96, 96));
	}
}

void CCtrlSlider::onSize () {

}

void CCtrlSlider::onMouseMove (CPoint pt, uint /*key*/) {
	if (disable) {
		return;
	}
	CRect rc = _GetThumbRect();
	if (m_pushAndCapture) {
		int v = _ValueByPoint(pt);
		if (v != m_curr) {
			m_curr = v;
			invalidate();
			assert(_GetTarget() != NULL);
			_GetTarget()->onSlider(m_id, m_min, m_max, m_curr, true, shared_from_this());
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

void CCtrlSlider::onLButtonDown (CPoint pt, uint /*key*/) {
	if (disable) {
		return;
	}
	CRect rc = _GetThumbRect();
	if (!rc.PtInRect(pt)) {
		int v = _ValueByPoint(pt);
		if (v != m_curr) {
			m_curr = v;
			assert(_GetTarget() != NULL);
			_GetTarget()->onSlider(m_id, m_min, m_max, m_curr, false, shared_from_this());

			// check for hover
			rc = _GetThumbRect();
			if (rc.PtInRect(pt)) {
				m_hoverThumb = true;
			}
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

void CCtrlSlider::onLButtonUp (CPoint /*pt*/, uint /*key*/) {
	if (disable) {
		return;
	}
	if (m_pushAndCapture) {
		m_pushAndCapture = false;
		m_mouseOffset = 0;
		_Capture(false);
		assert(_GetTarget() != NULL);
		_GetTarget()->onSlider(m_id, m_min, m_max, m_curr, false, shared_from_this());
		invalidate();
	}
}


UI_END
XL_END