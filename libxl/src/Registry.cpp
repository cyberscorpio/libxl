#include <assert.h>
#include "../include/Registry.h"

#pragma comment (lib, "Advapi32.lib")

XL_BEGIN

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


XL_END