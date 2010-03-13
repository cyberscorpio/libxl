#ifndef XL_TSPTR_H
#define XL_TSPTR_H
/**
 * A thread safe shared pointer
 */
#include <assert.h>
#include "common.h"
#include "lockable.h"
XL_BEGIN

__declspec(selectany) CUserLock _ts_shared_ptr_lock(4096);

template <class T>
class ts_shared_ptr {
public:
	typedef ts_shared_ptr<T>                       MyType;

private:
	T                  *m_obj;
	volatile long      *m_ref;

	void _Lock () {
		_ts_shared_ptr_lock.lock();
	}

	void _Unlock () {
		_ts_shared_ptr_lock.unlock();
	}

	void _Assign (T *obj) {
		assert(m_ref == NULL && m_obj == NULL);
		assert(obj != NULL);
		m_ref = new long(1);
		m_obj = obj;
	}

	void _Assign (const MyType &tsp) {
		m_obj = tsp.m_obj;
		m_ref = tsp.m_ref;
		if (m_ref != NULL) {
			assert(m_obj != NULL);
			InterlockedIncrement(m_ref);
		}
	}

	void _Destroy () {
		if (m_ref == NULL) {
			assert(m_obj == NULL);
			return;
		}

		long user_count = InterlockedDecrement(m_ref);
		assert(user_count >= 0);
		if (user_count == 0) {
			delete m_obj;
			delete m_ref;
		}

		m_obj = NULL;
		m_ref = NULL;
	}
public:

	ts_shared_ptr () 
		: m_obj(NULL)
		, m_ref(NULL)
	{
	}

	ts_shared_ptr (T *obj)
		: m_obj(NULL)
		, m_ref(NULL)
	{
		if (obj != NULL) {
			_Assign(obj);
		}
	}

	~ts_shared_ptr () {
		_Destroy();
	}

	MyType& operator = (const MyType &tsp) {
		_Lock();
		assert(tsp.use_count() > 0);
		assert(m_obj != tsp.get());
		_Destroy();
		_Assign(tsp);
		_Unlock();
		return *this;
	}

	void reset () {
		_Lock();
		_Destroy();
		_Unlock();
	}

	void reset (T *obj) {
		_Lock();
		assert(obj != m_obj);
		_Destroy(true);
		_Assign(obj);
		_Unlock();
	}

	T* get () const {
		return m_obj;
	}

	T& operator * () const {
		return *m_obj;
	}

	T* operator -> () const {
		return m_obj;
	}

	long use_count () const {
		if (m_ref != NULL) {
			return *m_ref;
		} else {
			return 0;
		}
	}

	bool unique () const {
		return (m_ref != NULL) && (*m_ref == 1);
	}

	operator bool () const {
		return m_obj != NULL;
	}
};


XL_END
#endif
