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

private:

	stNode* root;

	void printLoop(stNode* node) {

		if (node->left != nullptr) {
			printLoop(node->left);
		}
		printf("%d\n",node->data);
		if (node->right != nullptr) {
			printLoop(node->right);
		}
	}

	void eraseNode(stNode** node) {

		stNode** left = &(*node)->left;
		stNode** right = &(*node)->right;

		if (*left == nullptr && *right == nullptr) {
			delete(*node);
			*node = nullptr;
		}
		else if (*left != nullptr) {
			stNode* eraseNodeParent = *node;
			stNode* findEraseNode = *left;

			while (findEraseNode->right != nullptr) {
				eraseNodeParent = findEraseNode;
				findEraseNode = findEraseNode->right;
			}

			(*node)->data = findEraseNode->data;
			eraseNodeParent->right = findEraseNode->left;

			delete(findEraseNode);
			findEraseNode = nullptr;
		}
		else {

			stNode* eraseNodeParent = *node;
			stNode* findEraseNode = *right;

			while (findEraseNode->left != nullptr) {
				eraseNodeParent = findEraseNode;
				findEraseNode = findEraseNode->left;
			}

			(*node)->data = findEraseNode->data;
			eraseNodeParent->left = findEraseNode->right;

			delete(findEraseNode);
			findEraseNode = nullptr;
		}
	}

};