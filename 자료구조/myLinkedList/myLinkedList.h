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
			int capacity = 10 // capacity 만큼 arr의 크기 확보합니다.
		);

		void insert(
			T* value, // 값을 저장할 데이터
			_node<T>* parent = nullptr // 이 노드의 자식으로 저장됩니다. null 이라면 해당 value는 첫번째 노드가 됩니다.
		);

		void erase(
			_node<T>* node // 지울 노드를 전달합니다.
		);

		_node<T>* getFirstNode() {
			return arr[firstIdx];
		}


		~linkedList();

	private:

		// capacity 만큼 arr 크기를 변경합니다.
		void resize(int capacity);


	private:
		_node<T>** arr = nullptr; // 각 노드 주소값이 담길 배열
		int capacity = 0;		  // capacity 만큼 arr 크기 확보합니다.
		int firstIdx = -1;		  // root node의 인덱스

		// arr에서 현재 사용하지 않는 인덱스를 저장합니다.
		// 인덱스가 필요할 때 pop 해서 사용합니다.
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
	// value에 대한 할당 해제를 보장하지 않음.
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

	// 추가된 노드 인덱스 스택에 추가
	for (int idxCnt = this->capacity; idxCnt < capacity; ++idxCnt) {
		notUsedIdx.push(idxCnt);
	}

	// 최초 생성 시에만 return 합니다.
	if (beforeArr == nullptr) {
		return;
	}

	// 이전 노드를 다시 복사
	for (int arrCnt = 0; arrCnt < this->capacity; arrCnt++) {
		delete arr[arrCnt];
		arr[arrCnt] = ((_node<T>**)beforeArr)[arrCnt];
	}


}

template<typename T>
void my::linkedList<T>::insert(T* value, _node<T>* parent) {

	unsigned int stackSize = notUsedIdx.getSize();
	// arr 공간이 부족한 경우
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
		
	// 소멸자에서 중복 제거하지 않도록
	arr[node->index] = nullptr;

	delete node;
}