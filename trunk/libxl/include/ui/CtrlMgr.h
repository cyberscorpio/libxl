#ifndef XL_UI_CTRL_MGR_H
#define XL_UI_CTRL_MGR_H
#include <assert.h>
#include <vector>
#include <atlbase.h>
#include <atlwin.h>
#include "Ctrls.h"


namespace xl {
	namespace ui {

template <class T>
class CCtrlMgr
{
protected:
	typedef std::vector<CControlPtr>             CControlContainer;
	typedef CControlContainer::iterator          CControlIter;
	typedef CControlContainer::const_iterator    CControlConstIter;

	CControlContainer     m_controls;

public:
	bool insertControl (CControlPtr ctrl) {
		for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
			if ((*it)->getID() == ctrl->getID()) {
				return false;
			}
		}

		m_controls.push_back(ctrl);
		return true;
	}

	bool removeControl (CControlPtr ctrl) {
		for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
			if ((*it) == ctrl) {
				m_controls.erase(it);
				return true;
			}
		}
		return false;
	}

	bool removeControl (uint id) {
		for (CControlIter it = m_controls.begin(); it != m_controls.end(); ++ it) {
			if ((*it)->getID() == id) {
				m_controls.erase(it);
				return true;
			}
		}

		return false;
	}
};


	} // ui
} // xl


#endif
