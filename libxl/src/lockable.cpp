#include <assert.h>
#include "../include/lockable.h"
XL_BEGIN


//////////////////////////////////////////////////////////////////////////
/// user lock

CUserLock::CUserLock () : m_level(0) {
	::InitializeCriticalSection(&m_cs);
}

CUserLock::~CUserLock () {
	assert(m_level == 0);
	::DeleteCriticalSection(&m_cs);
}

void CUserLock::lock () const {
	::EnterCriticalSection(&m_cs);
	++ m_level;
}

void CUserLock::unlock () const {
	-- m_level;
	::LeaveCriticalSection(&m_cs);
}

bool CUserLock::tryLock () const {
	return !!::TryEnterCriticalSection(&m_cs);
}


XL_END
