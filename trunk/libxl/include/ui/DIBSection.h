#ifndef XL_UI_DIBSECTION_H
#define XL_UI_DIBSECTION_H
#include <memory>
#include <Windows.h>
#include "../common.h"
#include "../lockable.h"

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
	void* _GetLineNoLock (int line);
	void* _GetDataNoLock ();

	void _Clear ();

public:
	CDIBSection ();
	virtual ~CDIBSection ();

	bool create (int w, int h, int bitcount = 24, bool usefilemap = false);
	int getWidth () const;
	int getHeight () const;
	int getBitCounts () const;
	int getStride () const;
	void* getLine (int line);
	void* getData ();

	void attachToDC (HDC hdc);
	bool tryAttachToDC (HDC hdc);
	void detachFromDC (HDC hdc);

	CDIBSectionPtr clone ();
	CDIBSectionPtr resize (int w, int h, bool usehalftone = true, int bitcount = 24, bool usefilemap = false);

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
