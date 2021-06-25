#include <stdio.h>
#include <windows.h>

struct ARGB {
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

BITMAPCOREHEADER bmpCore;
BITMAPINFOHEADER bmpInfo;
char bm[2];

ARGB** loadBitmap(const char* fileName) {

	FILE* bmpFile;
	fopen_s(&bmpFile, fileName, "rb");


	fread(&bm, sizeof(char) * 2, 1, bmpFile);
	fread(&bmpCore, sizeof(BITMAPCOREHEADER), 1, bmpFile);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, bmpFile);

	ARGB** pixels = new ARGB*[bmpInfo.biHeight];
	for (int heightCnt = 0; heightCnt < bmpInfo.biHeight; ++heightCnt) {
		pixels[heightCnt] = new ARGB[bmpInfo.biWidth];
		fread(pixels[heightCnt], sizeof(ARGB), bmpInfo.biWidth, bmpFile);
	}

	fclose(bmpFile);

	return pixels;
}

void saveBitmap(ARGB** pixels) {

	FILE* bmpFile;
	fopen_s(&bmpFile, "output.bmp", "wb");

	fwrite(&bm, sizeof(char) * 2, 1, bmpFile);
	fwrite(&bmpCore, sizeof(BITMAPCOREHEADER), 1, bmpFile);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, bmpFile);

	for (int heightCnt = 0; heightCnt < bmpInfo.biHeight; ++heightCnt) {
		fwrite(pixels[heightCnt], sizeof(ARGB), bmpInfo.biWidth, bmpFile);
	}

	fclose(bmpFile);
}

int main()
{
	ARGB** bmp1 = loadBitmap("sample.bmp");
	ARGB** bmp2 = loadBitmap("sample2.bmp");

	for (int heightCnt = 0; heightCnt < bmpInfo.biHeight; ++heightCnt) {

		for (int widthCnt = 0; widthCnt < bmpInfo.biWidth; ++widthCnt) {

			bmp1[heightCnt][widthCnt].a = bmp1[heightCnt][widthCnt].a * 0.5 + bmp2[heightCnt][widthCnt].a * 0.5;
			bmp1[heightCnt][widthCnt].r = bmp1[heightCnt][widthCnt].r * 0.5 + bmp2[heightCnt][widthCnt].r * 0.5;
			bmp1[heightCnt][widthCnt].g = bmp1[heightCnt][widthCnt].g * 0.5 + bmp2[heightCnt][widthCnt].g * 0.5;
			bmp1[heightCnt][widthCnt].b = bmp1[heightCnt][widthCnt].b * 0.5 + bmp2[heightCnt][widthCnt].b * 0.5;

		}

	}

	saveBitmap(bmp1);

}