#pragma once

class SimpleProfiler;

extern SimpleProfiler* sp;

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

		if(sp != nullptr)
		sp->profileBegin("insert");

		do {
			stNode* newNode = new stNode(data);

			if (_root->_isNill == true) {
				delete(_root);
				newNode->_isRed = false;
				_root = newNode;
				break;
			}

			stNode* node = _root;
			while (node->_data != data) {
				if (node->_data < data) {
					if (node->_right->_isNill == true) {
						delete(node->_right);
						node->_right = newNode;
						newNode->_parent = node;;
						insertBalance(newNode);
						//diagnosis(_root);
						goto FIN;
					}
					node = node->_right;
				}
				else if (node->_data > data) {
					if (node->_left->_isNill == true) {
						delete(node->_left);
						node->_left = newNode;
						newNode->_parent = node;
						insertBalance(newNode);
						//diagnosis(_root);
						//break;
						goto FIN;
					}
					node = node->_left;
				}
			}
			delete(newNode);
		} while (false);

FIN:
		if (sp != nullptr)
		sp->profileEnd("insert");
	}

	void erase(T data) {
		if (sp != nullptr)
		sp->profileBegin("erase");
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
					//diagnosis(_root);
				}
				break;
			}
		}
		if (sp != nullptr)
		sp->profileEnd("erase");
	}

