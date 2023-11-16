/*
Copyright (c) 2012-2023 James Baker

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

The official repository for this library is at https://github.com/VA7ODR/json

*/
#if !defined JSTRING_HPP
#define JSTRING_HPP

#include <limits>
#include <cstring>
#include <string>
#include <sstream>

template <class T>
struct secure_delete_allocator {
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;

	size_type max_size() const noexcept { return std::numeric_limits<size_type>::max() / sizeof(value_type); }
	secure_delete_allocator() noexcept {}
	template <class U> secure_delete_allocator (secure_delete_allocator<U> const&) noexcept {}
	value_type* allocate (std::size_t n) { value_type* p = static_cast<value_type*>(malloc(n*sizeof(value_type))); return p;}
	void deallocate (value_type* p, std::size_t n) { memset((void*)p, 0, n); free(p); }
	template <class U> struct rebind { typedef secure_delete_allocator<U> other; };
};

#if defined USE_STD_STRING
#define sdstring std::string
#else

//typedef std::basic_string<char, std::char_traits<char>, secure_delete_allocator<char> > sdstring;
typedef std::basic_istringstream<char, std::char_traits<char>, secure_delete_allocator<char> > sdistringstream;

template <class T, class U>
constexpr bool operator== (const secure_delete_allocator<T>&, const secure_delete_allocator<U>&) noexcept
{return true;}

template <class T, class U>
constexpr bool operator!= (const secure_delete_allocator<T>&, const secure_delete_allocator<U>&) noexcept
{return false;}

typedef std::basic_string<char, std::char_traits<char>, secure_delete_allocator<char> > base_sdstring;

class sdstring : public base_sdstring
{
	public:
		sdstring(const std::string & rhs) : base_sdstring(rhs.data(), rhs.size()) { }
		sdstring(std::string && rhs) : base_sdstring(std::move(rhs.data()), rhs.size()) { }

		using base_sdstring::base_sdstring;
		using base_sdstring::operator=;
};

inline sdstring operator+(const sdstring & lhs, const std::string & rhs)
{
	sdstring ret;
	ret.reserve(lhs.size() + rhs.size());
	ret.append(lhs);
	ret.append(rhs.c_str());
	return ret;
}

inline sdstring operator+(const char* lhs, const sdstring & rhs)
{
	sdstring ret(lhs);
	ret.reserve(ret.size() + rhs.size());
	ret.append(rhs);
	return ret;
}

inline sdstring operator+(const sdstring & lhs, const char* rhs)
{
	sdstring ret(lhs);
	ret.append(rhs);
	return ret;
}

inline sdstring operator+(const sdstring & lhs, const sdstring & rhs)
{
	sdstring ret;
	ret.reserve(lhs.size() + rhs.size());
	ret.append(lhs);
	ret.append(rhs);
	return ret;
}

inline std::string operator+(const std::string & lhs, const sdstring & rhs)
{
	std::string ret;
	ret.reserve(lhs.size() + rhs.size());
	ret.append(lhs);
	ret.append(rhs.c_str());
	return ret;
}

inline bool operator==(const sdstring & lhs, const char * rhs)
{
	if (rhs) {
		const char * p = rhs;
		for (auto & c : lhs) {
			if (c != *p) {
				return false;
			}
			++p;
		}
		return true;
	}
	return false;
}

inline bool operator!=(const sdstring & lhs, const char * rhs)
{
	return !(lhs == rhs);
}

inline bool operator==(const std::string & lhs, const sdstring & rhs)
{
	return (lhs == rhs.c_str());
}

inline bool operator!=(const std::string & lhs, const sdstring & rhs)
{
	return !(lhs == rhs.c_str());
}

inline bool operator==(const sdstring & lhs, const std::string & rhs)
{
	return (lhs == rhs.c_str());
}

inline bool operator!=(const sdstring & lhs, const std::string & rhs)
{
	return !(lhs == rhs.c_str());
}

inline bool operator==(const char * lhs, const sdstring & rhs)
{
	return (rhs == lhs);
}

inline bool operator!=(const char * lhs, const sdstring & rhs)
{
	return !(rhs == lhs);
}

#endif
#endif //JSTRING_HPP
