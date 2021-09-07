#pragma once

template<typename T>
class CRedBlackTree{

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
			_root = newNode;
			return;
		}

		stNode* node = _root;
		while (node->_data != data) {
			if (node->_data < data) {
				if (node->_right->_isNill == true) {
					delete(node->_right);
					node->_right = newNode;
					newNode->parent = node;
					return;
				}
				node = node->_right;
			}
			else if (node->_data > data) {
				if (node->_left->_isNill == true) {
					delete(node->_left);
					node->left = newNode;
					newNode->parent = node;
					return;
				}
				node = node->_left;
			}
		}

		delete(newNode);
	}

	void erase(T data) {
	}



private:

	stNode* _root;
		

};