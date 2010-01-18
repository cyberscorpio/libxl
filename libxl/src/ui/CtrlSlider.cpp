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
	int x = (rc.Width() - thumbWidth) * (m_curr - m_min) / (m_max - m_min);
	rc.left += x;
	rc.right = rc.left + thumbWidth;
	return rc;
}

int CCtrlSlider::_ValueByPoint (CPoint pt) const {
	CRect rc = getClientRect();
	int v = m_min + (pt.x - rc.left) * (m_max - m_min) / rc.Width();
	return v;
}


CCtrlSlider::CCtrlSlider (int _min, int _max, int _curr) 
	: m_min(_min), m_max(_max), m_curr(_curr)
	, m_hover(false), m_push(false)
	, m_thumbWidth(10)
{
	assert (m_curr >= m_min && m_curr <= m_max);
}

CCtrlSlider::~CCtrlSlider() {

}


void CCtrlSlider::drawMe (HDC hdc) {
	int insY = 5;
	CRect rc = getClientRect();
	CRect rcBar = rc;
	rcBar.top += insY;
	rcBar.bottom -= insY;
	CDCHandle dc(hdc);
	dc.FillSolidRect(rcBar, RGB(127,127,127));

	rc = _GetThumbRect(m_thumbWidth);
	if (m_hover) {
		dc.FillSolidRect(rc, RGB(96, 96, 96));
	} else {
		dc.FillSolidRect(rc, RGB(0, 0, 0));
	}

	_DrawBorder(hdc);
}

void CCtrlSlider::onMouseIn (CPoint pt) {

}

void CCtrlSlider::onMouseOut (CPoint pt) {

}

void CCtrlSlider::onMouseMove (CPoint pt) {
	CRect rc = _GetThumbRect(m_thumbWidth);
	bool inThumb = rc.PtInRect(pt) ? true : false;
	if (inThumb != m_hover) {
		m_hover = inThumb;
		CCtrlMain *pCtrlMain = _GetMainCtrl();
		pCtrlMain->invalidateControl(shared_from_this());
	}
}

void CCtrlSlider::onLostCapture() {

}

void CCtrlSlider::onLButtonDown (CPoint pt) {
	CRect rc = _GetThumbRect(m_thumbWidth);
	if (!rc.PtInRect(pt)) {
		return;
	} else {
		m_push = true;
	}
}

void CCtrlSlider::onLButtonUp (CPoint pt) {

}



	} // ui
} // xl
