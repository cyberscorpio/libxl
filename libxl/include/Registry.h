#ifndef XL_REGISTRY_H
#define XL_REGISTRY_H
#include <Windows.h>
#include "common.h"
#include "string.h"
XL_BEGIN


class CRegistry
{
public:
	/**
	 * @param keyName
	 *  The full registry path of the key, beginning with a valid registry root, 
	 *  such as "HKEY_CURRENT_USER". The full list is:
	 *  "HKEY_CURRENT_USER", "HKEY_LOCAL_MACHINE", "HKEY_USERS" and "HKEY_PERFORMANCE_DATA" or
	 *  "HKCU", "HKLM"
	 * @param valueName
	 *  The name of the name/value pair, "" for default.
	 */
	static bool setValue (
		const xl::tstring &keyName,
		const xl::tstring &valueName,
		const xl::tstring &value
		);

	static bool setValue (
		const xl::tstring &keyName,
		const xl::tstring &valueName,
		DWORD value
		);

	static bool getStringValue (
		const xl::tstring &keyName,
		const xl::tstring &valueName,
		xl::tstring &value
		);

	static bool getDwordValue (
		const xl::tstring &keyName,
		const xl::tstring &valueName,
		DWORD &value
		);
};


#if 0
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
#endif


///////////////////////////////////////////////////////////////////////


XL_END
#endif
