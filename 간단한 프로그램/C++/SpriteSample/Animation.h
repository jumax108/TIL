#pragma once

class CImage;

struct CAnimation {

public:

	CAnimation(int imageNum);
	~CAnimation();

	void update();
	void draw(int x, int y);

	void loadImage(DWORD index, const WCHAR* fileName, DWORD delayFrame, int centerX, int centerY, DWORD alphaColor);

private:

	int _imageNum;
	CImage* _images;
	DWORD* _delayFrameNum;

	int _currentImageIndex;
	DWORD _currentDelayFrame;

	bool _doneSingleTime;

};

