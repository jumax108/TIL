
#include "framework.h"
#include "myLinkedList.h"
#include "JumpPointSearch.h"

#define abs(x) ((x) < 0 ? -(x) : (x))

CJumpPointSearch::CJumpPointSearch(int width, int height) {

	_width = width;
	_height = height;

	int mapByte = sizeof(MAP_STATE) * _width * _height;
	_map = (MAP_STATE*)malloc(mapByte);
	ZeroMemory(_map, mapByte);

	_openList = new linkedList<stNode*>();
	_closeList = new linkedList<stNode*>();

}

void CJumpPointSearch::pathFindInit() {

	_openList->push_back(new stNode(nullptr, 0, abs(_end._x - _start._x) + abs(_end._y - _start._y), new stCoord(_start._y, _start._x)));

}

void CJumpPointSearch::makeNode(stCoord* corner, stNode* parent) {

	int moveCnt = parent->_moveCnt + 1;
	int distance = abs(_end._y -  corner->_y) + abs(_end._x - corner->_x);
	_openList->push_back(new stNode(parent, moveCnt, distance, corner));
}

CJumpPointSearch::stNode* CJumpPointSearch::pathFindSingleLoop() {

	linkedList<stNode*>::iterator minIter = *findMin(_openList);
	stNode* selectNode = *minIter;
	_openList->erase(minIter);
	_closeList->push_back(selectNode);

	stCoord* coord = selectNode->_coord;

	stCoord* leftResult;
	stCoord* rightResult;
	stCoord* upResult;
	stCoord* downResult;

	stCoord* leftUpResult;
	stCoord* rightUpResult;
	stCoord* leftDownResult;
	stCoord* rightDownResult;

	switch (coord->_dir) {
		case DIRECTION::NONE:
		{
			leftResult = checkOrthogonal(DIRECTION::LEFT, coord->_y, coord->_x);
			if (leftResult != nullptr) {
				makeNode(leftResult, selectNode);
			}

			rightResult = checkOrthogonal(DIRECTION::RIGHT, coord->_y, coord->_x);
			if (rightResult != nullptr) {
				makeNode(rightResult, selectNode);
			}

			upResult = checkOrthogonal(DIRECTION::UP, coord->_y, coord->_x);
			if (upResult != nullptr) {
				makeNode(upResult, selectNode);
			}

			downResult = checkOrthogonal(DIRECTION::DOWN, coord->_y, coord->_x);
			if (downResult != nullptr) {
				makeNode(downResult, selectNode);
			}

			leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, coord->_y, coord->_x);
			if (leftUpResult != nullptr) {
				makeNode(leftUpResult, selectNode);
			}

			leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, coord->_y, coord->_x);
			if (leftDownResult != nullptr) {
				makeNode(leftDownResult, selectNode);
			}

			rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, coord->_y, coord->_x);
			if (rightUpResult != nullptr) {
				makeNode(rightUpResult, selectNode);
			}

			rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, coord->_y, coord->_x);
			if (rightDownResult != nullptr) {
				makeNode(rightDownResult, selectNode);
			}

			break;
		}
	}
	return nullptr;
}

CJumpPointSearch::stCoord* CJumpPointSearch::checkOrthogonal(DIRECTION dir, int y, int x) {

	int tyArr[4] = { 0,0,-1,1 };
	int txArr[4] = { -1,1,0,0 };

	int ty = tyArr[(int)dir];
	int tx = txArr[(int)dir];


	for (;;) {

		y += ty;
		x += tx;

		if (y == 0 || x == 0 || y + ty == _height || x + tx == _width) {
			// 맵 끝까지 감
			return nullptr;
		}

		if (*map(y, x) == MAP_STATE::WALL) {
			// 길 막힘
			return nullptr;
		}

		if (y == _end._y && x == _end._x) {
			// 목표지점 발견
			return new stCoord(y, x, dir);
		}


		if ((*map(y + tx, x + ty) == MAP_STATE::WALL && *map(y + tx + ty, x + ty + tx) == MAP_STATE::ROAD) || 
		   ((*map(y - tx, x - ty) == MAP_STATE::WALL && *map(y - tx + ty, x - ty + tx) == MAP_STATE::ROAD))) {
			// 코너
			return new stCoord(y, x, dir);
		}

	}

	return nullptr;
}

