#pragma once

struct stImage{

	stImage(int delayFrame) {
		_delayFrame = delayFrame;
	}

	~stImage() {
		free(_idleBuf);
	}
	
	void init() {
		_currentSpendFrame = 0;
	}

	int _width;
	int _height;
	BYTE* _idleBuf;

	int _currentSpendFrame = 0;
	int _delayFrame;
};


struct stAnimation {

	stAnimation(int imageNum) {
		_imageNum = imageNum;
		_images = (stImage*)malloc(sizeof(stImage) * imageNum);
	}
	~stAnimation() {
		for (int imageCnt = 0; imageCnt < _imageNum; ++imageCnt) {
			_images[imageCnt].~stImage();
		}
		free(_images);
	}

	inline stImage* getCurrentImage() {
		return &_images[_currentImage];
	}

	inline void nextImage() {
		_currentImage = (_currentImage + 1) % _imageNum;
	}

	void init() {
		_currentImage = 0;
		for (int cntImage = 0; cntImage < _imageNum; cntImage++) {
			_images[cntImage].init();
		}
	}

	int _imageNum;
	int _currentImage = 0;
	stImage* _images;

};

struct stSprite {

	enum class STATE {
		idle = 0,
		move = 1,
		attack = 2
	};

	~stSprite() {
		delete idle;
		delete attack;
		delete move;
	}

	stAnimation* idle;
	stAnimation* attack;
	stAnimation* move;

	stAnimation* getCurrentAnimation() {
		switch (state) {
		case STATE::idle:
			return idle;
		case STATE::move:
			return move;
		case STATE::attack:
			return attack;
		}
		return nullptr;
	}

	STATE state = STATE::idle;

	int x;
	int y;

};