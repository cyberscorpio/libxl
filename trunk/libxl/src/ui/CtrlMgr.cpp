#include <assert.h>
#include "../../include/ui/CtrlMgr.h"

namespace xl {
	namespace ui {

CControlPtr CCtrlMgr::_GetControlByPoint (CPoint pt) {
	for (CControlIterR itr = m_controls.rbegin(); itr != m_controls.rend(); ++ itr) {
		if ((*itr)->m_rect.PtInRect(pt)) {
			return *itr;
		}
	}

	return CControlPtr();
}




CCtrlMgr::CCtrlMgr (ATL::CWindow *pWindow) 
	: m_pWindow(pWindow)
	, m_captured(false) 
{
	assert (m_pWindow != NULL);
}

CCtrlMgr::~CCtrlMgr () {

}

bool CCtrlMgr::insertControl (CControlPtr ctrl) {
	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
		if ((*it)->getID() == ctrl->getID()) {
			return false;
		}
	}

	m_controls.push_back(ctrl);
	return true;
}

bool CCtrlMgr::removeControl (CControlPtr ctrl) {
	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
		if ((*it) == ctrl) {
			m_controls.erase(it);
			return true;
		}
	}
	return false;
}

bool CCtrlMgr::removeControl (uint id) {
	for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
		if ((*it)->getID() == id) {
			m_controls.erase(it);
			return true;
		}
	}

	return false;
}

// void CCtrlMgr::draw () {
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

void CCtrlMgr::invalidateControl(CControlPtr ctrl) {
	if (ctrl != NULL) {
		m_pWindow->InvalidateRect(ctrl->m_rect, FALSE);
	} else {
		m_pWindow->InvalidateRect(NULL);
	}
}


//////////////////////////////////////////////////////////////////////////
// message handles
LRESULT CCtrlMgr::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = false;
	return TRUE;
}

LRESULT CCtrlMgr::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = false;
	return TRUE;
}

LRESULT CCtrlMgr::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
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

LRESULT CCtrlMgr::OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	HWND hWndCaptured = (HWND)lParam;
	if (hWndCaptured != m_pWindow->m_hWnd) {
		if (m_captured) {
			m_captured = false;
			// TODO
		}
	}
	return 0;
}

LRESULT CCtrlMgr::OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	return 1;
}




	} // ui
} // xl