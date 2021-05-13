#pragma once

#include "myStack.h"

namespace my {

	template<typename T>
	struct _node {
		T* value;
		_node<T>* parent;
		_node<T>* child;
		int index = -1;
	};
	
	template<typename T>
	class linkedList {
	public:
		linkedList(
			int capacity = 10 // capacity ��ŭ arr�� ũ�� Ȯ���մϴ�.
		);

		void insert(
			T* value, // ���� ������ ������
			_node<T>* parent = nullptr // �� ����� �ڽ����� ����˴ϴ�. null �̶�� �ش� value�� ù��° ��尡 �˴ϴ�.
		);

		void erase(
			_node<T>* node // ���� ��带 �����մϴ�.
		);

		_node<T>* getFirstNode() {
			return arr[firstIdx];
		}


		~linkedList();

	private:

		// capacity ��ŭ arr ũ�⸦ �����մϴ�.
		void resize(int capacity);


	private:
		_node<T>** arr = nullptr; // �� ��� �ּҰ��� ��� �迭
		int capacity = 0;		  // capacity ��ŭ arr ũ�� Ȯ���մϴ�.
		int firstIdx = -1;		  // root node�� �ε���

		// arr���� ���� ������� �ʴ� �ε����� �����մϴ�.
		// �ε����� �ʿ��� �� pop �ؼ� ����մϴ�.
		stack<int> notUsedIdx;
	};
}

template<typename T>
my::linkedList<T>::linkedList(int capacity) : notUsedIdx(capacity) {
	resize(capacity);
	this->capacity = capacity;
}

template<typename T>
my::linkedList<T>::~linkedList() {
	// value�� ���� �Ҵ� ������ �������� ����.
	for (int arrCnt = 0; arrCnt < capacity; ++arrCnt) {
		delete arr[arrCnt];
	}
	delete[] arr;
}

template<typename T>
void my::linkedList<T>::resize(int capacity) {
	void* beforeArr = arr;

	arr = new _node<T>*[capacity];
	for (int arrCnt = 0; arrCnt < capacity; arrCnt++) {
		arr[arrCnt] = new _node<T>;
	}

	// �߰��� ��� �ε��� ���ÿ� �߰�
	for (int idxCnt = this->capacity; idxCnt < capacity; ++idxCnt) {
		notUsedIdx.push(idxCnt);
	}

	// ���� ���� �ÿ��� return �մϴ�.
	if (beforeArr == nullptr) {
		return;
	}

	// ���� ��带 �ٽ� ����
	for (int arrCnt = 0; arrCnt < this->capacity; arrCnt++) {
		delete arr[arrCnt];
		arr[arrCnt] = ((_node<T>**)beforeArr)[arrCnt];
	}


}

template<typename T>
void my::linkedList<T>::insert(T* value, _node<T>* parent) {

	unsigned int stackSize = notUsedIdx.getSize();
	// arr ������ ������ ���
	if (stackSize == 0) {
		resize(capacity * 2);
		capacity *= 2;
	}

	int idx = notUsedIdx.front();
	notUsedIdx.pop();

	arr[idx]->value = value;
	arr[idx]->parent = parent;
	arr[idx]->index = idx;

	if (parent == nullptr) {
		arr[idx]->child = nullptr;
		if (firstIdx != -1) {
			arr[idx]->child = arr[firstIdx];
			arr[firstIdx]->parent = arr[idx];
		}
		firstIdx = idx;
	}
	else {
		parent->child->parent = arr[idx];
		arr[idx]->child = parent->child;
		parent->child = arr[idx];
	}
}

template<typename T>
void my::linkedList<T>::erase(_node<T>* node) {
	notUsedIdx.push(node->index);

	if(node->child != nullptr)
		node->child->parent = node->parent;
	if(node->parent != nullptr)
		node->parent->child = node->child;
		
	// �Ҹ��ڿ��� �ߺ� �������� �ʵ���
	arr[node->index] = nullptr;

	delete node;
}