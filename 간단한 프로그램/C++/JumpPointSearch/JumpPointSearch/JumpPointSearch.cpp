
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

	int mapColorByte = sizeof(stRGB) * _width * _height;
	_mapColor = (stRGB*)malloc(mapColorByte);
	ZeroMemory(_mapColor, mapColorByte);

	_openList = new linkedList<stNode*>();
	_closeList = new linkedList<stNode*>();
	_path = new linkedList<stNode*>();
	_line = new linkedList<stNode*>();

	_start._x = -1;
	_start._y = -1;

	_end._x = -1;
	_end._y = -1;

	srand(992);

}

void CJumpPointSearch::pathFindInit() {

	int mapColorByte = sizeof(stRGB) * _width * _height;
	ZeroMemory(_mapColor, mapColorByte);
	listClear();
	_openList->push_back(new stNode(nullptr, 0, abs(_end._x - _start._x) + abs(_end._y - _start._y), new stCoord(_start._y, _start._x)));

}

bool CJumpPointSearch::isNodeInList(stCoord* coord, linkedList<stNode*>* list) {

	for (linkedList<stNode*>::iterator iter = list->begin(); iter != list->end(); ++iter) {
		stCoord* coordInList = (*iter)->_coord;
		if (coordInList->_y == coord->_y  && coordInList->_x == coord->_x) {
			return true;
		}
	}

	return false;

}

