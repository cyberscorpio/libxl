#include <assert.h>
#include <limits>
#include <map>
#include "../../include/utilities.h"
#include "../../include/ui/DIBSection.h"
#include "../../include/ui/Gdi.h"

#ifdef max // <windows.h> defines max & min
#define RESTORE_MIN_MAX
#pragma push_macro ("min")
#pragma push_macro ("max")
#undef max
#undef min
#endif

namespace {
	static int dibcount = 0;
}


XL_BEGIN
UI_BEGIN

//////////////////////////////////////////////////////////////////////////
// protected methods

void CDIBSection::_Clear () {
	if (m_hBitmap) {
		::DeleteObject(m_hBitmap);
		memset(&m_section, 0, sizeof(m_section));
	}
}


//////////////////////////////////////////////////////////////////////////
// public methods

CDIBSection::CDIBSection ()
	: m_hBitmap(NULL)
{
	dibcount ++;
}

CDIBSection::~CDIBSection () {
	_Clear();
	dibcount --;
	// ATLTRACE(_T("%d DIBSection remains\n"), dibcount);
}

bool CDIBSection::create (int w, int h, int bitcount /* = 24 */, bool usefilemap /* = false */) {
	GdiFlush();
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
		::GetObject(m_hBitmap, sizeof(m_section), &m_section);
		assert(m_section.dsBm.bmWidth == w);
		assert(m_section.dsBm.bmHeight == h);
		return true;
	} else {
		return false;
	}
}

int CDIBSection::getWidth () const {
	return m_hBitmap == NULL ? -1 : m_section.dsBm.bmWidth;
}

int CDIBSection::getHeight () const {
	return m_hBitmap == NULL ? -1 : m_section.dsBm.bmHeight;
}

int CDIBSection::getBitCounts () const {
	return m_hBitmap == NULL ? -1 : m_section.dsBm.bmBitsPixel;
}

int CDIBSection::getStride () const {
	if (m_hBitmap == NULL) {
		return -1;
	} else {
		uint stride = (uint)m_section.dsBm.bmWidth * (getBitCounts() / 8);
		stride += 3;
		stride &= ~(uint)3;
		assert(stride < (uint)std::numeric_limits<int>::max());
		assert(stride == m_section.dsBm.bmWidthBytes);
		return (int)stride;
	}
}

void* CDIBSection::getLine (int line) {
	if (m_hBitmap == NULL) {
		return NULL;
	} else {
		assert(line >= 0 && line < getHeight());
		unsigned char *data = (unsigned char *)m_section.dsBm.bmBits;
		assert(data != NULL);
		data += line * getStride();
		return data;
	}
}

void* CDIBSection::getData () {
	if (m_hBitmap == NULL) {
		assert(false);
		return NULL;
	}

	void *p = m_section.dsBm.bmBits;
	::GetObject(m_hBitmap, sizeof(m_section), &m_section);
	assert(p == (void *)m_section.dsBm.bmBits);
	return m_hBitmap == NULL ? NULL : m_section.dsBm.bmBits;
}

CDIBSection::operator HBITMAP() {
	return m_hBitmap;
}


CDIBSectionPtr CDIBSection::clone () {
	GdiFlush();
	if (m_hBitmap == NULL) {
		return CDIBSectionPtr(new CDIBSection());
	}

	assert(m_section.dshSection == NULL);
	assert(getWidth() > 0 && getHeight() > 0);
	CDIBSectionPtr dib = createDIBSection(getWidth(), getHeight(), getBitCounts());
	if (dib) {
		assert(getWidth() == dib->getWidth());
		assert(getHeight() == dib->getHeight());
		assert(getStride() == dib->getStride());
		void *src = getData();
		void *dst = dib->getData();
		memcpy(dst, src, getStride() * getHeight());
	}
	return CDIBSectionPtr(dib);
}

CDIBSectionPtr CDIBSection::resize (int w, int h, bool usehalftone, int bitcount, bool usefilemap) {
	GdiFlush();
	assert(m_hBitmap != NULL);
	assert(w > 0 && h > 0);
	CDIBSectionPtr dib = createDIBSection(w, h, bitcount, usefilemap);
	if (dib) {
		assert(w == dib->getWidth());
		assert(h == dib->getHeight());
		CDC dc;
		HDC hdc = ::GetDC(NULL);
		dc.CreateCompatibleDC(hdc);
		::ReleaseDC(NULL, hdc);

		HBITMAP oldBitmap = dc.SelectBitmap(*dib);
		CDC mdc;
		mdc.CreateCompatibleDC(dc);
		HBITMAP oldMBitmap = mdc.SelectBitmap(*this);
		int oldMode = dc.SetStretchBltMode(usehalftone ? HALFTONE : COLORONCOLOR);

		dc.StretchBlt(0, 0, w, h, mdc, 0, 0, getWidth(), getHeight(), SRCCOPY);

		dc.SetStretchBltMode(oldMode);
		mdc.SelectBitmap(oldMBitmap);
		dc.SelectBitmap(oldBitmap);
	}

	return CDIBSectionPtr(dib);
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
