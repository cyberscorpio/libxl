#ifndef XL_RESMGR_H
#define XL_RESMGR_H
#include <assert.h>
#include <map>
#include <memory>
#include <Windows.h>
#include "../common.h"
#include "../string.h"
#include "../lockable.h"
#include "Bitmap.h"

XL_BEGIN
UI_BEGIN

/**
 * Manage resources such as fonts, icons, cursors, and so on.
 */
class CResMgr : private CUserLock
{
public:
	//////////////////////////////////////////////////////////////////////////
	// public typedefs

private:
	CResMgr ();
	~CResMgr ();
	typedef std::map<uint, HGDIOBJ>       _GdiObjMapType;
	typedef _GdiObjMapType::iterator      _GdiObjMapIter;
	typedef std::map<uint64, HGDIOBJ>     _GdiObjBigMapType;
	typedef _GdiObjBigMapType::iterator   _GdiObjBigMapIter;


	_GdiObjMapType                        m_sysFonts;

	void _Lock ();
	void _Unlock ();

	HGDIOBJ _CreateSysFont(int height, uint style);
	void _MakeBitmaGray (CBitmapPtr bitmap);

public:
	static const uint FS_BOLD = 0x01;
	static const uint FS_ITALIC = 0x02;
	static const uint FS_UNDERLINE = 0x04;
	static const uint FS_STRIKEOUT = 0x08;
	static const uint FS_MASK = 0xffff;

	static CResMgr* getInstance ();
	void reset ();



	/**
	 * @param height The height of the font
	 */
	HFONT getSysFont (int height = 0, uint style = 0);
};

UI_END
XL_END

#endif
