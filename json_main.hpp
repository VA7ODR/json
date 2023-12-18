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
/*!
 * \mainpage
 * For more information on the JSON standard, visit: http://www.json.org/
 *
 * This library is designed to seamlessly integrate JSON parsing, encoding
 * and data access into c++ in a way that looks similar to Javascript or c++ structures.
 *
 * For example:
 \code
 #include <iostream>
 #include "json.hpp"

 int main(int argc, char ** argv) {
 document doc;

 for (int i = 0; i < 10; i++) {
 doc["one"][i] = true;
 }

 std::cout << doc.write(true);

 return 0;
 }
 \endcode
 * Will produce the following JSON:
 \code
 {
 "one": [
 true,
 true,
 true,
 true,
 true,
 true,
 true,
 true,
 true,
 true
 ]
 }
 \endcode
 */

#include <string>
#include <deque>
#include <cstdint>
#include <map>
#include "SDString/sdstring.hpp"

#if defined DO_OJSON_STUFF
#include "ArbitraryOrderMap/arbitrary_order_map.hpp"
#endif

#if !defined JSON_HPP_
#if !defined JSON_NAMESPACE
#define JSON_HPP_
#define JSON_NAMESPACE json
#endif

namespace JSON_NAMESPACE
{
#if defined MYMAP
#undef MYMAP
#endif
#if defined DO_OJSON_STUFF
#define MYMAP arbitrary_order_map<sdstring, value>
#else
#define MYMAP std::map<sdstring, value, std::less<sdstring>, secure_delete_allocator<std::pair<const sdstring, value>>>
#endif

	enum JSONTypes{
		JSON_VOID = -1,
		JSON_NULL,
		JSON_BOOLEAN,
		JSON_NUMBER,
		JSON_STRING,
		JSON_ARRAY,
		JSON_OBJECT,
	};

	class instring;
	class MovingCharPointer;
	class object;
	class array;
	class document;
	class iterator;
	class reverse_iterator;

	class value
	{
		public:

			friend class object;
			friend class array;
			friend class document;
			friend class query;
			friend class iterator;
			friend class reverse_iterator;
#if defined SUPPORT_ORDERED_JSON && !defined DO_OJSON_STUFF
			friend class ojson::value;
			friend class ojson::document;
#elif defined DO_OJSON_STUFF
			friend class json::value;
			friend class json::document;
#endif
			friend void objectParse(value& ret, instring& inputString, bool* bFailed);

			friend void arrayParse(value& arr, instring& inputString, bool* bFailed);

			friend void nullParse(value& ret, instring& inputString, bool* bFailed);
			friend void valueParse(value& a, instring& inputString, bool* bFailed);
			friend void numberParse(value& ret, instring& s, bool* bFailed);
			static void swap(value& lhs, value& rhs) {
				using std::swap;
				swap(lhs.m_number, rhs.m_number);
				swap(lhs.m_boolean, rhs.m_boolean);
				swap(lhs.str, rhs.str);
				swap(lhs.myType, rhs.myType);
				swap(lhs.obj, rhs.obj);
			}

			value() : m_number(0), m_places(-1), m_boolean(false), myType(JSON_VOID), obj(nullptr), pParentObject(nullptr), pParentArray(nullptr) {}
			value(const value& V);
			value(value&& V);

#if defined SUPPORT_ORDERED_JSON && !defined DO_OJSON_STUFF
			value(const ojson::value& V);
#elif defined SUPPORT_ORDERED_JSON && defined DO_OJSON_STUFF
			value(const json::value& V);
#endif
			value(bool V) : m_number((double)V), m_places(-1), m_boolean(V), myType(JSON_BOOLEAN), obj(nullptr), pParentObject(nullptr), pParentArray(nullptr) {}
			value(const char* V);
			value(char* V);
			value(const sdstring& V) : m_number(0), m_places(-1), m_boolean(false), str(V), myType(JSON_STRING), obj(nullptr), pParentObject(nullptr), pParentArray(nullptr) {}
			value(sdstring&& V) : m_number(0), m_places(-1), m_boolean(false), str(std::move(V)), myType(JSON_STRING), obj(nullptr), pParentObject(nullptr), pParentArray(nullptr) {}
			value(const std::string& V) : m_number(0), m_places(-1), m_boolean(false), str(V), myType(JSON_STRING), obj(nullptr), pParentObject(nullptr), pParentArray(nullptr) {}
			value(object& V);
			value(array& V);

