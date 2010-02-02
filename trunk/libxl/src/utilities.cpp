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
	tchar  *buffer, *bufferex;
	tchar   buf[MAX_PATH];
	tchar   bufex[MAX_PATH];
	tstring formatex;

	va_start(args, format);

	len = _vsctprintf(format, args) + 20;
	if (len > MAX_PATH) {
		buffer = (tchar *)malloc(len * sizeof(tchar));
		bufferex = (tchar *)malloc(len * sizeof(tchar));
	} else {
		buffer = buf;
		bufferex = bufex;
	}
	assert(buffer);
	_vstprintf_s(buffer, len, format, args);
	_stprintf_s(bufferex, len, _T("%d:\t%s"), ::GetTickCount(), buffer);

	OutputDebugString(bufferex);

	if (buffer != buf) {
		free(buffer);
	}
	if (bufferex != bufex) {
		free(bufferex);
	}
}


//////////////////////////////////////////////////////////////////////////
// CTimerLogger

CTimerLogger::CTimerLogger (const tstring &msg, bool useMsgBox /* = false */)
	: m_useMsgBox(useMsgBox), m_logged(false)
	, m_msg(msg), m_tick(::GetTickCount())
{
}

CTimerLogger::CTimerLogger (bool useMsgBox, const tchar *format, ...) 
	: m_useMsgBox(useMsgBox), m_logged(false)
	, m_tick(::GetTickCount())
{
	va_list args;
	int     len;
	tchar  *buffer;
	tchar   buf[MAX_PATH];
	tstring formatex;

	va_start(args, format);

	len = _vsctprintf(format, args) + 20;
	if (len > MAX_PATH) {
		buffer = (tchar *)malloc(len * sizeof(tchar));
	} else {
		buffer = buf;
	}
	assert(buffer);
	_vstprintf_s(buffer, len, format, args);

	m_msg = buffer;

	if (buffer != buf) {
		free(buffer);
	}
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


//////////////////////////////////////////////////////////////////////////
// CSimpleLock
CSimpleLock::CSimpleLock (CRITICAL_SECTION *pcs) : m_pcs(NULL) {
	lock(pcs);
}

CSimpleLock::~CSimpleLock () {
	unlock();
}

void CSimpleLock::lock (CRITICAL_SECTION *pcs) {
	assert(pcs != NULL);
	assert(m_pcs == NULL);

	m_pcs = pcs;
	::EnterCriticalSection(m_pcs);
}

void CSimpleLock::unlock () {
	if (m_pcs) {
		::LeaveCriticalSection(m_pcs);
		m_pcs = NULL;
	}
}



XL_END
