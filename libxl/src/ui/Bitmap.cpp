#include <assert.h>
#include "../../include/ui/Bitmap.h"
#include "../../include/ui/Gdi.h"
XL_BEGIN
UI_BEGIN

CBitmap::CBitmap () : m_pTransColor(NULL) {

}

CBitmap::CBitmap (COLORREF colorKey) : m_rgbTrans(colorKey), m_pTransColor(&m_rgbTrans) {

}

CBitmap::~CBitmap () {

}

void CBitmap::setColorKey (COLORREF colorKey) {
	m_rgbTrans = colorKey;
	m_pTransColor = &m_rgbTrans;
}

void CBitmap::clearColorKey () {
	m_pTransColor = NULL;
}

void CBitmap::gray () {
	assert(m_hBitmap != NULL);
	assert(getBitCounts() >= 24);

	int bitcount = getBitCounts();
	int bytespp = bitcount / 8;
	assert(bytespp > 2); // 3, 4
	int w = getWidth();
	int h = getHeight();
	bool trans = m_pTransColor != NULL;
	for (int y = 0; y < h; ++ y) {
		uint8 *data_line = getLine(y);
		if (trans) {
			COLORREF rgb = m_rgbTrans;
			uint8 rr = GetRValue(rgb), gg = GetGValue(rgb), bb = GetBValue(rgb);
			for (int x = 0; x < w; ++ x) {
				uint8 b = data_line[0], g = data_line[1], r = data_line[2];
				if (rr != r || gg != g || bb != b) {
					uint v = r + g + b;
					v /= 3;
					data_line[0] = data_line[1] = data_line[2] = (uint8)v;
				}
				data_line += bytespp;
			}
		} else {
			for (int x = 0; x < w; ++ x) {
				uint v = (uint)data_line[0] + (uint)data_line[1] + (uint)data_line[2];
				v /= 3;
				data_line[0] = data_line[1] = data_line[2] = (uint8)v;
				data_line += bytespp;
			}
		}
	}
}

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
		assert(stride == getStride());
		stride = stride;

		// use GetDIBits() to get the data
		BITMAPINFO info;
		memset(&info, 0, sizeof(info));
		BITMAPINFOHEADER &header = info.bmiHeader;
		header.biSize = sizeof(info.bmiHeader);
		header.biWidth = w;
		header.biHeight = -h;
		header.biPlanes = 1;
		header.biBitCount = (WORD)bitcount;
		header.biCompression = BI_RGB;

		uint8 *data = getData();
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
	draw(hdc, toX, toY, toW, toH, fromX, fromY, getWidth() - fromX, getHeight() - fromY, op);
}

void CBitmap::draw (
                    HDC hdc,
                    int toX, int toY, int toW, int toH,
                    int fromX, int fromY, int fromW, int fromH,
                    uint op
                   ) {
	assert(m_hBitmap != NULL);
	assert(fromX >= 0 && fromX < getWidth());
	assert(fromY >= 0 && fromY < getHeight());
	assert(fromW + fromX <= getWidth() && fromH + fromY <= getHeight());
	if (!m_hBitmap) {
		return;
	}

	assert(m_pTransColor == NULL || op == SRCCOPY); // when transparent, only SRCCOPY supported

	CDC mdc;
	mdc.CreateCompatibleDC(hdc);
	HBITMAP oldBitmap = mdc.SelectBitmap(m_hBitmap);
	int oldMode = ::SetStretchBltMode(hdc, HALFTONE);
	if (m_pTransColor == NULL) {
		if (toW == fromW && toH == fromH) {
			::BitBlt(hdc, toX, toY, toW, toH, mdc, fromX, fromY, op);
		} else {
			::StretchBlt(hdc, toX, toY, toW, toH, mdc, fromX, fromY, fromW, fromH, op);
		}
	} else {
		::TransparentBlt(hdc, toX, toY, toW, toH, mdc, fromX, fromY, fromW, fromH, *m_pTransColor);
	}
	::SetStretchBltMode(hdc, oldMode);
	mdc.SelectBitmap(oldBitmap);
}


UI_END
XL_END
