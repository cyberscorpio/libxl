#ifndef XL_UI_WIN_STYLE_H
#define XL_UI_WIN_STYLE_H

namespace xl {
	namespace ui {

static const int SIZE_AUTO = -1;
static const int SIZE_FILL = -2; // fill the remain space

struct SIZE {
	int cx;
	int cy;
};

struct POINT {
	int x;
	int y;
};

enum EDGETYPE {
	ET_TOP,
	ET_RIGHT,
	ET_BOTTOM,
	ET_LEFT,
	ET_ALL,
	ET_COUNT = ET_ALL
};

struct EDGE {
	int top;
	int right;
	int bottom;
	int left;

	EDGE (int t = 0, int r = 0, int b = 0, int l = 0)
		: top (t), right (r), bottom (b), left (l) {
	}
};

struct WinStyle {
protected:
	virtual void _StyleChanged ();

public:
//	static const int 
	EDGE margin;
	EDGE padding;

	int width;
	int height;

	///////////////////////////////////////////////////////
	// position
	enum POSITION {
		PT_NORMAL,
		PT_RELATIVE,
		PT_ABLOLUTE,
		PT_COUNT
	};
	POSITION position;

	///////////////////////////////////////////////////////
	// display
	enum DISPLAY {
		DISP_BLOCK,
		DISP_LEFT,
		// DISP_CENTER, // maybe not needed
		DISP_RIGHT,
		DISP_CLEAR,
		DISP_NONE,
		DISP_COUNT
	};
	DISPLAY display;


	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////

	WinStyle ();
	virtual ~WinStyle ();

	void setMargin (int margin, EDGETYPE et = ET_ALL);
	void setPadding (int padding, EDGETYPE et = ET_ALL);

	void setWidth (int width) {
		this->width = width;
		_StyleChanged();
	}
	void setHeight (int height) {
		this->height = height;
		_StyleChanged();
	}
	void setDisplay (DISPLAY display) {
		this->display = display;
		_StyleChanged();
	}

	virtual SIZE reportSize ();
};

	} // namespace ui
} // namespace xl


#endif
