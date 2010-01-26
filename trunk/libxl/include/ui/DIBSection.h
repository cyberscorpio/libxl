#ifndef XL_UI_DIBSECTION_H
#define XL_UI_DIBSECTION_H
#include <memory>
#include <Windows.h>
#include "../common.h"

XL_BEGIN
UI_BEGIN

class CDIBSection;
typedef std::tr1::shared_ptr<CDIBSection>    CDIBSectionPtr;

class CDIBSection
	: public std::tr1::enable_shared_from_this<CDIBSection>
{
protected:
	HBITMAP m_hBitmap;
	DIBSECTION m_section;

protected:
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

	operator HBITMAP ();

	CDIBSectionPtr clone ();
	CDIBSectionPtr resize (int w, int h, int bitcount = 24, bool usefilemap = false);

	static CDIBSectionPtr createDIBSection (int w, int h, int bitcount = 24, bool usefilemap = false);
};



UI_END
XL_END


#endif
