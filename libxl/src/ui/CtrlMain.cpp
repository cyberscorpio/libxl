#include <assert.h>
#include "../../include/ui/CtrlMain.h"
#include "../../include/utilities.h"

XL_BEGIN
UI_BEGIN

bool CCtrlMain::_SetCaptureCtrl(CControlPtr ctrl) {
	if (m_ctrlCapture == ctrl) {
		return true;
	}

	if (ctrl != NULL) {
		assert(getControlByID(ctrl->m_id) == ctrl);
	}

	m_ctrlCapture = ctrl;

	if (m_ctrlCapture == NULL) {
		_CheckMouseMove();
	}

	return true;
}

void CCtrlMain::_SetHoverCtrl (CControlPtr ctrlHover, CPoint pt) {
	if (m_ctrlHover == ctrlHover) {
		return;
	}

	if (m_ctrlHover != NULL) {
		CControlPtr ctrl = m_ctrlHover;
		ctrl->onMouseOut(pt);
		ctrl = ctrl->m_parent.lock();
		while (ctrl != NULL) {
			ctrl->onMouseOutChild(pt);
			ctrl = ctrl->m_parent.lock();
		}
	}

	m_ctrlHover = ctrlHover;

	if (m_ctrlHover != NULL) {
		CControlPtr ctrl = m_ctrlHover;
		ctrl->onMouseIn(pt);
		ctrl = ctrl->m_parent.lock();
		while (ctrl != NULL) {
			ctrl->onMouseInChild(pt);
			ctrl = ctrl->m_parent.lock();
		}
	}
}

void CCtrlMain::_BeforeRemoveCtrl (CControlPtr ctrl) {
	if (getCaptureCtrl() == ctrl) {
		ctrl->_Capture(false);
	}

	if (getHoverCtrl() == ctrl) {
		CPoint pt;
		::GetCursorPos(&pt);
		assert(m_pWindow);
		m_pWindow->ScreenToClient(&pt);
		_SetHoverCtrl(CControlPtr(), pt);
	}
}

void CCtrlMain::_CheckMouseMove (CPoint pt) {
	CPoint ptScreen = pt;
	m_pWindow->ClientToScreen(&ptScreen);
	CRect rc = m_rect;
	if (rc.PtInRect(pt) && ::WindowFromPoint(ptScreen) == m_pWindow->m_hWnd) {
		CControlPtr ctrl = getControlByPoint(pt);
		assert (ctrl == NULL || ctrl->display == true);
		if (ctrl != m_ctrlHover) {
			_SetHoverCtrl(ctrl, pt);
		}

		if (ctrl != NULL) {
			ctrl->onMouseMove(pt, 0);
		}

	} else {
		::ReleaseCapture();
		if (m_ctrlHover != NULL) {
			_SetHoverCtrl(CControlPtr(), pt);
		}
		m_captured = false;
	}
}

void CCtrlMain::_CheckMouseMove () {
	CPoint pt;
	::GetCursorPos(&pt);
	assert(m_pWindow);
	m_pWindow->ScreenToClient(&pt);
	_CheckMouseMove(pt);
}

uint CCtrlMain::_Wparam2KeyStatus (WPARAM wParam) {
	uint ks = 0;
	uint keys[] = {
		MK_CONTROL,
		MK_LBUTTON,
		MK_MBUTTON,
		MK_RBUTTON,
		MK_SHIFT,
	};
	uint values[] = {
		KP_CONTROL,
		KP_LBUTTON,
		KP_MBUTTON,
		KP_RBUTTON,
		KP_SHIFT,
	};
	assert(COUNT_OF(keys) == COUNT_OF(values));

	for (int i = 0; i < COUNT_OF(keys); ++ i) {
		if (wParam & keys[i]) {
			ks |= values[i];
		}
	}

	return ks;
}

uint CCtrlMain::_SetTimer (CControlPtr ctrl, uint elapse, uint id) {
	assert(ctrl != NULL);
	assert(m_pWindow != NULL && m_pWindow->IsWindow());

	id = m_pWindow->SetTimer(id, elapse, NULL);
	if (id != 0) {
		m_timerCtrls[id] = ctrl;
	} else {
		assert(false);
	}
	return id;
}


CCtrlMain::CCtrlMain (ATL::CWindow *pWindow, CCtrlTargetRawPtr target) 
	: CControl(0)
	, m_pWindow(pWindow)
	, m_captured(false) 
{
	::InitializeCriticalSection(&m_cs);
	assert (m_pWindow != NULL);
	assert (target != NULL);
	_SetTarget(target);
}

CCtrlMain::~CCtrlMain () {
	::DeleteCriticalSection(&m_cs);
}

void CCtrlMain::enableGesture (bool enable) {
	if (enable && m_ctrlGesture == NULL) {
		m_ctrlGesture.reset(new CCtrlGesture(this));
		return;
	}

	if (!enable && m_ctrlGesture != NULL) {
		// TODO: check whether gesture is in used ?
		m_ctrlGesture.reset();
	}
}


