#include <assert.h>
#include "../../include/ui/Gdi.h"
#include "../../include/ui/CtrlTarget.h"
#include "../../include/ui/CtrlMain.h"
#include "../../include/ui/CtrlButton.h"
#include "../../include/ui/ResMgr.h"

/**
 * setStyle:
 *
 * CCtrlButton:
 * button-image: id[ colorkey]
 * button-image-text-padding: int
 *
 * CCtrlImageButton:
 * imagebutton-image: int int int[ colorkey]
 */

XL_BEGIN
UI_BEGIN

void CCtrlButton::_DrawImageAndText (HDC hdc) {
	if (m_text.length() == 0) {
		return;
	}

	CDCHandle dc(hdc);
	CRect rc = getClientRect();
	if (m_pushAndCapture) {
		rc.OffsetRect(1, 1);
	}
	UINT format = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	COLORREF rgbOld = dc.SetTextColor(_GetColor());
	HFONT font = _GetFont();
	HFONT oldFont = dc.SelectFont(font);
	if (m_imgId == 0) {
		dc.drawTransparentText(m_text, m_text.length(), rc, format);
	} else {
		CRect rcTmp = rc;
		int textHeight = dc.drawTransparentText(m_text, m_text.length(), rcTmp, format | DT_CALCRECT);
		int textWidth = rcTmp.Width();

 		CResMgr *pResMgr = CResMgr::getInstance();
		CBitmapPtr image = m_imgTrans ? pResMgr->getTransBitmap(m_imgId, m_imgClrKey, disable) : pResMgr->getBitmap(m_imgId, disable);
		assert(image != NULL);
		int imgWidth = image->getWidth();
		int imgHeight = image->getHeight();
		int imgX = rc.left + (rc.Width() - imgWidth - textWidth - m_text_image_pading) / 2;
		int imgY = rc.top + (rc.Height() - imgHeight) / 2;
		if (imgX < rc.left) {
			imgX = rc.left;
		}
		if (imgY < rc.top) {
			imgY = rc.top;
		}

		image->draw(dc, imgX, imgY, imgWidth, imgHeight, 0, 0, SRCCOPY);

		rcTmp = rc;
		rcTmp.left = imgX + imgWidth + m_text_image_pading;
		rcTmp.right = rcTmp.left + textWidth;
		if (rcTmp.right > rc.right) {
			rcTmp.right = rc.right;
		}
		dc.drawTransparentText(m_text, m_text.length(), rcTmp, format);
	}
	dc.SelectFont(oldFont);
	dc.SetTextColor(rgbOld);
}

void CCtrlButton::_ParseProperty (const tstring &key, const tstring &value, bool &relayout, bool &redraw) {
	if (key == _T("button-image")) {
		ExplodeT<TCHAR>::ValueT values = explode(_T(" "), value);
		assert(values.size() == 1 || values.size() == 2);
		uint id = _tstoi(values[0]);
		if (values.size() == 1) {
			m_imgTrans = false;
		} else {
			m_imgTrans = true;
			m_imgClrKey = _ParseColor(values[1]);
		}
		if (id != m_imgId) {
			m_imgId = id;
			redraw = true;
		}
	} else if (key == _T("button-image-text-padding")) {
		int v = _tstoi(value);
		if (v != m_text_image_pading) {
			m_text_image_pading = v;
			redraw = true;
		}
	} else {
		CControl::_ParseProperty(key, value, relayout, redraw);
	}
}

CCtrlButton::CCtrlButton (uint id, const tstring &text, uint idImg, bool imgTrans, COLORREF clrKey)
	: CControl(id)
	, m_pushAndCapture(false)
	, m_text(text)
	, m_text_image_pading(4)
	, m_imgId(idImg)
	, m_imgTrans(imgTrans)
	, m_imgClrKey(clrKey)
{
	assert (id != 0);
}

CCtrlButton::~CCtrlButton () {

}

void CCtrlButton::setText (const tstring &text) {
	if (m_text != text) {
		m_text = text;
		invalidate();
	}
}

void CCtrlButton::drawMe (HDC hdc) {
	_DrawImageAndText(hdc);
}

