#include <assert.h>
#include "../include/lockable.h"
XL_BEGIN


//////////////////////////////////////////////////////////////////////////
/// user lock

CUserLock::CUserLock () {
	::InitializeCriticalSection(&m_cs);
}

CUserLock::~CUserLock () {
	VERIFY(tryLock());
	unlock();
	::DeleteCriticalSection(&m_cs);
}

void CUserLock::lock () const {
	::EnterCriticalSection(&m_cs);
}

void CUserLock::unlock () const {
	::LeaveCriticalSection(&m_cs);
}

bool CUserLock::tryLock () const {
	return !!::TryEnterCriticalSection(&m_cs);
}


XL_END
