#ifndef XL_RESMGR_H
#define XL_RESMGR_H
#include <assert.h>
#include <map>
#include <memory>
#include <Windows.h>
#include <gdiplus.h>
#include "../common.h"
#include "../string.h"

namespace xl {
	namespace ui {

/**
 * Manage resources such as fonts, icons, cursors, and so on.
 */
class CResMgr
{
public:
	//////////////////////////////////////////////////////////////////////////
	// public typedefs
	typedef std::tr1::shared_ptr<Gdiplus::Bitmap>  GpBmpPtr;

private:
	// gdi+
	Gdiplus::GdiplusStartupInput  gdiplusStartupInput;
	ULONG_PTR                     gdiplusToken;

	CResMgr ();
	~CResMgr ();
	typedef std::map<uint, HGDIOBJ>       _GdiObjMapType;
	typedef _GdiObjMapType::iterator      _GdiObjMapIter;
	typedef std::map<uint64, HGDIOBJ>     _GdiObjBigMapType;
	typedef _GdiObjBigMapType::iterator   _GdiObjBigMapIter;

	// typedef Gdiplus::Bitmap              *_GpBmpPtr;
	typedef std::map<uint, GpBmpPtr>      _GpBmpMapType;
	typedef _GpBmpMapType::iterator       _GpBmpMapIter;

	_GdiObjMapType                        m_sysFonts;
	_GpBmpMapType                         m_gpBitmaps;

	void _Lock ();
	void _Unlock ();

	HGDIOBJ _CreateSysFont(int height, uint style);
	void _MakeBitmaGray (GpBmpPtr bitmap);

public:
	static const uint FS_BOLD = 0x01;
	static const uint FS_ITALIC = 0x02;
	static const uint FS_UNDERLINE = 0x04;
	static const uint FS_STRIKEOUT = 0x08;
	static const uint FS_MASK = 0xffff;

	static CResMgr* getInstance ();
	void reset ();


	/**
	 * Load an image (bitmap) from the resource of hInst
	 * @note CResMgr doesn't manager the return bitmap, if you want CResMgr cache it,
	 *   use getBitmap() instead.
	 */
	GpBmpPtr loadBitmapFromResource (uint id, const tstring &type, HINSTANCE hInst = NULL);


	/**
	 * @param height The height of the font
	 */
	HFONT getSysFont (int height = 0, uint style = 0);

	/**
	 * get Gdiplus::Bitmap* from process resource (not DLL)
	 */
	GpBmpPtr getBitmap (ushort id, const tstring &type, bool grayscale = false);

};

	} // namespace ui
} // namespace xl


#endif
