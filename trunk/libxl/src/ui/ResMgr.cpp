#include <assert.h>
#include "ole2.h"
#include "../../include/ui/ResMgr.h"

#pragma comment (lib, "gdiplus.lib")
#pragma comment (lib, "ole32.lib")

XL_BEGIN
UI_BEGIN

///////////////////////////////////////////////////////////
// private

CResMgr::CResMgr() {
}

CResMgr::~CResMgr() {
	reset();
}

void CResMgr::_Lock() {
	lock();
}

void CResMgr::_Unlock() {
	unlock();
}


HFONT CResMgr::_CreateSysFont(int height, uint style) {
	HFONT font = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	assert (font != NULL);
	LOGFONT lf;
	VERIFY(::GetObject(font, sizeof(lf), &lf) == sizeof(lf));
	if ((height == 0 || height == lf.lfHeight) && style == 0) {
		return font;
	}

	// modify the parameters
	if (height == 0) {
		height = lf.lfHeight;
	}
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

	font = ::CreateFontIndirect(&lf);
	return font;
}


///////////////////////////////////////////////////////////
// public

CResMgr* CResMgr::getInstance() {
	static CResMgr This;
	return &This;
}

void CResMgr::reset () {
	for (_FontMapType::iterator it = m_sysFonts.begin(); it != m_sysFonts.end(); ++ it) {
		::DeleteObject(it->second);
	}
	m_sysFonts.clear();

	for (_PenMapType::iterator it = m_pens.begin(); it != m_pens.end(); ++ it) {
		::DeleteObject(it->second);
	}
	m_pens.clear();

	for (_IconMapType::iterator it = m_icons.begin(); it != m_icons.end(); ++ it) {
		::DestroyIcon(it->second);
	}
	m_icons.clear();
}

HFONT CResMgr::getSysFont (int height, uint style) {
	assert (style == (style & FS_MASK));
	// make key
	uint key = height << 16;
	key |= style;

	HFONT font = NULL;
	_Lock();
	// find font
	_FontMapType::iterator it = m_sysFonts.find(key);
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
	return font;
}

HPEN CResMgr::getPen (ushort style, ushort width, COLORREF color) {
	uint64 key = (uint64)style;
	key <<= 16;
	key |= (uint64)width;
	key <<= 32;
	key |= (uint)color;

	_PenMapType::iterator it = m_pens.find(key);
	if (it != m_pens.end()) {
		return it->second;
	} else {
		HPEN pen = ::CreatePen(style, width, color);
		if (pen != NULL) {
			m_pens[key] = pen;
			return pen;
		}
	}

	return NULL;
}

HICON CResMgr::getIcon (ushort id) {
	uint key = id;
	_IconMapType::iterator it = m_icons.find(key);
	if (it != m_icons.end()) {
		return it->second;
	} else {
		HICON icon = ::LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(id));
		if (icon != NULL) {
			m_icons[key] = icon;
			return icon;
		}
	}

	return NULL;
}

CBitmapPtr CResMgr::getBitmap (ushort bmpid, bool grayscale) {
	uint64 id = bmpid;
	if (grayscale) {
		id |= BMP_GRAY;
	}

	_BitmapBigMapType::iterator it = m_bitmaps.find(id);
	if (it != m_bitmaps.end()) {
		return it->second;
	} else {
		CBitmapPtr bitmap(new CBitmap());
		if (bitmap->load(bmpid)) {
			if (grayscale) {
				bitmap->gray();
			}

			m_bitmaps[id] = bitmap;
			return bitmap;
		}
	}

	return CBitmapPtr();
}

CBitmapPtr CResMgr::getTransBitmap (ushort bmpid, COLORREF colorKey, bool grayscale) {
	uint64 id = (uint)colorKey;
	id <<= 32;
	id |= (uint64)bmpid;
	if (grayscale) {
		id |= BMP_GRAY;
	}

	_BitmapBigMapType::iterator it = m_transBitmaps.find(id);
	if (it != m_transBitmaps.end()) {
		return it->second;
	} else {
		CBitmapPtr bitmap(new CBitmap());
		if (bitmap->load(bmpid)) {
			bitmap->setColorKey(colorKey);
			if (grayscale) {
				bitmap->gray();
			}

			m_transBitmaps[id] = bitmap;
			return bitmap;
		}
	}

	return CBitmapPtr();
}


UI_END
XL_END
