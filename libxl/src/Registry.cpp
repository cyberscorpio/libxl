#include <assert.h>
#include <algorithm>
#include <utility>
#include "../include/Registry.h"

#pragma comment (lib, "Advapi32.lib")


static std::pair<const xl::tchar *, HKEY> sRootMap[] = {
	std::make_pair(_T("HKCU\\"), HKEY_CURRENT_USER),
	std::make_pair(_T("HKLM\\"), HKEY_LOCAL_MACHINE),
	std::make_pair(_T("HKCR\\"), HKEY_CLASSES_ROOT),
	std::make_pair(_T("HKEY_CURRENT_USER\\"), HKEY_CURRENT_USER),
	std::make_pair(_T("HKEY_LOCAL_MACHINE\\"), HKEY_LOCAL_MACHINE),
	std::make_pair(_T("HKEY_CLASSES_ROOT\\"), HKEY_CLASSES_ROOT),
	std::make_pair(_T("HKEY_USERS\\"), HKEY_USERS),
	std::make_pair(_T("HKEY_PERFORMANCE_DATA\\"), HKEY_PERFORMANCE_DATA),
};

static bool sCheckKeyName (const xl::tstring &keyName) {

	bool foundRoot = false;
	for (size_t i = 0; i < COUNT_OF(sRootMap); ++ i) {
		if (keyName.find(sRootMap[i].first) == 0) {
			foundRoot = true;
			break;
		}
	}
	if (!foundRoot) {
		assert(false);
		return false;
	}

	if (keyName.at(keyName.length() - 1) == '\\') {
		assert(false);
		return false;
	}

	return true;
}

static HKEY sOpenKey (const xl::tstring &keyName, bool createWhenNonexists) {
	HKEY root = NULL;
	xl::tstring subKey;
	for (size_t i = 0; i < COUNT_OF(sRootMap); ++ i ) {
		const xl::tchar *name = sRootMap[i].first;
		HKEY key = sRootMap[i].second;
		if (keyName.find(name) == 0) {
			root = key;
			subKey = keyName.substr(_tcslen(name));
			break;
		}
	}
	assert(root != NULL);
	assert(subKey.length() > 0 &&
		(root != HKEY_CLASSES_ROOT || std::count(subKey.begin(), subKey.end(), _T('\\')) > 0));

	DWORD dwSam = createWhenNonexists ? 
		/* KEY_WRITE | */KEY_SET_VALUE | KEY_QUERY_VALUE : KEY_QUERY_VALUE;
		// KEY_ALL_ACCESS : KEY_QUERY_VALUE;
	HKEY key = NULL;
	if (::RegOpenKeyEx(root, subKey.c_str(), 0, dwSam, &key) == ERROR_SUCCESS) {
		assert(key != NULL);
		return key;
	}

	if (createWhenNonexists) {
		// try to create the key
		DWORD dwDisp = 0;
		if (::RegCreateKeyEx(root, subKey.c_str(), 0, NULL, 
				     REG_OPTION_NON_VOLATILE, dwSam, NULL, &key, &dwDisp
				    ) == ERROR_SUCCESS) {
			assert(key != NULL);
			return key;
		}
	}

	return NULL;
}


XL_BEGIN


bool CRegistry::setValue (
                          const xl::tstring &keyName,
                          const xl::tstring &valueName,
                          const xl::tstring &value
                         ) {
	// check the keyName
	if (!sCheckKeyName(keyName)) {
		return false;
	}

	assert(std::count(keyName.begin(), keyName.end(), _T('\\')) > 1);
	HKEY key = sOpenKey(keyName, true);
	if (!key) {
		return false;
	}

	long result = ::RegSetValueEx(key, valueName.c_str(), 0, REG_SZ,
		(const BYTE *)value.c_str(), sizeof(xl::tchar) * (value.length() + 1));
	::RegCloseKey(key);

	return result == ERROR_SUCCESS;
}

bool CRegistry::setValue (
                          const xl::tstring &keyName,
                          const xl::tstring &valueName,
                          DWORD value
                         ) {
	// check the keyName
	if (!sCheckKeyName(keyName)) {
		return false;
	}

	assert(std::count(keyName.begin(), keyName.end(), _T('\\')) > 1);
	HKEY key = sOpenKey(keyName, true);
	if (!key) {
		return false;
	}

	long result = ::RegSetValueEx(key, valueName.c_str(), 0, REG_DWORD,
		(const BYTE *)&value, sizeof(DWORD));
	::RegCloseKey(key);

	return result == ERROR_SUCCESS;
}

