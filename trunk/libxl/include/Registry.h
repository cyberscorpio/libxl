#ifndef XL_REGISTRY_H
#define XL_REGISTRY_H
#include <Windows.h>
#include "common.h"
#include "string.h"
XL_BEGIN

class CRegistry {
	HKEY m_key;
public:
	CRegistry (const tstring &key, HKEY root = HKEY_CURRENT_USER);
	~CRegistry ();

	void close ();
	void deleteKey ();
	bool setString (const string &value);
	bool setString (const tstring &name, const string &value);
};

/**
 * Write @progid into HKCU\SOFTWARE\Classes\
 * after called, the registry is:
 * HKCU\SOFTWARE\CLASSES\progid
 *	Shell
 *		verb
 *			Command: "path to the EXE" "%1"
 */
bool registerProgid (const tstring &progid, const tstring &verb);

XL_END
#endif
