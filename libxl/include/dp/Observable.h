#ifndef XL_DP_OBSERVABLE_H
#define XL_DP_OBSERVABLE_H
#include <assert.h>
#include <vector>
#include <windows.h>
#include "../common.h"

/**
 * -= How to use =-
 *
 * 1. derived class should be:
class CObj : public xl::dp::CObserableT<CObj> {
public:
	enum EVENT {
		EVT_1,
		EVT_2,
		// ...
		EVT_NUM
	};
public:
	// ...
	void doSth() {
		// ...
		// when some event occur:
		_TriggerEvent (evt, param);
		// ...
	}
};

 * 2. observer should be:
class CObserver : public CObj::IObserver {
public:
		virtual void onEvent (EVT evt, void *param) {
			// do something here
		}
};

 * param is derived class defined
 */

XL_BEGIN
DP_BEGIN

template <class T>
class CObserableT
{
protected:
	typedef unsigned int                        EVT;

public:
	class IObserver
	{
	public:
		typedef typename CObserableT::EVT   EVT;
		virtual void onEvent (
		                      EVT evt,
		                      void *param
		                     ) = 0;
	};

protected:
	typedef std::vector<IObserver *>            _Subscribers;
	typedef typename _Subscribers::iterator     _Iter;
	_Subscribers         m_subscribers;
	bool                 m_dispatching;

	CRITICAL_SECTION     m_cs;
	void _Lock () {::EnterCriticalSection(&m_cs);}
	void _Unlock () {::LeaveCriticalSection(&m_cs);}

	void _TriggerEvent (EVT evt, void *param) {
		assert (evt < T::EVT_NUM);
		_Lock();
		for (_Iter it = m_subscribers.begin(); it != m_subscribers.end(); ++ it) {
			(*it)->onEvent(evt, param);
		}
		_Unlock();
	}

	// before call it, _Lock() should already be called
	_Iter _FindSubscriber (IObserver *observer) {
		_Iter it;
		for (it = m_subscribers.begin(); it != m_subscribers.end(); ++ it) {
			if (*it == observer) {
				break;
			}
		}
		return it;
	}

public:
	CObserableT () : m_dispatching (false) {
		::InitializeCriticalSection(&m_cs);
	}

	virtual ~CObserableT () {
		::DeleteCriticalSection(&m_cs);
	}

	bool subscribe (IObserver *subscriber, bool asTail = true) {
		assert(subscriber != NULL);
		if (m_dispatching) {
			assert(false); // when dispatching event, subscribe / unsubscribe is forbidden.
			return false;
		}

		_Lock();
		assert(_FindSubscriber(subscriber) == m_subscribers.end());
		m_subscribers.insert(asTail ? m_subscribers.end() : m_subscribers.begin(), subscriber);
		_Unlock();
		return true;
	}

	bool unsubscribe (IObserver *observer) {
		assert(observer != NULL);
		if (m_dispatching) {
			assert(false); // when dispatching event, subscribe / unsubscribe is forbidden.
			return false;
		}

		_Lock();
		_Iter it = _FindSubscriber(observer);
		bool found = it != m_subscribers.end();
		assert(found);
		m_subscribers.erase(it);
		_Unlock();
		return found;
	}
};

DP_END
XL_END

#endif
