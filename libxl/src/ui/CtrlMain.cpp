#include <assert.h>
#include "../../include/ui/CtrlMain.h"

namespace xl {
	namespace ui {




CCtrlMain::CCtrlMain (ATL::CWindow *pWindow) 
	: CControl(0)
	, m_pWindow(pWindow)
	, m_captured(false) 
{
	assert (m_pWindow != NULL);
}

CCtrlMain::~CCtrlMain () {

}

// bool CCtrlMain::insertControl (CControlPtr ctrl) {
// 	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
// 		if ((*it)->getID() == ctrl->getID()) {
// 			return false;
// 		}
// 	}
// 
// 	m_controls.push_back(ctrl);
// 	ctrl->_Attach(this);
// 	return true;
// }
// 
// bool CCtrlMain::removeControl (CControlPtr ctrl) {
// 	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
// 		if ((*it) == ctrl) {
// 			m_controls.erase(it);
// 			return true;
// 		}
// 	}
// 	ctrl->_Detach();
// 	return false;
// }
// 
// bool CCtrlMain::removeControl (uint id) {
// 	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
// 		if ((*it)->getID() == id) {
// 			m_controls.erase(it);
// 			return true;
// 		}
// 	}
// 
// 	return false;
// }


// void CCtrlMain::draw () {
// 	HDC hdc = m_pWindow->GetDC();
// 
// 	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
// 		CControlPtr parent = (*it)->m_parent.lock();
// 		if (parent == NULL) {
// 			(*it)->draw(hdc);
// 		}
// 	}
// 
// 	m_pWindow->ReleaseDC(hdc);
// }

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
	}

	CRect rc;
	m_pWindow->GetClientRect(rc);
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	if (rc.PtInRect(pt)) {
		CControlPtr ctrl = _GetControlByPoint(pt);
		if (ctrl != m_ctrlHover) {
			if (m_ctrlHover != NULL) {
				m_ctrlHover->onMouseOut(pt);
			}
			m_ctrlHover = ctrl;
			if (m_ctrlHover != NULL) {
				m_ctrlHover->onMouseIn(pt);
			}
		}

		if (ctrl != NULL) {
			ctrl->onMouseMove(pt);
		}

	} else {
		::ReleaseCapture();
		if (m_ctrlHover != NULL) {
			m_ctrlHover->onMouseOut(pt);
			m_ctrlHover.reset();
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
			// TODO
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




	} // ui
} // xl