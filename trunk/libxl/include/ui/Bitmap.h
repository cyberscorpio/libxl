#ifndef XL_UI_BITMAP_H
#define XL_UI_BITMAP_H
#include <memory>
#include "../common.h"
#include "DIBSection.h"
XL_BEGIN
UI_BEGIN

class CBitmap;
typedef std::tr1::shared_ptr<CBitmap>  CBitmapPtr;

class CBitmap : public CDIBSection {
public:
};

UI_END
XL_END
#endif
