#ifndef XL_RESMGR_H
#define XL_RESMGR_H
#include <map>
#include <Windows.h>
#include "../common.h"

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
	typedef std::map<uint, HGDIOBJ>  _MapType;
	typedef _MapType::iterator       _MapIter;
	_MapType m_sysFonts;

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
	/**
	 * @param height The height of the font
	 */
	HFONT getSysFont (int height = 0, uint style = 0);
};

	} // namespace ui
} // namespace xl


#endif