#ifdef _WINDOWS_

	static const int addNum = 100;
	static const int eraseNum = 50;

	static CRedBlackTree<int>* singleCaseTest(int seed) {
		static CRedBlackTree<int>* tree = nullptr;

		static std::vector<int>* addNumList = nullptr;
		static std::vector<int>* eraseIndexList = nullptr;


		static std::vector<int>* ableNumList = nullptr;

		static std::vector<int>::iterator addNumListIter;
		static std::vector<int>::iterator eraseIndexListIter;

		if (addNumList == nullptr) {


			//int seed = rand() % 10000;
			//int seed = 1449;
			printf("seed : %d\n", seed);
			srand(seed);

			delete(tree);
			tree = new CRedBlackTree();
			addNumList = new std::vector<int>();
			eraseIndexList = new std::vector<int>();
			ableNumList = new std::vector<int>();

			std::vector<int> answerList;

			for (int numCnt = 0; numCnt < addNum; numCnt++) {
				ableNumList->push_back(numCnt);
			}

			printf("add num: ");
			for (int numCnt = 0; numCnt < addNum; numCnt++) {
				int index = rand() % ableNumList->size();
				addNumList->push_back(ableNumList->at(index));
				printf("%d ", ableNumList->at(index));
				answerList.push_back(ableNumList->at(index));
				ableNumList->erase(ableNumList->begin() + index);
			}
			printf("\n");

			for (int indexCnt = 0; indexCnt < eraseNum; ++indexCnt) {
				int index = rand() % (addNum - indexCnt);
				eraseIndexList->push_back(index);
				answerList.erase(answerList.begin() + index);
			}

			std::sort(answerList.begin(), answerList.end());

			addNumListIter = addNumList->begin();
			eraseIndexListIter = eraseIndexList->begin();

			printf("answer: ");
			for (std::vector<int>::iterator answerIter = answerList.begin(); answerIter != answerList.end(); ++answerIter) {

				printf("%d ", *answerIter);

			}
			printf("\n");

		}

		if (addNumListIter != addNumList->end()) {


			tree->insert(*addNumListIter);
			++addNumListIter;
			singleCaseTest(seed);
			return tree;

		}

		if (eraseIndexListIter != eraseIndexList->end()) {

			tree->erase(addNumList->at(*eraseIndexListIter));
			addNumList->erase(addNumList->begin() + *eraseIndexListIter);
			++eraseIndexListIter;
			addNumListIter = addNumList->end();
			//test();
			return tree;

		}

		std::vector<int> treeData;
		std::vector<int> blackNodeNum;
		tree->treeToList(&treeData, &blackNodeNum, tree->_root);

		std::vector<int>::iterator treeIter;
		std::vector<int>::iterator numIter;
		sort(addNumList->begin(), addNumList->end());

		for (treeIter = treeData.begin(), numIter = addNumList->begin(); treeIter != treeData.end(); ++treeIter, ++numIter) {
			if (*treeIter != *numIter) {
				break;
			}
		}
		if (treeIter != treeData.end()) {
			printf("Ʈ���� �����Ͱ� ������ �����Ͱ� �ƴ�\n");
			system("PAUSE>NUL");
		}

		std::vector<int>::iterator blackCntIter;
		for (blackCntIter = blackNodeNum.begin() + 1; blackCntIter != blackNodeNum.end(); ++blackCntIter) {
			if (*blackCntIter != blackNodeNum[0]) {
				break;
			}
		}

		if (blackCntIter != blackNodeNum.end()) {
			printf("�� ����� ���� ��� ���� ��ġ���� �ʽ��ϴ�.");
			system("PAUSE>NUL");
		}


		delete(addNumList);
		addNumList = nullptr;
		delete(eraseIndexList);
		eraseIndexList = nullptr;
		//delete(tree);
		//tree = nullptr;
		delete(ableNumList);
		ableNumList = nullptr;

		tree = test();

		return tree;
	}

	static CRedBlackTree<int>* test() {

		static CRedBlackTree<int>* tree = nullptr;

		static std::vector<int>* addNumList = nullptr;
		static std::vector<int>* eraseIndexList = nullptr;

		static std::vector<int>* ableNumList = nullptr;

		static std::vector<int>::iterator addNumListIter;
		static std::vector<int>::iterator eraseIndexListIter;

		if (addNumList == nullptr) {

			
			int seed = rand() % 10000;
			//int seed = 1449;
			printf("seed : %d\n", seed);
			srand(seed);

			delete(tree);
			tree = new CRedBlackTree();
			addNumList = new std::vector<int>();
			eraseIndexList = new std::vector<int>();
			ableNumList = new std::vector<int>();

			std::vector<int> answerList;

			for (int numCnt = 0; numCnt < addNum; numCnt++) {
				ableNumList->push_back(numCnt);
			}

			printf("add num: ");
			for (int numCnt = 0; numCnt < addNum; numCnt++) {
				int index = rand() % ableNumList->size();
				addNumList->push_back(ableNumList->at(index));
				printf("%d ", ableNumList->at(index));
				answerList.push_back(ableNumList->at(index));
				ableNumList->erase(ableNumList->begin() + index);
			}
			printf("\n");

			for (int indexCnt = 0; indexCnt < eraseNum; ++indexCnt) {
				int index = rand() % (addNum - indexCnt);
				eraseIndexList->push_back(index);
				answerList.erase(answerList.begin() + index);
			}

			std::sort(answerList.begin(), answerList.end());

			addNumListIter = addNumList->begin();
			eraseIndexListIter = eraseIndexList->begin();
			
			printf("answer: ");
			for (std::vector<int>::iterator answerIter = answerList.begin(); answerIter != answerList.end(); ++answerIter) {

				printf("%d ", *answerIter);

			}
			printf("\n");

		}

		if (addNumListIter != addNumList->end()) {

			
			tree->insert(*addNumListIter);
			++addNumListIter;
			test();
			return tree;

		} 

		if (eraseIndexListIter != eraseIndexList->end()) {

			tree->erase(addNumList->at(*eraseIndexListIter));
			addNumList->erase(addNumList->begin() + *eraseIndexListIter);
			++eraseIndexListIter;
			addNumListIter = addNumList->end();
			test();
			return tree;

		}
		
		std::vector<int> treeData;
		std::vector<int> blackNodeNum;
		tree->treeToList(&treeData, &blackNodeNum, tree->_root);

		std::vector<int>::iterator treeIter;
		std::vector<int>::iterator numIter;
		sort(addNumList->begin(), addNumList->end());

		for (treeIter = treeData.begin(), numIter = addNumList->begin(); treeIter != treeData.end(); ++treeIter, ++numIter) {
			if (*treeIter != *numIter) {
				break;
			}
		}

		if (treeIter != treeData.end()) {
			printf("Ʈ���� �����Ͱ� ������ �����Ͱ� �ƴ�\n");
			system("PAUSE>NUL");
		}
		
		std::vector<int>::iterator blackCntIter;
		for (blackCntIter = blackNodeNum.begin() + 1; blackCntIter != blackNodeNum.end(); ++blackCntIter) {
			if (*blackCntIter != blackNodeNum[0]) {
				break;
			}
		}

		if (blackCntIter != blackNodeNum.end()) {
			printf("�� ����� ���� ��� ���� ��ġ���� �ʽ��ϴ�.");
			system("PAUSE>NUL");
		}



		delete(addNumList);
		addNumList = nullptr;
		delete(eraseIndexList);
		eraseIndexList = nullptr;
		//delete(tree);
		//tree = nullptr;
		delete(ableNumList);
		ableNumList = nullptr;

		//tree = test();

		return tree;
	}

	void print(HDC hdc, int x) {
		if (_root->_isNill == true) {
			return;
		}

		_hdc = hdc;
		int printCnt = 0;
		printLoop(_root, 0, &printCnt, x);

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

		if (node->_data == 3) {
			int k = 3;
		}

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
					node->_left->_parent = parent;

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
					node->_right->_parent = parent;

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

	void leftRotation(stNode* parent, stNode* right) {

		right->_parent = parent->_parent;
		if (parent->_parent != nullptr) {
			if (parent->_parent->_left == parent) {
				parent->_parent->_left = right;
			}
			else {
				parent->_parent->_right = right;
			}
		}
		else {
			_root = right;
		}

		parent->_right = right->_left;
		right->_left->_parent = parent;

		right->_left = parent;
		parent->_parent = right;

	}

	void rightRotation(stNode* parent, stNode* left) {

		left->_parent = parent->_parent;
		if (parent->_parent != nullptr) {
			if (parent->_parent->_left == parent) {
				parent->_parent->_left = left;
			}
			else {
				parent->_parent->_right = left;
			}
		}
		else {
			_root = left;
		}

		parent->_left = left->_right;
		left->_right->_parent = parent;

		left->_right = parent;
		parent->_parent = left;


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

			if (sibling->_isRed == false) {
				// ������ ������ �� ��
				if (sibling->_left->_isRed == false && sibling->_right->_isRed == false) {
					// ������ �ڽĵ� ��� ������ ��
					if (_isNodeLeftParent == true) {
						leftRotation(parent, sibling);
					}
					else {
						rightRotation(parent, sibling);
					}
					return;
				}

				if (sibling->_left->_isRed == true && sibling->_right->_isRed == true) {
					// ���� �ڽ� ��� ������ ��
					sibling->_isRed = true;
					parent->_isRed = false;
					if (_isNodeLeftParent == true) {
						sibling->_right->_isRed = false;
						leftRotation(parent, sibling);
					}
					else {
						sibling->_left->_isRed = false;
						rightRotation(parent, sibling);
					}
					return;
				}

				stNode* left = sibling->_left;
				stNode* right = sibling->_right;

				if (_isNodeLeftParent == true) {

					// ��尡 �θ��� ���ʿ� ���� ��
					if (left->_isRed == true) {
						// ������ ���� ��尡 �����̸�
						sibling->_isRed = true;
						left->_isRed = false;
						rightRotation(sibling, left);
						sibling = left;
					}

					// ������ ���� �ڽ��� �����̸�
					leftRotation(parent, sibling);

				}
				else {
					// ��尡 �θ��� �����ʿ� ���� ��
					if (right->_isRed == true) {
						// ������ ���� ��尡 �����̸�
						sibling->_isRed = true;
						right->_isRed = false;
						leftRotation(sibling, right);
						sibling = right;
					}

					// ������ ���� ��尡 �����̸�
					rightRotation(parent, sibling);
				}
				return;
			}

			
		

			return;

		}

		// �θ� ������ ���

		if (sibling->_isRed == false) {
			// ������ ������ ���

			if (sibling->_isNill == true) {
				return;
			}

			if (_isNodeLeftParent == true) {

				// ������ ������ ���� ���
				if (sibling->_left->_isRed == true) {

					// ������ ���� �ڽ��� ������ ���

					// ���� - ������ �ڽ� �� ��ȸ��
					stNode* left = sibling->_left;
					stNode* right = sibling->_right;

					sibling->_isRed = true;
					left->_isRed = false;
					rightRotation(sibling, left);
					sibling = parent->_right;

				}

				if (sibling->_right->_isRed == true) {

					// ������ ���� �ڽ��� ������ ���
					sibling->_right->_isRed = false;
					leftRotation(parent, sibling);
					//eraseBalance(parent);

					return;
				}
			}
			else {
				// ������ ������ ���� ���
				if (sibling->_right->_isRed == true) {
					// ������ ���� �ڽ��� ������ ���

					// ���� - ������ �ڽ� �� ��ȸ��
					stNode* left = sibling->_left;
					stNode* right = sibling->_right;

					sibling->_isRed = true;
					right->_isRed = false;
					leftRotation(sibling, right);
					sibling = parent->_left;
				}
				if (sibling->_left->_isRed == true) {

					// ������ ���� �ڽ��� ������ ���
					sibling->_left->_isRed = false;
					rightRotation(parent, sibling);
					//eraseBalance(parent);

					return;
				}
				
				
			}
			sibling->_isRed = true;
			eraseBalance(parent);
			return;
		}

		if (sibling->_isRed == true) {
			// ������ ������ ���

			sibling->_isRed = false;
			parent->_isRed = true;


			if (_isNodeLeftParent == true) {                                                                                                                                      
				// �θ� ������ ��ȸ��
				sibling->_isRed = false;
				leftRotation(parent, sibling);
			}
			else {

				// �θ� ������ ��ȸ��
				rightRotation(parent, sibling);
			}

			eraseBalance(node);

		}


	}

	void diagnosis(stNode* node = _root) {

		if (node->_left != nullptr) {

			if (node->_left->_parent != node) {
				// left�� �θ� ���� �ƴ� ���
				printf("{\n");
				printf("\t���� ����� �θ� ���� ��尡 �ƴմϴ�.\n");
				printf("\t���� ��� �� : %d\n", node->_data);
				printf("\t���� ��� �� : %d\n", node->_left->_data);
				printf("\t���� ��� �θ� �� : %d\n", node->_left->_parent->_data);
				printf("}\n");
				system("PAUSE>NUL");
			}

			if (node->_isRed == true) {
				if (node->_left->_isRed == true) {

					printf("{\n");
					printf("\t���� �� ���� ��尡 �����Դϴ�.\n");
					printf("\tnode data: %d\n", node->_data);
					printf("}\n");
					system("PAUSE>NUL");
				}
			}

			diagnosis(node->_left);
		}
		else if (node->_isNill == false) {
			// left�� ���ε�, ���� �� ��尡 �ƴϸ� ����
			printf("{\n");
			printf("\t���� ��尡 Null ������ ���� �� ��尡 �ƴմϴ�.\n");
			printf("\tnode data: %d\n", node->_data);
			printf("}\n");
			system("PAUSE>NUL");
		}

		//printf("���� ��ȸ, ���� ��� ��: %d\n", node->_data);

		if (node->_isNill == true) {
			if (node->_isRed == true) {
				// �ҳ�� �� ����
				printf("{\n");
				printf("\tNill ��尡 �������Դϴ�.\n");
				printf("\tnode data: %d\n", node->_data);
				printf("}\n");
				system("PAUSE>NUL");
			}
		}

		if (node->_right != nullptr) {

			if (node->_right->_parent != node) {
				// right�� �θ� ���� �ƴ� ���
				printf("{\n");
				printf("\t���� ����� �θ� ���� ��尡 �ƴմϴ�.\n");
				printf("\t���� ��� �� : %d\n", node->_data);
				printf("\t���� ��� �� : %d\n", node->_right->_data);
				printf("\t���� ��� �θ� �� : %d\n", node->_right->_parent->_data);
				printf("}\n");
				system("PAUSE>NUL");
			}

			if (node->_isRed == true) {
				if (node->_right->_isRed == true) {

					printf("{\n");
					printf("\t���� �� ���� ��尡 �����Դϴ�.\n");
					printf("\tnode data: %d\n", node->_data);
					printf("}\n");
					system("PAUSE>NUL");
				}
			}

			diagnosis(node->_right);
		}
		else if (node->_isNill == false) {
			// right�� ���ε�, ���� �� ��尡 �ƴϸ� ����
			printf("{\n");
			printf("\t���� ��尡 Null ������ ���� �� ��尡 �ƴմϴ�.\n");
			printf("\tnode data: %d\n", node->_data);
			printf("}\n");
			system("PAUSE>NUL");
		}

	}

	void treeToList(std::vector<T>* vector, std::vector<T>* blackNumList, stNode* node, int blackNum = 0) {
		blackNum += (node->_isRed == false);
		if (node->_left->_isNill == false) {
			treeToList(vector, blackNumList, node->_left, blackNum);
		}
		else {
			blackNumList->push_back(blackNum);
		}

		vector->push_back(node->_data);

		if (node->_right->_isNill == false) {
			treeToList(vector, blackNumList, node->_right, blackNum );
		}
		else {
			blackNumList->push_back(blackNum);
		}

	}

