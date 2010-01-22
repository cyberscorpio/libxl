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
 * button-image: int type
 * button-image-text-padding: int
 *
 * CCtrlImageButton:
 * imagebutton-image: int int int type
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

void CCtrlButton::_ParseProperty (const tstring &key, const tstring &value, bool &relayout, bool &redraw) {
	if (key == _T("button-image")) {
		ExplodeT<TCHAR>::ValueT values = explode(_T(" "), value);
		assert(values.size() == 1 || values.size() == 2);
		uint id = _tstoi(values[0]);
		if (values.size() == 1) {
			assert(id == 0);
			m_imgType.clear();
		} else {
			m_imgType = values[1];
		}
		if (id != m_idImg) {
			m_idImg = id;
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

CCtrlButton::CCtrlButton (uint id, const tstring &text, uint idImg, const tstring &imgType)
	: CControl(id)
	, m_pushAndCapture(false)
	, m_text(text)
	, m_text_image_pading(4)
	, m_idImg(idImg)
	, m_imgType(imgType)
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


void CCtrlImageButton::_ParseProperty (const tstring &key, const tstring &value, bool &relayout, bool &redraw) {
	if (key == _T("imagebutton-image")) { // id id id type
		ExplodeT<TCHAR>::ValueT values = explode(_T(" "), value);
		assert(values.size() == 3 || values.size() == 4);
		m_idImageNormal = _tstoi(values[0]);
		m_idImageHover = _tstoi(values[1]);
		m_idImagePush = _tstoi(values[2]);
		if (values.size() == 3) {
			assert(m_idImageNormal == 0 && m_idImageHover == 0 && m_idImagePush == 0);
			setStyle(_T("background:none"));
		} else {
			m_imgType = values[3];
			if (m_idImageNormal != 0) {
				TCHAR buf[256];
				_stprintf_s(buf, 256, _T("background-image-id: %d %s"), m_idImageNormal, m_imgType.c_str());
				setStyle(buf);
			} else {
				setStyle(_T("background:none;"));
			}
		}
		redraw = true;
	} else {
		CCtrlButton::_ParseProperty(key, value, relayout, redraw);
	}
}


CCtrlImageButton::CCtrlImageButton (uint id, uint n, uint h, uint p, const tstring &imgType)
	: CCtrlButton(id)
	, m_idImageNormal(n), m_idImageHover(h), m_idImagePush(p)
	, m_imgType(imgType)
{
	TCHAR buf[256];
	if (m_idImageNormal != 0) {
		_stprintf_s(buf, 256, _T("background-image-id: %d %s"), m_idImageNormal, m_imgType.c_str());
		setStyle(buf);
	} else {
		setStyle(_T("background:none;"));
	}
}


void CCtrlImageButton::onMouseIn (CPoint pt) {
	if (disable) {
		return;
	}
	CCtrlButton::onMouseIn(pt);

	if (m_idImageHover != 0) {
		TCHAR buf[256];
		_stprintf_s(buf, 256, _T("background-image-id: %d %s"), m_idImageHover, m_imgType.c_str());
		setStyle(buf);
	} else {
		setStyle(_T("background:none;"));
	}
}

void CCtrlImageButton::onMouseOut (CPoint pt) {
	if (disable) {
		return;
	}
	CCtrlButton::onMouseOut(pt);

	if (m_idImageNormal != 0) {
		TCHAR buf[256];
		_stprintf_s(buf, 256, _T("background-image-id: %d %s"), m_idImageNormal, m_imgType.c_str());
		setStyle(buf);
	} else {
		setStyle(_T("background:none;"));
	}
}

void CCtrlImageButton::onLostCapture () {
	CCtrlButton::onLostCapture();

	if (m_idImageNormal != 0) {
		TCHAR buf[256];
		_stprintf_s(buf, 256, _T("background-image-id: %d %s"), m_idImageNormal, m_imgType.c_str());
		setStyle(buf);
	} else {
		setStyle(_T("background:none"));
	}
}

void CCtrlImageButton::onLButtonDown (CPoint pt, uint key) {
	if (disable) {
		return;
	}
	CCtrlButton::onLButtonDown(pt, key);

	if (m_idImagePush != 0) {
		TCHAR buf[256];
		_stprintf_s(buf, 256, _T("background-image-id: %d %s"), m_idImagePush, m_imgType.c_str());
		setStyle(buf);
	} else {
		setStyle(_T("background:none"));
	}
}

void CCtrlImageButton::onLButtonUp (CPoint pt, uint key) {
	if (disable) {
		if (_GetMainCtrl()->getCaptureCtrl() == shared_from_this()) {
			_Capture(false);
		}
		return;
	}
	CCtrlButton::onLButtonUp(pt, key);

	if (m_idImageHover != 0 && m_idImageHover != 0) {
		TCHAR buf[256];
		_stprintf_s(buf, 256, _T("background-image-id: %d %s"), 
			isPointIn(pt) ? m_idImageHover : m_idImageNormal, m_imgType.c_str());
		setStyle(buf);
	} else {
		setStyle(_T("background:none"));
	}
}





UI_END
XL_END
