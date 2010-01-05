#ifndef XL_UI_WIN_STYLE_H
#define XL_UI_WIN_STYLE_H

namespace xl {
	namespace ui {

static const int SIZE_AUTO = -1;

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
};

struct WinStyle {
protected:
	void _StyleChanged ();

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
		DISP_INLINE,
		DISP_COUNT
	};
	DISPLAY display;

	///////////////////////////////////////////////////////
	// float
	enum FLOAT {
		FLOAT_NONE,
		FLOAT_LEFT,
		FLOAT_CENTER,
		FLOAT_RIGHT,
		FLOAT_COUNT
	};
	FLOAT xfloat; // float is a key word

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
