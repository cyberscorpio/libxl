#include <assert.h>
#include "../../include/ui/Gdi.h"
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
		invalidate();
	}
}

void CCtrlButton::setText (const tstring &text) {
	if (m_text != text) {
		m_text = text;
		invalidate();
	}
}

void CCtrlButton::setTextImagePadding (int padding) {
	if (padding != m_text_image_pading) {
		m_text_image_pading = padding;
		invalidate();
	}
}

void CCtrlButton::drawMe (HDC hdc) {
#if 0
	CCtrlMain *pCtrlMain = _GetMainCtrl();
	assert (pCtrlMain);
	bool hover = pCtrlMain->getHoverCtrl().get() == (CControl *)this;
	if (hover && m_push && !disable) {
		drawPush(hdc);
	} else if (hover && !disable) {
		drawHover(hdc);
	} else {
		drawNormal(hdc);
	}
#endif
	_DrawImageAndText(hdc);
}

void CCtrlButton::onMouseIn (CPoint pt) {
	invalidate();
}

void CCtrlButton::onMouseOut (CPoint pt) {
	invalidate();
}

void CCtrlButton::onLostCapture () {
	m_push = false;
	invalidate();
}

void CCtrlButton::onLButtonDown (CPoint pt) {
	if (disable) {
		return;
	}
	_SetCapture(true);
	m_push = true;
	invalidate();
}

void CCtrlButton::onLButtonUp (CPoint pt) {
	if (disable) {
		if (_GetMainCtrl()->getCaptureCtrl() == shared_from_this()) {
			_SetCapture(false);
		}
		return;
	}
	_SetCapture(false);
	m_push = false;
	invalidate();

	assert (m_target != NULL);
	if (isPointIn(pt)) {
		m_target->onCommand(m_id, shared_from_this());
	}
}

#if 0
void CCtrlButton::drawNormal (HDC hdc) {
	CDCHandle dc(hdc);
	_DrawImageAndText(hdc);
}

void CCtrlButton::drawHover (HDC hdc) {
	CDCHandle dc(hdc);
	_DrawImageAndText(hdc);
}

void CCtrlButton::drawPush (HDC hdc) {
	CDCHandle dc(hdc);
	_DrawImageAndText(hdc);
}
#endif

//////////////////////////////////////////////////////////////////////////

CCtrlImageButton::CCtrlImageButton (uint id, uint n, uint h, uint p, const tstring &imgType)
	: CCtrlButton(id)
	, m_idImageNormal(n), m_idImageHover(h), m_idImagePush(p)
	, m_imgType(imgType)
{
	TCHAR buf[256];
	_stprintf_s(buf, 256, _T("background-image-id: %d %s"), m_idImageNormal, m_imgType.c_str());
	setStyle(buf);
}

#if 0
void CCtrlImageButton::drawNormal (HDC hdc) {
	if (m_idImageNormal == 0) {
		CCtrlButton::drawNormal(hdc);
	} else {
		CResMgr *pResMgr = CResMgr::getInstance();
		CResMgr::GpBmpPtr bitmap = pResMgr->getBitmap(m_idImageNormal, m_imgType, disable);
		Gdiplus::Graphics g(hdc);
		if (m_rect.Height() == bitmap->GetHeight() && m_rect.Width() == bitmap->GetWidth()) {
			g.DrawImage(bitmap.get(), m_rect.left, m_rect.top);
		} else {
			Gdiplus::RectF dst((float)m_rect.left, (float)m_rect.top, (float)m_rect.Width(), (float)m_rect.Height());
			g.DrawImage(bitmap.get(), dst, 0, 0, (float)bitmap->GetWidth(), (float)bitmap->GetHeight(), Gdiplus::UnitPixel);
		}
	}
	_DrawImageAndText(hdc);
}

void CCtrlImageButton::drawHover (HDC hdc) {
	if (m_idImageHover == 0) {
		CCtrlButton::drawHover(hdc);
	} else {
		CResMgr *pResMgr = CResMgr::getInstance();
		CResMgr::GpBmpPtr bitmap = pResMgr->getBitmap(m_idImageHover, m_imgType, disable);
		Gdiplus::Graphics g(hdc);
		if (m_rect.Height() == bitmap->GetHeight() && m_rect.Width() == bitmap->GetWidth()) {
			g.DrawImage(bitmap.get(), m_rect.left, m_rect.top);
		} else {
			Gdiplus::RectF dst((float)m_rect.left, (float)m_rect.top, (float)m_rect.Width(), (float)m_rect.Height());
			g.DrawImage(bitmap.get(), dst, 0, 0, (float)bitmap->GetWidth(), (float)bitmap->GetHeight(), Gdiplus::UnitPixel);
		}
	}
	_DrawImageAndText(hdc);
}

void CCtrlImageButton::drawPush (HDC hdc) {
	if (m_idImagePush == 0) {
		CCtrlButton::drawPush(hdc);
	} else {
		CResMgr *pResMgr = CResMgr::getInstance();
		CResMgr::GpBmpPtr bitmap = pResMgr->getBitmap(m_idImagePush, m_imgType, disable);
		Gdiplus::Graphics g(hdc);
		if (m_rect.Height() == bitmap->GetHeight() && m_rect.Width() == bitmap->GetWidth()) {
			g.DrawImage(bitmap.get(), m_rect.left, m_rect.top);
		} else {
			Gdiplus::RectF dst((float)m_rect.left, (float)m_rect.top, (float)m_rect.Width(), (float)m_rect.Height());
			g.DrawImage(bitmap.get(), dst, 0, 0, (float)bitmap->GetWidth(), (float)bitmap->GetHeight(), Gdiplus::UnitPixel);
		}
	}
	_DrawImageAndText(hdc);
}
#endif

void CCtrlImageButton::onMouseIn (CPoint pt) {
	if (disable) {
		return;
	}
	CCtrlButton::onMouseIn(pt);
	TCHAR buf[256];
	_stprintf_s(buf, 256, _T("background-image-id: %d %s"), m_idImageHover, m_imgType.c_str());
	setStyle(buf);
}

void CCtrlImageButton::onMouseOut (CPoint pt) {
	if (disable) {
		return;
	}
	CCtrlButton::onMouseOut(pt);
	TCHAR buf[256];
	_stprintf_s(buf, 256, _T("background-image-id: %d %s"), m_idImageNormal, m_imgType.c_str());
	setStyle(buf);
}

void CCtrlImageButton::onLostCapture () {
	CCtrlButton::onLostCapture();
	TCHAR buf[256];
	_stprintf_s(buf, 256, _T("background-image-id: %d %s"), m_idImageNormal, m_imgType.c_str());
	setStyle(buf);
}

void CCtrlImageButton::onLButtonDown (CPoint pt) {
	if (disable) {
		return;
	}
	CCtrlButton::onLButtonDown(pt);
	TCHAR buf[256];
	_stprintf_s(buf, 256, _T("background-image-id: %d %s"), m_idImagePush, m_imgType.c_str());
	setStyle(buf);
}

void CCtrlImageButton::onLButtonUp (CPoint pt) {
	if (disable) {
		if (_GetMainCtrl()->getCaptureCtrl() == shared_from_this()) {
			_SetCapture(false);
		}
		return;
	}
	CCtrlButton::onLButtonUp(pt);
	TCHAR buf[256];
	_stprintf_s(buf, 256, _T("background-image-id: %d %s"), 
		isPointIn(pt) ? m_idImageHover : m_idImageNormal, m_imgType.c_str());
	setStyle(buf);
}




	}
}