#pragma once

template<typename T>
class CRedBlackTree {

public:

	struct stNode {

		stNode(T data, bool isRed = true, bool isNill = false, stNode* parent = nullptr) {
			_data = data;
			if (isNill == false) {
				_left = new stNode(0, false, true, this);
				_right = new stNode(0, false, true, this);
			}
			else {
				_left = nullptr;
				_right = nullptr;
			}
			_parent = parent;
			_isRed = isRed;
			_isNill = isNill;
		}

		T _data;
		stNode* _left;
		stNode* _right;
		stNode* _parent;
		bool _isRed;
		bool _isNill;

	};

	CRedBlackTree() {
		_root = new stNode(0, false, true);
	}

	void insert(T data) {

		stNode* newNode = new stNode(data);

		if (_root->_isNill == true) {
			delete(_root);
			newNode->_isRed = false;
			_root = newNode;
			return;
		}

		stNode* node = _root;
		while (node->_data != data) {
			if (node->_data < data) {
				if (node->_right->_isNill == true) {
					delete(node->_right);
					node->_right = newNode;
					newNode->_parent = node;;
					insertBalance(newNode);
					return;
				}
				node = node->_right;
			}
			else if (node->_data > data) {
				if (node->_left->_isNill == true) {
					delete(node->_left);
					node->_left = newNode;
					newNode->_parent = node;
					insertBalance(newNode);
					return;
				}
				node = node->_left;
			}
		}

		delete(newNode);
	}

	void erase(T data) {

		stNode** node = &_root;

		while ((*node)->_isNill == false) {

			if ((*node)->_data < data) {
				//right
				node = &(*node)->_right;
			}
			else if ((*node)->_data > data) {
				//left
				node = &(*node)->_left;
			}
			else {
				// correct
				bool isRed;
				stNode* erasedNode = eraseNode(node, &isRed);
				if (isRed == false) {
					eraseBalance(erasedNode);
				}
				break;
			}
		}

	}

#ifdef _WINDOWS_
	void print(HDC hdc) {

		if (_root->_isNill == true) {
			return;
		}

		_hdc = hdc;
		int printCnt = 0;
		printLoop(_root, 0, &printCnt);

	}

	static CRedBlackTree<T>* test(HDC hdc, HWND hWnd) {

		srand(908);

		constexpr int dataNum = 10;
		constexpr int eraseNum = 3;

		std::vector<int> insertData;
		std::vector<int> eraseIndex;
		std::vector<int> ableData;

		CRedBlackTree<int>* tree = new CRedBlackTree<int>();

		for (int dataCnt = 0; dataCnt < dataNum; ++dataCnt) {
			ableData.push_back(dataCnt);
		}

		printf("Insert Data: ");
		for (int dataCnt = 0; dataCnt < dataNum; ++dataCnt) {
			int index = rand() % ableData.size();
			int data = ableData[index];
			ableData.erase(ableData.begin() + index);
			insertData.push_back(data);
			tree->insert(data);
			printf("%d ", data);
		}
		printf("\n");

		for (int eraseCnt = 0; eraseCnt < eraseNum; ++eraseCnt) {
			eraseIndex.push_back(rand() % (dataNum - eraseCnt));
		}

		for (std::vector<int>::iterator erase = eraseIndex.begin(); erase != eraseIndex.end(); ++erase) {

			InvalidateRect(hWnd, nullptr, true);
			tree->print(hdc);

			int data = *(insertData.begin() + *erase);
			insertData.erase(insertData.begin() + *erase);
			tree->erase(data);

		}

		std::sort(insertData.begin(), insertData.end());
		printf("correct answer: ");
		for (std::vector<int>::iterator data = insertData.begin(); data != insertData.end(); ++data) {
			printf("%d ", *data);
		}
		printf("\n");

		return tree;
	}
#endif

private:

	stNode* _root;

