#include <assert.h>
#include "ole2.h"
#include "../../include/ui/ResMgr.h"

#pragma comment (lib, "gdiplus.lib")
#pragma comment (lib, "ole32.lib")

namespace xl {
	namespace ui {

///////////////////////////////////////////////////////////
// private

CResMgr::CResMgr() {
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

CResMgr::~CResMgr() {
	reset();
	Gdiplus::GdiplusShutdown(gdiplusToken);
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

	m_gpBitmaps.clear();
}

Gdiplus::Bitmap* CResMgr::loadBitmapFromResource (uint id, const tstring &type, HINSTANCE hInst) {
	if (hInst == NULL) {
		hInst = (HINSTANCE)::GetModuleHandle(NULL);
	}

	HRSRC hRes = ::FindResource(hInst, MAKEINTRESOURCE(id), type.c_str());
	assert (hRes != NULL);
	if (hRes == NULL) {
		return NULL;
	}

	DWORD reslen = ::SizeofResource(hInst, hRes);
	if (reslen == 0) {
		assert (false);
		return NULL;
	}

	const void* pResourceData = ::LockResource(::LoadResource(hInst, hRes));
	if (!pResourceData) {
		assert (false);
		return NULL;
	}

	Gdiplus::Bitmap *pBitmap = NULL;
	HGLOBAL hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, reslen);
	if (hBuffer)
	{
		void* pBuffer = ::GlobalLock(hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, reslen);

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
			return pBitmap;
		} else {
			delete pBitmap;
			pBitmap = NULL;
		}
	}

	return NULL;
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

CResMgr::GpBmpPtr CResMgr::getBitmap (uint id, const tstring &type) {
	assert (id != NULL);

	_GpBmpMapIter it = m_gpBitmaps.find(id);
	if (it != m_gpBitmaps.end()) {
		return it->second;
	}


	Gdiplus::Bitmap *pBitmap = loadBitmapFromResource(id, type, NULL);

	if (pBitmap) { 
		GpBmpPtr p = GpBmpPtr(pBitmap);
		m_gpBitmaps[id] = p;
		return p;
	} else {
		return GpBmpPtr();
	}
}


	} // namespace ui
} // namespace xl

