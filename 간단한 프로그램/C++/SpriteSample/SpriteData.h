#pragma once

class CAnimation;

class CSpriteData {

public:

	CSpriteData(DWORD animationNum, DWORD* imgNumOfAnimations);
	~CSpriteData();

	inline CAnimation* operator[](DWORD index) {
		return &_animation[index];
	}

private:

	DWORD _animationNum;
	CAnimation* _animation;


};