void CJumpPointSearch::makeNode(stCoord* corner, stNode* parent) {

	if (isNodeInList(corner, _openList) == true || isNodeInList(corner, _closeList) == true) {
		return;
	}

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

	if (coord->_y == _end._y && coord->_x == _end._x) {
		return selectNode;
	}

	stCoord* leftResult;
	stCoord* rightResult;
	stCoord* upResult;
	stCoord* downResult;

	stCoord* leftUpResult;
	stCoord* rightUpResult;
	stCoord* leftDownResult;
	stCoord* rightDownResult;

	int y = coord->_y;
	int x = coord->_x;

	stRGB color;
	color.red   = rand() % 200 + 55;
	color.green = rand() % 200 + 55;
	color.blue  = rand() % 200 + 55;

	switch (coord->_dir) {
		case DIRECTION::NONE:
		{
			leftResult = checkOrthogonal(DIRECTION::LEFT, y, x, &color);
			if (leftResult != nullptr) {
				makeNode(leftResult, selectNode);
			}

			rightResult = checkOrthogonal(DIRECTION::RIGHT, y, x, &color);
			if (rightResult != nullptr) {
				makeNode(rightResult, selectNode);
			}

			upResult = checkOrthogonal(DIRECTION::UP, y, x, &color);
			if (upResult != nullptr) {
				makeNode(upResult, selectNode);
			}

			downResult = checkOrthogonal(DIRECTION::DOWN, y, x, &color);
			if (downResult != nullptr) {
				makeNode(downResult, selectNode);
			}

			leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, y, x, &color);
			if (leftUpResult != nullptr) {
				makeNode(leftUpResult, selectNode);
			}

			leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, y, x, &color);
			if (leftDownResult != nullptr) {
				makeNode(leftDownResult, selectNode);
			}

			rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, y, x, &color);
			if (rightUpResult != nullptr) {
				makeNode(rightUpResult, selectNode);
			}

			rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, y, x, &color);
			if (rightDownResult != nullptr) {
				makeNode(rightDownResult, selectNode);
			}

			break;
		}
		case DIRECTION::LEFT:
		{
			leftResult = checkOrthogonal(DIRECTION::LEFT, y, x, &color);
			if (leftResult != nullptr) {
				makeNode(leftResult, selectNode);
			}

			if (y+1 < _height && *map(y+1, x) == MAP_STATE::WALL) {
				// 하단에 벽이 존재
				leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, y, x, &color);
				if (leftDownResult != nullptr) {
					makeNode(leftDownResult, selectNode);
				}
			}

			if (y - 1 >= 0 && *map(y - 1, x) == MAP_STATE::WALL) {
				// 상단에 벽 존재
				leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, y, x, &color);
				if (leftUpResult != nullptr) {
					makeNode(leftUpResult, selectNode);
				}
			}

		}
			break;

		case DIRECTION::RIGHT:
		{
			rightResult = checkOrthogonal(DIRECTION::RIGHT, y, x, &color);
			if (rightResult != nullptr) {
				makeNode(rightResult, selectNode);
			}

			if (y + 1 < _height && *map(y + 1, x) == MAP_STATE::WALL) {
				// 하단에 벽 존재
				rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, y, x, &color);
				if (rightDownResult != nullptr) {
					makeNode(rightDownResult, selectNode);
				}
			}

			if (y - 1 >= 0 && *map(y - 1, x) == MAP_STATE::WALL) {
				// 상단에 벽 존재
				rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, y, x, &color);
				if (rightUpResult != nullptr) {
					makeNode(rightUpResult, selectNode);
				}

			}
		}
		break;
		case DIRECTION::UP:
		{

			upResult = checkOrthogonal(DIRECTION::UP, y, x, &color);
			if (upResult != nullptr) {
				makeNode(upResult, selectNode);
			}

			if (x + 1 < _width && *map(y, x + 1) == MAP_STATE::WALL) {
				// 우측에 벽 존재
				rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, y, x, &color);
				if (rightUpResult != nullptr) {
					makeNode(rightUpResult, selectNode);
				}
			}

			if (x - 1 >= 0 && *map(y, x - 1) == MAP_STATE::WALL) {
				// 좌측에 벽 존재
				leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, y, x, &color);
				if (leftUpResult != nullptr) {
					makeNode(leftUpResult, selectNode);
				}
			}

		}
		break;
		case DIRECTION::DOWN:
		{
			downResult = checkOrthogonal(DIRECTION::DOWN, y, x, &color);
			if (downResult != nullptr) {
				makeNode(downResult, selectNode);
			}

			if (x + 1 < _width && *map(y, x + 1) == MAP_STATE::WALL) {
				// 우측에 벽 존재
				rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, y, x, &color);
				if (rightDownResult != nullptr) {
					makeNode(rightDownResult, selectNode);
				}
			}

			if (x - 1 >= 0 && *map(y, x - 1) == MAP_STATE::WALL) {
				//좌측에 벽 존재
				leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, y, x, &color);
				if (leftDownResult != nullptr) {
					makeNode(leftDownResult, selectNode);
				}
			}
		}
		break;
		case DIRECTION::LEFT_DOWN:
		{
			leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, y, x, &color);
			if (leftDownResult != nullptr) {
				makeNode(leftDownResult, selectNode);
			}

			leftResult = checkOrthogonal(DIRECTION::LEFT, y, x, &color);
			if (leftResult != nullptr) {
				makeNode(leftResult, selectNode);
			}

			downResult = checkOrthogonal(DIRECTION::DOWN, y, x, &color);
			if (downResult != nullptr) {
				makeNode(downResult, selectNode);
			}

			if (x + 1 < _width && *map(y, x + 1) == MAP_STATE::WALL) {
				// 우측에 벽 존재
				rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, y, x, &color);
				if (rightDownResult != nullptr) {
					makeNode(rightDownResult, selectNode);
				}
				
			}

			if (y - 1 >= 0 && *map(y - 1, x) == MAP_STATE::WALL) {
				// 상단에 벽 존재
				leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, y, x, &color);
				if (leftUpResult != nullptr) {
					makeNode(leftUpResult, selectNode);
				}
			}

		}
		break;
		case DIRECTION::LEFT_UP:
		{
			leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, y, x, &color);
			if (leftUpResult != nullptr) {
				makeNode(leftUpResult, selectNode);
			}

			leftResult = checkOrthogonal(DIRECTION::LEFT, y, x, &color);
			if (leftResult != nullptr) {
				makeNode(leftResult, selectNode);
			}

			upResult = checkOrthogonal(DIRECTION::UP, y, x, &color);
			if (upResult != nullptr) {
				makeNode(upResult, selectNode);
			}

			if (x + 1 < _width && *map(y, x + 1) == MAP_STATE::WALL) {
				// 우측에 벽 존재
				rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, y, x, &color);
				if (rightUpResult != nullptr) {
					makeNode(rightUpResult, selectNode);
				}
			}

			if (y + 1 < _height && *map(y + 1, x) == MAP_STATE::WALL) {
				// 하단에 벽 존재
				leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, y, x, &color);
				if (leftDownResult != nullptr) {
					makeNode(leftDownResult, selectNode);
				}
				
			}
		}
		break;
		case DIRECTION::RIGHT_DOWN:
		{
			rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, y, x, &color);
			if (rightDownResult != nullptr) {
				makeNode(rightDownResult, selectNode);
			}

			rightResult = checkOrthogonal(DIRECTION::RIGHT, y, x, &color);
			if (rightResult != nullptr) {
				makeNode(rightResult, selectNode);
			}

			downResult = checkOrthogonal(DIRECTION::DOWN, y, x, &color);
			if (downResult != nullptr) {
				makeNode(downResult, selectNode);
			}

			if (x - 1 >= 0 && *map(y, x - 1) == MAP_STATE::WALL) {
				// 좌측에 벽 존재
				leftDownResult = checkDiagonal(DIRECTION::LEFT_DOWN, y, x, &color);
				if (leftDownResult != nullptr) {
					makeNode(leftDownResult, selectNode);
				}
				
			}

			if (y - 1 >= 0 && *map(y - 1, x) == MAP_STATE::WALL) {
				// 상단에 벽 존재
				rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, y, x, &color);
				if (rightUpResult != nullptr) {
					makeNode(rightUpResult, selectNode);
				}
			}
		}
		break;
		case DIRECTION::RIGHT_UP:
		{
			rightUpResult = checkDiagonal(DIRECTION::RIGHT_UP, y, x, &color);
			if (rightUpResult != nullptr) {
				makeNode(rightUpResult, selectNode);
			}

			rightResult = checkOrthogonal(DIRECTION::RIGHT, y, x, &color);
			if (rightResult != nullptr) {
				makeNode(rightResult, selectNode);
			}

			upResult = checkOrthogonal(DIRECTION::UP, y, x, &color);
			if (upResult != nullptr) {
				makeNode(upResult, selectNode);
			}

			if (x - 1 >= 0 && *map(y, x - 1) == MAP_STATE::WALL) {
				// 좌측에 벽 존재
				leftUpResult = checkDiagonal(DIRECTION::LEFT_UP, y, x, &color);
				if (leftUpResult != nullptr) {
					makeNode(leftUpResult, selectNode);
				}
			}

			if (y + 1 < _height && *map(y + 1, x) == MAP_STATE::WALL) {
				//하단에 벽 존재
				rightDownResult = checkDiagonal(DIRECTION::RIGHT_DOWN, y, x, &color);
				if (rightDownResult != nullptr) {
					makeNode(rightDownResult, selectNode);
				}
			}
		}
		break;

	}

	if (_openList->empty() == false) {
		// 아직 확인할 노드가 남음
		return (stNode*)1;
	}

	return nullptr;
}

