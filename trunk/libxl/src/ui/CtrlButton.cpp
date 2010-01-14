#include <assert.h>
#include "../../include/ui/gdi.h"
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

void CCtrlButton::setText (const tstring &text) {
	m_text = text;
	CCtrlMain *pCtrlMain = _GetMainCtrl();
	if (pCtrlMain) {
		pCtrlMain->invalidateControl(shared_from_this());
	}
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
	setStyle(_T("border-color:#808080;opacity:100;"));
	m_hover = true;
	_GetMainCtrl()->invalidateControl(shared_from_this());
}

void CCtrlButton::onMouseOut (CPoint pt) {
	setStyle(_T("border-color:#cccccc;opacity:50;"));
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
	_DrawBorder(hdc);
	CRect rc = m_rect;
	UINT format = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
	dc.drawTransparentTextWithDefaultFont(m_text, m_text.length(), rc, format);
}

void CCtrlButton::drawHover (HDC hdc) {
	CDCHandle dc(hdc);
	_DrawBorder(hdc);
 	CRect rc = m_rect;
	UINT format = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
	dc.drawTransparentTextWithDefaultFont(m_text, m_text.length(), rc, format);
}

void CCtrlButton::drawPush (HDC hdc) {
	CDCHandle dc(hdc);
	_DrawBorder(hdc);
	CRect rc = m_rect;
	rc.left += 2;
	rc.top += 2;
	UINT format = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
	dc.drawTransparentTextWithDefaultFont(m_text, m_text.length(), rc, format);
}



	}
}