			template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
			value(T V) : m_number((double)V), m_places(-1), m_boolean(!(m_number == 0.0)), myType(JSON_NUMBER), obj(nullptr), pParentObject(nullptr), pParentArray(nullptr) {}

			~value();

			void setParentObject(object* pSetTo);

			void setParentArray(array* pSetTo);

			value& operator=(const value& V);
			value& operator=(value&& V);

			int isA() const;
			bool isA(int i) const
			{
				return (isA() == i);
			}

			bool IsVoid() { return (isA() == JSON_VOID); }
			bool IsNull() { return (isA() == JSON_NULL); }
			bool IsBoolean() { return (isA() == JSON_BOOLEAN); }
			bool IsNumber() { return (isA() == JSON_NUMBER); }
			bool IsString() { return (isA() == JSON_STRING); }
			bool IsArray() { return (isA() == JSON_ARRAY); }
			bool IsObject() { return (isA() == JSON_OBJECT); }

			value& emptyArray();
			value& emptyObject();

			value& toArray();
			value& toObject(const sdstring& key);
			value& toString();
			value& toString(int iDecimalPlaces = -1);
			value& toNumber();
			value& toBool();
			value& toNull();
			value& fixedDecimal(int iPlaces);
			int places() const { return m_places; };

			bool boolean() const;

			double number() const;

			double _double() const {
				return number();
			}

			float _float() const {
				return (float)number();
			}

			int64_t integer() const {
				return (int64_t)number();
			}

			int64_t _int64() const {
				return (int64_t)number();
			}

			uint64_t _uint64() const {
				return (uint64_t)number();
			}

			size_t _size_t() const {
				return (size_t)number();
			}

			long _long() const {
				return (long)number();
			}

			unsigned long _ulong() const {
				return (unsigned long)number();
			}

			int _int() const {
				return (int)number();
			}

			int _int32() const {
				return (int32_t)number();
			}

			uint32_t _uint32() const {
				return (uint32_t)number();
			}
			unsigned int _uint() const {
				return (unsigned int)number();
			}

			short _short() const {
				return (short)number();
			}

			unsigned short _ushort() const {
				return (unsigned short)number();
			}

			short _int16() const {
				return (int16_t)number();
			}

			short _uint16() const {
				return (uint16_t)number();
			}

			char _char() const {
				return (char)number();
			}

			unsigned char _uchar() const {
				return (unsigned char)number();
			}

			char _int8() const {
				return (int8_t)number();
			}

			char _uint8() const {
				return (uint8_t)number();
			}

			const char* c_str()
			{
				makeString(str);
				return str.c_str();
			};

			sdstring& _sdstring() {
				makeString(str);
				return str;
			}

			std::string& string() {
				makeString(str);
				return str;
			}

			value& operator[](size_t index);
			value& operator[](const sdstring& index);
			template <typename T, typename std::enable_if<std::is_same<T, value>::value>::type* = nullptr>
			value& operator[](const T & index)
			{
				assert(index.m_type == JSON_NUMBER || index.m_type == JSON_STRING && "Wrong value type used as index.");
				switch (index.m_type) {
					case JSON_STRING:
						return operator[](index.str);

					case JSON_NUMBER:
						return operator[](index._size_t());

					default:
						break;
				}
				if (debug) {
					debug("json operator[value]: of type %s used as index. Returning self: ", typeName(index.myType));
				}
				return *this;
			}

			void push_back(const value& val);				// Array
			void push_back(value&& val);					// Array
			void push_front(const value& val);				// Array

			value pop_back();								// Array
			value pop_front();								// Array

			value & front();								// Array / Object
			value & back();									// Array / Object

			void erase(size_t index);						// Array
			size_t erase(const sdstring &index);			// Object
			iterator erase(iterator &it);					// Array / Object
			iterator erase(iterator &first, iterator &last);	// Array / Object

			bool exists(size_t index);
			bool exists(const sdstring& index);

			iterator insert(size_t index, value& V);			  // Array
			iterator insert(const sdstring& index, value& V);		 // Object
			iterator insert(iterator position, const sdstring& key, value& V);		 // Object
			iterator insert(iterator position, value &V);		 // Array
			void insert(iterator position, iterator first, iterator last);  // Array / Object (position ignored unless ojson)
			void insert(iterator first, iterator last);	 // Array (append) / Object

