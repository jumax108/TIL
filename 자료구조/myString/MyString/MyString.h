#pragma once

#include <malloc.h>

class MyString {
private:
	char* ptrStr;
	int length;

	static bool strcpy(char* dest, int destLenght, char const* src, int srcLength);
	static unsigned char strcmp(char const* left, int leftLength, char const* right, int rightLength);
	static bool strcat(char* dest, char const* add, int addLength);
	static char* strchr(char* str, int const length, char const find);
	static char* strstr(char* str, int const strLength, char const* find, int const findLength);
	static void strlwr(char* dest, char const* src, int srcLength);
public:

	// set "\0", not null ptr
	MyString();

	// check null character
	MyString(
		char const* src
	);

	MyString(
		char const* ptrSrc,
		int length
	);

	MyString(
		MyString* ptrString
	);

	MyString(
		int length // reserve
	);

	static int strlen(char const*); // str end is null character, same std strlen
	static int strlen(MyString const*); // return length
	int strlen(); // return my length

	// if (src len > dest len) return false;
	static bool strcpy(char* dest, int destLength, char const* src); // copy src to dest
	static bool strcpy(char* dest, int destLength, MyString const* src);
	static bool strcpy(MyString* dest, char const* src);
	static bool strcpy(MyString* dest, MyString const* src);
	bool strcpy(MyString const* src); // copy to me
	bool strcpy(char const* src); // copy to me

	// force change, like new
	const MyString* operator=(MyString const* src);
	const MyString* operator=(char const* src); // find null character

	// return value -1, 0, 1 only
	static unsigned char strcmp(char const* left, char const* right);
	static unsigned char strcmp(char const* left, MyString const* right);
	static unsigned char strcmp(MyString const* left, char const* right);
	static unsigned char strcmp(MyString const* left, MyString const* right);
	unsigned char strcmp(char const* right);
	unsigned char strcmp(MyString const* right);

	static bool strcat(char* dest, char const* add);
	static bool strcat(char* dest, MyString const* add);
	static bool strcat(MyString const* dest, char const* add);
	static bool strcat(MyString const* dest, MyString const* add);
	bool strcat(char const* add);
	bool strcat(MyString const* add);

	static char* strchr(char* str, char const find);
	static char* strchr(MyString* str, char const find);
	char* strchr(char const find);

	static char* strstr(char* str, char const* find);
	static char* strstr(char* str, MyString const* find);
	static char* strstr(MyString* str, char const* find);
	static char* strstr(MyString* str, MyString const* find);
	char* strstr(char* find);
	char* strstr(MyString* find);

	static void strlwr(char* dest, char const* str);
	static void strlwr(char* dest, MyString const* str);
	void strlwr();

	char* operator[](int& const index);
	char* operator[](int& const index) const;

	char* getStr() {
		return ptrStr;
	}
	int getLength() {
		return length;
	}
};