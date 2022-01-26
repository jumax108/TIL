#pragma once

#pragma pack(1)
struct stHeader{
	unsigned char code;
	unsigned short size;
	unsigned char randKey;
	unsigned char checkSum;
};
#pragma pack(0)