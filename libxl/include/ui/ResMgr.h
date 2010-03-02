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
	typedef std::map<uint, HFONT>                  _FontMapType;
	typedef std::map<uint64, CBitmapPtr>           _BitmapBigMapType;

	_FontMapType                                   m_sysFonts;
	_BitmapBigMapType                              m_bitmaps;
	_BitmapBigMapType                              m_transBitmaps;

	void _Lock ();
	void _Unlock ();

	HFONT _CreateSysFont(int height, uint style);

public:
	static const uint FS_BOLD = 0x01;
	static const uint FS_ITALIC = 0x02;
	static const uint FS_UNDERLINE = 0x04;
	static const uint FS_STRIKEOUT = 0x08;
	static const uint FS_MASK = 0xffff;

	static const uint64 BMP_GRAY = 0x10000000;

	static CResMgr* getInstance ();
	void reset ();

	/**
	 * @param height The height of the font
	 */
	HFONT getSysFont (int height = 0, uint style = 0);

	CBitmapPtr getBitmap (ushort id, bool grayscale = false);
	CBitmapPtr getTransBitmap (ushort id, COLORREF colorKey, bool grayscale = false);
};

UI_END
XL_END

#endif
