#include "../../include/ui/Application.h"


CApplication::CApplication() {
}

CApplication::~CApplication() {
}

bool CApplication::initilise() {
	return true;
}

void CApplication::preRun() {
}

void CApplication::postRun() {
}

int CApplication::run(LPTSTR lpstrCmdLine, int nCmdShow) {
	CMessageLoop msgLoop;

}
