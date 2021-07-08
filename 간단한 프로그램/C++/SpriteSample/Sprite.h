#pragma once

class CSpriteData;

class CSprite {

public:

	CSprite(CSpriteData* aniData, void (*updateFunc)(void* argv));

	void update(void* argv);
	void draw();

	void create(int x, int y);
	void die();
	inline bool isLive() {
		return _isLive;
	}

	inline void setAnimation(DWORD index) {
		_currentAnimationIndex = index;
	}

	CSpriteData* _aniData;

	int _x;
	int _y;
	bool _seeRight;
	bool _changeAnimation;

private:

	bool _isLive;
	DWORD _currentAnimationIndex;
	DWORD _oldAnimationIndex;

	void (*_update)(void* argv);

};