CJumpPointSearch::stCoord* CJumpPointSearch::checkOrthogonal(DIRECTION dir, int y, int x, const stRGB* color) {

	int tyArr[4] = { 0,0,-1,1 };
	int txArr[4] = { -1,1,0,0 };

	int ty = tyArr[(int)dir];
	int tx = txArr[(int)dir];


	for (;;) {

		y += ty;
		x += tx;

		if (y < 0 || x < 0 || y >= _height || x >= _width) {
			// 맵 끝까지 감
			return nullptr;
		}

		if (*map(y, x) == MAP_STATE::WALL) {
			// 길 막힘
			return nullptr;
		}

		stRGB* pMapColor = mapColor(y, x);
		pMapColor->red = color->red;
		pMapColor->blue = color->blue;
		pMapColor->green = color->green;

		if (y == _end._y && x == _end._x) {
			// 목표지점 발견
			return new stCoord(y, x, dir);
		}

		// y - ty 등 계산 예외처리를 한 번에 하면, 
		// 맵 끝자락에서 코너를 발견했을 때 아래 오류가 발견됨.

		/*
		* 아래 상황에서 코너이나 노드 아래에 대한 계산식이 맵 밖이라 break 되었음
		* 0 벽 0
		* 0 녿 0
		* --------- < 맵 끝
		*/

		// 그래서 이동 중 코너 체크를 분리함

		if (0 <= y + tx && y + tx < _height &&
			0 <= x + ty && x + ty < _width &&
			0 <= y + tx + ty && y + tx + ty < _height &&
			0 <= x + ty + tx && x + ty + tx < _width) {

			if (*map(y + tx, x + ty) == MAP_STATE::WALL && *map(y + tx + ty, x + ty + tx) == MAP_STATE::ROAD) {
				// 코너 발견
				return new stCoord(y, x, dir);
			}
		}


		if (0 <= y - tx && y - tx < _height &&
			0 <= x - ty && x - ty < _width &&
			0 <= y - tx + ty && y - tx + ty < _height &&
			0 <= x - ty + tx && x - ty + tx < _width) {

			if (*map(y - tx, x - ty) == MAP_STATE::WALL && *map(y - tx + ty, x - ty + tx) == MAP_STATE::ROAD) {
				// 코너 발견
				return new stCoord(y, x, dir);
			}
		}

	}

	return nullptr;
}

