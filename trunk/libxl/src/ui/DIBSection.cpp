#include <assert.h>
#include <limits>
#include <map>
#include "../../include/ui/DIBSection.h"
#include "../../include/ui/Gdi.h"
#include "../../include/utilities.h"

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
	CSimpleLock lock(&m_cs);
	assert(m_hOldBitmap == INVALID_HANDLE_VALUE);
	if (m_hBitmap) {
		::DeleteObject(m_hBitmap);
		memset(&m_section, 0, sizeof(m_section));
	}
}


//////////////////////////////////////////////////////////////////////////
// public methods

CDIBSection::CDIBSection ()
	: m_hBitmap(NULL)
	, m_hOldBitmap((HBITMAP)INVALID_HANDLE_VALUE)
{
	::InitializeCriticalSection(&m_cs);
	dibcount ++;
}

CDIBSection::~CDIBSection () {
	_Clear();
	dibcount --;
	::DeleteCriticalSection(&m_cs);
	// ATLTRACE(_T("%d DIBSection remains\n"), dibcount);
}

bool CDIBSection::create (int w, int h, int bitcount /* = 24 */, bool usefilemap /* = false */) {
	CSimpleLock lock(&m_cs);
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
	CSimpleLock lock(&m_cs);
	return m_hBitmap == NULL ? -1 : m_section.dsBm.bmWidth;
}

int CDIBSection::getHeight () const {
	CSimpleLock lock(&m_cs);
	return m_hBitmap == NULL ? -1 : m_section.dsBm.bmHeight;
}

int CDIBSection::getBitCounts () const {
	CSimpleLock lock(&m_cs);
	return m_hBitmap == NULL ? -1 : m_section.dsBm.bmBitsPixel;
}

int CDIBSection::getStride () const {
	CSimpleLock lock(&m_cs);
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
	CSimpleLock lock(&m_cs);
	assert(m_hOldBitmap == INVALID_HANDLE_VALUE);
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
	CSimpleLock lock(&m_cs);
	assert(m_hOldBitmap == INVALID_HANDLE_VALUE);
	if (m_hBitmap == NULL) {
		assert(false);
		return NULL;
	}

	void *p = m_section.dsBm.bmBits;
	::GetObject(m_hBitmap, sizeof(m_section), &m_section);
	assert(p == (void *)m_section.dsBm.bmBits);
	return m_hBitmap == NULL ? NULL : m_section.dsBm.bmBits;
}

void CDIBSection::attachToDC (HDC hdc) {
	::EnterCriticalSection(&m_cs);
	assert(m_hBitmap != NULL);
	assert(INVALID_HANDLE_VALUE == m_hOldBitmap);
	m_hOldBitmap = (HBITMAP)::SelectObject(hdc, m_hBitmap);
	assert(INVALID_HANDLE_VALUE != m_hOldBitmap);
}

bool CDIBSection::tryAttachToDC (HDC hdc) {
	if (!::TryEnterCriticalSection(&m_cs)) {
		return false;
	}
	attachToDC(hdc);
	::LeaveCriticalSection(&m_cs);
	return true;
}

void CDIBSection::detachFromDC (HDC hdc) {
	assert(m_hBitmap != NULL);
	assert(INVALID_HANDLE_VALUE != m_hOldBitmap);
	HBITMAP bitmap = (HBITMAP)::SelectObject(hdc, m_hOldBitmap);
	m_hOldBitmap = (HBITMAP)INVALID_HANDLE_VALUE;
	assert(bitmap == m_hBitmap);
	::LeaveCriticalSection(&m_cs);
}


CDIBSectionPtr CDIBSection::clone () {
	CSimpleLock lock(&m_cs);
	assert(INVALID_HANDLE_VALUE == m_hOldBitmap);
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

		dib->attachToDC(dc);
		CDC mdc;
		mdc.CreateCompatibleDC(dc);
		attachToDC(mdc);
		int oldMode = dc.SetStretchBltMode(usehalftone ? HALFTONE : COLORONCOLOR);

		dc.StretchBlt(0, 0, w, h, mdc, 0, 0, getWidth(), getHeight(), SRCCOPY);

		dc.SetStretchBltMode(oldMode);
		detachFromDC(mdc);
		dib->detachFromDC(dc);
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


//////////////////////////////////////////////////////////////////////////
// helper
CDIBSectionHelper::CDIBSectionHelper (CDIBSectionPtr dib, HDC hdc)
	: m_dib(dib)
	, m_hdc(hdc)
{
	assert(m_dib != NULL);
	assert(hdc != NULL);

	m_dib->attachToDC(m_hdc);
}

CDIBSectionHelper::~CDIBSectionHelper () {
	if (m_dib != NULL) {
		assert(m_hdc != NULL);
		detach();
	} else {
		assert(m_hdc == NULL);
	}
}

void CDIBSectionHelper::detach () {
	assert(m_dib != NULL);
	assert(m_hdc != NULL);

	m_dib->detachFromDC(m_hdc);
	m_dib.reset();
	m_hdc = NULL;
}



UI_END
XL_END


#ifdef RESTORE_MIN_MAX
#pragma pop_macro ("min")
#pragma pop_macro ("max")
#undef RESTORE_MIN_MAX
#endif
