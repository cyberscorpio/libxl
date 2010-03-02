#ifndef XL_UI_DIBSECTION_H
#define XL_UI_DIBSECTION_H
#include <memory>
#include <Windows.h>
#include "../common.h"
#include "../interfaces.h"
#include "../lockable.h"
#include "DIBResizerFilter.h"

XL_BEGIN
UI_BEGIN

class CResizeEngine;
class CDIBSection;
typedef std::tr1::shared_ptr<CDIBSection>    CDIBSectionPtr;

class CDIBSection
	: public CUserLock
	, public std::tr1::enable_shared_from_this<CDIBSection>
{
	friend class CResizeEngine;
protected:
	HBITMAP                                               m_hBitmap;
	DIBSECTION                                            m_section;

	HBITMAP                                               m_hOldBitmap;

protected:
	void _ClearNoLock ();
	int _GetWidthNoLock () const;
	int _GetHeightNoLock () const;
	int _GetBitCountsNoLock () const;
	int _GetStrideNoLock () const;
	uint8* _GetLineNoLock (int line);
	uint8* _GetDataNoLock ();

	void _Clear ();

public:
	enum RESIZE_TYPE {
		RT_FAST = 0,
		RT_BOX,
		RT_BICUBIC,
		RT_BILINEAR,
		RT_BSPLINE,
		RT_CATMULLROM,
		RT_LANCZOS3,
		RT_COUNT
	};

	CDIBSection ();
	virtual ~CDIBSection ();

	bool create (int w, int h, int bitcount = 24, bool usefilemap = false);
	int getWidth () const;
	int getHeight () const;
	int getBitCounts () const;
	int getStride () const;
	uint8* getLine (int line);
	uint8* getData ();

	void attachToDC (HDC hdc);
	bool tryAttachToDC (HDC hdc);
	void detachFromDC (HDC hdc);
	// attach & detach no lock version, use them with your own risk
	void attachToDCNoLock (HDC hdc);
	void detachFromDCNoLock (HDC hdc);
	void stretchBlt (HDC hdc, int xDest, int yDest, int nDestWidth, int nDestHeight,
		int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwRop,
		bool highQuality = true);

	CDIBSectionPtr clone ();
	CDIBSectionPtr cloneAndResize (int w, int h, RESIZE_TYPE rt = RT_BOX, ILongTimeRunCallback *pCallback = NULL, bool usefilemap = false);
	bool resize (CDIBSection *dib, RESIZE_TYPE rt = RT_BOX, ILongTimeRunCallback *pCallback = NULL);

	static CDIBSectionPtr createDIBSection (int w, int h, int bitcount = 24, bool usefilemap = false);
};


//////////////////////////////////////////////////////////////////////////
// A scope helper wrap attachToDC() and detachFromDC()
class CDIBSectionHelper
{
	CDIBSectionPtr     m_dib;
	HDC                m_hdc;

public:
	CDIBSectionHelper (CDIBSectionPtr dib, HDC hdc);
	~CDIBSectionHelper ();

	void detach ();
};



UI_END
XL_END


#endif
