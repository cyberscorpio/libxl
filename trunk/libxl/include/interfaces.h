#ifndef XL_INTERFACES_H
#define XL_INTERFACES_H
/**
 * define the common interfaces
 */
#include "common.h"
XL_BEGIN

//////////////////////////////////////////////////////////////////////////
/// ILockable
class ILockable {
public:
	virtual ~ILockable () {}
	virtual void lock () const = 0;
	virtual void unlock () const = 0;
	virtual bool tryLock () const = 0;
};


//////////////////////////////////////////////////////////////////////////
// ILongTimeRunCallback
class ILongTimeRunCallback {
public:
	virtual ~ILongTimeRunCallback () {}

	/**
	 * inform the LTR (long time run) procedure.
	 * called by the LTR procedure, if it return true,
	 * the procedure stops immediately.
	 */
	virtual bool shouldStop () const = 0;

	/**
	 * called by the LTR procedure,
	 * to notify the callback object the progress
	 * @param progress range [0, 100]
	 */
	virtual void onProgress (uint /*progress*/) {}
};

//////////////////////////////////////////////////////////////////////////
// IExecutable
class IExecutable {
public:
	virtual bool operator() () = 0;
};


XL_END
#endif
