#pragma once

#define abs(x) ((x)<0?-(x):(x))

class CAstar {

public:

	enum class MAP_STATE {
		ROAD = 0,
		WALL = 1
	};

	struct stCoord {
		int _x;
		int _y;
		stCoord(int y, int x) {
			_x = x;
			_y = y;
		}
	};

	struct stNode{
		stNode* _parent;
		int _distance;
		float _moveCnt;
		CAstar::stCoord* _coord;
		stNode(stNode* parent, int distance, float moveCnt, CAstar::stCoord* coord) {
			_parent = parent;
			_distance = distance;
			_moveCnt = moveCnt;
			_coord = coord;
		}
		~stNode() {
			delete(_coord);
		}
	};

	CAstar(int width, int height, int blockSize);
	~CAstar();

	stCoord* startPoint(int y, int x);
	inline stCoord* startPoint() { return _start; }
	stCoord* endPoint(int y, int x);
	inline stCoord* endPoint() { return _end; }

	void pathFind();
	void pathFindInit();
	stNode* pathFindSingleLoop();

	inline int width() { return _width; }
	inline int height() { return _height; }

	inline MAP_STATE& map(int y, int x) {
		return _map[y * _width + x];
	}

	void listClear();

private:

	int _width;
	int _height;
	int _blockSize;

	MAP_STATE* _map;

	stCoord* _start;
	stCoord* _end;

	linkedList<stNode*> _openList;
	linkedList<stNode*> _closeList;

	bool checkList(linkedList<stNode*> *list, stNode* node);
	bool checkList(linkedList<stNode*> *list, int y, int x);

	bool checkMakeNode(int y, int x);

public:

#ifdef _WINDOWS_

	void print(HDC hdc) {

		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

		for (int heightCnt = 0; heightCnt <= _height; ++heightCnt) {

			MoveToEx(hdc, 0, heightCnt * _blockSize, nullptr);
			LineTo(hdc, _width * _blockSize, heightCnt * _blockSize);

		}

		for (int widthCnt = 0; widthCnt <= _width; ++widthCnt) {

			MoveToEx(hdc, widthCnt * _blockSize, 0, nullptr);
			LineTo(hdc, widthCnt * _blockSize, _height * _blockSize);

		}

		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);

		{
			HBRUSH hBrush = CreateSolidBrush(RGB(100, 100, 100));
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

			for (int heightCnt = 0; heightCnt < _height; heightCnt++) {
				for (int widthCnt = 0; widthCnt < _width; widthCnt++) {
					if (map(heightCnt, widthCnt) == MAP_STATE::WALL) {
						Rectangle(hdc, widthCnt * _blockSize + 1, heightCnt * _blockSize + 1, widthCnt * _blockSize + _blockSize, heightCnt * _blockSize + _blockSize);
					}
				}
			}

			SelectObject(hdc, hOldBrush);
			DeleteObject(hBrush);
		}

		{
			HBRUSH hBrush = CreateSolidBrush(RGB(255, 200, 20));
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

			for (linkedList<stNode*>::iterator iter = _openList.begin(); iter != _openList.end(); ++iter) {
				stCoord* coord = (*iter)->_coord;
				Rectangle(hdc, coord->_x * _blockSize + 1, coord->_y * _blockSize + 1, coord->_x * _blockSize + _blockSize, coord->_y * _blockSize + _blockSize);
			}

			SelectObject(hdc, hOldBrush);
			DeleteObject(hBrush);
		}

		{
			HBRUSH hBrush = CreateSolidBrush(RGB(255, 120, 40));
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

			for (linkedList<stNode*>::iterator iter = _closeList.begin(); iter != _closeList.end(); ++iter) {
				stCoord* coord = (*iter)->_coord;
				Rectangle(hdc, coord->_x * _blockSize + 1, coord->_y * _blockSize + 1, coord->_x * _blockSize + _blockSize, coord->_y * _blockSize + _blockSize);
			}

			SelectObject(hdc, hOldBrush);
			DeleteObject(hBrush);
		}
		if (_start->_x != -1) {

			HBRUSH hBrush = CreateSolidBrush(RGB(50, 200, 50));
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

			Rectangle(hdc, _start->_x * _blockSize + 1, _start->_y * _blockSize + 1, _start->_x * _blockSize + _blockSize, _start->_y * _blockSize + _blockSize);

			SelectObject(hdc, hOldBrush);
			DeleteObject(hBrush);
		}

		if (_end->_x != -1) {

			HBRUSH hBrush = CreateSolidBrush(RGB(200, 50, 50));
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

			Rectangle(hdc, _end->_x * _blockSize + 1, _end->_y * _blockSize + 1, _end->_x * _blockSize + _blockSize, _end->_y * _blockSize + _blockSize);

			SelectObject(hdc, hOldBrush);
			DeleteObject(hBrush);
		}
	}

#endif

};