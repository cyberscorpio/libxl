#include <assert.h>
#include <limits>
#include "../../include/ui/DIBSection.h"

#ifdef max // <windows.h> defines max & min
#define RESTORE_MIN_MAX
#pragma push_macro ("min")
#pragma push_macro ("max")
#undef max
#undef min
#endif


XL_BEGIN
UI_BEGIN

//////////////////////////////////////////////////////////////////////////
// protected methods

void CDIBSection::_Clear () {
	if (m_hBitmap) {
		::DeleteObject(m_hBitmap);
	}
}


//////////////////////////////////////////////////////////////////////////
// public methods

CDIBSection::CDIBSection ()
	: m_hBitmap(NULL)
{

}

CDIBSection::~CDIBSection () {
	_Clear();
}

bool CDIBSection::create (int w, int h, int bitcount /* = 24 */, bool usefilemap /* = false */) {
	_Clear();

	assert(w > 0 && h > 0);
	assert(bitcount == 24 || bitcount == 32);
	assert(!usefilemap); // not support now

	BITMAPINFO info;
	memset(&info, 0, sizeof(info));
	BITMAPINFOHEADER &bih = info.bmiHeader;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = w;
	bih.biHeight = -h;
	bih.biPlanes = 1;
	bih.biBitCount = bitcount;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = 0;

	void *data = NULL;
	HDC hdc = ::GetDC(NULL);
	m_hBitmap = ::CreateDIBSection(hdc, &info, DIB_RGB_COLORS, &data, NULL, 0);
	::ReleaseDC(NULL, hdc);

	if (m_hBitmap) {
		::GetObject(m_hBitmap, sizeof(m_bitmap), &m_bitmap);
		return true;
	} else {
		return false;
	}
}

int CDIBSection::getWidth () const {
	return m_hBitmap == NULL ? -1 : m_bitmap.bmWidth;
}

int CDIBSection::getHeight () const {
	return m_hBitmap == NULL ? -1 : m_bitmap.bmHeight;
}

int CDIBSection::getBitCounts () const {
	return m_hBitmap == NULL ? -1 : m_bitmap.bmBitsPixel;
}

int CDIBSection::getStride () const {
	if (m_hBitmap == NULL) {
		return -1;
	} else {
		uint stride = (uint)m_bitmap.bmWidth * (getBitCounts() / 8);
		stride += 3;
		stride &= ~(uint)3;
		assert(stride < (uint)std::numeric_limits<int>::max());
		assert(stride == m_bitmap.bmWidthBytes);
		return (int)stride;
	}
}

void* CDIBSection::getLine (int line) {
	if (m_hBitmap == NULL) {
		return NULL;
	} else {
		assert(line >= 0 && line < getHeight());
		unsigned char *data = (unsigned char *)m_bitmap.bmBits;
		assert(data != NULL);
		data += line * getStride();
		return data;
	}
}

void* CDIBSection::getData () {
	return m_hBitmap == NULL ? NULL : m_bitmap.bmBits;
}

CDIBSection::operator HBITMAP() {
	return m_hBitmap;
}


CDIBSectionPtr CDIBSection::clone () {
	// CDIBSectionPtr dib = CreateDIBSection();
	return CDIBSectionPtr();
}


//////////////////////////////////////////////////////////////////////////
// STATIC
CDIBSectionPtr CDIBSection::createDIBSection (int w, int h, int bitcount /* = 24 */, bool usefilemap /* = false */) {
	CDIBSection *pDIB = new CDIBSection();
	if (pDIB) {
		if (!pDIB->create(w, h, bitcount, usefilemap)) {
			delete pDIB;
			pDIB = NULL;
		}
	}

	return CDIBSectionPtr(pDIB);
}



UI_END
XL_END


#ifdef RESTORE_MIN_MAX
#pragma pop_macro ("min")
#pragma pop_macro ("max")
#undef RESTORE_MIN_MAX
#endif
