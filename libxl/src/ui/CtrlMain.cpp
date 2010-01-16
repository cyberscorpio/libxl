#include <assert.h>
#include "../../include/ui/CtrlMain.h"

namespace xl {
	namespace ui {

bool CCtrlMain::_SetCapture(CControlPtr ctrl) {
	if (m_ctrlCapture == ctrl) {
		return true;
	}

	if (ctrl != NULL) {
		assert (getControlByID(ctrl->m_id) == ctrl);
	}

	if (m_ctrlCapture != NULL) {
		m_ctrlCapture->onLostCapture();
	}
	m_ctrlCapture = ctrl;
	if (m_ctrlCapture != NULL) {
		m_ctrlCapture->onGetCapture();
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



CCtrlMain::CCtrlMain (ATL::CWindow *pWindow, CCtrlTargetRawPtr target) 
	: CControl(0)
	, m_pWindow(pWindow)
	, m_captured(false) 
{
	assert (m_pWindow != NULL);
	assert (target != NULL);
	_SetTarget(target);
}

CCtrlMain::~CCtrlMain () {

}


void CCtrlMain::invalidateControl(CControlPtr ctrl) {
	if (ctrl != NULL) {
		m_pWindow->InvalidateRect(ctrl->m_rect, FALSE);
	} else {
		m_pWindow->InvalidateRect(NULL);
	}
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

	CRect rc;
	m_pWindow->GetClientRect(rc);
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	if (m_ctrlCapture != NULL) {
		m_ctrlCapture->onMouseMove(pt);
		return 0;
	}

	CPoint ptScreen = pt;
	m_pWindow->ClientToScreen(&ptScreen);
	if (rc.PtInRect(pt) && ::WindowFromPoint(ptScreen) == m_pWindow->m_hWnd) {
		CControlPtr ctrl = _GetControlByPoint(pt);
		if (ctrl != m_ctrlHover) {
			_SetHoverCtrl(ctrl, pt);
		}

		if (ctrl != NULL) {
			ctrl->onMouseMove(pt);
		}

	} else {
		::ReleaseCapture();
		if (m_ctrlHover != NULL) {
			_SetHoverCtrl(CControlPtr(), pt);
		}
		m_captured = false;
	}

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
				_SetCapture(CControlPtr());
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

LRESULT CCtrlMain::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	if (m_ctrlCapture != NULL) {
		m_ctrlCapture->onLButtonDown(pt);
	} else if (m_ctrlHover != NULL) {
		m_ctrlHover->onLButtonDown(pt);
	}
	return 0;
}

LRESULT CCtrlMain::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	if (m_ctrlCapture != NULL) {
		m_ctrlCapture->onLButtonUp(pt);
	} else if (m_ctrlHover != NULL) {
		m_ctrlHover->onLButtonUp(pt);
	}
	return 0;
}

LRESULT CCtrlMain::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	if (m_ctrlCapture != NULL) {
		m_ctrlCapture->onRButtonDown(pt);
	} else if (m_ctrlHover != NULL) {
		m_ctrlHover->onRButtonDown(pt);
	}
	return 0;
}

LRESULT CCtrlMain::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	if (m_ctrlCapture != NULL) {
		m_ctrlCapture->onRButtonUp(pt);
	} else if (m_ctrlHover != NULL) {
		m_ctrlHover->onRButtonUp(pt);
	}
	return 0;
}



	} // ui
} // xl