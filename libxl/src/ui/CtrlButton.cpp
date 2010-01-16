#include <assert.h>
#include "../../include/ui/gdi.h"
#include "../../include/ui/CtrlTarget.h"
#include "../../include/ui/CtrlMain.h"
#include "../../include/ui/CtrlButton.h"
#include "../../include/ui/ResMgr.h"

namespace xl {
	namespace ui {

void CCtrlButton::_DrawText (HDC hdc) {
	if (m_text.length() == 0) {
		return;
	}

	CDCHandle dc(hdc);
	CRect rc = getClientRect();
	if (m_push) {
		rc.OffsetRect(1, 1);
	}
	UINT format = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	COLORREF rgbOld = dc.SetTextColor(color);
	HFONT font = _GetFont();
	HFONT oldFont = dc.SelectFont(font);
	dc.drawTransparentText(m_text, m_text.length(), rc, format);
	dc.SelectFont(oldFont);
	dc.SetTextColor(rgbOld);
}

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
	_DrawBorder(hdc);
	_DrawText(hdc);
}

void CCtrlButton::drawHover (HDC hdc) {
	CDCHandle dc(hdc);
	_DrawBorder(hdc);
	_DrawText(hdc);
}

void CCtrlButton::drawPush (HDC hdc) {
	CDCHandle dc(hdc);
	_DrawBorder(hdc);
	_DrawText(hdc);
}


//////////////////////////////////////////////////////////////////////////

CCtrlImageButton::CCtrlImageButton (uint id, uint n, uint h, uint p)
	: CCtrlButton(id)
	, m_idImageNormal(n), m_idImageHover(h), m_idImagePush(p)
{
}

void CCtrlImageButton::drawNormal (HDC hdc) {
	if (m_idImageNormal == 0) {
		CCtrlButton::drawNormal(hdc);
	} else {
		CResMgr *pResMgr = CResMgr::getInstance();
		CResMgr::GpBmpPtr bitmap = pResMgr->getBitmap(m_idImageNormal, _T("PNG"));
		Gdiplus::Graphics g(hdc);
		g.DrawImage(bitmap.get(), m_rect.left, m_rect.top);
	}
	_DrawText(hdc);
}

void CCtrlImageButton::drawHover (HDC hdc) {
	if (m_idImageHover == 0) {
		CCtrlButton::drawNormal(hdc);
	} else {
		CResMgr *pResMgr = CResMgr::getInstance();
		CResMgr::GpBmpPtr bitmap = pResMgr->getBitmap(m_idImageHover, _T("PNG"));
		Gdiplus::Graphics g(hdc);
		g.DrawImage(bitmap.get(), m_rect.left, m_rect.top);
	}
	_DrawText(hdc);
}

void CCtrlImageButton::drawPush (HDC hdc) {
	if (m_idImagePush == 0) {
		CCtrlButton::drawNormal(hdc);
	} else {
		CResMgr *pResMgr = CResMgr::getInstance();
		CResMgr::GpBmpPtr bitmap = pResMgr->getBitmap(m_idImagePush, _T("PNG"));
		Gdiplus::Graphics g(hdc);
		g.DrawImage(bitmap.get(), m_rect.left, m_rect.top);
	}
	_DrawText(hdc);
}




	}
}