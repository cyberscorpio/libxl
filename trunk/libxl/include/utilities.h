#ifndef XL_UTLITIES_H
#define XL_UTLITIES_H
#include "common.h"
#include "string.h"
XL_BEGIN

//////////////////////////////////////////////////////////////////////////
// trace
void trace (const tchar *format, ...);


//////////////////////////////////////////////////////////////////////////
// TimeLogger
class CTimerLogger
{
	bool m_useMsgBox;
	bool m_logged;
	tstring m_msg;
	uint m_tick;
public:
	CTimerLogger (const tstring &msg, bool useMsgBox = false);
	~CTimerLogger ();

	void log ();
	void restart (const tstring &msg, bool useMsgBox = false);
};


XL_END
#endif
