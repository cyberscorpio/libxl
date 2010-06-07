#ifndef XL_INI_H
#define XL_INI_H
/**
 * A simple ini processor
 */
#include <map>
#include "common.h"
#include "string.h"
XL_BEGIN

class CIni {
	tstring                                        m_fileName;

	typedef std::map<tstring, tstring>             _KVType;
	typedef std::map<tstring, _KVType>             _MapType;
	_MapType                                       m_ini;

	tstring _LoadFile ();
	void _Load ();

public:
	typedef _KVType::iterator                      Iterator;

	CIni (const tstring &file);

	Iterator begin (const tstring &section);
	Iterator end (const tstring &section);

	tstring get (const tstring &section, const tstring &key);
	void set (const tstring &section, const tstring &key, const tstring &value);

	void write (tstring fileName = _T(""));
	void reload ();
};

XL_END
#endif