			void resize(size_t iCount);
			void resize(size_t iCount, value &val);

			bool pruneEmptyValues();

			bool compact();

			bool empty() const; // Is array empty or object empty or string empty.  Number and booleans return false, nullptr and VOID return true.

			value& at(size_t index);

			bool operator==(const value& V) const;
			bool operator!=(const value& V) const;
			bool operator>(const value& V) const;
			bool operator<(const value& V) const;
			bool operator>=(const value& V) const;
			bool operator<=(const value& V) const;

			value operator+(const value& V) const;
			value operator-(const value& V) const;
			value operator*(const value& V) const;
			value operator/(const value& V) const;
			value operator%(const value& V) const;

			value &operator+=(const value& V);
			value &operator-=(const value& V);
			value &operator*=(const value& V);
			value &operator/=(const value& V);
			value &operator%=(const value& V);

			value operator++(int);
			value &operator++();
			value operator--(int);
			value &operator--();

			value operator-() const;

			size_t size() const;
			size_t arraySize();
			size_t length();
			void clear();
			void destroy();

			void sort(bool (*compareFunc)(value&, value&));

			value simpleSearch(value& searchFor, bool bSubStr = false);
			size_t simpleCount(value& searchFor, bool bSubStr = false);
			value merge(value& V);

			iterator begin() const;
			iterator end() const;
			reverse_iterator rbegin() const;
			reverse_iterator rend() const;
			iterator find(size_t index) const;
			iterator find(const sdstring& index) const;
			iterator find(const char* index) const;
			reverse_iterator rfind(size_t index) const;
			reverse_iterator rfind(const sdstring& index) const;
			reverse_iterator rfind(const char* index) const;

			typedef void (*DEBUGPTR)(const char *, ...);
			void debugPrint() { if (debug()) { debug()("%s\n", print(0, true).c_str()); } }
			static void setDebug(DEBUGPTR setTo) { debug() = setTo; }

			static const char* typeName(int type);
			const sdstring& key() { return m_key; }

		protected:
			sdstring &makeString(sdstring &dest)const;

			void cprint(MovingCharPointer& ptr, size_t depth = 1, bool bPretty = false);
			sdstring print(size_t depth = 0, bool bPretty = false);

			size_t psize(size_t depth, bool bPretty);

			double m_number;
			int m_places;
			bool m_boolean;
			sdstring str;
			JSONTypes myType;
			union {
				public:
					object* obj;
					array* arr;
			};
			sdstring m_key;

			object* pParentObject;
			array* pParentArray;

			friend void debugTypeChangeReal(const sdstring & func, value& oldType, value& newType);
			static DEBUGPTR& debug();

		private:
			static sdstring &makeStringFromNumber(sdstring & in, int iPlaces, double dNumber);
	};

	void numberParse(value& ret, instring& s, bool* bFailed);
	void SkipWhitespace(instring& in);

	class instring
	{
		public:
			instring(const sdstring &in) : sString(in)
			{
				itPos = sString.begin();
			}

			const char &take()
			{
				return *(itPos++);
			}

			const void skip()
			{
				++itPos;
			}

			const char &peek() const
			{
				if (itPos != sString.end()) {
					return *(itPos);
				} else {
					static char c;
					c = 0;
					return c;
				}
			}

			size_t tell() const
			{
				return itPos - sString.begin();
			}

			size_t size() const
			{
				return sString.size();
			}


			void seek(size_t newPos) {
				if (newPos < size()) {
					itPos = sString.begin() + newPos;
				}
			}

			const char *getPos() const {
				return &(*itPos);
			}

			const sdstring &Str() const
			{
				return sString;
			}

			sdstring SoFar() const
			{
				return sdstring(sString.begin(), itPos);
			}

			void UpToAndIncluding(sdstring & sRet, char c)
			{
				auto it = itPos;
				auto end = sString.end();
				bool bEscape = false;
				for(; itPos != end; ++itPos) {
					if (bEscape == false && *itPos == c) {
						++itPos;
						break;
					}
					if (*itPos == '\\') {
						if (bEscape) {
							bEscape = false;
						} else {
							bEscape = true;
						}
					} else {
						bEscape = false;
					}
				}
				if (it != itPos && itPos != end) {
					sRet.assign(it, itPos);
				} else {
					sRet.clear();
				}
			}

