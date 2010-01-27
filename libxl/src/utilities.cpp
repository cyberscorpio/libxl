#include <assert.h>
#include <Windows.h>
#include "../include/utilities.h"
XL_BEGIN

CTimerLogger::CTimerLogger (const tstring &msg, bool useMsgBox /* = false */)
	: m_useMsgBox(useMsgBox), m_logged(false)
	, m_msg(msg), m_tick(::GetTickCount())
{
}

CTimerLogger::~CTimerLogger () {
	log();
}

void CTimerLogger::log () {
	if (!m_logged) {
		m_logged = true;

		m_tick = ::GetTickCount() - m_tick;
		TCHAR buf[MAX_PATH];
		if (m_useMsgBox) {
			_stprintf_s(buf, MAX_PATH, _T("%s: %dms"), m_msg.c_str(), m_tick);
			::MessageBox(NULL, buf, _T("log:"), MB_OK);
		} else {
			_stprintf_s(buf, MAX_PATH, _T("%s: %dms\n"), m_msg.c_str(), m_tick);
			::OutputDebugString(buf);
		}
	}
}

void CTimerLogger::restart (const tstring &msg, bool useMsgBox) {
	m_logged = false;
	m_msg = msg;
	m_useMsgBox = useMsgBox;
	m_tick = ::GetTickCount();
}



XL_END
