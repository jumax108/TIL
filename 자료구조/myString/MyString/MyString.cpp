#include "MyString.h"

MyString::MyString() {
	ptrStr = (char*)malloc(sizeof(char) * 1);
	ptrStr[0] = 0;
	length = 0;
}
MyString::MyString(char const* src) {

	int srcLength = strlen(src);
	length = srcLength;
	ptrStr = (char*)malloc(sizeof(char) * (length + 1));

	for (int chrCnt = 0; chrCnt < length; chrCnt++) {
		ptrStr[chrCnt] = src[chrCnt];
	}
	ptrStr[length] = '\0';
}
MyString::MyString(char const* ptrSrc, int length) {
	this->length = length;
	ptrStr = (char*)malloc(sizeof(char) * (length + 1));

	for (int chrCnt = 0; chrCnt < length; chrCnt++) {
		ptrStr[chrCnt] = ptrSrc[chrCnt];
	}
	ptrStr[length] = '\0';
}
MyString::MyString(MyString* ptrStr) {
	length = ptrStr->length;
	this->ptrStr = (char*)malloc(sizeof(char) * (length + 1));

	for (int chrCnt = 0; chrCnt < length; chrCnt++) {
		this->ptrStr[chrCnt] = *(*ptrStr)[chrCnt];
	}
	this->ptrStr[length] = '\0';
}
MyString::MyString(int length) {
	ptrStr = (char*)malloc(sizeof(char) * (length + 1));
	for (int chrCnt = 0; chrCnt <= length; chrCnt++)
		ptrStr[chrCnt] = '\0';
}

int MyString::strlen(char const* str) {
	int length = 0;
	while (str[length] != '\0') {
		length += 1;
	}
	return length;
}
int MyString::strlen(MyString const* str) {
	return str->length;
}
int MyString::strlen() {
	return length;
}

bool MyString::strcpy(char* dest, int destLength, char const* src, int srcLength) {
	if (destLength < srcLength) {
		return false;
	}

	for (int srcChrCnt = 0; srcChrCnt < srcLength; ++srcChrCnt) {
		dest[srcChrCnt] = src[srcChrCnt];
	}
	dest[srcLength] = '\0';

	return true;
}
bool MyString::strcpy(char* dest, int destLength, char const* src) {
	int srcLength = strlen(src);
	return strcpy(dest, destLength, src, srcLength);
}
bool MyString::strcpy(char* dest, int destLength, MyString const* src) {
	return strcpy(dest, destLength, src->ptrStr, src->length);
}
bool MyString::strcpy(MyString* dest, char const* src) {
	int srcLength = strlen(src);
	return strcpy(dest->ptrStr, dest->length, src, srcLength);
}
bool MyString::strcpy(MyString* dest, MyString const* src) {
	return strcpy(dest->ptrStr, dest->length, src->ptrStr, src->length);
}
bool MyString::strcpy(MyString const* src) {
	return strcpy(ptrStr, length, src->ptrStr, src->length);
}
bool MyString::strcpy(char const* src) {
	int srcLength = strlen(src);
	return strcpy(ptrStr, length, src, srcLength);
}

const MyString* MyString::operator=(MyString const* src) {
	free(ptrStr);
	ptrStr = (char*)malloc(sizeof(char) * src->length);
	length = src->length;
	for (int srcChrCnt = 0; srcChrCnt < src->length; srcChrCnt++) {
		ptrStr[srcChrCnt] = *(*src)[srcChrCnt];
	}

	return this;
}
const MyString* MyString::operator=(char const* src) {
	free(ptrStr);
	int srcLength = strlen(src);
	ptrStr = (char*)malloc(sizeof(char) * srcLength);
	for (int srcChrCnt = 0; srcChrCnt < srcLength; srcChrCnt++) {
		ptrStr[srcChrCnt] = src[srcChrCnt];
	}
	return this;
}

unsigned char MyString::strcmp(char const* left, int leftLength, char const* right, int rightLength) {
	if (leftLength < rightLength) {
		return -1;
	}
	else if (leftLength > rightLength) {
		return 1;
	}

	for (int chrCnt = 0; chrCnt < leftLength; ++chrCnt) {
		if (left[chrCnt] < right[chrCnt]) {
			return -1;
		}
		else if (left[chrCnt] > right[chrCnt]) {
			return 1;
		}
	}

	return 0;

}
unsigned char MyString::strcmp(char const* left, char const* right) {
	int leftLength = strlen(left);
	int rightLength = strlen(right);

	return strcmp(left, leftLength, right, rightLength);
}
unsigned char MyString::strcmp(char const* left, MyString const* right) {
	int leftLength = strlen(left);
	return strcmp(left, leftLength, right->ptrStr, right->length);
}
unsigned char MyString::strcmp(MyString const* left, char const* right) {
	int rightLength = strlen(right);
	return strcmp(left->ptrStr, left->length, right, rightLength);
}
unsigned char MyString::strcmp(MyString const* left, MyString const* right) {
	return strcmp(left->ptrStr, left->length,
		right->ptrStr, right->length);
}
unsigned char MyString::strcmp(char const* right) {
	int rightLength = strlen(right);
	return strcmp(ptrStr, length, right, rightLength);
}
unsigned char MyString::strcmp(MyString const* right) {
	return strcmp(ptrStr, length, right->ptrStr, right->length);
}

