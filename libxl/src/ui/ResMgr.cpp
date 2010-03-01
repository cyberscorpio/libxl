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


HGDIOBJ CResMgr::_CreateSysFont(int height, uint style) {
	HGDIOBJ font = ::GetStockObject(DEFAULT_GUI_FONT);
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

	font = (HGDIOBJ)::CreateFontIndirect(&lf);
	return font;
}

void CResMgr::_MakeBitmaGray (CBitmapPtr bitmap) {
	assert (bitmap != NULL);
	CBitmap *pBitmap = bitmap.get();
	UINT w = pBitmap->getWidth();
	UINT h = pBitmap->getHeight();
	int bitcount = bitmap->getBitCounts();
	int bytespp = bitcount / 8;
	assert(bytespp > 2); // 3, 4
	for (uint y = 0; y < h; ++ y) {
		uint8 *data_line = (uint8 *)pBitmap->getLine(y);
		for (uint x = 0; x < w; ++ x) {
			uint v = (uint)data_line[0] + (uint)data_line[1] + (uint)data_line[2];
			v /= 3;
			data_line[0] = data_line[1] = data_line[2] = v;
			data_line += bytespp;
		}
	}
}


///////////////////////////////////////////////////////////
// public

CResMgr* CResMgr::getInstance() {
	static CResMgr This;
	return &This;
}

void CResMgr::reset () {
	for (_GdiObjMapIter it = m_sysFonts.begin(); it != m_sysFonts.end(); ++ it) {
		::DeleteObject(it->second);
	}
	m_sysFonts.clear();
}

HFONT CResMgr::getSysFont (int height, uint style) {
	assert (style == (style & FS_MASK));
	// make key
	uint key = height << 16;
	key |= style;

	HGDIOBJ font = NULL;
	_Lock();
	// find font
	_GdiObjMapIter it = m_sysFonts.find(key);
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


UI_END
XL_END
