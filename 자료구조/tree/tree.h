#pragma once


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
		while (*node != nullptr) {
			
			if ((*node)->data == data) {
				delete addNode;
				return;
			}

			if ((*node)->data > data) {
				
				node = &(*node)->left;
			}
			else {
				node = &(*node)->right;
			}
		}

		*node = addNode;

	}

	void erase(T data) {

		stNode** node = &root;
		while (*node != nullptr) {

			if ((*node)->data == data) {
				eraseNode(node);
				return;
			}

			if ((*node)->data > data) {

				node = &(*node)->left;
			}
			else {
				node = &(*node)->right;
			}
		}

	}

	void print() {

		printLoop(root);

	}

	static void test() {

		CBinaryTree<int> tree;
		std::vector<int> addList;
		std::vector<int> eraseList;

		constexpr int addNum = 500;
		constexpr int eraseNum = 485;


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
			tree.insert(*addIter);
		}

		printf("Erase Index\n ");
		for (std::vector<int>::iterator eraseIter = eraseList.begin(); eraseIter != eraseList.end(); ++eraseIter) {
			printf("%d ", *eraseIter);
		}
		printf("\n");

		for (std::vector<int>::iterator eraseIter = eraseList.begin(); eraseIter != eraseList.end(); ++eraseIter) {
		//	printf("erase num: %d\n", addList[*eraseIter]);
			tree.erase(addList[*eraseIter]);
			addList.erase(addList.begin() + *eraseIter);
		}

		printf("Correct Answer\n ");
		std::sort(addList.begin(), addList.end());
		for (std::vector<int>::iterator addIter = addList.begin(); addIter != addList.end(); ++addIter) {
			printf("%d ", *addIter);
		}
		printf("\n");

		printf("Actual Tree Value\n ");
		tree.print();

		return;
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


	void eraseNode(stNode** node) {

		stNode* left = (*node)->left;
		stNode* right = (*node)->right;

		if (left == nullptr && right == nullptr) {
			delete(*node);
			*node = nullptr;
		}
		else if (left != nullptr) {
			stNode* eraseNodeParent = *node;
			stNode* findEraseNode = left;

			while (findEraseNode->right != nullptr) {
				eraseNodeParent = findEraseNode;
				findEraseNode = findEraseNode->right;
			}

			(*node)->data = findEraseNode->data;
			if (eraseNodeParent != *node) {
				eraseNodeParent->right = findEraseNode->left;
			}
			else {
				eraseNodeParent->left = findEraseNode->left;
			}

			delete(findEraseNode);
		}
		else {

			stNode* eraseNodeParent = *node;
			stNode* findEraseNode = right;

			while (findEraseNode->left != nullptr) {
				eraseNodeParent = findEraseNode;
				findEraseNode = findEraseNode->left;
			}
			(*node)->data = findEraseNode->data;
			if (eraseNodeParent != *node) {
				eraseNodeParent->left = findEraseNode->right;
			}
			else {
				eraseNodeParent->right = findEraseNode->right;
			}

			delete(findEraseNode);
		
		}
	}

};