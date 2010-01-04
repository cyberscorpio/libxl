#ifndef XL_UI_WIN_STYLE_H
#define XL_UI_WIN_STYLE_H

namespace xl {
	namespace ui {


enum EDGE {
	EDGE_TOP,
	EDGE_RIGHT,
	EDGE_BOTTOM,
	EDGE_LEFT,
	EDGE_COUNT
};

struct WinStyle {
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
	~WinStyle ();

	void setMargin (int margin, EDGE edge = EDGE_COUNT) {
		if (edge == EDGE_COUNT) {
			for (int i = 0; i < 4; ++ i) {
				this->margin[i] = margin;
			}
		} else {
			this->margin[edge] = margin;
		}
	}

	void setPadding (int padding, EDGE edge = EDGE_COUNT) {
		if (edge == EDGE_COUNT) {
			for (int i = 0; i < 4; ++ i) {
				this->padding[i] = padding;
			}
		} else {
			this->padding[edge] = padding;
		}
	}


	void setWidth (int width) {
		this->width = width;
	}

	void setHeight (int height) {
		this->height = height;
	}

};

	} // namespace ui
} // namespace xl


#endif