	stNode* eraseNode(stNode** node, bool* isRed) {

		stNode* left = (*node)->_left;
		stNode* right = (*node)->_right;

		bool isLeftNill = left->_isNill;
		bool isRightNill = right->_isNill;

		if (isLeftNill == true) {

			// ��� ���� ���� ��¥�� ���⼭ �θ𿡼� �� ��尡 ����Ǿ� �ִ� ���� �ҷ� �����
			// ����� ���� �ڽ� ����

			right->_parent = (*node)->_parent;
			delete(left); // ���� �� ����
			*isRed = (*node)->_isRed;
			delete(*node); // ���� ��� ����
			*node = right; // �θ� ��尡 �ٶ󺸰� �ִ� ��带 ���� ���� ����
			return *node;

		}
		else if (isRightNill == true) {

			// ����� ���� �ڽ� ����

			left->_parent = (*node)->_parent;
			delete(right); // ���� �� ����
			*isRed = (*node)->_isRed;
			delete(*node); // ���� ��� ����
			*node = left;  // �θ� �ٶ󺸰� �ִ� ��带 ���� ���� ����
			return *node;

		}
		else {

			// ����� ���� �ڽ� ����
			// ����� ���� �ڽ��� �ֿ��� �ڽİ� ������ ��ü

			if (left->_right->_isNill == true) {

				// ����� ���� �ڽ��� ���� �ڽ��� ���� ��

				// ����� ���� �ڽİ� ����� �����͸� ��ü�ϰ�
				// ����� ���� �ڽ��� ���� �ڽ��� ���� ����

				(*node)->_data = left->_data; // ����� �����͸� ���� �ڽ��� �����ͷ� ����
				(*node)->_left = left->_left; // ����� ���� �ڽ��� ���� �ڽ��� ���� �ڽ����� ����
				left->_left->_parent = *node;
				*isRed = left->_isRed;
				delete(left); // ���� ��� ����
				return (*node)->_left;

			}
			else {

				// ����� ���� �ڽ��� ���� �ڽ��� ���� ��

				// �ֿ��� �ڽ��� ã�� ���� ����
				stNode** lastRightNode = &left; // ���� ������ ������ �ڽ�
				while ((*lastRightNode)->_right->_isNill == false) { // ���� �ڽ��� ���� �ƴϸ�
					lastRightNode = &((*lastRightNode)->_right); // ���� �ڽ����� ��ü
				}

				(*node)->_data = (*lastRightNode)->_data; // ����� ���� �ֿ�������� ������ ����

				// �ֿ��� ����� ���� ��� ���� ����
				// �ش� ���� �ֿ��� ����� �θ�� ����
				// ���̸� �� �����

				stNode* lastNodeLeft = (*lastRightNode)->_left; // �ֿ��� ����� ���� ���
				lastNodeLeft->_parent = (*lastRightNode)->_parent;
				*isRed = (*lastRightNode)->_isRed;
				delete(*lastRightNode); // �ֿ��� ��� ����
				*lastRightNode = lastNodeLeft; // �ֿ��� ����� �θ� ���� �ֿ��� ����� ���� ��� ����
				

				return *lastRightNode;
			}

		}

	}


	void insertBalance(stNode* node) {

		// ��尡 ��Ʈ�� Ż��
		if (node == _root) {
			node->_isRed = false;
			return;
		}

		stNode* parent = node->_parent;

		// ����� �θ� �����̸� Ż��
		if (parent->_isRed == false) {
			return;
		}

		stNode* grand = parent->_parent;
		stNode* uncle;
		bool isNodeLeftGrand;
		if (grand->_left == parent) {
			uncle = grand->_right;
			isNodeLeftGrand = true;
		}
		else {
			uncle = grand->_left;
			isNodeLeftGrand = false;
		}

		stNode* sibling;
		bool isSiblingLeftParent;
		if (parent->_left == node) {
			sibling = parent->_right;
			isSiblingLeftParent = false;
		}
		else {
			sibling = parent->_left;
			isSiblingLeftParent = true;
		}

		if (parent->_isRed == true) {

			// �θ�� ������ ������ ���
			if (uncle->_isRed == true) {
				grand->_isRed = true; // ���θ� �������� ��
				parent->_isRed = false; // �θ� �������� ��
				uncle->_isRed = false; // ������ �������� ��
				insertBalance(grand); // ���η� ���
				return;
			}

			// �θ� ������ ���

			if (isSiblingLeftParent == true) {
				// ������ ���ʿ� ���� ���
				if (isNodeLeftGrand == true) {
					// ���� ������ ���
					
					// ��� - �θ� - ������ ȸ��

					// ���� �������� ��带 �̵�
					grand->_left = node;
					node->_parent = grand;


					// �θ� �������� �ҳ��� ����
					parent->_right = node->_left;

					// ��� �������� �θ� �̵�
					node->_left = parent;
					parent->_parent = node;


					// ��� ��Ī ����
					stNode* temp = node;
					node = parent;
					parent = temp;
					sibling = parent->_right; // �� ���
					
				}
			}
			else {

				// ������ �����ʿ� ���� ���
				if (isNodeLeftGrand == false) {
					// ���� �������� ���

					// ��� - �θ� - ������ ȸ��

					// ���� ���������� ��带 �̵�
					grand->_right = node;
					node->_parent = grand;

					// �θ� ������ �ҳ��� ����
					parent->_left = node->_right;

					// ��� ���������� �θ� �̵�
					node->_right = parent;
					parent->_parent = node;

					// ��� ��Ī ����
					stNode* temp = node;
					node = parent;
					parent = temp;
					sibling = parent->_left; // �� ���
				}

			}

			// ���α��� ������ ȸ��

			if (grand->_parent != nullptr) {
				if (grand->_parent->_left == grand) {
					grand->_parent->_left = parent;
					parent->_parent = grand->_parent;
				}
				else {
					grand->_parent->_right = parent;
					parent->_parent = grand->_parent;
				}
			}
			else {
				parent->_parent = nullptr;
				_root = parent;
			}


			if (isNodeLeftGrand == true) {
				// ��尡 ���� ���ʿ� ���� ���
				grand->_left = sibling;
				sibling->_parent = grand;
				grand->_parent = parent;
				parent->_right = grand;

			}
			else {
				// ��尡 ���� �����ʿ� ���� ���
				grand->_right = sibling;
				sibling->_parent = grand;
				grand->_parent = parent;
				parent->_left = grand;

			}

			grand->_isRed = true;
			parent->_isRed = false;

			return;
		}



	}

