/**
 * @brief Defines common used types of libxl.
 */
#ifndef XL_COMMON_H
#define XL_COMMON_H

#define XL_BEGIN namespace xl {
#define XL_END }

#define UI_BEGIN namespace ui {
#define UI_END }

#define DP_BEGIN namespace dp {
#define DP_END }

//////////////////////////////////////////////////////////////////////////
// typedef 
#ifdef _MSC_VER
#include <tchar.h>
#else
#error "Implement tchar.h here"
#endif

XL_BEGIN

typedef TCHAR tchar;

typedef unsigned char       uint8;
typedef unsigned int        uint;
typedef unsigned short      ushort;
typedef __int64             int64;
typedef unsigned __int64    uint64;

XL_END



//////////////////////////////////////////////////////////////////////////
// macros

// count of an array
#ifndef COUNT_OF
#define COUNT_OF(x)    (sizeof(x)/sizeof((x)[0]))
#endif

// when debugging, it is assert, otherwise, it run the expression.
#ifndef VERIFY
	#ifdef NDEBUG
		#define VERIFY(x) (x)
	#else
		#define VERIFY(x) assert(x)
	#endif
#endif

// parameter not used
#ifndef XL_PARAMETER_NOT_USED
#define XL_PARAMETER_NOT_USED(x)  (x) = (x)
#endif

#endif

