#ifndef XL_UI_WIN_STYLE_H
#define XL_UI_WIN_STYLE_H

namespace xl {
	namespace ui {

static const int SIZE_AUTO = -1;

struct SIZE {
	int cx;
	int cy;
};

enum EDGE {
	EDGE_TOP,
	EDGE_RIGHT,
	EDGE_BOTTOM,
	EDGE_LEFT,
	EDGE_COUNT
};

struct WinStyle {
protected:
	void _StyleChanged ();

public:
//	static const int 
	int margin[4];
	int padding[4];

	int width;
	int height;

	///////////////////////////////////////////////////////
	// position
	enum POSITION {
		PT_NORMAL,
		PT_RELATIVE,
		PT_ABLOLUTE,
		PT_FIXED,
		PT_COUNT
	};
	POSITION position;

	///////////////////////////////////////////////////////
	// display
	enum DISPLAY {
		DISP_BLOCK,
		DISP_INLINE,
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
	///////////////////////////////////////////////////////

	WinStyle ();
	virtual ~WinStyle ();

	void setMargin (int margin, EDGE edge = EDGE_COUNT);
	void setPadding (int padding, EDGE edge = EDGE_COUNT);

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
