#include <assert.h>
#include "../../include/ui/ResMgr.h"

namespace xl {
	namespace ui {

///////////////////////////////////////////////////////////
// private

CResMgr::CResMgr() {

}

CResMgr::~CResMgr() {
	for (_MapIter it = m_sysFonts.begin(); it != m_sysFonts.end(); ++ it) {
		::DeleteObject(it->second);
	}
	m_sysFonts.clear();
}

void CResMgr::_Lock() {
}

void CResMgr::_Unlock() {
}


HGDIOBJ CResMgr::_CreateSysFont(int height, uint style) {
	HGDIOBJ font = ::GetStockObject(DEFAULT_GUI_FONT);
	assert (font != NULL);
	LOGFONT lf;
	VERIFY(::GetObject(font, sizeof(lf), &lf) == sizeof(lf));
	if ((height == 0 || height == lf.lfHeight) && style == 0) {
		return font;
	}

	// modify the parameters
	lf.lfHeight = height;
	if (style & FS_BOLD) {
		lf.lfWeight = FW_BOLD;
	}
	if (style & FS_ITALIC) {
		lf.lfItalic = TRUE;
	}
	if (style & FS_UNDERLINE) {
		lf.lfUnderline = TRUE;
	}
	if (style & FS_STRIKEOUT) {
		lf.lfStrikeOut = TRUE;
	}

	font = (HGDIOBJ)::CreateFontIndirect(&lf);
	return font;
}


///////////////////////////////////////////////////////////
// public

CResMgr* CResMgr::getInstance() {
	static CResMgr This;
	return &This;
}

HFONT CResMgr::getSysFont (int height, uint style) {
	assert (style == (style & FS_MASK));
	// make key
	uint key = height << 16;
	key |= style;

	HGDIOBJ font = NULL;
	_Lock();
	// find font
	_MapIter it = m_sysFonts.find(key);
	if (it == m_sysFonts.end()) {
		// create font
		font = _CreateSysFont(height, style);
		if (font != NULL) {
			m_sysFonts[key] = font;
		}
	} else {
		font = it->second;
	}
	_Unlock();
	return (HFONT)font;
}

	} // namespace ui
} // namespace xl

