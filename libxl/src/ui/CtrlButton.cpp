#include <assert.h>
#include "../../include/ui/gdi.h"
#include "../../include/ui/CtrlTarget.h"
#include "../../include/ui/CtrlMain.h"
#include "../../include/ui/CtrlButton.h"
#include "../../include/ui/ResMgr.h"

namespace xl {
	namespace ui {

void CCtrlButton::_DrawImageAndText (HDC hdc) {
	if (m_text.length() == 0) {
		return;
	}

	CDCHandle dc(hdc);
	CRect rc = getClientRect();
	if (m_push) {
		rc.OffsetRect(1, 1);
	}
	UINT format = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	COLORREF rgbOld = dc.SetTextColor(_GetColor());
	HFONT font = _GetFont();
	HFONT oldFont = dc.SelectFont(font);
	if (m_idImg == 0) {
		dc.drawTransparentText(m_text, m_text.length(), rc, format);
	} else {
		CRect rcTmp = rc;
		int textHeight = dc.drawTransparentText(m_text, m_text.length(), rcTmp, format | DT_CALCRECT);
		int textWidth = rcTmp.Width();
		CResMgr *pResMgr = CResMgr::getInstance();
		CResMgr::GpBmpPtr img = pResMgr->getBitmap(m_idImg, m_imgType, disable);
		int imgWidth = img->GetWidth(), imgHeight = img->GetHeight();
		int imgX = rc.left + (rc.Width() - imgWidth - textWidth - m_text_image_pading) / 2;
		int imgY = rc.top + (rc.Height() - imgHeight) / 2;
		{
			Gdiplus::Graphics g(hdc);
			g.DrawImage(img.get(), imgX, imgY, imgWidth, imgHeight);
		}
		rcTmp = rc;
		rcTmp.left = imgX + imgWidth + m_text_image_pading;
		rcTmp.right = rcTmp.left + textWidth;
		dc.drawTransparentText(m_text, m_text.length(), rcTmp, format);
	}
	dc.SelectFont(oldFont);
	dc.SetTextColor(rgbOld);
}

CCtrlButton::CCtrlButton (uint id, const tstring &text, uint idImg, const tstring &imgType)
	: CControl(id)
	, m_push(false)
	, m_text(text)
	, m_text_image_pading(2)
	, m_idImg(idImg)
	, m_imgType(imgType)
{
	assert (id != 0);
}

CCtrlButton::~CCtrlButton () {

}

void CCtrlButton::setImg (uint idImg, const tstring &imgType) {
	if (m_idImg != idImg) {
		m_idImg = idImg;
		m_imgType = imgType;
		CCtrlMain *pCtrlMain = _GetMainCtrl();
		if (pCtrlMain) {
			pCtrlMain->invalidateControl(shared_from_this());
		}
	}
}

void CCtrlButton::setText (const tstring &text) {
	if (m_text != text) {
		m_text = text;
		CCtrlMain *pCtrlMain = _GetMainCtrl();
		if (pCtrlMain) {
			pCtrlMain->invalidateControl(shared_from_this());
		}
	}
}

void CCtrlButton::setTextImagePadding (int padding) {
	if (padding != m_text_image_pading) {
		m_text_image_pading = padding;
		CCtrlMain *pCtrlMain = _GetMainCtrl();
		if (pCtrlMain) {
			pCtrlMain->invalidateControl(shared_from_this());
		}
	}
}

void CCtrlButton::drawMe (HDC hdc) {
	// ATLTRACE(_T("id=%d hover:%d push:%d\n"), m_id, m_hover ? 1 : 0, m_push ? 1 : 0);
	CCtrlMain *pCtrlMain = _GetMainCtrl();
	assert (pCtrlMain);
	bool hover = pCtrlMain->getHoverCtrl().get() == (CControl *)this;
	if (hover && m_push) {
		drawPush(hdc);
	} else if (hover) {
		drawHover(hdc);
	} else {
		drawNormal(hdc);
	}
}

void CCtrlButton::onMouseIn (CPoint pt) {
	_GetMainCtrl()->invalidateControl(shared_from_this());
}

void CCtrlButton::onMouseOut (CPoint pt) {
	_GetMainCtrl()->invalidateControl(shared_from_this());
}

void CCtrlButton::onLostCapture () {
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
	if (isPointIn(pt)) {
		m_target->onCommand(m_id, shared_from_this());
	}
}

void CCtrlButton::drawNormal (HDC hdc) {
	CDCHandle dc(hdc);
	_DrawBorder(hdc);
	_DrawImageAndText(hdc);
}

void CCtrlButton::drawHover (HDC hdc) {
	CDCHandle dc(hdc);
	_DrawBorder(hdc);
	_DrawImageAndText(hdc);
}

void CCtrlButton::drawPush (HDC hdc) {
	CDCHandle dc(hdc);
	_DrawBorder(hdc);
	_DrawImageAndText(hdc);
}


//////////////////////////////////////////////////////////////////////////

CCtrlImageButton::CCtrlImageButton (uint id, uint n, uint h, uint p, const tstring &imgType)
	: CCtrlButton(id)
	, m_idImageNormal(n), m_idImageHover(h), m_idImagePush(p)
	, m_imgType(imgType)
{
}

void CCtrlImageButton::drawNormal (HDC hdc) {
	if (m_idImageNormal == 0) {
		CCtrlButton::drawNormal(hdc);
	} else {
		CResMgr *pResMgr = CResMgr::getInstance();
		CResMgr::GpBmpPtr bitmap = pResMgr->getBitmap(m_idImageNormal, m_imgType, disable);
		Gdiplus::Graphics g(hdc);
		g.DrawImage(bitmap.get(), m_rect.left, m_rect.top);
	}
	_DrawImageAndText(hdc);
}

void CCtrlImageButton::drawHover (HDC hdc) {
	if (m_idImageHover == 0) {
		CCtrlButton::drawNormal(hdc);
	} else {
		CResMgr *pResMgr = CResMgr::getInstance();
		CResMgr::GpBmpPtr bitmap = pResMgr->getBitmap(m_idImageHover, m_imgType, disable);
		Gdiplus::Graphics g(hdc);
		g.DrawImage(bitmap.get(), m_rect.left, m_rect.top);
	}
	_DrawImageAndText(hdc);
}

void CCtrlImageButton::drawPush (HDC hdc) {
	if (m_idImagePush == 0) {
		CCtrlButton::drawNormal(hdc);
	} else {
		CResMgr *pResMgr = CResMgr::getInstance();
		CResMgr::GpBmpPtr bitmap = pResMgr->getBitmap(m_idImagePush, m_imgType, disable);
		Gdiplus::Graphics g(hdc);
		g.DrawImage(bitmap.get(), m_rect.left, m_rect.top);
	}
	_DrawImageAndText(hdc);
}




	}
}