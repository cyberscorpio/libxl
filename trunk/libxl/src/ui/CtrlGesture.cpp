#include <assert.h>
#include <math.h>
#include "../../include/ui/Gdi.h"
#include "../../include/ui/CtrlTarget.h"
#include "../../include/ui/CtrlMain.h"
#include "../../include/ui/CtrlGesture.h"
#include "../../include/ui/ResMgr.h"

/**
 * setStyle:
 * gesture-sensitivity: int
 * gesture-timeout: int
 * gesture-line-width: int
 */

namespace xl {
	namespace ui {

void CCtrlGesture::_ParseProperty (const tstring &key, const tstring &value, bool &relayout, bool &redraw) {
	if (key == _T("gesture-sensitivity")) {
		m_gestureSensitivity = _tstoi(value);
		assert(m_gestureSensitivity != 0);
	} else if (key == _T("gesture-timeout")) {
		m_gestureTimeout = _tstoi(value);
		assert(m_gestureTimeout > 0);
	} else if (key == _T("gesture-line-width")) {
		m_gestureLineWidth = _tstoi(value);
		assert(m_gestureLineWidth >= 0);
	} else {
		CControl::_ParseProperty(key, value, relayout, redraw);
	}
}


CCtrlGesture::CCtrlGesture (CCtrlMain *pCtrlMain)
	: m_pCtrlMain(pCtrlMain)
	, m_gestureSensitivity(10)
	, m_gestureTimeout(1000)
	, m_gestureLineWidth(5)
	, m_lastMove(0)
	, m_isTimeout(false)
{
	assert(m_pCtrlMain);
	// setStyle(_T("px:left;py:top;width:fill;height:fill;color:#ffffff;background-color:#000000;opacity:50;"));
	setStyle(_T("px:left;py:top;width:fill;height:fill;"));
}

CCtrlGesture::~CCtrlGesture () {

}



void CCtrlGesture::onLostCapture() {
	// m_pCtrlMain->postMessage(WM_XL_REMOVE_CONTROL, m_id, 0);
	m_pCtrlMain->removeChild(m_id);
 	m_points.clear();
 	m_gesture.clear();
}

void CCtrlGesture::onRButtonDown (CPoint pt, uint key) {
	m_pCtrlMain->insertChild(shared_from_this());
	assert (m_pCtrlMain->getControlByID(m_id) == shared_from_this());
	_Capture(true);
	m_points.clear();
	m_gesture.clear();
	m_points.push_back(pt);
	m_isTimeout = false;
}

void CCtrlGesture::onRButtonUp (CPoint pt, uint key) {
	assert(m_pCtrlMain->getCaptureCtrl() == shared_from_this());
	assert(m_points.size() > 0);

	CPoint ptDown = m_points[0];
	bool pass2background = m_points.size() == 1;
	tstring gesture = m_gesture;
	bool isTimeout = m_isTimeout;
	
	_Capture(false);
	m_pCtrlMain->removeChild(m_id);
	m_points.clear();
	m_gesture.clear();
	m_isTimeout = false;

	if (::GetTickCount() - m_lastMove < m_gestureTimeout && !isTimeout) {
		assert(m_target);
		m_target->onGesture(gesture, true);
		m_lastMove = 0;
	} else if (pass2background && !isTimeout) {
		CControlPtr ctrl = m_pCtrlMain->getControlByPoint(ptDown);
		if (ctrl != NULL) {
			ctrl->onRButtonDown(ptDown, key);
		}

		ctrl = m_pCtrlMain->getControlByPoint(pt);
		if (ctrl != NULL) {
			ctrl->onRButtonUp(pt, key);
		}
	}
}

void CCtrlGesture::onMouseMove (CPoint pt, uint key) {
	if (m_points.size() == 0) {
		return; // when called by CCtrlMain::_CheckMouseMove(), this could happen
	}

	CPoint ptLast = m_points[m_points.size() - 1];
	if (abs(pt.x - ptLast.x) < m_gestureSensitivity && abs(pt.y - ptLast.y) < m_gestureSensitivity) {
		return;
	}

	assert(m_gestureTimeout > 0);
	m_isTimeout = false;
	_SetTimer(m_gestureTimeout, (uint)this);
	m_lastMove = ::GetTickCount();
	int x = pt.x - ptLast.x;
	int y = pt.y - ptLast.y;

	TCHAR c;
	if (abs(x) > abs(y)) {
		if (x > 0) {
			c = _T('R');
		} else {
			c = _T('L');
		}
	} else {
		if (y > 0) {
			c = _T('D');
		} else {
			c = _T('U');
		}
	}

	if (m_gesture.length() == 0 || m_gesture.at(m_gesture.length() - 1) != c) {
		m_gesture += c;
	}
	m_points.push_back(pt);

	invalidate();
}

void CCtrlGesture::onTimer (uint id) {
	assert(id == (uint)this);
	m_isTimeout = true;
	invalidate();
}

void CCtrlGesture::drawMe (HDC hdc) {
	if (m_points.size() <= 1) {
		return;
	}

	CRect rc = getClientRect();

	CDCHandle dc(hdc);
	if (m_gestureLineWidth > 0) {
		HPEN pen = ::CreatePen(PS_SOLID, m_gestureLineWidth, _GetColor());
		HPEN oldPen = dc.SelectPen(pen);

		dc.Polyline(&m_points[0], m_points.size());

		dc.SelectPen(oldPen);
		::DeleteObject(pen);
	}

	assert(m_target);
	tstring text = m_gesture;
	text += _T(" (");
	text += m_target->onGesture(m_isTimeout ? _T("canceled") : m_gesture, false);
	text += _T(")");

	rc.top = rc.bottom - 20;
	COLORREF oldColor = dc.SetTextColor(_GetColor());
	dc.drawTransparentTextWithDefaultFont(text, text.length(), rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	dc.SetTextColor(oldColor);
}


	} // ui
} // xl