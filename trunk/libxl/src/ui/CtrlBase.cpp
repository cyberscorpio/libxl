#include "../../include/ui/gdi.h"
#include "../../include/ui/CtrlBase.h"

namespace xl {
	namespace ui {


CCtrlBase::CCtrlBase () {
	width = 100;
	height = 100;
	setMargin(10);
	setPadding(10);
}

CCtrlBase::~CCtrlBase () {

}



void CCtrlBase::draw (HDC hdc) {
	// TODO
}


void CCtrlBase::setRect (CRect rc) {
	m_rc = rc;
}






	} // namespace ui
} // namespace xl