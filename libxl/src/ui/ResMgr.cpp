#include <assert.h>
#include "ole2.h"
#include "../../include/ui/ResMgr.h"

#pragma comment (lib, "ole32.lib")

namespace xl {
	namespace ui {

///////////////////////////////////////////////////////////
// private

CResMgr::CResMgr() {

}

CResMgr::~CResMgr() {
	reset();
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

void CResMgr::reset () {
	for (_GdiObjMapIter it = m_sysFonts.begin(); it != m_sysFonts.end(); ++ it) {
		::DeleteObject(it->second);
	}
	m_sysFonts.clear();

	for (_GpBmpMapIter it = m_gpBitmaps.begin(); it != m_gpBitmaps.end(); ++ it) {
		delete it->second;
	}
	m_gpBitmaps.clear();
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

Gdiplus::Bitmap* CResMgr::getBitmap (uint id, const tstring &type) {
	assert (id != NULL);

	_GpBmpMapIter it = m_gpBitmaps.find(id);
	if (it != m_gpBitmaps.end()) {
		return it->second;//.get();
	}

	HINSTANCE hInst = (HINSTANCE)::GetModuleHandle(NULL);
	HRSRC hRes = ::FindResource(hInst, MAKEINTRESOURCE(id), type.c_str());
	assert (hRes != NULL);
	DWORD imageSize = ::SizeofResource(hInst, hRes);
	if (!imageSize) {
		assert (false);
		return NULL;
	}

	const void* pResourceData = ::LockResource(::LoadResource(hInst, hRes));
	if (!pResourceData) {
		assert (false);
		return NULL;
	}

	Gdiplus::Bitmap *pBitmap = NULL;
	HGLOBAL hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (hBuffer)
	{
		void* pBuffer = ::GlobalLock(hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);

			IStream* pStream = NULL;
			if (::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK)
			{
				pBitmap = Gdiplus::Bitmap::FromStream(pStream);
				pStream->Release();
			}
			::GlobalUnlock(hBuffer);
		}
		::GlobalFree(hBuffer);
		hBuffer = NULL;
	}

	if (pBitmap) { 
		if (pBitmap->GetLastStatus() == Gdiplus::Ok) {
			m_gpBitmaps[id] = _GpBmpPtr(pBitmap);
		} else {
			delete pBitmap;
			pBitmap = NULL;
		}
	}

	return pBitmap;
}


	} // namespace ui
} // namespace xl

