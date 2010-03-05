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
	assert(m_level > 0);
	-- m_level;
	::LeaveCriticalSection(&m_cs);
}

bool CUserLock::tryLock () const {
	if (::TryEnterCriticalSection(&m_cs)) {
		++ m_level;
		return true;
	} else {
		return false;
	}
}

int CUserLock::getLockLevel () const {
	if (m_cs.OwningThread != (HANDLE)::GetCurrentThreadId()) {
		return 0;
	} else {
		assert(m_cs.RecursionCount == m_level);
		return m_cs.RecursionCount;
	}
}


XL_END
