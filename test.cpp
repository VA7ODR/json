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
#include "data.hpp"
#include <iostream>
#include <stdarg.h>

using namespace std;

void Debug(const char * format, ...)
{
	std::string s;

	size_t 	n,
			size = 100;

	bool b = false;

	va_list marker;

	while (!b) {
		s.resize(size);
		va_start(marker, format);
		n = vsnprintf((char*)s.c_str(), size, format, marker);
		va_end(marker);
		b = (n < size);
		if (n > 0 && n != (size_t)-1 && b) {
			size = n;
		} else if (n == (size_t)-1) {
			size = size * 2; // stupid nonconformant microsoft
		} else {
			size = n * 2;
		}
	}
	printf("%s", s.c_str());	
}

template <class T>
void test(const char * type)
{
	sdstring sTest("Hello");
	sTest.assign("Hello");
	sTest.append("There");
	std::string sTest2("This");
	sTest.append(sTest2);

	sTest += sTest2;
	sTest += sTest;
	sTest += "This is another test";

	cout << "sdstring Testing: " << sTest << endl;

	cout << (sTest  == "HelloThereThisThisHelloThereThisThisThis is another test" ? "Y" : "N") << std::endl;
	cout << ("HelloThereThisThisHelloThereThisThisThis is another test" == sTest ? "Y" : "N") << std::endl;
	cout << (sTest2 == sTest  ? "Y" : "N") << std::endl;
	cout << (sTest  == sTest2 ? "Y" : "N") << std::endl;
	cout << (sTest  == sTest ? "Y" : "N") << std::endl;

	cout << (sTest  != "HelloThereThisThisHelloThereThisThisThis is another test" ? "Y" : "N") << std::endl;
	cout << ("HelloThereThisThisHelloThereThisThisThis is another test" != sTest ? "Y" : "N") << std::endl;
	cout << (sTest2 != sTest  ? "Y" : "N") << std::endl;
	cout << (sTest  != sTest2 ? "Y" : "N") << std::endl;
	cout << (sTest  != sTest ? "Y" : "N") << std::endl;

	cout << sTest[3] << endl;
	cout << (sTest < sTest) << endl;
	cout << (sTest < "sTest") << endl;
	cout << (sTest > sTest) << endl;
	cout << (sTest > "sTest") << endl;

	sdstring sTest3(sTest2);
	std::string sTest4(sTest3);

	cout << (sTest3.find("te") == sdstring::npos) << endl;
	cout << (sTest3.find(sTest4) == sdstring::npos) << endl;
	cout << (sTest3.find(sTest) == sdstring::npos) << endl;
	cout << sTest3.size() << endl;

	// bStarted = true;
	cout << "Testing: " << type << endl;
	T jEnum;

	jEnum[sTest2] = sTest;
	jEnum["Array"][0] = "Value";
	sdstring mstring("String");
	jEnum[mstring] = "Value";
	jEnum["Array"][0] = "Value";

	jEnum.writeFile("test1.json", true);
	T j1;
	cout << "Parse File 1:               ";
	if (j1.parseFile("test1.json")) {
		cout << "Success!" << endl;
	} else {
		cout << "Failed!" << endl;
	}

	cout << "Compare ==:                 ";
	if (jEnum == j1) {
		cout << "Success!" << endl;
	} else {
		cout << "Failed!" << endl;
		jEnum.writeFile("jenum.json", true);
		j1.writeFile("j1.json", true);
	}

	cout << "Build                       ";
	T j2;
	for (size_t i = 0; i < 10; i++) {
		string k1 = to_string(i);
		// lpad(k1, '0', 3);
		for (size_t j = 0; j < 10; j++) {
			string k2 = to_string(j);
			// lpad(k2, '0', 3);
			for (size_t k = 0; k < 10; k++) {
				j2[k1][k2]["a"].push_back(k);
				j2[k1][k2]["b"][k] = k;
			}
		}
	}
	cout << "Done!" << endl;;

	cout << "Write File 2:               ";
	if (j2.writeFile("test2.json", true)) {
		cout << "Success!" << endl;
	} else {
		cout << "Failed!" << endl;
	}

	T j3;
	j3 = j2;
	for (auto it = j3.begin(); it != j3.end(); it++) {
		cout << " - " << (*it)["001"]["a"][2]._int() << " - " << endl;
	}
	cout << "Write File 3:               ";
	if (j3.writeFile("test3.json", true)) {
		cout << "Success!" << endl;
	} else {
		cout << "Failed!" << endl;
	}

	json::document j4;
	j4.push_back("test");
	cout << "push_back                   ";
	if (j4[0] == "test") {
		cout << "Success!" << endl;
		cout << j4[0].string() << endl;
	} else {
		cout << "Failed!" << endl;
	}

	cout << endl;
}	


std::string & lpad(std::string & in, char with, size_t length)
{
	if (length > in.size()) {
		return in.insert(0, length - in.size(), with);
	} else {
		return in;
	}
}

#define TEST(x) test<x>(#x)

int main(int, char**)
{
	TEST(json::document);
	TEST(ojson::document);
	return 0;
}