void CCtrlMain::invalidateControl(CControlPtr ctrl) const {
	if (ctrl != NULL) {
		m_pWindow->InvalidateRect(ctrl->m_rect, FALSE);
	} else {
		m_pWindow->InvalidateRect(NULL);
	}
}

bool CCtrlMain::postMessage (UINT msg, WPARAM wParam, LPARAM lParam) {
	if (!m_pWindow || !m_pWindow->IsWindow()) {
		return false;
	}

	return !!m_pWindow->PostMessage(msg, wParam, lParam);
}

void CCtrlMain::reLayout () {
	layout(m_rcLayout);

	_CheckMouseMove();
	
	// invalidateControl(); // called already in CCtrlMain::layout()
}

CRect CCtrlMain::layout (CRect rc) {
	CSimpleLock lock(&m_cs);

	invalidateControl();
	m_rcLayout = rc;
	return CControl::layout(rc);
}

void CCtrlMain::draw (HDC hdc, CRect rcClip) {
	CSimpleLock lock(&m_cs);
	// CTimerLogger log(_T("CCtrlMain::draw cost"));
	CControl::draw(hdc, rcClip);
}

HWND CCtrlMain::getHWND () {
	if (m_pWindow) {
		return m_pWindow->m_hWnd;
	} else {
		return NULL;
	}
}

ATL::CWindow* CCtrlMain::getWindow () {
	return m_pWindow;
}

//////////////////////////////////////////////////////////////////////////
// message handles
LRESULT CCtrlMain::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = false;
	return TRUE;
}

LRESULT CCtrlMain::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = false;
	return TRUE;
}

LRESULT CCtrlMain::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	if (!m_captured) {
		::SetCapture(m_pWindow->m_hWnd);
		m_captured = true;
	}

	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	if (m_ctrlCapture != NULL) {
		m_ctrlCapture->onMouseMove(pt, _Wparam2KeyStatus(wParam));
		return 0;
	}

	_CheckMouseMove(pt);

	return 0;
}

LRESULT CCtrlMain::OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	HWND hWndCaptured = (HWND)lParam;
	if (hWndCaptured != m_pWindow->m_hWnd) {
		if (m_captured) {
			m_captured = false;
			if (m_ctrlHover != NULL) {
				CPoint pt;
				::GetCursorPos(&pt);
				m_pWindow->ScreenToClient(&pt);
				_SetHoverCtrl(CControlPtr(), pt);
			}

			if (m_ctrlCapture != NULL) {
				m_ctrlCapture->onLostCapture();
				_SetCaptureCtrl(CControlPtr());
			}
		}
	}
	return 0;
}

LRESULT CCtrlMain::OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	return 1;
}

LRESULT CCtrlMain::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CPaintDC dc(m_pWindow->m_hWnd);
	draw(dc.m_hDC, dc.m_ps.rcPaint);
	return 0;
}

LRESULT CCtrlMain::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	assert(m_pWindow != NULL && m_pWindow->IsWindow());
	uint id = (uint)wParam;
	m_pWindow->KillTimer(id);

	_TimerControlIter it = m_timerCtrls.find(id);
	if (it == m_timerCtrls.end()) {
		bHandled = false;
	} else {
		CControlPtr ctrl = it->second;
		m_timerCtrls.erase(it);
		if (ctrl->_GetMainCtrl() == this) {
			ctrl->onTimer(id);
		}
	}

	return 0;
}

LRESULT CCtrlMain::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	if (m_ctrlCapture != NULL) {
		m_ctrlCapture->onLButtonDown(pt, _Wparam2KeyStatus(wParam));
	} else if (m_ctrlHover != NULL) {
		m_ctrlHover->onLButtonDown(pt, _Wparam2KeyStatus(wParam));
	}
	return 0;
}

LRESULT CCtrlMain::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	if (m_ctrlCapture != NULL) {
		m_ctrlCapture->onLButtonUp(pt, _Wparam2KeyStatus(wParam));
	} else if (m_ctrlHover != NULL) {
		m_ctrlHover->onLButtonUp(pt, _Wparam2KeyStatus(wParam));
	}
	return 0;
}

LRESULT CCtrlMain::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	if (m_ctrlCapture != NULL) {
		m_ctrlCapture->onRButtonDown(pt, _Wparam2KeyStatus(wParam));
	} else if (m_ctrlGesture != NULL) {
		m_ctrlGesture->onRButtonDown(pt, _Wparam2KeyStatus(wParam));
	} else if (m_ctrlHover != NULL) {
		m_ctrlHover->onRButtonDown(pt, _Wparam2KeyStatus(wParam));
	}
	return 0;
}

LRESULT CCtrlMain::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	if (m_ctrlCapture != NULL) {
		m_ctrlCapture->onRButtonUp(pt, _Wparam2KeyStatus(wParam));
	} else if (m_ctrlHover != NULL) {
		m_ctrlHover->onRButtonUp(pt, _Wparam2KeyStatus(wParam));
	}
	return 0;
}

LRESULT CCtrlMain::OnWMRemoveControl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {

	removeChild((uint)wParam);

	return 0;
}


UI_END
XL_END