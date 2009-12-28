/**
 * @brief implement our xl::basic_string, derived from std::basic_string.
 * And then typedef our xl::string, xl::wstring and xl::tstring.
 */
#ifndef XL_STRING_H
#define XL_STRING_H
#include <assert.h>
#include <limits>
#include <string>
#include "common.h"

namespace xl {

template< class CharType,
          class Traits,
          class Allocator>
          class basic_string
	: public std::basic_string<CharType, Traits, Allocator>
{
	typedef std::basic_string<CharType, Traits, Allocator> _Base;

public:
	typedef basic_string<CharType, Traits, Allocator>      MyType;

	//////////////////////////////////////////////////////////////////////////
	// constructors
	basic_string (const MyType &_Right, size_type _Roff, size_type _Count = npos)
		: _Base(_Right, _Roff, _Count) {}
	basic_string (const MyType &_Right)
		: _Base(_Right) {}
	basic_string (const _Base &_Right, size_type _Roff, size_t _Count = npos)
		: _Base(_Right, _Roff, _Count) {}
	basic_string (const _Base &_Right)
		: _Base(_Right) {}
	basic_string (const CharType *_Ptr, size_type _Count)
		: _Base(_Ptr, _Count) {}
	basic_string (const CharType *_Ptr)
		: _Base(_Ptr) {}
	basic_string (size_type _Count, CharType _Ch)
		: _Base(_Count, _Ch) {}


	//////////////////////////////////////////////////////////////////////////
	// destructor
	~basic_string() {
	}

	/**
	 * @brief A common used function for replacing 
	 */
	int replace (
	             const MyType &search,
	             const MyType &replace,
	             int count = -1
	            ) {
		assert (search.length() > 0);
		assert (count > 0 || count == -1);

		if (count == -1) {
			count = std::numeric_limits<int>::max();
		}
		if (length() < search.length()) {
			return 0;
		}

		int replaced = 0;
		size_t start = 0;
		do {
			size_t offset = find(search, start);
			if (offset == npos) {
				break;
			}
			erase(offset, search.length());
			insert(offset, replace);

			start = offset + replace.length();
			replaced ++;
		} while (replaced < count);

		return replaced;
	}

	void trim (const CharType *charlist = NULL) {
		size_t start = 0, end = 0;
		const CharType *s = c_str();
		const CharType *p = s;
		if (charlist == NULL) {
			while (*p == (CharType)' ') {
				++ p;
			}
			start = p - s;
			if (*p == 0) {
				end = start;
			} else {
				p = s + length() - 1;
				while (p != s && *p == (CharType)' ') {
					-- p;
				}
				++ p;
				end = p - s;
				assert (end >= start);
			}
		} else {
			assert (*charlist != 0);
			while (*p == (CharType)' ') {
				++ p;
			}
			start = p - s;
			if (*p == 0) {
				end = start;
			} else {
				p = s + length() - 1;
				while (p != s && *p == (CharType)' ') {
					-- p;
				}
				++ p;
				end = p - s;
				assert (end >= start);
			}
		}
		std::swap(*this, substr(start, end - start));
	}

	/**
	 * So we don't need to call c_str() every time.
	 */
	operator const CharType* () const {
		return c_str();
	}
};

//////////////////////////////////////////////////////////////////////////
//
typedef basic_string<char, std::char_traits<char>, std::allocator<char> >            string;
typedef basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >   wstring;
typedef basic_string<tchar, std::char_traits<tchar>, std::allocator<tchar> >         tstring;

}


#endif
