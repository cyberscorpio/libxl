#ifndef XL_UTLITIES_H
#define XL_UTLITIES_H
#include "common.h"
#include <stdarg.h>
#include "string.h"
#include "lockable.h"

#ifdef XL_TRACE_ENABLE
#define XLTRACE(format, ...) xl::trace(format, __VA_ARGS__)
#else
#define XLTRACE(format, ...)
#endif

XL_BEGIN

//////////////////////////////////////////////////////////////////////////
// trace
void trace (const tchar *format, ...);


//////////////////////////////////////////////////////////////////////////
// TimeLogger
class CTimerLogger
{
	bool m_logged;
	tstring m_msg;
	uint m_tick;

	void _Init (const tchar *, va_list);

public:
	CTimerLogger (const tchar *format, ...);
	~CTimerLogger ();

	void log ();
	void restart (const tchar *format, ...);
};


//////////////////////////////////////////////////////////////////////////
// CScopeLock
class CScopeLock
{
	const ILockable   *m_lock;
public:
	CScopeLock (const ILockable *lock);
	CScopeLock (ILockable *lock);
	~CScopeLock ();
	void lock (const ILockable *lock);
	void unlock ();
};


//////////////////////////////////////////////////////////////////////////
// OS Version
#ifdef WIN32
bool os_is_xp ();
bool os_is_vista_or_later ();
#endif


XL_END
#endif