#ifdef _WINDOWS_
	HDC _hdc;
	int printLoop(stNode* node, int deepth, int* printCnt, int x) {

		if (node->_left->_isNill == false) {
			int leftPrintCnt = printLoop(node->_left, deepth + 1, printCnt, x);

			// ���� ����� ���� ������
			int leftLineX = leftPrintCnt * 80 + 80 - (80 * 0.14); // right
			int leftLineY = (deepth + 1) * 80 + (80 * 0.14); // top

			// ���� ����� ���� ������
			int nodeLineX = *printCnt * 80 + (80 * 0.14); // left
			int nodeLineY = deepth * 80 + 80 - (80 * 0.14); // bottom

			MoveToEx(_hdc, leftLineX - x, leftLineY, nullptr);
			LineTo(_hdc, nodeLineX - x, nodeLineY);

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
		Ellipse(_hdc, ellipseRect.left - x, ellipseRect.top, ellipseRect.right - x, ellipseRect.bottom);
		SelectObject(_hdc, oldEllipseBrush);
		DeleteObject(ellipseBrush);

		WCHAR text[10] = { 0, };
		_itow_s(node->_data, text, 10, 10);
		TextOutW(_hdc, ellipseRect.left + 40 - x, ellipseRect.top + 40, text, wcslen(text));

		int nodePrintCnt = *printCnt;
		*printCnt += 1;

		if (node->_right->_isNill == false) {
			int rightPrintCnt = printLoop(node->_right, deepth + 1, printCnt, x);

			// ���� ����� ���� ������
			int rightLineX = rightPrintCnt * 80 + (80 * 0.14); // left
			int rightLineY = (deepth + 1) * 80 + (80 * 0.14); // top

			// ����� ���� ������
			int nodeLineX = nodePrintCnt * 80 + 80 - (80 * 0.14); // right
			int nodeLineY = deepth * 80 + 80 - (80 * 0.14); // bottom

			MoveToEx(_hdc, rightLineX - x, rightLineY, nullptr);
			LineTo(_hdc, nodeLineX - x, nodeLineY);

		}

		return nodePrintCnt;

	}
#endif
};