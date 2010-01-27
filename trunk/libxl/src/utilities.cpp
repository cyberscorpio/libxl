#include <assert.h>
#include <stdarg.h>

#include <Windows.h>

#include "../include/utilities.h"

XL_BEGIN

//////////////////////////////////////////////////////////////////////////
// trace

void trace (const tchar *format, ...) {
	va_list args;
	int     len;
	tchar    *buffer;

	va_start(args, format);

	len = _vsctprintf(format, args) + 1;
	buffer = (tchar *)malloc(len * sizeof(tchar));
	assert(buffer);
	_vstprintf_s(buffer, len, format, args);

	OutputDebugString(buffer);
	// OutputDebugString(_T("\n"));

	free(buffer);
}


//////////////////////////////////////////////////////////////////////////
// CTimerLogger

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
		if (m_useMsgBox) {
			TCHAR buf[MAX_PATH];
			_stprintf_s(buf, MAX_PATH, _T("%s: %dms"), m_msg.c_str(), m_tick);
			::MessageBox(NULL, buf, _T("log:"), MB_OK);
		} else {
			trace(_T("%s: %dms\n"), m_msg.c_str(), m_tick);
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