CJumpPointSearch::stCoord* CJumpPointSearch::checkDiagonal(DIRECTION dir, int y, int x, const stRGB* color) {

	int tyArr[4] = {-1, -1, 1, 1};
	int txArr[4] = {-1, 1, -1, 1};

	dir = (DIRECTION)((int)dir - (int)DIRECTION::LEFT_UP);

	int ty = tyArr[(int)dir];
	int tx = txArr[(int)dir];

	/* 
	LEFT_UP
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


		if (y < 0 || x < 0 || y >= _height || x >= _width) {
			// 맵 끝까지 감
			return nullptr;
		}

		if (*map(y, x) == MAP_STATE::WALL) {
			// 길 막힘
			return nullptr;
		}

		stRGB* pMapColor = mapColor(y, x);
		pMapColor->red = color->red;
		pMapColor->blue = color->blue;
		pMapColor->green = color->green;
		if (y == _end._y && x == _end._x) {
			// 목표지점 발견
			return new stCoord(y, x, dir);
		}

		for (int lineCheckCnt = 0; lineCheckCnt < 2; ++lineCheckCnt) {
			// 대각선 이동 중 저 멀리서 코너 발견함
			stCoord* lineCheckResult = checkOrthogonal(lineCheckDir[(int)dir][lineCheckCnt], y, x, color);
			if (lineCheckResult != nullptr) {
				stCoord* result = new stCoord(y, x, (DIRECTION)((int)dir + (int)DIRECTION::LEFT_UP));
				delete(lineCheckResult);
				return result;
			}
		}

		

		// y - ty 등 계산 예외처리를 한 번에 하면, 
		// 맵 끝자락에서 코너를 발견했을 때 아래 오류가 발견됨.

		/*
		* 아래 상황에서 코너이나 노드 아래에 대한 계산식이 맵 밖이라 break 되었음
		* 0 벽 0
		* 0 녿 0
		* --------- < 맵 끝
		*/

		// 그래서 이동 중 코너 체크를 분리함

		if (0 <= y - ty && y - ty < _height &&
			0 <= x + tx && x + tx <  _width) {
			if ((*map(y - ty, x) == MAP_STATE::WALL && *map(y - ty, x + tx) == MAP_STATE::ROAD)) {
				// 대각선 이동 중 코너 발견함
				return new stCoord(y, x, (DIRECTION)((int)dir + (int)DIRECTION::LEFT_UP));
			}
		}

		if (0 <= x - tx && x - tx < _width &&
			0 <= y + ty && y + ty < _height) {
			if (*map(y, x - tx) == MAP_STATE::WALL && *map(y + ty, x - tx) == MAP_STATE::ROAD) {
				// 대각선 이동 중 코너 발견함
				return new stCoord(y, x, (DIRECTION)((int)dir + (int)DIRECTION::LEFT_UP));
			}
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
void CJumpPointSearch::print(HDC hdc, int blockSize, iterator endNodeIter) {
	
	{
		// 선 그리기
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

		for (int heightCnt = 0; heightCnt < _height + 1; ++heightCnt) {

			int y = heightCnt * blockSize;
			MoveToEx(hdc, 0, y, nullptr);
			LineTo(hdc, _width * blockSize, y);

		}

		for (int widthCnt = 0; widthCnt < _width + 1; ++widthCnt) {

			int x = widthCnt * blockSize;
			MoveToEx(hdc, x, 0, nullptr);
			LineTo(hdc, x, _height * blockSize);

		}

		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);

	}

	{
		// 맵 색 채우기

		for (int heightCnt = 0; heightCnt < _height; ++heightCnt) {
			for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {
				stRGB* pMapColor = mapColor(heightCnt, widthCnt);
				if (pMapColor->red != 0 && pMapColor->blue != 0 && pMapColor->green != 0) {
					HBRUSH hBrush = CreateSolidBrush(RGB(pMapColor->red, pMapColor->green, pMapColor->blue));
					HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

					int left = widthCnt * blockSize + 1;
					int top = heightCnt * blockSize + 1;
					int right = left + blockSize - 1;
					int bottom = top + blockSize - 1;

					Rectangle(hdc, left, top, right, bottom);

					SelectObject(hdc, hOldBrush);
					DeleteObject(hBrush);
				}
			}
		}

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

			bool result = Ellipse(hdc, left, top, right, bottom);
		}

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}

	{
		// 클로즈 리스트 그리기
		HBRUSH hBrush = CreateSolidBrush(RGB(220, 220, 0));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		for (linkedList<stNode*>::iterator iter = _closeList->begin(); iter != _closeList->end(); ++iter) {

			stCoord* coord = (*iter)->_coord;
			int y = coord->_y;
			int x = coord->_x;

			int left = x * blockSize + 1;
			int right = left + blockSize - 1;
			int top = y * blockSize + 1;
			int bottom = top + blockSize - 1;

			bool result = Ellipse(hdc, left, top, right, bottom);
		}

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}


	do {
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

		bool result = Ellipse(hdc, left, top, right, bottom);

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	} while (false);

	do {
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

		bool result = Ellipse(hdc, left, top, right, bottom);

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	} while (false);
	{
		// 각 노드의 부모를 이어보자 !

		HPEN hPen = CreatePen(PS_SOLID, blockSize / 8, RGB(190, 110, 190));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

		if (_closeList->empty() == false) {
			for (linkedList<stNode*>::iterator iter = _closeList->begin(); iter != _closeList->end(); ++iter) {

				if ((*iter)->_parent == nullptr) {
					continue;
				}

				stCoord* coord = (*iter)->_coord;
				stCoord* parentCoord = (*iter)->_parent->_coord;

				MoveToEx(hdc, coord->_x * blockSize + blockSize / 2, coord->_y* blockSize + blockSize / 2, nullptr);
				LineTo(hdc, parentCoord->_x* blockSize + blockSize / 2, parentCoord->_y* blockSize + blockSize / 2);

			}
		}

		if (_openList->empty() == false) {
			for (linkedList<stNode*>::iterator iter = _openList->begin(); iter != _openList->end(); ++iter) {

				if ((*iter)->_parent == nullptr) {
					continue;
				}

				stCoord* coord = (*iter)->_coord;
				stCoord* parentCoord = (*iter)->_parent->_coord;

				MoveToEx(hdc, coord->_x * blockSize + blockSize / 2, coord->_y * blockSize + blockSize / 2, nullptr);
				LineTo(hdc, parentCoord->_x * blockSize + blockSize / 2, parentCoord->_y * blockSize + blockSize / 2);
			}
		}

		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);
	}

	{

		// 확정된 길을 이어봅니다 ~

		HPEN hPen = CreatePen(PS_SOLID, blockSize / 4, RGB(240, 70, 70));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

		if (endNodeIter != pathEnd()) {

			for(iterator nodeIter = _path->begin(); nodeIter != pathEnd(); ++nodeIter) {
				stNode* node = *nodeIter;

				stCoord* coord = node->_coord;
				stCoord* parentCoord = node->_parent->_coord;

				MoveToEx(hdc, coord->_x * blockSize + blockSize / 2, coord->_y * blockSize + blockSize / 2, nullptr);
				LineTo(hdc, parentCoord->_x * blockSize + blockSize / 2, parentCoord->_y * blockSize + blockSize / 2);

			}

		}

		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);

	}


}
#endif

