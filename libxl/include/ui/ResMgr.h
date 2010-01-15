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
private:
	CResMgr ();
	~CResMgr ();
	typedef std::map<uint, HGDIOBJ>       _GdiObjMapType;
	typedef _GdiObjMapType::iterator      _GdiObjMapIter;
	typedef std::map<uint64, HGDIOBJ>     _GdiObjBigMapType;
	typedef _GdiObjBigMapType::iterator   _GdiObjBigMapIter;

	// typedef std::tr1::shared_ptr<Gdiplus::Bitmap>  _GpBmpPtr;
	typedef Gdiplus::Bitmap               *_GpBmpPtr;
	typedef std::map<uint, _GpBmpPtr>              _GpBmpMapType;
	typedef _GpBmpMapType::iterator                _GpBmpMapIter;

	_GdiObjMapType                        m_sysFonts;
	_GpBmpMapType                         m_gpBitmaps;

	void _Lock ();
	void _Unlock ();

	HGDIOBJ _CreateSysFont(int height, uint style);

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

	Gdiplus::Bitmap* getBitmap (uint id, const tstring &type);
};

	} // namespace ui
} // namespace xl


#endif
