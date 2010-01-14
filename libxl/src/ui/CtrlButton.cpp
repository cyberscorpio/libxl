#include <assert.h>
#include "../../include/ui/CtrlTarget.h"
#include "../../include/ui/CtrlMain.h"
#include "../../include/ui/CtrlButton.h"

namespace xl {
	namespace ui {

CCtrlButton::CCtrlButton (uint id)
	: CControl (id)
	, m_hover (false)
	, m_push (false)
{
	assert (id != 0);
}

CCtrlButton::~CCtrlButton () {

}

void CCtrlButton::drawMe (HDC hdc) {
	if (m_hover && m_push) {
		drawPush(hdc);
	} else if (m_hover) {
		drawHover(hdc);
	} else {
		drawNormal(hdc);
	}
}

void CCtrlButton::onMouseIn (CPoint pt) {
	m_hover = true;
	_GetMainCtrl()->invalidateControl(shared_from_this());
}

void CCtrlButton::onMouseOut (CPoint pt) {
	m_hover = false;
	_GetMainCtrl()->invalidateControl(shared_from_this());
}

void CCtrlButton::onLostCapture () {
	m_hover = false;
	m_push = false;
	_GetMainCtrl()->invalidateControl(shared_from_this());
}

void CCtrlButton::onLButtonDown (CPoint pt) {
	_SetCapture(true);
	m_push = true;
	_GetMainCtrl()->invalidateControl(shared_from_this());
}

void CCtrlButton::onLButtonUp (CPoint pt) {
	_SetCapture(false);
	m_push = false;
	_GetMainCtrl()->invalidateControl(shared_from_this());

	assert (m_target != NULL);
	if (m_rect.PtInRect(pt)) {
		m_target->onCommand(m_id, shared_from_this());
	} else {
		m_hover = false;
	}
}

void CCtrlButton::drawNormal (HDC hdc) {
	CDCHandle dc(hdc);
	// dc.Draw3dRect(m_rect, RGB(192,192,192), RGB(96,96,96));
	dc.DrawEdge(m_rect, BDR_SUNKENINNER, BF_FLAT | BF_RECT);
}

void CCtrlButton::drawHover (HDC hdc) {
	CDCHandle dc(hdc);
	dc.Draw3dRect(m_rect, RGB(192,192,192), RGB(96,96,96));
// 	CRect rc = m_rect;
// 	rc.DeflateRect(1,1,1,1);
// 	dc.FillSolidRect(rc, RGB(220,220,220));
}

void CCtrlButton::drawPush (HDC hdc) {
	CDCHandle dc(hdc);
	dc.Draw3dRect(m_rect, RGB(96,96,96), RGB(192,192,192));
}



	}
}