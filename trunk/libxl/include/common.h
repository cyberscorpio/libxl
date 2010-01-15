/**
 * @brief Defines common used types of libxl.
 */
#ifndef XL_COMMON_H
#define XL_COMMON_H

//////////////////////////////////////////////////////////////////////////
// typedef 
#ifdef _MSC_VER
#include <tchar.h>
#else
#error "Implement tchar.h here"
#endif

namespace xl {

typedef TCHAR tchar;

typedef unsigned int        uint;
typedef __int64             int64;
typedef unsigned __int64    uint64;

}



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

#endif

