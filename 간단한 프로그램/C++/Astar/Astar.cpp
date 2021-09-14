#include "framework.h"

#include "myLinkedList.h"

#include "Astar.h"

CAstar::CAstar(int width, int height, int blockSize) {
	_width = width;
	_height = height;
	_blockSize = blockSize;

	_map = new MAP_STATE[_height * _width];
	ZeroMemory(_map, _height * _width * sizeof(MAP_STATE));

	_start = new stCoord(-1, -1);
	_end = new stCoord(-1, -1);
}

CAstar::~CAstar() {
	listClear();
	delete(_map);
}

CAstar::stCoord* CAstar::startPoint(int y, int x) {

	if (x / _blockSize >= _width || y / _blockSize >= _height) {
		return _start;
	}

	_start->_x = x;
	_start->_y = y;
	return _start;

}

CAstar::stCoord* CAstar::endPoint(int y, int x) {

	if (x / _blockSize >= _width || y / _blockSize >= _height) {
		return _end;
	}

	_end->_x = x;
	_end->_y = y;
	return _end;

}

void CAstar::pathFind() {

	pathFindInit();

	void* result;

	do {
		result = pathFindSingleLoop();


	} while ((int)result == 1);

	do {
		if (result == nullptr) {
			// 경로 없음
			break;
		}
		// 경로 있음
		
	} while (false);

	// 마무리

}

void CAstar::pathFindInit() {

	_openList.clear();
	_closeList.clear();

	_openList.push_back(new stNode(nullptr, abs(_start->_x - _end->_x) + abs(_start->_y - _end->_y), 0, new stCoord(_start->_y, _start->_x)));

}

bool CAstar::checkList(linkedList<CAstar::stNode*> *list,  CAstar::stNode* node) {

	for (linkedList<stNode*>::iterator iter = list->begin(); iter != list->end(); ++iter) {
		if (*iter == node) {
			return true;
		}
	}
	return false;
}

bool CAstar::checkList(linkedList<CAstar::stNode*> *list, int y, int x) {

	for (linkedList<stNode*>::iterator iter = list->begin(); iter != list->end(); ++iter) {
		stCoord* coord = (*iter)->_coord;
		if (coord->_y == y && coord->_x == x) {
			return true;
		}
	}
	return false;
}

bool CAstar::checkMakeNode(int y, int x) {

	if (y < 0 || x < 0 || y >= _height || x >= _width) {
		return false;
	}

	if (map(y, x) == MAP_STATE::WALL) {
		return false;
	}

	if (checkList(&_openList, y, x) == true || checkList(&_closeList, y, x) == true) {
		return false;
	}

	return true;
}

CAstar::stNode* CAstar::pathFindSingleLoop() {

	int min = 1 << 30;
	stNode* node = nullptr;
	linkedList<stNode*>::iterator nodeIter;
	for (linkedList<stNode*>::iterator iter = _openList.begin(); iter != _openList.end(); ++iter) {

		if (min > (*iter)->_distance + (*iter)->_moveCnt) {
			min = (*iter)->_distance + (*iter)->_moveCnt;
			node = *iter;
			nodeIter = iter;
		}

	}

	_openList.erase(nodeIter);
	_closeList.push_back(node);

	if (node->_coord->_y == _end->_y && node->_coord->_x == _end->_x) {
		return node;
	}

	int ty[8] = { -1,-1,-1, 0,0,  1,1,1 };
	int tx[8] = { -1,0,1,  -1,1,  -1,0,1 };

	int* pTy = ty;
	int* pTx = tx;

	int* tyEnd = ty + 8;

	int y = node->_coord->_y;
	int x = node->_coord->_x;

	for (; pTy != tyEnd; ++pTy, ++pTx) {

		int dy = *pTy + y;
		int dx = *pTx + x;

		MAP_STATE nextMapState = map(dy, dx);

		if (checkMakeNode(dy, dx) == true) {

			if (*pTy == 0 || *pTx == 0) {
				_openList.push_back(new stNode(node, abs(_end->_x - dx) + abs(_end->_y - dy), node->_moveCnt + 1, new stCoord(dy, dx)));
			}
			else {
				_openList.push_back(new stNode(node, abs(_end->_x - dx) + abs(_end->_y - dy), node->_moveCnt + 1.5, new stCoord(dy, dx)));
			}
		}
		else {

			int moveCnt = 0;
			if (*pTy == 0 || *pTx == 0) {
				moveCnt = node->_moveCnt + 1;
			}
			else {
				moveCnt = node->_moveCnt + 1.5;
			}

			for (linkedList<stNode*>::iterator iter = _openList.begin(); iter != _openList.end(); ++iter) {
				if ((*iter)->_coord->_y == dy && (*iter)->_coord->_x == dx && (*iter)->_moveCnt > moveCnt) {
					(*iter)->_moveCnt = moveCnt;
					(*iter)->_parent = node;
					break;
				}
			}

		}

	}

	if (_openList.empty() == false) {
		return (stNode*)1;
	}

	return nullptr;

}

void CAstar::listClear() {

	for (linkedList<stNode*>::iterator iter = _closeList.begin(); iter != _closeList.end(); ++iter) {
		
		delete (*iter);
	}
	for (linkedList<stNode*>::iterator iter = _openList.begin(); iter != _openList.end(); ++iter) {
		delete (*iter);
	}

	_closeList.clear();
	_openList.clear();

}