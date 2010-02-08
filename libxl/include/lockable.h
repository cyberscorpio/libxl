#ifndef XL_LOCKABLE_H
#define XL_LOCKABLE_H
#include <assert.h>
#include <Windows.h>
#include "common.h"
XL_BEGIN

//////////////////////////////////////////////////////////////////////////
/// interface
class ILockable {
public:
	virtual ~ILockable () {}
	virtual void lock () const = 0;
	virtual void unlock () const = 0;
	virtual bool tryLock () const = 0;
};


//////////////////////////////////////////////////////////////////////////
/// user lock, use critical section in windows

class CUserLock : public ILockable
{
protected:
	mutable CRITICAL_SECTION                       m_cs;
	mutable uint       m_level;

public:
	CUserLock ();
	virtual ~CUserLock ();

	virtual void lock () const;
	virtual void unlock () const;
	virtual bool tryLock () const;

	uint getLockLevel () const {
		return m_level;
	}
};



XL_END
#endif