void CJumpPointSearch::listClear() {

	for (linkedList<stNode*>::iterator iter = _openList->begin(); iter != _openList->end(); ++iter) {
		delete(*iter);
	}
	for (linkedList<stNode*>::iterator iter = _closeList->begin(); iter != _closeList->end(); ++iter) {
		delete(*iter);
	}

	_openList->clear();
	_closeList->clear();

}

CJumpPointSearch::iterator CJumpPointSearch::pathFind() {

	pathFindInit();
	void* singleLoopResult = nullptr;
	do {
		singleLoopResult = pathFindSingleLoop();
		// 결과가 1 이면 반복해야함
	} while ((int)singleLoopResult == 1);

	if (singleLoopResult == nullptr) {
		return _path->end();
	}
	
	// 못찾으면 nullptr, 찾으면 도착 지점 노드
	return makePath((stNode*)singleLoopResult);

}

void CJumpPointSearch::test(const WCHAR* fileName) {

	MAP_STATE patterns[][3][3] = {
		{
			{MAP_STATE::ROAD, MAP_STATE::ROAD, MAP_STATE::ROAD},
			{MAP_STATE::WALL, MAP_STATE::WALL, MAP_STATE::WALL},
			{MAP_STATE::ROAD, MAP_STATE::ROAD, MAP_STATE::ROAD}
		} ,
		{
			{MAP_STATE::ROAD, MAP_STATE::WALL, MAP_STATE::ROAD},
			{MAP_STATE::ROAD, MAP_STATE::WALL, MAP_STATE::ROAD},
			{MAP_STATE::ROAD, MAP_STATE::WALL, MAP_STATE::ROAD}
		}, {
			{MAP_STATE::ROAD, MAP_STATE::ROAD, MAP_STATE::ROAD},
			{MAP_STATE::WALL, MAP_STATE::WALL, MAP_STATE::ROAD},
			{MAP_STATE::ROAD, MAP_STATE::WALL, MAP_STATE::ROAD}
		}
	};

	int width = 90;
	int height = 90;

	CJumpPointSearch* jps = new CJumpPointSearch(width, height);

	int patternCnt = 0;
	for (int heightCnt = 0; heightCnt < height; heightCnt += 3) {

		memcpy(jps->map(heightCnt, 0), patterns[patternCnt][0], sizeof(MAP_STATE) * 3);
		memcpy(jps->map(heightCnt + 1, 0), patterns[patternCnt][1], sizeof(MAP_STATE) * 3);
		memcpy(jps->map(heightCnt + 2, 0), patterns[patternCnt][2], sizeof(MAP_STATE) * 3);

		patternCnt = 1 - patternCnt;

		memcpy(jps->map(heightCnt, width - 3), patterns[patternCnt][0], sizeof(MAP_STATE) * 3);
		memcpy(jps->map(heightCnt + 1, width - 3), patterns[patternCnt][1], sizeof(MAP_STATE) * 3);
		memcpy(jps->map(heightCnt + 2, width - 3), patterns[patternCnt][2], sizeof(MAP_STATE) * 3);

		for (int widthCnt = 3; widthCnt < width - 3; widthCnt += 3) {

			int randPatternIndex = rand() % 3;

			memcpy(jps->map(heightCnt, widthCnt), patterns[randPatternIndex][0], sizeof(MAP_STATE) * 3);
			memcpy(jps->map(heightCnt + 1, widthCnt), patterns[randPatternIndex][1], sizeof(MAP_STATE) * 3);
			memcpy(jps->map(heightCnt + 2, widthCnt), patterns[randPatternIndex][2], sizeof(MAP_STATE) * 3);

		}

	}

	jps->_start._x = 0;
	jps->_start._y = 0;

	jps->_end._x = width - 1;
	jps->_end._y = height - 1;

	iterator findResultIter = jps->pathFind();
	if (findResultIter == jps->pathEnd()) {
		MessageBoxW(NULL, L"길 찾기 실패", L"JumpPointSearch", MB_OK);
	}

	jps->printToBitmap(fileName, 20, findResultIter);

}