	void eraseBalance(stNode* node) {

		if (node == _root) {
			_root->_isRed = false;
			return;
						
		}

		if (node->_isRed == true) {
			node->_isRed = false;
			return;
		}

		stNode* parent = node->_parent;
		stNode* sibling;
		bool _isNodeLeftParent;

		if (parent->_left == node) {
			sibling = parent->_right;
			_isNodeLeftParent = true;
		}
		else {
			sibling = parent->_left;
			_isNodeLeftParent = false;
		}

		if (parent->_isRed == true) {
			// �θ� ������ ���
			
			if (sibling->_isNill == true) {
				return;
			}

			if (sibling->_left->_isRed == false && sibling->_right->_isRed == false) {
				return;
			}

			// ������ �������� �����ߴµ�, ������ �ڽĵ� �������� ���� ���
			

			stNode* left = sibling->_left;
			stNode* right = sibling->_right;

			if (left->_isRed == true && right->_isRed == false) {
				// ������ ���� �ڽ��� ����

				// ���� - ���� �ڽ� �� ȸ��

				sibling->_isRed = true;
				left->_isRed = false;

				if (_isNodeLeftParent == true) {
					parent->_right == left;
				}
				else {
					parent->_left = left;
				}
				left->_parent = parent;

				sibling->_left = left->_right;
				left->_right->_parent = sibling;

				left->_right = sibling;
				sibling->_parent = left;


			}

			// ������ ���� �ڽ��� ����
			
			// �θ� - �ڽ� �� ȸ��

			if (parent->_parent != nullptr) {
				if (parent->_parent->_left == parent) {
					parent->_parent->_left = sibling;
				}
				else {
					parent->_parent->_right = sibling;
				}
				sibling->_parent = parent->_parent;
			}
			else {
				sibling->_parent = nullptr;
			}

			if (_isNodeLeftParent == true) {
				
				// ��ȸ��
				parent->_right = sibling->_left;
				sibling->_left->_parent = parent;

				sibling->_left = parent;
				parent->_parent = sibling;


			}
			else {

				// ��ȸ��
				parent->_left = sibling->_right;
				sibling->_right->_parent = parent;

				sibling->_right = parent;
				parent->_parent = sibling;
			}

			return;

		}

		// �θ� ������ ���

		if (sibling->_isRed == false) {
			// ������ ������ ���

			if (sibling->_isNill == false) {
				sibling->_isRed = true;

				if (sibling->_left->_isRed == true) {

					// ������ ���� �ڽ��� ������ ���

					// ���� - ������ �ڽ� �� ��ȸ��
					stNode* left = sibling->_left;
					stNode* right = sibling->_right;

					sibling->_isRed = true;
					left->_isRed = false;

					parent->_right = left;
					left->_parent = parent;

					sibling->_left = left->_right;
					left->_right->_parent = sibling;

					left->_right = sibling;
					sibling->_parent = left;

					sibling = parent->_right;

				}

				if (sibling->_right->_isRed == true) {

					// ������ ���� �ڽ��� ������ ���
					sibling->_right->_isRed = false;

					// �θ��� �θ���� ����
					if (parent->_parent != nullptr) {
						if (parent->_parent->_left == parent) {

							parent->_parent->_left = sibling;

						}
						else {
							parent->_parent->_right = sibling;
						}
						sibling->_parent = parent->_parent;
					}
					else {
						sibling->_parent = nullptr;
					}

					if (_isNodeLeftParent == true) {
						// �θ� - �ڽ� �� ��ȸ��

						// ������ ���� �ڽ� ������
						parent->_right = sibling->_left;
						sibling->_left->_parent = parent;

						// �θ� ���� ����
						sibling->_left = parent;
						parent->_parent = sibling;

					}
					else {
						// �θ� - �ڽ� �� ��ȸ��

						// ������ ���� �ڽ� ������
						parent->_left = sibling->_right;
						sibling->_right->_parent = parent;

						// �θ� ���� ����
						sibling->_right = parent;
						parent->_parent = sibling;
					}

					return;
				}
			}
			eraseBalance(parent);
			return;
		}

		if (sibling->_isRed == true) {
			// ������ ������ ���

			if (sibling->_left->_isNill == false) {
				sibling->_left->_isRed = true;
			}

			// ���θ��� �ڽ� ��带 ������ ����
			if (parent->_parent != nullptr) {
				if (parent->_parent->_left == parent) {
					parent->_parent->_left = sibling;
				}
				else {
					parent->_parent->_right = sibling;
				}
				sibling->_parent = parent->_parent;
			}
			else {
				sibling->_parent = nullptr;
				_root = sibling;
			}

			if (_isNodeLeftParent == true) {                                                                                                                                      
				// �θ� ������ ��ȸ��
				sibling->_isRed = false;

				parent->_right = sibling->_left; // �θ��� ���� �ڽ��� ������ ���ڽ����� ����
				sibling->_left->_parent = parent; // ������ �ڽ��� �θ� �θ�� ����

				parent->_parent = sibling;// �θ��� �θ� ������ ����
				sibling->_left = parent; // ������ ���ڽ��� �θ�� ����
			}
			else {

				// �θ� ������ ��ȸ��
				sibling->_isRed = false;

				parent->_left = sibling->_right; // �θ��� ���ڽ��� ������ �����ڽ����� ����
				sibling->_right->_parent = parent; // ������ �ڽ��� �θ� �θ�� ����

				parent->_parent = sibling;// ���θ� ������ ����
				sibling->_right = parent; // ������ �����ڽ��� �θ�� ����
			}

			

		}


	}


#ifdef _WINDOWS_
	HDC _hdc;
	int printLoop(stNode* node, int deepth, int* printCnt) {

		if (node->_left->_isNill == false) {
			int leftPrintCnt = printLoop(node->_left, deepth + 1, printCnt);

			// ���� ����� ���� ������
			int leftLineX = leftPrintCnt * 80 + 80 - (80 * 0.14); // right
			int leftLineY = (deepth + 1) * 80 + (80 * 0.14); // top

			// ���� ����� ���� ������
			int nodeLineX = *printCnt * 80 + (80 * 0.14); // left
			int nodeLineY = deepth * 80 + 80 - (80 * 0.14); // bottom

			MoveToEx(_hdc, leftLineX, leftLineY, nullptr);
			LineTo(_hdc, nodeLineX, nodeLineY);

		}

		RECT ellipseRect;
		ZeroMemory(&ellipseRect, sizeof(RECT));

		ellipseRect.left = *printCnt * 80;
		ellipseRect.top = deepth * 80;
		ellipseRect.right = ellipseRect.left + 80;
		ellipseRect.bottom = ellipseRect.top + 80;

		HBRUSH ellipseBrush;
		if (node->_isRed == true) {
			ellipseBrush = CreateSolidBrush(RGB(240, 70, 80));
			SetBkColor(_hdc, RGB(240, 70, 80));
			SetTextColor(_hdc, RGB(0, 0, 0));
		}
		else {
			ellipseBrush = CreateSolidBrush(RGB(43, 43, 43));
			SetBkColor(_hdc, RGB(43, 43, 43));
			SetTextColor(_hdc, RGB(255, 255, 255));
		}

		HBRUSH oldEllipseBrush = (HBRUSH)SelectObject(_hdc, ellipseBrush);
		Ellipse(_hdc, ellipseRect.left, ellipseRect.top, ellipseRect.right, ellipseRect.bottom);
		SelectObject(_hdc, oldEllipseBrush);
		DeleteObject(ellipseBrush);

		WCHAR text[10] = { 0, };
		_itow_s(node->_data, text, 10, 10);
		TextOutW(_hdc, ellipseRect.left + 40, ellipseRect.top + 40, text, wcslen(text));

		int nodePrintCnt = *printCnt;
		*printCnt += 1;

		if (node->_right->_isNill == false) {
			int rightPrintCnt = printLoop(node->_right, deepth + 1, printCnt);

			// ���� ����� ���� ������
			int rightLineX = rightPrintCnt * 80 + (80 * 0.14); // left
			int rightLineY = (deepth + 1) * 80 + (80 * 0.14); // top

			// ����� ���� ������
			int nodeLineX = nodePrintCnt * 80 + 80 - (80 * 0.14); // right
			int nodeLineY = deepth * 80 + 80 - (80 * 0.14); // bottom

			MoveToEx(_hdc, rightLineX, rightLineY, nullptr);
			LineTo(_hdc, nodeLineX, nodeLineY);

		}

		return nodePrintCnt;

	}
#endif
};