			void Error(const sdstring & sErrorIn)
			{
				sError = sErrorIn;
			}

			sdstring & Error()
			{
				return sError;
			}

			bool HasError()
			{
				return sError.size() > 0;
			}

		private:
			const sdstring & sString;
			sdstring::const_iterator itPos;
			sdstring sError;
	};

	typedef std::deque<value, secure_delete_allocator<value>> myVec;
	typedef MYMAP myMap;
	class object : public myMap
	{
		public:
			object() : myMap(), bNotEmpty(false), pParentArray(nullptr), pParentObject(nullptr) {}
			~object() {
			}

			object(const object& V) : myMap((myMap)V), bNotEmpty(V.bNotEmpty), pParentArray(nullptr), pParentObject(nullptr) {}

			object(object&& V) : myMap((myMap)V)
			{
				std::swap(bNotEmpty, V.bNotEmpty);
				std::swap(pParentArray, V.pParentArray);
				std::swap(pParentObject, V.pParentObject);
			}

			object(const object* V) : myMap(static_cast<myMap>(*V)), bNotEmpty(V->bNotEmpty), pParentArray(nullptr), pParentObject(nullptr) {}
#if defined SUPPORT_ORDERED_JSON && !defined DO_OJSON_STUFF
			friend class ojson::object;
			object(const ojson::object& V);
			object(const ojson::object* V);
#elif defined SUPPORT_ORDERED_JSON && defined DO_OJSON_STUFF
			friend class json::object;
			object(const json::object& V);
			object(const json::object* V);
#endif

			using myMap::size;
			using myMap::find;
			using myMap::begin;
			using myMap::end;
			using myMap::rbegin;
			using myMap::rend;
			using myMap::insert;
			using myMap::erase;
			using myMap::iterator;
			using myMap::reverse_iterator;
			using myMap::const_iterator;

			object& operator=(const object& rhs)
			{
				if (this != &rhs) {
					static_cast<myMap&>(*this) = static_cast<const myMap&>(rhs);
				}
				return *this;
			}

			bool operator==(const object & rhs) const
			{
				return static_cast<const myMap&>(*this) == static_cast<const myMap&>(rhs);
			}

			bool operator>(const object & rhs) const
			{
				return static_cast<const myMap&>(*this) > static_cast<const myMap&>(rhs);
			}

			bool operator<(const object & rhs) const
			{
				return static_cast<const myMap&>(*this) < static_cast<const myMap&>(rhs);
			}

			bool operator>=(const object & rhs) const
			{
				return static_cast<const myMap&>(*this) >= static_cast<const myMap&>(rhs);
			}

			bool operator<=(const object & rhs) const
			{
				return static_cast<const myMap&>(*this) <= static_cast<const myMap&>(rhs);
			}

			bool operator!=(const object & rhs) const
			{
				return static_cast<const myMap&>(*this) != static_cast<const myMap&>(rhs);
			}

			value & operator[](const sdstring & key)
			{
				return static_cast<myMap&>(*this)[key];
			}

			value & operator[](sdstring && key)
			{
				return static_cast<myMap&>(*this)[std::move(key)];
			}

#if defined DO_OJSON_STUFF
			value & operator[](size_t index)
			{
				return static_cast<myMap&>(*this)[index];
			}
#endif

			bool empty() const;
			void setNotEmpty();
			void setParentArray(array * pSetTo);
			void setParentObject(object * pSetTo);
			void cprint(MovingCharPointer& ptr, size_t depth = 1, bool bPretty = false);
			size_t psize(size_t depth, bool bPretty);
			bool notEmpty() { return bNotEmpty; }
		protected:
			bool bNotEmpty;
		private:
			array* pParentArray;
			object* pParentObject;
	};

	class array : private myVec
	{
		public:
			array() : myVec(), bNotEmpty(false), pParentArray(nullptr), pParentObject(nullptr) {}
			array(size_t C) : myVec(C), bNotEmpty(true), pParentArray(nullptr), pParentObject(nullptr) {}
#if defined SUPPORT_ORDERED_JSON && !defined DO_OJSON_STUFF
			friend class ojson::array;
			array(const ojson::array& V);
			array(const ojson::array* V);
#elif defined SUPPORT_ORDERED_JSON && defined DO_OJSON_STUFF
			friend class json::array;
			array(const json::array& V);
			array(const json::array* V);
#endif
			array(const array& V)
				: myVec((myVec)V), bNotEmpty(V.bNotEmpty), pParentArray(nullptr), pParentObject(nullptr) {}

