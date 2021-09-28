#include "framework.h"

#include "myLinkedList.h"

#include "Astar.h"

#include "SimpleProfiler.h"

CAstar::CAstar(int width, int height, int blockSize) {
	_width = width;
	_height = height;
	_blockSize = blockSize;

	_map = new MAP_STATE[_height * _width];
	ZeroMemory(_map, _height * _width * sizeof(MAP_STATE));

	_node = (stNode*)malloc(sizeof(stNode) * _width * _height);
	ZeroMemory(_node, _height * _width * sizeof(stNode));

	_start = new stCoord(-1, -1);
	_end = new stCoord(-1, -1);

	sp = nullptr;
}

CAstar::~CAstar() {
	listClear();
	delete(_map);
	int nodeNum = _width * _height;
	for (int nodeCnt = 0; nodeCnt < nodeNum; ++nodeCnt) {
		_node[nodeCnt].~stNode();
	}
	free(_node);
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

CAstar::stNode* CAstar::pathFind() {

	sp = new SimpleProfiler();

	pathFindInit();

	void* result;

	do {
		result = pathFindSingleLoop();


	} while ((int)result == 1);

	if(sp != nullptr)
		sp->printToFile();

	delete(sp);
	sp = nullptr;
	
	// 마무리
	return (stNode*)result;

}

void CAstar::pathFindInit() {

	_openList.clear();
	//_closeList.clear();

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

	if (map(y, x) == MAP_STATE::WALL) {
		return false;
	}

	if (node(y,x)->_coord != nullptr) {
		return false;
	}

	return true;
}

CAstar::stNode* CAstar::pathFindSingleLoop() {

	if(sp != nullptr)
		sp->profileBegin("logic");

	int min = 1 << 30;
	stNode* selectedNode = nullptr;
	linkedList<stNode*>::iterator nodeIter;
	for (linkedList<stNode*>::iterator iter = _openList.begin(); iter != _openList.end(); ++iter) {

		if (min > (*iter)->_distance + (*iter)->_moveCnt) {
			min = (*iter)->_distance + (*iter)->_moveCnt;
			selectedNode = *iter;
			nodeIter = iter;
		}

	}

	_openList.erase(nodeIter);
	
	if (selectedNode->_coord->_y == _end->_y && selectedNode->_coord->_x == _end->_x) {
		if (sp != nullptr)
			sp->profileEnd("logic");
		return selectedNode;
	}

	int ty[8] = { -1,-1,-1, 0,0,  1,1,1 };
	int tx[8] = { -1,0,1,  -1,1,  -1,0,1 };

	int* pTy = ty;
	int* pTx = tx;

	int* tyEnd = ty + 8;

	int y = selectedNode->_coord->_y;
	int x = selectedNode->_coord->_x;

	for (; pTy != tyEnd; ++pTy, ++pTx) {

		int dy = *pTy + y;
		int dx = *pTx + x;

		if (dy < 0 || dx < 0 || dy == _height || dx == _width) {
			continue;
		}

		MAP_STATE nextMapState = map(dy, dx);

		if (checkMakeNode(dy, dx) == true) {

			if (*pTy == 0 || *pTx == 0) {
				_openList.push_back(node(dy, dx, new stNode(selectedNode, abs(_end->_x - dx) + abs(_end->_y - dy), selectedNode->_moveCnt + 1, new stCoord(dy, dx))));
			}
			else {
				_openList.push_back(node(dy, dx, new stNode(selectedNode, abs(_end->_x - dx) + abs(_end->_y - dy), selectedNode->_moveCnt + 1.5, new stCoord(dy, dx))));
			}
		}
		else {

			int moveCnt = 0;
			if (*pTy == 0 || *pTx == 0) {
				moveCnt = selectedNode->_moveCnt + 1;
			}
			else {
				moveCnt = selectedNode->_moveCnt + 1.5;
			}

			stNode* changeNode = node(dy, dx);

			changeNode->_parent = selectedNode;
			changeNode->_moveCnt = moveCnt;

			/*
			for (linkedList<stNode*>::iterator iter = _openList.begin(); iter != _openList.end(); ++iter) {
				if ((*iter)->_coord->_y == dy && (*iter)->_coord->_x == dx && (*iter)->_moveCnt > moveCnt) {
					(*iter)->_moveCnt = moveCnt;
					(*iter)->_parent = node;
					break;
				}
			}
			*/

		}

	}
	if (_openList.empty() == false) {
		if (sp != nullptr)
			sp->profileEnd("logic");
		return (stNode*)1;
	}

	if (sp != nullptr)
		sp->profileEnd("logic");
	return nullptr;

}

void CAstar::listClear() {

	//_closeList.clear();
	_openList.clear();

}

void CAstar::test(int blockSize, const WCHAR* fileName) {

	constexpr int width  = 90;
	constexpr int height = 90;

	CAstar* astar = new CAstar(width, height, blockSize);
	
	MAP_STATE pattern[2][3][3] = {
		{
			{MAP_STATE::ROAD, MAP_STATE::WALL, MAP_STATE::ROAD},
			{MAP_STATE::ROAD, MAP_STATE::WALL, MAP_STATE::ROAD},
			{MAP_STATE::ROAD, MAP_STATE::WALL, MAP_STATE::ROAD}
		},
		{
			{MAP_STATE::ROAD, MAP_STATE::ROAD, MAP_STATE::ROAD},
			{MAP_STATE::WALL, MAP_STATE::WALL, MAP_STATE::WALL},
			{MAP_STATE::ROAD, MAP_STATE::ROAD, MAP_STATE::ROAD}
		} 
	};

	stNode* endNode;
	for (int heightCnt = 0; heightCnt < height; heightCnt += 3) {

		for (int widthCnt = 0; widthCnt < width; widthCnt += 3) {

			if (widthCnt == 0) {

				for (int patternHeightCnt = 0; patternHeightCnt < 3; ++patternHeightCnt) {
					for (int patternWidthCnt = 0; patternWidthCnt < 3; ++patternWidthCnt) {
						astar->map(heightCnt + patternHeightCnt, widthCnt + patternWidthCnt) = pattern[heightCnt % 2][patternHeightCnt][patternWidthCnt];
					}
				}


			}

			else if (widthCnt + 3 == width){

				for (int patternHeightCnt = 0; patternHeightCnt < 3; ++patternHeightCnt) {
					for (int patternWidthCnt = 0; patternWidthCnt < 3; ++patternWidthCnt) {
						astar->map(heightCnt + patternHeightCnt, widthCnt + patternWidthCnt) = pattern[1 - heightCnt % 2][patternHeightCnt][patternWidthCnt];
					}
				}
			}

			else {
				int patternCnt = rand() % 2;
				for (int patternHeightCnt = 0; patternHeightCnt < 3; ++patternHeightCnt) {
					for (int patternWidthCnt = 0; patternWidthCnt < 3; ++patternWidthCnt) {
						astar->map(heightCnt + patternHeightCnt, widthCnt + patternWidthCnt) = pattern[patternCnt][patternHeightCnt][patternWidthCnt];
					}
				}
			}

		}

		astar->startPoint(0, 0);
		astar->endPoint(height - 1, width - 1);

		endNode = astar->pathFind();
		if (endNode == nullptr) {
			astar->printMapToBitmap(fileName, nullptr, 20);
			printf("경로 찾기 실패\n");
			system("PAUSE>NUL");
		}
	}

	astar->printMapToBitmap(fileName, endNode, 20);

	delete(astar);
}

#ifdef _WINDOWS_

void CAstar::printMapToBitmap(const WCHAR* fileName, stNode* endNode, int printRatio) {

	BITMAPINFOHEADER infoHeader;
	ZeroMemory(&infoHeader, sizeof(BITMAPINFOHEADER));

	BITMAPFILEHEADER header;
	ZeroMemory(&header, sizeof(BITMAPFILEHEADER));
	header.bfType = 0x4D42;
	header.bfSize = _width * printRatio * _height * printRatio * 3 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	infoHeader.biWidth = _width * printRatio;
	infoHeader.biHeight = _height * printRatio;
	infoHeader.biBitCount = 24;
	infoHeader.biSize = sizeof(BITMAPINFOHEADER);
	infoHeader.biSizeImage = _width * printRatio * _height * printRatio * 3;
	infoHeader.biPlanes = 1;

	char* buffer = (char*)malloc(_width * printRatio * _height * printRatio * 3);

	for (int heightCnt = 0; heightCnt < _height; ++heightCnt) {
		for (int ratioHeightCnt = 0; ratioHeightCnt < printRatio; ++ratioHeightCnt) {
			for (int widthCnt = 0; widthCnt < _width; ++widthCnt) {

				for (int ratioWidthCnt = 0; ratioWidthCnt < printRatio; ++ratioWidthCnt) {
					int bufferCnt = ((heightCnt * printRatio + ratioHeightCnt) * (_width * printRatio) + (widthCnt * printRatio + ratioWidthCnt));

					switch (map(_height - heightCnt - 1, widthCnt)) {
					case MAP_STATE::ROAD:
						buffer[bufferCnt * 3] = 43;
						buffer[bufferCnt * 3 + 1] = 43;
						buffer[bufferCnt * 3 + 2] = 43;
						break;
					case MAP_STATE::WALL:
						buffer[bufferCnt * 3] = 200;
						buffer[bufferCnt * 3 + 1] = 200;
						buffer[bufferCnt * 3 + 2] = 200;
						break;
					}
				}
			}
		}
	}

	if (endNode != nullptr) {
		stNode* node = endNode;
		while (node->_parent != nullptr) {

			stNode* parent = node->_parent;

			int startX = node->_coord->_x;
			int startY = node->_coord->_y;
			int endX = parent->_coord->_x;
			int endY = parent->_coord->_y;

			
			int startBufferX = startX * printRatio + printRatio / 4;
			int startBufferY = (_height - startY - 1) * printRatio + printRatio / 4;
			int endBufferX = endX * printRatio + printRatio / 4;
			int endBufferY = (_height - endY - 1) * printRatio + printRatio / 4;

			while (startBufferX != endBufferX || startBufferY != endBufferY) {

				for (int heightCnt = 0; heightCnt < printRatio / 2; ++heightCnt) {
					for (int widthCnt = 0; widthCnt < printRatio / 2; ++widthCnt) {
						int bufferCnt = (startBufferY + heightCnt) * (_width * printRatio) + (startBufferX + widthCnt);
						buffer[bufferCnt * 3] = 0;
						buffer[bufferCnt * 3 + 1] = 0;
						buffer[bufferCnt * 3 + 2] = 200;
					}
				}

				if (endBufferX > startBufferX) {
					startBufferX += 1;
				}
				else if (endBufferX < startBufferX) {
					startBufferX -= 1;
				}

				if (endBufferY > startBufferY) {
					startBufferY += 1;
				}
				else if (endBufferY < startBufferY) {
					startBufferY -= 1;
				}
			}

			node = parent;

		}
	}

	FILE* bitmap;
	_wfopen_s(&bitmap, fileName, L"w");

	fwrite(&header, sizeof(BITMAPFILEHEADER), 1, bitmap);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, bitmap);
	fwrite(buffer, 3, _width * printRatio * _height * printRatio, bitmap);

	fclose(bitmap);
	free(buffer);
}

#endif