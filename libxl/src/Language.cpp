#include <assert.h>
#include "../include/fs.h"
#include "../include/ini.h"
#include "../include/Language.h"

static TCHAR *sDefLcid = _T("0409");

XL_BEGIN

CLanguage::CLanguage () {
	tchar buf[MAX_PATH];
	::GetModuleFileName(NULL, buf, MAX_PATH);
	tstring baseDir = file_get_directory(buf);
	setBaseDir(baseDir);
	_SetDefaultLcid();
}

CLanguage::~CLanguage () {

}

void CLanguage::_SetDefaultLcid () {
	tchar code[10];
#if 0
	LCID lcid = GetSystemDefaultLCID();
	_stprintf_s(code, 10, _T("%04x"), (lcid & 0xffff)); // use its 0-15 bits
#else
	LANGID langid = GetUserDefaultUILanguage();
	if (langid == LOCALE_CUSTOM_DEFAULT || langid == LOCALE_CUSTOM_UI_DEFAULT || langid == LOCALE_CUSTOM_UNSPECIFIED) {
		LCID lcid = GetSystemDefaultLCID();
		_stprintf_s(code, 10, _T("%04x"), (lcid & 0xffff)); // use its 0-15 bits
	} else {
		_stprintf_s(code, 10, _T("%04x"), langid);
	}
#endif
	setLcid (code);
}

void CLanguage::_LoadStrings () {

	tstring ini = m_baseDir + _T("\\lang-") + m_lcid + _T(".ini");
	if (!file_exists(ini)) {
		ini = m_baseDir + _T("\\lang-") + ::sDefLcid + _T(".ini");
		if (!file_exists(ini)) {
			return;
		}
	}

	m_pIni.reset(new CIni(ini));
}

void CLanguage::setBaseDir (const tstring &baseDir) {
	assert(baseDir.length() > 0);
	tstring base = baseDir;
	if (base.at(base.length() - 1) == _T('\\') || base.at(base.length() - 1) == _T('/')) {
		base = base.substr(0, base.length() - 1);
	}

	if (m_baseDir != base) {
		m_baseDir = base;
		m_pIni.reset();
	}
}

void CLanguage::setLcid (const tstring &lcid) {
	if (m_lcid != lcid) {
		m_lcid = lcid;
		m_pIni.reset();
	}
}

CLanguage* CLanguage::getInstance () {
	static CLanguage language;
	return &language;
}

tstring CLanguage::getString (const tstring &key) {
	if (m_pIni.get() == NULL) {
		this->_LoadStrings();
		if (m_pIni.get() == NULL) {
			return tstring();
		}
	}

	tstring section;
	return m_pIni->get(section, key);
}


XL_END