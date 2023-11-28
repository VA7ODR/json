/*
Copyright (c) 2012-2020 James Baker

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

The official repository for this library is at https://github.com/odhinnsrunes/json

*/
#pragma once
#include <limits>
#include <cstring>
#include <string>
#include <type_traits>

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

template <class T, class U>
constexpr bool operator== (const secure_delete_allocator<T>&, const secure_delete_allocator<U>&) noexcept
{return true;}

template <class T, class U>
constexpr bool operator!= (const secure_delete_allocator<T>&, const secure_delete_allocator<U>&) noexcept
{return false;}

typedef std::basic_istringstream<char, std::char_traits<char>, secure_delete_allocator<char> > sdistringstream;
typedef std::basic_ostringstream<char, std::char_traits<char>, secure_delete_allocator<char> > sdostringstream;

typedef std::basic_string<char, std::char_traits<char>, secure_delete_allocator<char> > base_sdstring;

class sdstring : public base_sdstring
{
	public:
		using base_sdstring::basic_string;

		sdstring(const base_sdstring & in) : base_sdstring(in) {}
		sdstring(base_sdstring && in) : base_sdstring(std::move(in)) {}

		sdstring(const std::string & in) : base_sdstring(in.begin(), in.end()) {}
		sdstring(std::string && in) : base_sdstring(std::make_move_iterator(in.begin()), std::make_move_iterator(in.end())) {}
};

template <class T>
auto operator==(const T & lhs, const sdstring & rhs)
	-> std::enable_if_t<std::is_same<T, std::string>::value, bool>
{
	return (lhs == rhs.c_str());
}

template <class T>
auto operator!=(const T & lhs, const sdstring & rhs)
	-> std::enable_if_t<std::is_same<T, std::string>::value, bool>
{
	return (lhs != rhs.c_str());
}

template <class T>
auto operator==(const T & lhs, const std::string & rhs)
	-> std::enable_if_t<std::is_same<T, sdstring>::value, bool>
{
	return (lhs == rhs.c_str());
}

template <class T>
auto operator!=(const T & lhs, const std::string & rhs)
	-> std::enable_if_t<std::is_same<T, sdstring>::value, bool>
{
	return (lhs == rhs.c_str());
}

inline bool operator==(const sdstring & lhs, const char * rhs)
{
	return (lhs == rhs);
}

inline bool operator!=(const sdstring & lhs, const char * rhs)
{
	return (lhs != rhs);
}

inline bool operator==(const char * lhs, const sdstring & rhs)
{
	return (rhs == lhs);
}

inline bool operator!=(const char * lhs, const sdstring & rhs)
{
	return (rhs != lhs);
}
