#include <assert.h>
#include "../../include/ui/Bitmap.h"
#include "../../include/ui/Gdi.h"
XL_BEGIN
UI_BEGIN

bool CBitmap::load (HBITMAP hSrc) {
	assert(hSrc != NULL);
	_Clear();

	BITMAP bmp;
	if (::GetObject(hSrc, sizeof(bmp), &bmp) != 0) {
		int w = bmp.bmWidth, h = bmp.bmHeight;
		int bitcount = bmp.bmBitsPixel;
		int stride = bmp.bmWidthBytes;
		assert(w > 0 && h > 0);
		assert(bitcount >= 24);
		if (!create(w, h, bitcount)) {
			return false;
		}
		assert(stride == _GetStrideNoLock());

		// use GetDIBits() to get the data
		BITMAPINFO info;
		memset(&info, 0, sizeof(info));
		BITMAPINFOHEADER &header = info.bmiHeader;
		header.biSize = sizeof(info.bmiHeader);
		header.biWidth = w;
		header.biHeight = -h;
		header.biPlanes = 1;
		header.biBitCount = bitcount;
		header.biCompression = BI_RGB;

		uint8 *data = (uint8 *)_GetDataNoLock();
		HDC hDC = ::GetDC(NULL);
		::GetDIBits(hDC, hSrc, 0, h, data, &info, DIB_RGB_COLORS);
		::ReleaseDC(NULL, hDC);
	}

	return m_hBitmap != NULL;
}

bool CBitmap::load (int id) {
	_Clear();
	HBITMAP hSrc = ::LoadBitmap(::GetModuleHandle(NULL), MAKEINTRESOURCE(id));
	if (hSrc) {
		load (hSrc);
		::DeleteObject(hSrc);
	}
	return m_hBitmap != NULL;
}

void CBitmap::draw (HDC hdc, int toX, int toY, int toW, int toH, int fromX, int fromY, uint op) {
	assert(m_hBitmap != NULL);
	if (!m_hBitmap) {
		return;
	}

	CDC mdc;
	mdc.CreateCompatibleDC(hdc);
	HBITMAP oldBitmap = mdc.SelectBitmap(m_hBitmap);
	::BitBlt(hdc, toX, toY, toW, toH, mdc, fromX, fromY, op);
	mdc.SelectBitmap(oldBitmap);
}

UI_END
XL_END