void CCtrlButton::onMouseIn (CPoint pt) {
	invalidate();
}

void CCtrlButton::onMouseOut (CPoint pt) {
	invalidate();
}

void CCtrlButton::onLostCapture () {
	m_pushAndCapture = false;
	invalidate();
}

void CCtrlButton::onLButtonDown (CPoint pt, uint key) {
	if (disable) {
		return;
	}
	_Capture(true);
	m_pushAndCapture = true;
	invalidate();
}

void CCtrlButton::onLButtonUp (CPoint pt, uint key) {
	if (disable) {
		if (_GetMainCtrl()->getCaptureCtrl() == shared_from_this()) {
			_Capture(false);
		}
		return;
	}
	_Capture(false);
	m_pushAndCapture = false;
	invalidate();

	assert (m_target != NULL);
	if (isPointIn(pt)) {
		m_target->onCommand(m_id, shared_from_this());
	}
}


//////////////////////////////////////////////////////////////////////////

CBitmapPtr CCtrlImageButton::_GetImage() {
	uint id = m_imageIds[0];
	if (m_pushAndCapture) {
		id = m_imageIds[2];
	} else if (_GetMainCtrl()->getHoverCtrl() == shared_from_this()) {
		id = m_imageIds[1];
	}

	if (disable) {
		id = m_imageIds[0];
	}

	if (id != 0) {
		CResMgr *pResMgr = CResMgr::getInstance();
		if (m_pColorKey != NULL) {
			return pResMgr->getTransBitmap(id, m_colorKey, disable);
		} else {
			return pResMgr->getBitmap(id, disable);
		}
	}
	return CBitmapPtr();
}


void CCtrlImageButton::_ParseProperty (const tstring &key, const tstring &value, bool &relayout, bool &redraw) {
	if (key == _T("imagebutton-image")) { // id id id type
		ExplodeT<TCHAR>::ValueT values = explode(_T(" "), value);
		assert(values.size() == 3 || values.size() == 4);
		m_imageIds[0] = _tstoi(values[0]);
		m_imageIds[1] = _tstoi(values[1]);
		m_imageIds[2] = _tstoi(values[2]);
		if (values.size() == 4) {
			m_pColorKey = &m_colorKey;
			m_colorKey = _ParseColor(values[3]);
		} else {
			m_pColorKey = NULL;
		}
		redraw = true;
	} else {
		CCtrlButton::_ParseProperty(key, value, relayout, redraw);
	}
}


CCtrlImageButton::CCtrlImageButton (uint id, uint n, uint h, uint p, bool bitmapTrans, COLORREF colorKey)
	: CCtrlButton(id)
	, m_colorKey(colorKey)
	, m_pColorKey(bitmapTrans ? &m_colorKey : NULL)
{
	m_imageIds[0] = n;
	m_imageIds[1] = h;
	m_imageIds[2] = p;
}

void CCtrlImageButton::drawMe (HDC hdc) {

	CBitmapPtr image = _GetImage();
	if (image != NULL) {
		CRect rc = getClientRect();
		image->draw(hdc, rc.left, rc.top, rc.Width(), rc.Height(), 0, 0);
	}

	CCtrlButton::drawMe(hdc);
}

void CCtrlImageButton::onMouseIn (CPoint pt) {
	if (disable) {
		return;
	}
	CCtrlButton::onMouseIn(pt);
}

void CCtrlImageButton::onMouseOut (CPoint pt) {
	if (disable) {
		return;
	}
	CCtrlButton::onMouseOut(pt);
}

void CCtrlImageButton::onLostCapture () {
	CCtrlButton::onLostCapture();
}

void CCtrlImageButton::onLButtonDown (CPoint pt, uint key) {
	if (disable) {
		return;
	}
	CCtrlButton::onLButtonDown(pt, key);
}

void CCtrlImageButton::onLButtonUp (CPoint pt, uint key) {
	if (disable) {
		if (_GetMainCtrl()->getCaptureCtrl() == shared_from_this()) {
			_Capture(false);
		}
		return;
	}
	CCtrlButton::onLButtonUp(pt, key);
}





UI_END
XL_END
