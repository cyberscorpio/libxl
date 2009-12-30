#ifndef XL_APPLICATION_H
#define XL_APPLICATION_H
// #define _WTL_NO_CSTRING
#include <atlbase.h>
#include <atlapp.h>

class CApplication : public CAppModule {
public:
	CApplication ();
	~CApplication ();

	virtual bool initilize ();
	virtual void preRun ();
	virtual void postRun ();

	int run ();
};


#endif