			array(array&& V)
				: myVec((myVec)V)
			{
				std::swap(bNotEmpty, V.bNotEmpty);
				std::swap(pParentArray, V.pParentArray);
				std::swap(pParentObject, V.pParentObject);
			}

			array(const array* V) : myVec(static_cast<myVec>(*V)), bNotEmpty(V->bNotEmpty), pParentArray(nullptr), pParentObject(nullptr) {}
			~array()  {}

			using myVec::size;
			using myVec::resize;
			using myVec::at;
			using myVec::begin;
			using myVec::end;
			using myVec::rbegin;
			using myVec::rend;
			using myVec::insert;
			using myVec::erase;
			using myVec::clear;
			using myVec::iterator;
			using myVec::reverse_iterator;
			using myVec::const_iterator;
			using myVec::pop_front;
			using myVec::pop_back;
			using myVec::push_front;
			using myVec::push_back;
			using myVec::emplace_front;
			using myVec::emplace_back;
			using myVec::front;
			using myVec::back;
			using myVec::operator=;

			array& operator=(const array& rhs)
			{
				if (this != &rhs) {
					static_cast<myVec&>(*this) = static_cast<const myVec&>(rhs);
				}
				return *this;
			}

			bool operator==(const array & rhs) const
			{
				return static_cast<const myVec&>(*this) == static_cast<const myVec&>(rhs);
			}

			bool operator>(const array & rhs) const
			{
				return static_cast<const myVec&>(*this) > static_cast<const myVec&>(rhs);
			}

			bool operator<(const array & rhs) const
			{
				return static_cast<const myVec&>(*this) < static_cast<const myVec&>(rhs);
			}

			bool operator>=(const array & rhs) const
			{
				return static_cast<const myVec&>(*this) >= static_cast<const myVec&>(rhs);
			}

			bool operator<=(const array & rhs) const
			{
				return static_cast<const myVec&>(*this) <= static_cast<const myVec&>(rhs);
			}

			bool operator!=(const array & rhs) const
			{
				return static_cast<const myVec&>(*this) != static_cast<const myVec&>(rhs);
			}

			bool empty() const;
			void setNotEmpty();
			void setParentArray(array * pSetTo);
			void setParentObject(object * pSetTo);
			void cprint(MovingCharPointer& ptr, size_t depth = 1, bool bPretty = false);
			size_t psize(size_t depth, bool bPretty);
			bool notEmpty() { return bNotEmpty; }
		protected:
			bool bNotEmpty;
		private:
			array* pParentArray;
			object* pParentObject;
	};

class iterator
	{
		public:
			friend class reverse_iterator;

			typedef std::input_iterator_tag  	iterator_category;
			typedef value        				value_type;
			typedef ptrdiff_t					difference_type;
			typedef value*   					pointer;
			typedef value& 						reference;
		
			iterator() : bNone(true), bIsArray(false) {}
			iterator(const myMap::iterator & it) : obj_it(it), bNone(false), bIsArray(false) {}
			iterator(const myVec::iterator & it) : arr_it(it), bNone(false), bIsArray(true) {}
			iterator(const iterator& it) : arr_it(it.arr_it), obj_it(it.obj_it), bNone(it.bNone), bIsArray(it.bIsArray), dumbRet() {}
			iterator(iterator&& it);
			iterator& operator=(const iterator& it);
			iterator& operator=(iterator&& it);

			iterator& operator++();
			iterator operator++(int);
			iterator& operator--();
			iterator operator--(int);
#if defined _WIN32
		bool operator==(const iterator& rhs) const;
		bool operator!=(const iterator& rhs) const;
#else
			bool operator==(const iterator& rhs);
			bool operator!=(const iterator& rhs);
#endif
			value& operator*();
			value key();

			bool Neither() { return bNone; } // changed to Neither because X.h defines None as 0L in the global namespace for some stupid reason.
			bool IsArray() { return bIsArray; }

