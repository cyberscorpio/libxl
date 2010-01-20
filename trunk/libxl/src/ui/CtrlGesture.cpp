#include <assert.h>
#include <math.h>
#include "../../include/ui/Gdi.h"
#include "../../include/ui/CtrlTarget.h"
#include "../../include/ui/CtrlMain.h"
#include "../../include/ui/CtrlGesture.h"
#include "../../include/ui/ResMgr.h"

namespace xl {
	namespace ui {

static const uint GESTURE_SENSITIVITY = 10;
static const uint GESTURE_TIMEOUT = 1000;

CCtrlGesture::CCtrlGesture (CCtrlMain *pCtrlMain)
	: m_pCtrlMain(pCtrlMain)
	, m_lastMove(0)
	, m_inRButtonUp(false)
{
	assert(m_pCtrlMain);
	setStyle(_T("px:left;py:top;width:fill;height:fill;color:#ffffff;background-color:#000000;opacity:50;"));
}

CCtrlGesture::~CCtrlGesture () {

}



void CCtrlGesture::onLostCapture() {
	if (m_inRButtonUp) {
		return;
	}

	CCtrlMain *pCtrlMain = _GetMainCtrl();
	assert(pCtrlMain);
	pCtrlMain->removeChild((uint)this);
	m_points.clear();
	m_gesture.clear();
}

void CCtrlGesture::onRButtonDown (CPoint pt) {
	m_pCtrlMain->insertChild(shared_from_this());
	_SetCapture(true);
	m_points.clear();
	m_points.push_back(pt);
	m_gesture.clear();
}

void CCtrlGesture::onRButtonUp (CPoint pt) {
	m_inRButtonUp = true;
	m_pCtrlMain->removeChild((uint)this);

	if (::GetTickCount() - m_lastMove < GESTURE_TIMEOUT) {
		assert(m_target);
		m_target->onGesture(m_gesture, true);
		m_lastMove = 0;
	}

	m_points.clear();
	m_gesture.clear();
	m_inRButtonUp = false;
}

void CCtrlGesture::onMouseMove (CPoint pt) {
	assert(m_points.size() > 0);
	CPoint ptLast = m_points[m_points.size() - 1];
	if (abs(pt.x - ptLast.x) < GESTURE_SENSITIVITY && abs(pt.y - ptLast.y) < GESTURE_SENSITIVITY) {
		return;
	}

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
// 		if (m_gesture.length() > 0) {
// 			m_gesture += _T(",");
// 		}
		m_gesture += c;
	}
	m_points.push_back(pt);

	invalidate();
}

void CCtrlGesture::drawMe (HDC hdc) {
	CRect rc = getClientRect();

	CDCHandle dc(hdc);
	HPEN pen = ::CreatePen(PS_SOLID, 5, RGB(255,0,0));
	HPEN oldPen = dc.SelectPen(pen);

	for (size_t i = 0; i < m_points.size() - 1; ++ i) {
		dc.drawLine(m_points[i], m_points[i + 1]);
	}

	dc.SelectPen(oldPen);
	::DeleteObject(pen);

	assert(m_target);
	tstring text = m_gesture;
	text += _T(" (");
	text += m_target->onGesture(m_gesture, false);
	text += _T(")");

	rc.top = rc.bottom - 20;
	COLORREF oldColor = dc.SetTextColor(_GetColor());
	dc.drawTransparentTextWithDefaultFont(text, text.length(), rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	dc.SetTextColor(oldColor);
}


	} // ui
} // xl