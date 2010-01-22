#ifndef XL_LANGUAGE_H
#define XL_LANGUAGE_H
#include "common.h"
#include "string.h"

XL_BEGIN

class CLanguage {
private:
	CLanguage ();
	~CLanguage ();

public:
	static CLanguage* getInstance ();
	tstring getString (const tstring &key);
};

XL_END

#endif