bool CRegistry::getStringValue (
                                const xl::tstring &keyName,
                                const xl::tstring &valueName,
                                xl::tstring &value
                               ) {
	if (!sCheckKeyName(keyName)) {
		return false;
	}

	assert(std::count(keyName.begin(), keyName.end(), _T('\\')) > 1);
	HKEY key = sOpenKey(keyName, false);
	if (!key) {
		return false;
	}

	DWORD dwType = REG_NONE;
	DWORD dwSize = 0;
	long result = ::RegQueryValueEx(key, valueName.c_str(), 0, &dwType, NULL, &dwSize);
	if (result == ERROR_SUCCESS && dwSize > 0) {
		if (dwType == REG_SZ) {
#ifdef UNICODE
			assert(dwSize % 2 == 0);
#endif
			std::auto_ptr<BYTE> p(new BYTE[dwSize]);
			result = ::RegQueryValueEx(key, valueName.c_str(), 0, &dwType, p.get(), &dwSize);
			if (result == ERROR_SUCCESS) {
				value.append((tchar *)p.get(), dwSize / 2);
			}
		} else {
			assert(false);
		}
	}

	::RegCloseKey(key);
	return result == ERROR_SUCCESS && dwType == REG_SZ;
}

bool CRegistry::getDwordValue (
                                const xl::tstring &keyName,
                                const xl::tstring &valueName,
                                DWORD &value
                               ) {
	if (!sCheckKeyName(keyName)) {
		return false;
	}

	assert(std::count(keyName.begin(), keyName.end(), _T('\\')) > 1);
	HKEY key = sOpenKey(keyName, false);
	if (!key) {
		return false;
	}

	DWORD dwType = REG_NONE;
	DWORD dwSize = 0;
	long result = ::RegQueryValueEx(key, valueName.c_str(), 0, &dwType, NULL, &dwSize);
	if (result == ERROR_SUCCESS && dwSize == sizeof(DWORD)) {
		if (dwType == REG_DWORD) {
			result = ::RegQueryValueEx(key, valueName.c_str(), 0, &dwType, (BYTE *)&value, &dwSize);
			assert(result == ERROR_SUCCESS);
		} else {
			assert(false);
		}
	}

	::RegCloseKey(key);
	return result == ERROR_SUCCESS && dwType == REG_DWORD;
}


#if 0
CRegistry::CRegistry (const tstring &key, HKEY root /* = HKEY_CURRENT_USER */) {
	assert(key.length() > 0);
	m_key = 0;
	DWORD dwDisp = 0;
	if (RegCreateKeyEx(root, key.c_str(), 
	    0, NULL, REG_OPTION_NON_VOLATILE, 
	    KEY_ALL_ACCESS, NULL, &m_key, &dwDisp) != ERROR_SUCCESS) {
		m_key = NULL;
	}
}

CRegistry::~CRegistry () {
	close();
}

void CRegistry::close () {
	if (m_key != NULL) {
		RegCloseKey(m_key);
	}
}

void CRegistry::deleteKey () {
	
}

bool CRegistry::setString (const string &value) {
	assert(value.length() > 0);
	if (m_key == NULL) {
		return false;
	}

	return ::RegSetValueEx(m_key, NULL, 0, REG_SZ,
		(const BYTE *)value.c_str(), (value.length() + 1)) == ERROR_SUCCESS;
}

bool CRegistry::setString (const tstring &name, const string &value) {
	assert(name.length() > 0 && value.length() > 0);
	if (m_key == NULL) {
		return false;
	}

	return ::RegSetValueEx(m_key, name.c_str(), 0, REG_SZ,
		(const BYTE *)value.c_str(), (value.length() + 1)) == ERROR_SUCCESS;
}


bool registerProgid (const tstring &progid, const tstring &verb) {
	assert(progid.length() > 0);
	assert(verb.length() > 0);

	HKEY hProgid = NULL;
	tstring progidKey = _T("SOFTWARE\\Classes\\");
	progidKey += progid;

	DWORD dwDisp = 0;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, progidKey.c_str(), 
	    NULL, NULL, REG_OPTION_NON_VOLATILE,
	    KEY_ALL_ACCESS, NULL, &hProgid, &dwDisp) != ERROR_SUCCESS) {
		return false;
	}

	HKEY hShell = NULL;
	// shell
	if (hProgid != NULL) {
		RegCreateKeyEx(hProgid, _T("Shell"), 
			NULL, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hShell, &dwDisp);
	}

	// verb
	HKEY hVerb = NULL;
	if (hShell != NULL) {
		RegCreateKeyEx(hShell, verb.c_str(),
			NULL, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hVerb, &dwDisp);
	}

	// command
	HKEY hCommand = NULL;
	if (hVerb != NULL) {
		RegCreateKeyEx(hVerb, _T("Command"),
			NULL, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hCommand, &dwDisp);
	}
	
	// command value
	bool ret = false;
	if (hCommand != NULL) {
		tchar exe[MAX_PATH];
		::GetModuleFileName(NULL, exe, MAX_PATH);
		tstring cmd = _T("\"");
		cmd += exe;
		cmd += _T("\" \"%1\"");
		if (RegSetValue(hCommand, NULL, REG_SZ, cmd.c_str(), cmd.length()) == ERROR_SUCCESS) {
			ret = true;
		}
	}

	if (hProgid != NULL) {
		RegCloseKey(hProgid);
	}
	if (hShell != NULL) {
		RegCloseKey(hShell);
	}
	if (hVerb != NULL) {
		RegCloseKey(hVerb);
	}
	if (hCommand != NULL) {
		RegCloseKey(hCommand);
	}

	return ret;
}
#endif


XL_END