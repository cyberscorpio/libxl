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

typedef TCHAR tchar;



//////////////////////////////////////////////////////////////////////////
// macros
#ifndef COUNT_OF
#define COUNT_OF(x)    (sizeof(x)/sizeof((x)[0]))
#endif

#endif
