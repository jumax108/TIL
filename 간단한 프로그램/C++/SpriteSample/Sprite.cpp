#include <Windows.h>

#include "Image.h"
#include "Animation.h"
#include "SpriteData.h"

#include "Sprite.h"

CSprite::CSprite(CSpriteData* aniData, void (*updateFunc)(CSprite* sprite, void* argv)) {

	_aniData = new CSpriteData();
	*_aniData = *aniData;

	_x = -1;
	_y = -1;

	_isLive = false;

	_update = updateFunc;
	_currentAnimationIndex = 0;
	_oldAnimationIndex = 0;
	_seeRight = false;
	_changeAnimation = false;

	_shadow = nullptr;

	_msg = INPUT_MESSAGE::NONE;
	_oldMsg = INPUT_MESSAGE::NONE;

	_effect._doneSingleTime = true;
}

void CSprite::update(void* argv) {

	if(_update != nullptr)
		_update(this, argv);

	(*_aniData)[_currentAnimationIndex + (int)_seeRight]->update();
	if (_oldAnimationIndex == _currentAnimationIndex) {
		_changeAnimation = false;
	}
	else {
		_oldAnimationIndex = _currentAnimationIndex;
		_changeAnimation = true;
		(*_aniData)[_currentAnimationIndex + (int)_seeRight]->_doneSingleTime = false;
	}

	if (_effect._doneSingleTime == false) {
		_effect.update();
	}

}

void CSprite::create(int x, int y) {

	_x = x;
	_y = y;
	_isLive = true;

}

void CSprite::draw(float redRatio, float blueRatio, float greenRatio) {
	if (_shadow != nullptr) {
		_shadow->draw(_x, _y);
	}
	if (_hpBar != nullptr) {
		_hpBar->draw(_x, _y, (float)_nowHp / _maxHp);
	}
	if (_effect._doneSingleTime == false) {
		_effect.draw(_x, _y);
	}
	(*_aniData)[_currentAnimationIndex + (int)_seeRight]->draw(_x, _y, redRatio, blueRatio, greenRatio);


}

void CSprite::die() {
	_isLive = false;
}