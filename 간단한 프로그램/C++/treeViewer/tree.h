#pragma once

class SimpleProfiler;


template<typename T>
class CBinaryTree {

public:

	CBinaryTree() {
		root = nullptr;
	}

	struct stNode {

		stNode(T data) {
			this->data = data;
			left = nullptr;
			right = nullptr;
		}

		T data;
		stNode* left;
		stNode* right;
	};

	void insert(T data) {

		stNode* addNode = new stNode(data);

		if (root == nullptr) {
			root = addNode;
			return;
		}

		stNode** node = &root;
		while ((*node) != nullptr) {
			
			if (((*node))->data == data) {
				delete addNode;
				return;
			}

			if (((*node))->data > data) {
				
				node = &((*node))->left;
			}
			else {
				node = &((*node))->right;
			}
		}

		(*node) = addNode;

	}

	void erase(T data) {

		stNode** node = &root;
		while ((*node) != nullptr) {

			if (((*node))->data == data) {
				eraseNode(node);
				return;
			}

			if (((*node))->data > data) {

				node = &((*node))->left;
			}
			else {
				node = &((*node))->right;
			}
		}

	}

	void print() {

		if (root == nullptr) {
			return;
		}

		printLoop(root);

	}

	void print(HDC hdc) {

		if (root == nullptr) {
			return;
		}

		printLoop(root, hdc, 1, new int(0));

	}


	static CBinaryTree<int>* test(HDC hdc, HWND hWnd) {

		CBinaryTree<int>* tree = new CBinaryTree<int>();
		std::vector<int> addList;
		std::vector<int> eraseList;

		constexpr int addNum = 100;
		constexpr int eraseNum = 90;


		srand(1000);

		std::vector<int> valueList;
		for (int value = 1; value <= addNum; ++value) {
			valueList.push_back(value);
		}

		for (int addCnt = 0; addCnt < addNum; ++addCnt) {
			int index = rand() % (valueList.size());
			addList.push_back(valueList[index]);
			valueList.erase(valueList.begin() + index);
		}

		for (int eraseCnt = 0; eraseCnt < eraseNum; ++eraseCnt) {
			eraseList.push_back(rand() % (addNum - eraseCnt));
		}

		printf("Add Num\n ");
		for (std::vector<int>::iterator addIter = addList.begin(); addIter != addList.end(); ++addIter) {
			printf("%d ", *addIter);
		}
		printf("\n");

		for (std::vector<int>::iterator addIter = addList.begin(); addIter != addList.end(); ++addIter) {
			tree->insert(*addIter);
		}

		printf("Erase Index\n ");
		for (std::vector<int>::iterator eraseIter = eraseList.begin(); eraseIter != eraseList.end(); ++eraseIter) {
			printf("%d ", *eraseIter);
		}
		printf("\n");
		tree->print(hdc);
		for (std::vector<int>::iterator eraseIter = eraseList.begin(); eraseIter != eraseList.end(); ++eraseIter) {
			tree->erase(addList[*eraseIter]);
			addList.erase(addList.begin() + *eraseIter);


			HBRUSH oldBrush;
			HBRUSH greenBrush = CreateSolidBrush(RGB(20, 120, 50));
			oldBrush = (HBRUSH)SelectObject(hdc, greenBrush);
			Rectangle(hdc, 0, 0, 1920, 1080);
			SelectObject(hdc, oldBrush);
			DeleteObject(greenBrush);

			tree->print(hdc);
		}

		printf("Correct Answer\n ");
		std::sort(addList.begin(), addList.end());
		for (std::vector<int>::iterator addIter = addList.begin(); addIter != addList.end(); ++addIter) {
			printf("%d ", *addIter);
		}
		printf("\n");

		printf("Actual Tree Value\n ");
		tree->print();

		return tree;
	}
private:

	stNode* root;

	void printLoop(stNode* node) {

		if (node->left != nullptr) {
			printLoop(node->left);
		}
		printf("%d ",node->data);
		if (node->right != nullptr) {
			printLoop(node->right);
		}
	}

	int printLoop(stNode* node, HDC hdc, int deepth, int* printCnt) {


		if (node->left != nullptr) {

			int leftNodePrintCnt = printLoop(node->left, hdc, deepth + 1, printCnt);

			int lineNodeX = (*printCnt * 2 + 1) * 40 + 30;
			int lineNodeY = deepth * 85 + 40;
			int lineLeftNodeX = (leftNodePrintCnt * 2 + 1) * 40 + 80 - (80 * 0.14);
			int lineLeftNodeY = (deepth + 1) * 85 + (80 * 0.14);

			MoveToEx(hdc, lineNodeX, lineNodeY, nullptr);
			LineTo(hdc, lineLeftNodeX, lineLeftNodeY);
		}

		int nodePrintCnt = *printCnt;
		int left = (*printCnt * 2 + 1) * 40;
		int top = deepth * 85;
		Ellipse(hdc, left, top, left + 80, top + 80);
		char buf[5];
		ZeroMemory(buf, 5);
		_itoa_s(node->data, buf, 10);
		TextOutA(hdc, left + 30, top + 40, buf, strlen(buf));

		*printCnt += 1;
		if (node->right != nullptr) {

			int rightNodePrintCnt = printLoop(node->right, hdc, deepth + 1, printCnt);

			int lineNodeX = (nodePrintCnt * 2 + 1) * 40 + (80 * 0.86);
			int lineNodeY = deepth * 85 + (80 * 0.86);
			int lineRightNodeX = (rightNodePrintCnt * 2 + 1) * 40 + (80 * 0.14);
			int lineRightNodeY = (deepth + 1) * 85 + (80 * 0.14);

			MoveToEx(hdc, lineNodeX, lineNodeY, nullptr);
			LineTo(hdc, lineRightNodeX, lineRightNodeY);
		}
		return nodePrintCnt;
	}
	void eraseNode(stNode** node) {

		stNode* left = ((*node))->left;
		stNode* right = ((*node))->right;

		if (left == nullptr && right == nullptr) {
			delete((*node));
			(*node) = nullptr;
		}
		else if (left != nullptr) {
			stNode* eraseNodeParent = (*node);
			stNode* findEraseNode = left;

			while (findEraseNode->right != nullptr) {
				eraseNodeParent = findEraseNode;
				findEraseNode = findEraseNode->right;
			}

			((*node))->data = findEraseNode->data;
			if (eraseNodeParent != (*node)) {
				eraseNodeParent->right = findEraseNode->left;
			}
			else {
				eraseNodeParent->left = findEraseNode->left;
			}

			delete(findEraseNode);
		}
		else {

			stNode* eraseNodeParent = (*node);
			stNode* findEraseNode = right;

			while (findEraseNode->left != nullptr) {
				eraseNodeParent = findEraseNode;
				findEraseNode = findEraseNode->left;
			}
			((*node))->data = findEraseNode->data;
			if (eraseNodeParent != (*node)) {
				eraseNodeParent->left = findEraseNode->right;
			}
			else {
				eraseNodeParent->right = findEraseNode->right;
			}

			delete(findEraseNode);

		}
	}


};