CJumpPointSearch::stCoord* CJumpPointSearch::checkDiagonal(DIRECTION dir, int y, int x) {

	int tyArr[4] = {-1, -1, 1, 1};
	int txArr[4] = {-1, 1, -1, 1};

	dir = (DIRECTION)((int)dir - (int)DIRECTION::LEFT_UP);

	int ty = tyArr[(int)dir];
	int tx = txArr[(int)dir];

	/* 
	LEFT_UP5
	RIGHT_UP
	LEFT_DOWN
	RIGHT_DOWN
	*/

	DIRECTION lineCheckDir[4][2] = {
		{DIRECTION::LEFT, DIRECTION::UP},
		{DIRECTION::RIGHT, DIRECTION::UP},
		{DIRECTION::LEFT, DIRECTION::DOWN},
		{DIRECTION::RIGHT, DIRECTION::DOWN}
	};

	for (;;) {

		y += ty;
		x += tx;

		if (y == 0 || x == 0 || y + ty == _height || x + tx == _width) {
			// 맵 끝까지 감
			return nullptr;
		}

		if (*map(y, x) == MAP_STATE::WALL) {
			// 길 막힘
			return nullptr;
		}

		if (y == _end._y && x == _end._x) {
			// 목표지점 발견
			return new stCoord(y, x, dir);
		}

		for (int lineCheckCnt = 0; lineCheckCnt < 2; ++lineCheckCnt) {
			// 대각선 이동 중 저 멀리서 코너 발견함
			stCoord* lineCheckResult = checkOrthogonal(lineCheckDir[(int)dir][lineCheckCnt], y, x);
			if (lineCheckResult != nullptr) {
				stCoord* result = new stCoord(y, x, lineCheckResult->_dir);
				delete(lineCheckResult);
				return result;
			}
		}

		if ((*map(y - ty, x) == MAP_STATE::WALL && *map(y - ty, x - tx) == MAP_STATE::ROAD) ||
			(*map(y, x - tx) == MAP_STATE::WALL && *map(y + ty, x - tx) == MAP_STATE::ROAD)) {
			// 대각선 이동 중 코너 발견함
			return new stCoord(y, x, dir);
		}

	}

	return nullptr;
}

linkedList<CJumpPointSearch::stNode*>::iterator* CJumpPointSearch::findMin(linkedList<stNode*>* list) {

	int minValue = 0x7FFFFFFF;
	linkedList<stNode*>::iterator minIter;
	for (linkedList<stNode*>::iterator iter = list->begin(); iter != list->end(); ++iter) {
		int distance = (*iter)->_distance;
		int moveCnt = (*iter)->_moveCnt;
		int fValue = distance + moveCnt;
		if (minValue > fValue) {
			minValue = fValue;
			minIter = iter;
		}
	}

	return &minIter;
}

#ifdef _WINDOWS_
void CJumpPointSearch::print(HDC hdc, int blockSize) {
	
	{
		// 선 그리기
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

		for (int heightCnt = 1; heightCnt < _height + 1; ++heightCnt) {

			int y = heightCnt * blockSize;
			MoveToEx(hdc, 0, y, nullptr);
			LineTo(hdc, _width * blockSize, y);

		}

		for (int widthCnt = 1; widthCnt < _width + 1; ++widthCnt) {

			int x = widthCnt * blockSize;
			MoveToEx(hdc, x, 0, nullptr);
			LineTo(hdc, x, _height * blockSize);

		}

		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);

	}

	{
		// 벽 그리기

		HBRUSH hBrush = CreateSolidBrush(RGB(200, 200, 200));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		for (int heightCnt = 0; heightCnt < _height; ++heightCnt) {
			for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {
				if (*map(heightCnt, widthCnt) == MAP_STATE::WALL) {
					int left = widthCnt * blockSize + 1;
					int top = heightCnt * blockSize + 1;
					int right = left + blockSize - 1;
					int bottom = top + blockSize - 1;
					Rectangle(hdc, left, top, right, bottom);
				}
			}
		}

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}

	do{
		// 시작 지점 그리기

		if (_start._x == -1 || _start._y == -1) {
			break;
		}
		HBRUSH hBrush = CreateSolidBrush(RGB(50, 200, 50));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		int left = _start._x * blockSize + 1;
		int right = left + blockSize - 1;
		int top = _start._y * blockSize + 1;
		int bottom = top + blockSize - 1;

		bool result = Rectangle(hdc, left, top, right, bottom);

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	} while (false);

	do{
		// 끝 지점 그리기

		if (_end._x == -1 || _end._y == -1) {
			break;
		}

		HBRUSH hBrush = CreateSolidBrush(RGB(200, 50, 50));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		int left = _end._x * blockSize + 1;
		int right = left + blockSize - 1;
		int top = _end._y * blockSize + 1;
		int bottom = top + blockSize - 1;

		bool result = Rectangle(hdc, left, top, right, bottom);

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	} while (false);

	{
		// 오픈 리스트 그리기

		HBRUSH hBrush = CreateSolidBrush(RGB(150, 210, 230));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		
		for (linkedList<stNode*>::iterator iter = _openList->begin(); iter != _openList->end(); ++iter) {

			stCoord* coord = (*iter)->_coord;
			int y = coord->_y;
			int x = coord->_x;

			int left = x * blockSize + 1;
			int right = left + blockSize - 1;
			int top = y * blockSize + 1;
			int bottom = top + blockSize - 1;

			bool result = Rectangle(hdc, left, top, right, bottom);
		}

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}
}
#endif