bool MyString::strcat(char* dest, char const* add, int addLength) {

	int destLength = strlen(dest);

	for (int chrCnt = 0; chrCnt < addLength; ++chrCnt) {
		dest[chrCnt + destLength] = add[chrCnt];
	}

	return true;
}
bool MyString::strcat(char* dest, char const* add) {
	int addLength = strlen(add);

	return strcat(dest, add, addLength);
}
bool MyString::strcat(char* dest, MyString const* add) {
	int destLength = strlen(dest);
	return strcat(dest, add->ptrStr, add->length);
}
bool MyString::strcat(MyString const* dest, char const* add) {
	int addLength = strlen(add);
	return strcat(dest->ptrStr, add, addLength);
}
bool MyString::strcat(MyString const* dest, MyString const* add) {
	return strcat(dest->ptrStr, add->ptrStr, add->length);
}
bool MyString::strcat(char const* add) {
	int addLength = strlen(add);
	return strcat(ptrStr, add, addLength);
}
bool MyString::strcat(MyString const* add) {
	return strcat(ptrStr, add->ptrStr, add->length);
}

char* MyString::strchr(char* str, int const length, char const find) {

	for (int chrCnt = 0; chrCnt < length; ++chrCnt) {
		if (str[chrCnt] == find) {
			return &(str[chrCnt]);
		}
	}

	return nullptr;
}
char* MyString::strchr(char* str, char const find) {
	int strLength = strlen(str);
	return strchr(str, strLength, find);
}
char* MyString::strchr(MyString* str, char const find) {
	return strchr(str->ptrStr, str->length, find);
}
char* MyString::strchr(char const find) {
	return strchr(ptrStr, length, find);
}

char* MyString::strstr(char* str, int const strLength, char const* find, int const findLength) {
	int findCharIdx = 0;

	for (int chrCnt = 0; chrCnt < strLength; ++chrCnt) {
		if (str[chrCnt] == find[findCharIdx]) {
			findCharIdx += 1;
			if (findCharIdx == findLength) {
				return &str[chrCnt - findLength + 1];
			}
		}
		else {
			// not same
			findCharIdx = (str[chrCnt] == find[0]);
		}
	}

	return nullptr;
}
char* MyString::strstr(char* str, char const* find) {
	int strLength = strlen(str);
	int findLength = strlen(find);
	return strstr(str, strLength, find, findLength);
}
char* MyString::strstr(char* str, MyString const* find) {
	int strLength = strlen(str);
	return strstr(str, strLength, find->ptrStr, find->length);
}
char* MyString::strstr(MyString* str, char const* find) {
	int findLength = strlen(find);
	return strstr(str->ptrStr, str->length, find, findLength);
}
char* MyString::strstr(MyString* str, MyString const* find) {
	return strstr(str->ptrStr, str->length, find->ptrStr, find->length);
}
char* MyString::strstr(char* find) {
	int findLength = strlen(find);
	return strstr(ptrStr, length, find, findLength);
}
char* MyString::strstr(MyString* find) {
	return strstr(ptrStr, length, find->ptrStr, find->length);
}

void MyString::strlwr(char* dest, char const* src, int srcLength) {
	for (int chrCnt = 0; chrCnt < srcLength; ++chrCnt) {
		if ('A' <= src[chrCnt] && src[chrCnt] <= 'Z') {
			dest[chrCnt] += 'a' - 'A';
		}

	}
}
void MyString::strlwr(char* dest, char const* src) {
	int srcLength = strlen(src);
	strlwr(dest, src, srcLength);
}
void MyString::strlwr(char* dest, MyString const* src) {
	return strlwr(dest, src->ptrStr, src->length);
}
void MyString::strlwr() {
	return strlwr(ptrStr, ptrStr, length);
}

char* MyString::operator[](int& const index) {
	return &ptrStr[index];
}
char* MyString::operator[](int& const index) const {
	return &ptrStr[index];
}