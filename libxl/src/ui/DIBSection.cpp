#include <assert.h>
#include <limits>
#include <memory>
#include <map>
#include "../../include/ui/DIBSection.h"
#include "../../include/ui/DIBResizer.h"
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
	assert(m_hOldBitmap == INVALID_HANDLE_VALUE);
	if (m_hBitmap) {
		::DeleteObject(m_hBitmap);
		memset(&m_section, 0, sizeof(m_section));
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
		assert(stride == (uint)m_section.dsBm.bmWidthBytes);
		return (int)stride;
	}
}

uint8* CDIBSection::getLine (int line) {
	// assert(m_hOldBitmap == INVALID_HANDLE_VALUE);
	if (m_hBitmap == NULL) {
		return NULL;
	} else {
		assert(line >= 0 && line < getHeight());
		uint8 *data = (uint8 *)m_section.dsBm.bmBits;
		assert(data != NULL);
		data += line * getStride();
		return data;
	}
}

uint8* CDIBSection::getData () {
	// assert(m_hOldBitmap == INVALID_HANDLE_VALUE);
	if (m_hBitmap == NULL) {
		assert(false);
		return NULL;
	}

	::GetObject(m_hBitmap, sizeof(m_section), &m_section);
	return m_hBitmap == NULL ? NULL : (uint8 *)m_section.dsBm.bmBits;
}


//////////////////////////////////////////////////////////////////////////
// public methods

CDIBSection::CDIBSection ()
	: m_hBitmap(NULL)
	, m_hOldBitmap((HBITMAP)INVALID_HANDLE_VALUE)
{
	dibcount ++;
}

CDIBSection::~CDIBSection () {
	_Clear();
	dibcount --;
	// XLTRACE(_T("%d DIBSection remains\n"), dibcount);
}

bool CDIBSection::create (int w, int h, int bitcount /* = 24 */, bool usefilemap /* = false */) {
	XL_PARAMETER_NOT_USED(usefilemap);
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
	bih.biBitCount = (WORD)bitcount;
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



bool CDIBSection::attachToDC (HDC hdc) {
	assert(m_hBitmap != NULL);
#ifndef _WIN64
	long v = InterlockedCompareExchange((long *)&m_hOldBitmap, 1, (long)INVALID_HANDLE_VALUE);
#else
	LONGLONG v = InterlockedCompareExchange64((LONGLONG *)&m_hOldBitmap, 1, (LONGLONG)INVALID_HANDLE_VALUE);
#endif
	if ((HANDLE)v != INVALID_HANDLE_VALUE) {
		return false;
	}

	m_hOldBitmap = (HBITMAP)::SelectObject(hdc, m_hBitmap);
	assert(INVALID_HANDLE_VALUE != m_hOldBitmap);
	return true;
}

void CDIBSection::detachFromDC (HDC hdc) {
	assert(m_hBitmap != NULL);
	assert(INVALID_HANDLE_VALUE != m_hOldBitmap);
	HBITMAP bitmap = (HBITMAP)::SelectObject(hdc, m_hOldBitmap);
	m_hOldBitmap = (HBITMAP)INVALID_HANDLE_VALUE;
	assert(bitmap == m_hBitmap);
	bitmap = NULL;
}


void CDIBSection::stretchBlt (HDC hdc, int xDest, int yDest, int nDestWidth, int nDestHeight,
                              int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwRop,
			      bool highQuality) {
	assert(false); // it seems doesn't work!!!
	assert(m_hBitmap != NULL);
	assert(getBitCounts() >= 16);

	xl::ui::CDCHandle dc(hdc);

	BITMAPINFO info;
	memset(&info, 0, sizeof(info));
	BITMAPINFOHEADER &bih = info.bmiHeader;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = getWidth();
	bih.biHeight = -getHeight();
	bih.biPlanes = 1;
	bih.biBitCount = (WORD)getBitCounts();
	bih.biCompression = BI_RGB;
	bih.biSizeImage = 0;

	void *data = getData();
	assert(data != NULL);

	int mode = highQuality ? HALFTONE : COLORONCOLOR;
	int oldMode = dc.SetStretchBltMode(mode);
	dc.StretchDIBits(xDest, yDest, nDestWidth, nDestHeight, 
		xSrc, ySrc, nSrcWidth, nSrcHeight, 
		data, &info, DIB_RGB_COLORS, 
		dwRop);
	dc.SetStretchBltMode(oldMode);
}



CDIBSectionPtr CDIBSection::clone () {
	GdiFlush();
	assert(m_hBitmap != NULL);
	if (m_hBitmap == NULL) {
		return CDIBSectionPtr();
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

// #define USE_STRETCHBLT
CDIBSectionPtr CDIBSection::cloneAndResize (int w, int h, RESIZE_TYPE rt,
                                            ILongTimeRunCallback *pCallback, bool usefilemap
                                           ) {
	GdiFlush();
	assert(m_hBitmap != NULL);
	assert(w > 0 && h > 0);

	if (getWidth() == w && getHeight() == h) {
		return clone();
	}

	CDIBSectionPtr dib = createDIBSection(w, h, getBitCounts(), usefilemap);
	if (dib) {
#ifdef USE_STRETCHBLT
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
		int oldMode = dc.SetStretchBltMode(rt != RT_FAST ? HALFTONE : COLORONCOLOR);

		dc.StretchBlt(0, 0, w, h, mdc, 0, 0, getWidth(), getHeight(), SRCCOPY);

		dc.SetStretchBltMode(oldMode);
		detachFromDC(mdc);
		dib->detachFromDC(dc);
#else
		if (!resize(dib.get(), rt, pCallback)) {
			dib.reset();
		}
#endif
	}

	return dib;
}

bool CDIBSection::resize (CDIBSection *dib, RESIZE_TYPE rt, ILongTimeRunCallback *pCallback) {
	assert(dib != NULL);

	std::auto_ptr<CGenericFilter> pFilter;
	switch (rt) {
		case RT_FAST:
			break;
		case RT_BOX:
			pFilter.reset(new CBoxFilter());
			break;
		case RT_BICUBIC:
			pFilter.reset(new CBicubicFilter());
			break;
		case RT_BILINEAR:
			pFilter.reset(new CBilinearFilter());
			break;
		case RT_BSPLINE:
			pFilter.reset(new CBSplineFilter());
			break;
		case RT_CATMULLROM:
			pFilter.reset(new CCatmullRomFilter());
			break;
		case RT_LANCZOS3:
			pFilter.reset(new CLanczos3Filter());
			break;
		default:
			assert(false);
	}

	CResizeEngine engine(pFilter.get());
	return engine.scale(this, dib, pCallback);
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
