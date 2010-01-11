/**
 * @brief implement our xl::basic_string, derived from std::basic_string.
 * And then typedef our xl::string, xl::wstring and xl::tstring.
 */
#ifndef XL_STRING_H
#define XL_STRING_H
#include <assert.h>
#include <limits>
#include <string>
#include <vector>
#include "common.h"

#ifdef max // <windows.h> defines max & min
#undef max
#undef min
#endif

namespace xl {

template< class CharT,
          class Traits,
          class Allocator>
          class basic_string
	: public std::basic_string<CharT, Traits, Allocator>
{
protected:
	typedef std::basic_string<CharT, Traits, Allocator> _Base;

	void _TrimSingle (CharT c) {
		const CharT *s = c_str();
		const CharT *p = s;
		while (*p == c) {
			++ p;
		}
		size_t start = p - s;
		size_t end = start;
		if (*p != 0) { // not reach the end, so we begin from the end.
			p = s + length() - 1;
			while (p != s && *p == c) {
				-- p;
			}
			++ p;
			end = p - s;
			assert (end >= start);
		}
		std::swap(*this, substr(start, end - start));
	}

	void _TrimMulti (const CharT *charlist) {
		assert (charlist);
		const CharT *l = NULL;
		const CharT *s = c_str();
		const CharT *p = s;
		while (*p != NULL) {
			CharT c = *p;
			l = charlist;
			bool matched = false;
			while (*l != NULL) {
				if (*l == c) {
					matched = true;
					break;
				}
				++ l;
			}
			if (matched) {
				++ p;
			} else {
				break;
			}
		}
		// now *p is not in [charlist]
		size_t start = p - s;
		size_t end = start;
		if (*p != NULL) { // not reach the end, so we search from the end
			p = s + length() - 1;
			while (p != s) {
				CharT c = *p;
				l = charlist;
				bool matched = false;
				while (*l != NULL) {
					if (*l == c) {
						matched = true;
						break;
					}
					++ l;
				}
				if (matched) {
					-- p;
				} else {
					break;
				}
			}
			// now *p is not in [charlist]
			++ p;
			end = p - s;
			assert (end > start);
		}
		std::swap(*this, substr(start, end - start));
	}

public:
	typedef basic_string<CharT, Traits, Allocator>      MyType;

	//////////////////////////////////////////////////////////////////////////
	// constructors
	basic_string ()
		: _Base() {}
	basic_string (const MyType &_Right, size_type _Roff, size_type _Count = npos)
		: _Base(_Right, _Roff, _Count) {}
	basic_string (const MyType &_Right)
		: _Base(_Right) {}
	basic_string (const _Base &_Right, size_type _Roff, size_t _Count = npos)
		: _Base(_Right, _Roff, _Count) {}
	basic_string (const _Base &_Right)
		: _Base(_Right) {}
	basic_string (const CharT *_Ptr, size_type _Count)
		: _Base(_Ptr, _Count) {}
	basic_string (const CharT *_Ptr)
		: _Base(_Ptr) {}
	basic_string (size_type _Count, CharT _Ch)
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

	/**
	 * @brief Borrowed from PHP also.
	 * @param charlist 
	 *  if NULL, trim all ' ' from the begin and the end.
	 *  else, trim charlist[0], charlist[1]... from the begin and the end.
	 * @note pass charlist = "" is forbidden because it make no sense.
	 */
	void trim (const CharT *charlist = NULL) {
		assert (!charlist || *charlist != 0);
		if (charlist == NULL || charlist[1] == 0) { // single
			CharT c = charlist == NULL ? (CharT)' ' : charlist[0];
			_TrimSingle(c);
		} else { // many
			_TrimMulti(charlist);
		}
	}

	/**
	 * So we don't need to call c_str() every time.
	 */
	operator const CharT* () const {
		return c_str();
	}
};

//////////////////////////////////////////////////////////////////////////
//
typedef basic_string<char, std::char_traits<char>, std::allocator<char> >            string;
typedef basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >   wstring;
typedef basic_string<tchar, std::char_traits<tchar>, std::allocator<tchar> >         tstring;

//////////////////////////////////////////////////////////////////////////
// 
template <class CharT>
std::vector<basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT> > >
explode (const basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT> > &delimiter,
         const basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT> > &str, 
         int max_parts = -1
        ) {
	if (max_parts == -1) {
		max_parts = std::numeric_limits<int>::max();
	}
	-- max_parts;
	std::vector<basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT> > > ret;
	basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT> > s = str;
	int parts = 0;
	while (parts < max_parts) {
		size_t offset = s.find(delimiter);
		if (offset == std::string::npos) {
			break;
		}
		ret.push_back(s.substr(0, offset));
		s = s.substr(offset + delimiter.length());
		++ parts;
	}
	if (s.length() > 0) {
		ret.push_back(s);
	}
	return ret;
}

template <class CharT>
std::vector<basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT> > >
explode (const CharT *delimiter, const CharT *str, int max_parts = -1) {
	basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT> > d(delimiter);
	basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT> > s(str);
	return explode(d, s, max_parts);
}

template <class CharT>
std::vector<basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT> > >
explode (const CharT *delimiter, 
         const basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT> > &str,
         int max_parts = -1
        ) {
	basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT> > d(delimiter);
	return explode(d, str, max_parts);
}


}


#endif
