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

void CDIBSection::_ClearNoLock () {
	assert(m_hOldBitmap == INVALID_HANDLE_VALUE);
	if (m_hBitmap) {
		::DeleteObject(m_hBitmap);
		memset(&m_section, 0, sizeof(m_section));
	}
}

int CDIBSection::_GetWidthNoLock () const {
	return m_hBitmap == NULL ? -1 : m_section.dsBm.bmWidth;
}

int CDIBSection::_GetHeightNoLock () const {
	return m_hBitmap == NULL ? -1 : m_section.dsBm.bmHeight;
}

int CDIBSection::_GetBitCountsNoLock () const {
	return m_hBitmap == NULL ? -1 : m_section.dsBm.bmBitsPixel;
}

int CDIBSection::_GetStrideNoLock () const {
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

void* CDIBSection::_GetLineNoLock (int line) {
	CScopeLock lock(this);
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

void* CDIBSection::_GetDataNoLock () {
	CScopeLock lock(this);
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


void CDIBSection::_Clear () {
	CScopeLock lock(this);
	_ClearNoLock();
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
	CScopeLock lock(this);
	GdiFlush();
	_ClearNoLock();

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
	CScopeLock lock(this);
	return _GetWidthNoLock();
}

int CDIBSection::getHeight () const {
	CScopeLock lock(this);
	return _GetHeightNoLock();
}

int CDIBSection::getBitCounts () const {
	CScopeLock lock(this);
	return _GetBitCountsNoLock();
}

int CDIBSection::getStride () const {
	CScopeLock lock(this);
	return _GetStrideNoLock();
}

void* CDIBSection::getLine (int line) {
	CScopeLock lock(this);
	return _GetLineNoLock(line);
}

void* CDIBSection::getData () {
	CScopeLock lock(this);
	return _GetDataNoLock();
}

void CDIBSection::attachToDC (HDC hdc) {
	lock();
	assert(m_hBitmap != NULL);
	assert(INVALID_HANDLE_VALUE == m_hOldBitmap);
	m_hOldBitmap = (HBITMAP)::SelectObject(hdc, m_hBitmap);
	assert(INVALID_HANDLE_VALUE != m_hOldBitmap);
}

bool CDIBSection::tryAttachToDC (HDC hdc) {
	if (!tryLock()) {
		return false;
	}
	attachToDC(hdc);
	unlock();
	return true;
}

void CDIBSection::detachFromDC (HDC hdc) {
	assert(m_hBitmap != NULL);
	assert(INVALID_HANDLE_VALUE != m_hOldBitmap);
	HBITMAP bitmap = (HBITMAP)::SelectObject(hdc, m_hOldBitmap);
	m_hOldBitmap = (HBITMAP)INVALID_HANDLE_VALUE;
	assert(bitmap == m_hBitmap);
	unlock();
}


CDIBSectionPtr CDIBSection::clone () {
	CScopeLock lock(this);
	GdiFlush();
	assert(INVALID_HANDLE_VALUE == m_hOldBitmap);
	if (m_hBitmap == NULL) {
		return CDIBSectionPtr(new CDIBSection());
	}

	assert(m_section.dshSection == NULL);
	assert(_GetWidthNoLock() > 0 && _GetHeightNoLock() > 0);
	CDIBSectionPtr dib = createDIBSection(getWidth(), getHeight(), getBitCounts());
	if (dib) {
		assert(_GetWidthNoLock() == dib->getWidth());
		assert(_GetHeightNoLock() == dib->getHeight());
		assert(_GetStrideNoLock() == dib->getStride());
		void *src = _GetDataNoLock();
		void *dst = dib->getData();
		memcpy(dst, src, _GetStrideNoLock() * _GetHeightNoLock());
	}
	return CDIBSectionPtr(dib);
}

// #define USE_STRETCHBLT
CDIBSectionPtr CDIBSection::resize_ (int w, int h, RESIZE_TYPE rt, bool usefilemap) {
	CScopeLock lock(this);
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
		if (!resize(dib.get(), rt)) {
			dib.reset();
		}
#endif
	}

	return dib;
}

bool CDIBSection::resize (CDIBSection *dib, RESIZE_TYPE rt) {
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
	return engine.scale(this, dib);
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
