#include <Windows.h>

#include "Animation.h"
#include "SpriteData.h"

#include "Sprite.h"

CSprite::CSprite(CSpriteData* aniData, void (*updateFunc)(void* argv)) {

	_aniData = aniData;

	_x = -1;
	_y = -1;

	_isLive = false;

	_update = updateFunc;
	_currentAnimationIndex = 0;
	_oldAnimationIndex = 0;
	_seeRight = false;
	_changeAnimation = false;
}

void CSprite::update(void* argv) {

	_update(argv);

	(*_aniData)[_currentAnimationIndex + (int)_seeRight]->update();
	if (_oldAnimationIndex == _currentAnimationIndex) {
		_changeAnimation = false;
	}
	else {
		_oldAnimationIndex = _currentAnimationIndex;
		_changeAnimation = true;
	}
}

void CSprite::create(int x, int y) {

	_x = x;
	_y = y;
	_isLive = true;

}

void CSprite::draw() {

	(*_aniData)[_currentAnimationIndex + (int)_seeRight]->draw(_x, _y);

}

void CSprite::die() {
	_isLive = false;
}