void CJumpPointSearch::printToBitmap(const WCHAR* fileName, const int printRatio, iterator endNodeIter) {

	BITMAPINFOHEADER info;
	ZeroMemory(&info, sizeof(BITMAPINFOHEADER));

	info.biWidth = _width * printRatio;
	info.biHeight = _height * printRatio;
	info.biPlanes = 1;
	info.biBitCount = 24;
	info.biSize = sizeof(BITMAPINFOHEADER);

	BITMAPFILEHEADER header;
	ZeroMemory(&header, sizeof(BITMAPFILEHEADER));

	header.bfType = 0x4d42;
	header.bfSize = _width * printRatio * _height * printRatio * 3;
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	FILE* bitmapFile;
	_wfopen_s(&bitmapFile, fileName, L"w");

	fwrite(&header, sizeof(BITMAPFILEHEADER), 1, bitmapFile);
	fwrite(&info, sizeof(BITMAPINFOHEADER), 1, bitmapFile);

	stReverseRGB* rgbData = (stReverseRGB*)malloc(sizeof(stReverseRGB) * _width * printRatio * _height * printRatio);
	
	stReverseRGB* singleRgb = rgbData;

	for (int heightCnt = _height - 1; heightCnt >= 0; --heightCnt) {
		for (int heightRatioCnt = 0; heightRatioCnt < printRatio; ++heightRatioCnt) {
			for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {
				for (int widthRatioCnt = 0; widthRatioCnt < printRatio; ++widthRatioCnt) {


					switch (*map(heightCnt, widthCnt)) {
					case MAP_STATE::ROAD:
						singleRgb->red = 43;
						singleRgb->green = 43;
						singleRgb->blue = 43;
						break;
					case MAP_STATE::WALL:
						singleRgb->red = 200;
						singleRgb->green = 200;
						singleRgb->blue = 200;
						break;
					}

					singleRgb += 1;
				}
			}
		}
	}

	singleRgb = rgbData;
	// 탐색 경로 출력
	for (int heightCnt = _height - 1; heightCnt >= 0; --heightCnt) {
		for (int heightRatioCnt = 0; heightRatioCnt < printRatio; ++heightRatioCnt) {
			for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {
				for (int widthRatioCnt = 0; widthRatioCnt < printRatio; ++widthRatioCnt, singleRgb += 1) {

					stRGB* mapRgb = mapColor(heightCnt, widthCnt);
					if (mapRgb->red == 0 && mapRgb->blue == 0 && mapRgb->green == 0) {
						continue;
					}

					singleRgb->red = mapRgb->red;
					singleRgb->blue = mapRgb->blue;
					singleRgb->green = mapRgb->green;

				}
			}
		}
	}

	// 클로즈 리스트 노드 출력

	for (linkedList<stNode*>::iterator iter = _closeList->begin(); iter != _closeList->end(); ++iter) {
		int nodeX = (*iter)->_coord->_x * printRatio;
		int nodeY = (_height-1 - (*iter)->_coord->_y) * printRatio;

		for (int heightRatioCnt = 1; heightRatioCnt <= printRatio/2; ++heightRatioCnt) {
			nodeX = (*iter)->_coord->_x * printRatio + printRatio / 2 - heightRatioCnt;
			for (int widthRatioCnt = 0; widthRatioCnt < heightRatioCnt * 2 - 1; ++widthRatioCnt) {
				stReverseRGB* rgb = &rgbData[(nodeY + heightRatioCnt) * (_width * printRatio) + nodeX + widthRatioCnt];
				rgb->red = 220;
				rgb->green = 220;
				rgb->blue = 0;
			}
		}

		nodeY += printRatio / 2;
		
		for (int heightRatioCnt = 1; heightRatioCnt <= printRatio/2; ++heightRatioCnt) {
			nodeX = (*iter)->_coord->_x * printRatio + heightRatioCnt;
			for (int widthRatioCnt = 0; widthRatioCnt < (printRatio/2-1-heightRatioCnt) * 2 - 1; ++widthRatioCnt) {
				stReverseRGB* rgb = &rgbData[(nodeY + heightRatioCnt) * (_width * printRatio) + nodeX + widthRatioCnt];
				rgb->red = 220;
				rgb->green = 220;
				rgb->blue = 0;
			}
		}

	}

	// 오픈 리스트 노드 출력

	for (linkedList<stNode*>::iterator iter = _openList->begin(); iter != _openList->end(); ++iter) {
		int nodeX = (*iter)->_coord->_x * printRatio;
		int nodeY = (_height - 1 - (*iter)->_coord->_y) * printRatio;

		for (int heightRatioCnt = 1; heightRatioCnt <= printRatio / 2; ++heightRatioCnt) {
			nodeX = (*iter)->_coord->_x * printRatio + printRatio / 2 - heightRatioCnt;
			for (int widthRatioCnt = 0; widthRatioCnt < heightRatioCnt * 2 - 1; ++widthRatioCnt) {
				stReverseRGB* rgb = &rgbData[(nodeY + heightRatioCnt) * (_width * printRatio) + nodeX + widthRatioCnt];
				rgb->red = 150;
				rgb->green = 210;
				rgb->blue = 230;
			}
		}

		nodeY += printRatio / 2;

		for (int heightRatioCnt = 1; heightRatioCnt <= printRatio / 2; ++heightRatioCnt) {
			nodeX = (*iter)->_coord->_x * printRatio + heightRatioCnt;
			for (int widthRatioCnt = 0; widthRatioCnt < (printRatio / 2 - 1 - heightRatioCnt) * 2 - 1; ++widthRatioCnt) {
				stReverseRGB* rgb = &rgbData[(nodeY + heightRatioCnt) * (_width * printRatio) + nodeX + widthRatioCnt];
				rgb->red = 150;
				rgb->green = 210;
				rgb->blue = 230;
			}
		}

	}

	

	// 경로 출력
	do {
		//break;
		if (endNodeIter == pathEnd()) {
			break;
		}

		for(iterator nodeIter = pathBegin(); nodeIter != pathEnd(); ++nodeIter) {

			stNode* node = *nodeIter;

			stCoord* coord = node->_coord;
			int startX = coord->_x * printRatio + printRatio / 4;
			int startY = (_height - coord->_y - 1) * printRatio + printRatio / 4;

			stCoord* parentCoord = node->_parent->_coord;
			int endX = parentCoord->_x * printRatio + printRatio / 4;
			int endY = (_height - parentCoord->_y - 1) * printRatio + printRatio / 4;

			int distanceX = endX - startX;
			int distanceY = endY - startY;

			int xDirection = (distanceX < 0) * -1 + (distanceX > 0);
			int yDirection = (distanceY < 0) * -1 + (distanceY > 0);

			while (startX != endX || startY != endY) {
				for (int heightCnt = startY; heightCnt < startY + printRatio / 2; ++heightCnt) {
					for (int widthCnt = startX; widthCnt < startX + printRatio / 2; ++widthCnt) {
						stReverseRGB* rgb = &rgbData[heightCnt * (_width*printRatio) + widthCnt];

						rgb->red = 200;
						rgb->green = 0;
						rgb->blue = 0;

					}
				}

				if (startX != endX) {
					startX += xDirection;
				}
				if (startY != endY) {
					startY += yDirection;
				}

			}


		}
	} while (false);

	fwrite(rgbData, sizeof(stReverseRGB), _width * printRatio * _height * printRatio, bitmapFile);
	fclose(bitmapFile); 

	free(rgbData);

}

