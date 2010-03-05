#ifndef XL_LOCKABLE_H
#define XL_LOCKABLE_H
/**
 * implements the ILockable interface
 */
#include <assert.h>
#include <Windows.h>
#include "common.h"
#include "interfaces.h"
XL_BEGIN


//////////////////////////////////////////////////////////////////////////
/// user lock, use critical section in windows

class CUserLock : public ILockable
{
protected:
	mutable CRITICAL_SECTION                       m_cs;
	mutable int                                    m_level;

public:
	CUserLock ();
	virtual ~CUserLock ();

	virtual void lock () const;
	virtual void unlock () const;
	virtual bool tryLock () const;

	int getLockLevel () const;
};



XL_END
#endif
