
class CJumpPointSearch {


public:

	enum class MAP_STATE {
		ROAD = 0,
		WALL = 1
	};
	enum class DIRECTION {
		NONE = -1,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		LEFT_UP,
		RIGHT_UP,
		LEFT_DOWN,
		RIGHT_DOWN
	};

	struct stCoord {
		int _x;
		int _y;
		CJumpPointSearch::DIRECTION _dir;
		stCoord(int y = 0, int x = 0, DIRECTION dir = DIRECTION::NONE) {
			_x = x;
			_y = y;
			_dir = dir;
		}
	};

	struct stNode {
		int _moveCnt;
		stNode* _parent;
		int _distance;
		CJumpPointSearch::stCoord* _coord;

		stNode(stNode* parent, int moveCnt, int distance, stCoord* coord) {
			_coord = coord;
			_moveCnt = moveCnt;
			_parent = parent;
			_distance = distance;
		}
		~stNode() {
			delete(_coord);
		}
	};

public:

	inline MAP_STATE* map(int y, int x) {
		return &_map[y * _width + x];
	}
	CJumpPointSearch(int width, int height);

	void pathFindInit();
	stNode* pathFindSingleLoop();
	void makeNode(stCoord* corner, stNode* parent);

	inline int width() {
		return _width;
	}
	inline int height() {
		return _height;
	}

	stCoord _start;
	stCoord _end;

#ifdef _WINDOWS_

	void print(HDC hdc, int blockSize = 10);

#endif

private:

	MAP_STATE* _map;
	int _width;
	int _height;


	linkedList<stNode*>* _openList;
	linkedList<stNode*>* _closeList;

	linkedList<stNode*>::iterator* findMin(linkedList<stNode*>* list);

	stCoord* checkOrthogonal(DIRECTION dir, int y , int x);
	stCoord* checkDiagonal(DIRECTION dir, int y, int x);

	bool isNodeInList(stCoord* coord, linkedList<stNode*>* list);
};