CJumpPointSearch::iterator CJumpPointSearch::makePath(stNode* endNode) {

	_path->clear();
	while (endNode->_parent != nullptr) {
		_path->push_front(endNode);
		endNode = endNode->_parent;
	}

	return _path->begin();
}

CJumpPointSearch::iterator CJumpPointSearch::lineTo(int sx, int sy, int ex, int ey) {

	_line->clear();

	stCoord direction(ey - sy, ex - sx);

	stCoord move( (direction._y < 0) * -1 + (direction._y > 0), (direction._x < 0) * -1 + (direction._x > 0));
	
	stCoord mid((ey + sy) / 2, (ex + sx) / 2);

	_line->push_back(new stNode(nullptr, 0, 0, new stCoord(mid._y, mid._x)));
	mapColor(mid._y, mid._x)->red = 160;
	mapColor(mid._y, mid._x)->green = 140;
	mapColor(mid._y, mid._x)->blue = 200;

	int *mainAxis;
	int mainAxisMove;

	int *subAxis;
	int subAxisMove;

	int addWeight;
	int minusWeight;

	stCoord coord(mid._y, mid._x);

	if (direction._x > direction._y) {
		addWeight = direction._y;
		minusWeight = direction._x;

		mainAxis = &coord._x;
		mainAxisMove = move._x;

		subAxis = &coord._y;
		subAxisMove = move._y;
	}
	else {
		addWeight = direction._x;
		minusWeight = direction._y;

		mainAxis = &coord._y;
		mainAxisMove = move._y;

		subAxis = &coord._x;
		subAxisMove = move._x;
	}

	// 중앙선 그리기
	int weight = addWeight;
	*mainAxis -= mainAxisMove;

	int cnt = 2;
	int createDirection = 1;

	stCoord midLineLeft;
	stCoord midLineRight;

	while (weight < minusWeight && (coord._x > sx || coord._y > sy)){

		_line->push_front(new stNode(nullptr, 0, 0, new stCoord(coord._y, coord._x)));
		mapColor(coord._y, coord._x)->red = 20;
		mapColor(coord._y, coord._x)->green = 200;
		mapColor(coord._y, coord._x)->blue = 200;

		if (createDirection == 1) {

			midLineLeft._x = coord._x;
			midLineLeft._y = coord._y;
		}
		else {
			midLineRight._x = coord._x;
			midLineRight._y = coord._y;
		}

		weight += addWeight;
		*mainAxis += mainAxisMove * createDirection * cnt;
		cnt += 1;
		createDirection = 0 - createDirection;

	}

	coord._x = midLineLeft._x;
	coord._y = midLineLeft._y;

	//return pathBegin();

	*subAxis -= subAxisMove;
	*mainAxis -= mainAxisMove;
	weight -= minusWeight;
	//weight = 0;
	// 중앙선 기준 왼쪽 선들 그리기
	while (coord._x > sx || coord._y > sy) {


		_line->push_front(new stNode(nullptr, 0, 0, new stCoord(coord._y, coord._x)));
		mapColor(coord._y, coord._x)->red = 170;
		mapColor(coord._y, coord._x)->green = 30;
		mapColor(coord._y, coord._x)->blue = 150;

		weight += addWeight;

		if (weight >= minusWeight) {
			weight -= minusWeight;
			*subAxis -= subAxisMove;
		}
		*mainAxis -= mainAxisMove;
	}

	weight = 0;
	coord._x = midLineRight._x;
	coord._y = midLineRight._y;
	*subAxis += subAxisMove;
	*mainAxis += mainAxisMove;

	// 중앙선 기준 오른쪽 선들 그리기
	while (coord._x < ex || coord._y < ey) {

		
		_line->push_back(new stNode(nullptr, 0, 0, new stCoord(coord._y, coord._x)));
		mapColor(coord._y, coord._x)->red = 200;
		mapColor(coord._y, coord._x)->green = 100;
		mapColor(coord._y, coord._x)->blue = 100;

		weight += addWeight;

		if (weight >= minusWeight) {
			weight -= minusWeight;
			*subAxis += subAxisMove;
		}
		*mainAxis += mainAxisMove;
	}

	

	return _line->begin();
}