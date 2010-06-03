#ifndef XL_LANGUAGE_H
#define XL_LANGUAGE_H
/**
 * CLanguage looks for lang-xxxx.ini as source of the language.
 * And xxxx is the local ID, for example:
 *
 *	- 0409		English
 *	- 0804		Simplified Chinese
 *	- 0404		Traditional Chinese
 *
 * The ini file MUST be placed in the SAME directory with the executable, unless you
 * set 
 */
#include <memory>
#include "common.h"
#include "string.h"

XL_BEGIN

class CIni;
class CLanguage {
private:
	CLanguage ();
	~CLanguage ();

	typedef std::auto_ptr<CIni>                    CIniPtr;
	CIniPtr                                        m_pIni;

	tstring                                        m_baseDir;
	tstring                                        m_lcid;

	void _SetDefaultLcid ();
	void _LoadStrings ();

public:
	void setBaseDir (const tstring &baseDir);
	void setLcid (const tstring &lcid);

	static CLanguage* getInstance ();
	tstring getString (const tstring &key);
};

XL_END

#endif