			myVec::iterator & arr() { return arr_it; }
			myMap::iterator & obj() { return obj_it; }

		protected:
			friend class value;
			myVec::iterator arr_it;
			myMap::iterator obj_it;
			bool bNone;
			bool bIsArray;
			value dumbRet;
	};

	class reverse_iterator : public std::reverse_iterator<iterator>
	{
		public:
			reverse_iterator() : bNone(true), bIsArray(false) {}
			reverse_iterator(const myMap::reverse_iterator & it) : obj_it(it), bNone(false), bIsArray(false) {}
			reverse_iterator(const myVec::reverse_iterator & it) : arr_it(it), bNone(false), bIsArray(true) {}
			reverse_iterator(const myMap::iterator & it) : obj_it(myMap::reverse_iterator(it)), bNone(false), bIsArray(false) {}
			reverse_iterator(const myVec::iterator & it) : arr_it(myVec::reverse_iterator(it)), bNone(false), bIsArray(true) {}
			reverse_iterator(const reverse_iterator& it) : arr_it(it.arr_it), obj_it(it.obj_it), bNone(it.bNone), bIsArray(it.bIsArray), dumbRet() {}
			reverse_iterator(const JSON_NAMESPACE::iterator& it) : arr_it(myVec::reverse_iterator(it.arr_it)), obj_it(myMap::reverse_iterator(it.obj_it)) , bNone(it.bNone), bIsArray(it.bIsArray) {}

			reverse_iterator(reverse_iterator&& it);
			reverse_iterator & operator=(const reverse_iterator& it);
			reverse_iterator & operator=(reverse_iterator&& it);

			reverse_iterator& operator++();
			reverse_iterator operator++(int);
			reverse_iterator& operator--();
			reverse_iterator operator--(int);
		bool operator==(const reverse_iterator& rhs) const;
		bool operator!=(const reverse_iterator& rhs) const;
			value& operator*();
			value key();

			bool Neither() { return bNone; }
			bool IsArray() { return bIsArray; }

			myVec::reverse_iterator & arr() { return arr_it; }
			myMap::reverse_iterator & obj() { return obj_it; }

		private:
			myVec::reverse_iterator arr_it;
			myMap::reverse_iterator obj_it;
			bool bNone;
			bool bIsArray;
			value dumbRet;
	};

	class document : public value
	{
		public:
			friend class value;
			document()
				: value() {
				bParseSuccessful = false;
			}

			document(const value& V)
				: value(V) {
				bParseSuccessful = true;
			}

			document(const document& V);
			document(document&& V);

			document& operator=(const document& V);
			document& operator=(document&& V);

			typedef sdstring& (*PREPARSEPTR)(const sdstring& in, sdstring& out);
			typedef sdstring& (*PREWRITEPTR)(const sdstring& in, sdstring& out);
			bool parse(const sdstring& inStr, PREPARSEPTR = nullptr);
			bool parseFile(const sdstring &instr, PREPARSEPTR = nullptr);

			sdstring write(bool bPretty = false, PREWRITEPTR = nullptr);
			sdstring write(size_t iDepth, bool bPretty = false, PREWRITEPTR = nullptr);
			static sdstring write(value & val, bool bPretty = false, PREWRITEPTR preWriter = nullptr)
			{
				return write(val, 1, bPretty, preWriter);
			}
			static sdstring write(value &val, size_t iDepth, bool bPretty = false, PREWRITEPTR = nullptr);

			sdstring print(bool bPretty = false, PREWRITEPTR = nullptr)
			{
				return write(bPretty);
			}

			bool writeFile(const sdstring &inStr, bool bPretty = false, PREWRITEPTR = nullptr);

			sdstring parseResult() const
			{
				return strParseResult;
			}

			bool parseSuccessful() const
			{
				return bParseSuccessful;
			}

			template<typename T>
			document(T V) : value(V) { bParseSuccessful = true; }

			static int appendToArrayFile(const sdstring &sFile, document &atm, bool bPretty);

			const char * classInfo() { return STRINGIFY(JSON_NAMESPACE) "::" STRINGIFY(MYMAP); }

		protected:
			sdstring strParseResult;
			bool bParseSuccessful;
	};

	std::ostream& operator<<(std::ostream& S, document& doc);
	std::ostream& operator<<(std::ostream& S, value& doc);
}

#endif

