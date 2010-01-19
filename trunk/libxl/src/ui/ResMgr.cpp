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

void CResMgr::_MakeBitmaGray (GpBmpPtr bitmap) {
	assert (bitmap != NULL);
	Gdiplus::Bitmap *pBitmap = bitmap.get();
	UINT w = pBitmap->GetWidth();
	UINT h = pBitmap->GetHeight();
	Gdiplus::PixelFormat pf = pBitmap->GetPixelFormat();

	Gdiplus::BitmapData bd;
	Gdiplus::Rect rc(0, 0, w, h);
	Gdiplus::Status res = pBitmap->LockBits(&rc,
		Gdiplus::ImageLockModeWrite,
		PixelFormat32bppARGB,
		&bd);
	assert (res == Gdiplus::Ok);
	if (res != Gdiplus::Ok) {
		return;
	}

	UINT len = w * h;
	for (UINT y = 0; y < h; ++ y) {
		unsigned char *pixels = (unsigned char *)bd.Scan0 + y * bd.Stride;
		for (UINT x = 0; x < w; ++ x) {
			UINT v = pixels[0];
			v += pixels[1];
			v += pixels[2];
			v /= 3;
			pixels[0] = pixels[1] = pixels[2] = v;
			pixels += 4;
		}
	}

	pBitmap->UnlockBits(&bd);
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

	m_gpBmpsById.clear();
	m_gpBmpsByFile.clear();
	m_gpGrayBmpsByFile.clear();
}

CResMgr::GpBmpPtr CResMgr::loadBitmapFromResource (uint id, const tstring &type, HINSTANCE hInst) {
	if (hInst == NULL) {
		hInst = (HINSTANCE)::GetModuleHandle(NULL);
	}

	HRSRC hRes = ::FindResource(hInst, MAKEINTRESOURCE(id), type.c_str());
	assert (hRes != NULL);
	if (hRes == NULL) {
		return GpBmpPtr();
	}

	DWORD reslen = ::SizeofResource(hInst, hRes);
	if (reslen == 0) {
		assert (false);
		return GpBmpPtr();
	}

	const void* pResourceData = ::LockResource(::LoadResource(hInst, hRes));
	if (!pResourceData) {
		assert (false);
		return GpBmpPtr();
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
			return GpBmpPtr(pBitmap);
		} else {
			delete pBitmap;
			pBitmap = NULL;
		}
	}

	return GpBmpPtr();
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

CResMgr::GpBmpPtr CResMgr::getBitmap (ushort id, const tstring &type, bool grayscale)
{
	assert (id != NULL);

	uint realid = id;
	if (grayscale) {
		realid = (1 << (sizeof(id) * 8)) | id;
	}

	_GpBmpIdMapIter it = m_gpBmpsById.find(realid);
	if (it != m_gpBmpsById.end()) {
		return it->second;
	}

	GpBmpPtr bitmap = loadBitmapFromResource(id, type, NULL);
	if (bitmap != NULL) {
		if (grayscale) {
			_MakeBitmaGray(bitmap);
		}
		m_gpBmpsById[realid] = bitmap;
		return bitmap;
	}
	return GpBmpPtr();
}

CResMgr::GpBmpPtr CResMgr::getBitmap (const tstring &file, bool grayscale) {
	_GpBmpFileMapType *pContainer = &m_gpBmpsByFile;
	if (grayscale) {
		pContainer = &m_gpGrayBmpsByFile;
	}

	_GpBmpFileMapIter it = pContainer->find(file);
	if (it != pContainer->end()) {
		return it->second;
	}

	// TODO: make ANSI work
	Gdiplus::Bitmap *pBitmap = new Gdiplus::Bitmap(file);
	if (pBitmap->GetLastStatus() != Gdiplus::Ok) {
		delete pBitmap;
		return GpBmpPtr();
	}

	GpBmpPtr bitmap(pBitmap);
	if (grayscale) {
		_MakeBitmaGray(bitmap);
	}

	(*pContainer)[file] = bitmap;
	return bitmap;
}


	} // namespace ui
} // namespace xl

