#include <assert.h>
#include <stdio.h>
#include "../include/fs.h"
#include "../include/ini.h"
XL_BEGIN

// FF FE (little endian)
// FE FF (big endian)
// EF BB BF (UTF8)

/**
 * Load the file content, and translate (if needed) to tstring
 */
tstring CIni::_LoadFile () {
	string data;
	if (!file_get_contents(m_fileName, data)) {
		return tstring(); // error!
	}

	size_t len = data.length();
	const uint8 *p = (const uint8 *)data.c_str();
	if (len <= 4) { // MB
		return s2ts(data);
	}

	// UCS LE ?
	if (p[0] == 0xff && p[1] == 0xfe) {
		wstring ws;
		ws.reserve(len + 1);
		size_t wl = len / 2;
		for (size_t i = 1; i < wl; ++ i) {
			wchar_t w = p[i * 2 + 1];
			w <<= 8;
			w |= p[i * 2];
			ws += w;
		}
		return ws2ts(ws);
	}

	// UCS BE
	if (p[0] == 0xfe && p[1] == 0xff) {
		wstring ws;
		ws.reserve(len + 1);
		size_t wl = len / 2;
		for (size_t i = 1; i < wl; ++ i) {
			wchar_t w = p[i * 2];
			w <<= 8;
			w |= p[i * 2 + 1];
			ws += w;
		}
		return ws2ts(ws);
	}

	// TODO: Test UTF8 CASE
	if (p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF) {
		assert(false);
	}

	// treats as MB
	return s2ts(data);
}

void CIni::_Load () {
	tstring data = _LoadFile();

	// split it into lines
	ExplodeT<tchar>::ValueT lines = explode(_T("\r\n"), data);

	tstring section = _T("");
	// parse every line
	for (auto it = lines.begin(); it != lines.end(); ++ it) {
		tstring line = *it;
		line.trim(_T(" \t"));

		if (line.length() == 0) {
			continue;
		}

		const tchar *l = line.c_str();
		size_t len = line.length();

		// comment ?
		if (l[0] == _T(';')) {
			continue;
		}

		// [section] ?
		if (l[0] == _T('[') && l[len - 1] == _T(']')) {
			section = line.substr(1, len - 2);
			continue;
		}

		// key=value ?
		ExplodeT<tchar>::ValueT kv = explode(_T("="), line);
		if (kv.size() < 2) {
			assert(false);
			continue; // parse error
		}

		tstring key = kv[0];
		tstring value = kv[1];
		if (kv.size() > 2) {
			for (size_t i = 2; i < kv.size(); ++ i) {
				value += _T("=");
				value += kv[i];
			}
		}

		m_ini[section][key] = value;
	}
}

CIni::CIni (const tstring &fileName) : m_fileName(fileName)
{
	_Load();
}

CIni::Iterator CIni::begin (const tstring &section) {
	return m_ini[section].begin();
}

CIni::Iterator CIni::end (const tstring &section) {
	return m_ini[section].end();
}

tstring CIni::get (const tstring &section, const tstring &key) {
	auto it = m_ini.find(section);
	if (it != m_ini.end()) {
		auto v = it->second.find(key);
		if (v != it->second.end()) {
			return v->second;
		}
	}

	return tstring();
}

void CIni::set (const tstring &section, const tstring &key, const tstring &value) {
	m_ini[section][key] = value;
}

void CIni::write (tstring fileName) {
	if (fileName.length() == 0) {
		fileName = m_fileName;
	}
	assert(fileName.length() > 0);

	// save as UCS2 LE
	wstring ws = L"";
	ws += (wchar_t)0xFEFF;
	ws.reserve(1024 * 10);
	for (auto sit = m_ini.begin(); sit != m_ini.end(); ++ sit) {
		tstring section = sit->first;
		if (section.length() > 0) {
			ws += _T("[");
			ws += ts2ws(section);
			ws += _T("]\r\n");
		}

		for (auto it = sit->second.begin(); it != sit->second.end(); ++ it) {
			tstring line = it->first;
			line += _T("=");
			line += it->second;
			ws += ts2ws(line);
			ws += _T("\r\n");
		}

		ws += _T("\r\n\r\n");
	}

	const wchar_t *p = ws.c_str();
	size_t len = ws.length();
	const char *text = (const char *)p;
	len = (len + 1) * 2;
	string data;
	data.append(text, len);

	file_put_contents(fileName, data);
}


XL_END
