#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../libxl/include/ui/MainWindow.h"



class CMainWindow : public xl::ui::CMainWindowT<CMainWindow>
{
//	xl::ui::CControl *m_ctrl;
public:
	virtual void onCommand (xl::uint id, xl::ui::CControlPtr ctrl);
	virtual void onSlider (xl::uint id, int _min, int _max, int _curr, bool tracking, xl::ui::CControlPtr ctrl);
	virtual xl::tstring onGesture (const xl::tstring &gesture, CPoint ptDown, bool release);

public:
	DECLARE_WND_CLASS_EX (_T("CMainWindow"), 0, COLOR_INFOBK)
	BEGIN_MSG_MAP (CMainWindow)
		MESSAGE_HANDLER (WM_CREATE, OnCreate)
		MESSAGE_HANDLER (WM_SIZE, OnSize)
		CHAIN_MSG_MAP(CMainWindowT)
	END_MSG_MAP ()

	LRESULT OnCreate (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnSize (UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
};

#endif
