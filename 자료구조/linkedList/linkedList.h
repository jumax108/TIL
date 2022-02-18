#pragma once

template<typename T>
class CLinkedList {
public:

	struct stNode {

	public:
		inline stNode* operator=(const stNode& const node);
		inline bool operator==(const stNode& const node);
		inline bool operator!=(const stNode& const node);

	private:
		T _value;
		stNode* _parent;
		stNode* _child;
		friend class CIterator;
	};

	class CIterator {
	public:
		CIterator();
		CIterator(const stNode* const node);

		inline CIterator& operator=(const stNode& node);
		inline T& operator*();

		inline CIterator& operator++();
		inline CIterator& operator--();

		inline bool operator!=(const stNode* const node);
		inline bool operator==(const stNode* const node);

	private:
		stNode* _node;
	};

	CLinkedList();

	void push_front(
		const T const value // 저장할 값
	);
	void push_back(
		const T const value // 저장할 값
	);

	void insert(
		const T const value, // 값을 저장할 데이터
		stNode* parent  // 이 노드의 자식으로 저장됩니다.
	);

	CIterator& erase(
		CIterator& iter // 지울 노드를 전달합니다.
	);

	void clear();

	inline stNode* begin() {
		return head.child;
	}

	inline stNode* end() {
		return &tail;
	}

	~CLinkedList();



private:
	stNode head;
	stNode tail;

};

template<typename T>
CLinkedList<T>::CLinkedList(){
	
	head.parent = nullptr;
	head.child = &tail;

	tail.parent = &head;
	tail.child = nullptr;
}

template<typename T>
CLinkedList<T>::~CLinkedList() {
	clear();
}

template<typename T>
void CLinkedList<T>::insert(T value, CLinkedList<T>::stNode* parent) {

	stNode* node = (CLinkedList<T>::stNode*)malloc(sizeof(CLinkedList<T>::stNode));

	node->value = value;
	node->parent = parent;

	parent->child->parent = node;
	node->child = parent->child;
	parent->child = node;
	
}

template<typename T>
void CLinkedList<T>::push_front(const T const value) {
	stNode* node = (CLinkedList<T>::stNode*)malloc(sizeof(CLinkedList<T>::stNode));

	node->value = value;

	node->parent = &head;
	node->child = head.child;

	head->child->parent = node;
	head->child = node;
}

template<typename T>
void CLinkedList<T>::push_back(const T const value) {
	stNode* node = (CLinkedList<T>::stNode*)malloc(sizeof(CLinkedList<T>::stNode));

	node->value = value;
	node->parent = tail.parent;
	node->child = &tail;

	tail.parent->child = node;
	tail.parent = node;
}

template<typename T>
void CLinkedList<T>::clear() {
	for (CLinkedList<T>::CIterator iter = begin(); iter != end(); ) {
		iter = erase(iter);
	}
}

template<typename T>
CLinkedList<T>::CIterator& CLinkedList<T>::erase(
	CIterator& iter // 지울 노드를 전달합니다.
) {

	CLinkedList<T>::stNode* node = iter.node;
	CLinkedList<T>::stNode* child = node->child;

	node->child->parent = node->parent;
	node->parent->child = node->child;

	CIterator nextIter = ++iter;

	free(node);

	return nextIter;
}

template <typename T>
CLinkedList<T>::stNode* CLinkedList<T>::stNode::operator=(const stNode& const node){
	_value	= node._value	;
	_parent = node._parent	;
	_child	= node._child	;

	return this;
}

template <typename T>
bool CLinkedList<T>::stNode::operator==(const stNode& const node) {
	return	_value  == node._value  &&
			_parent == node._parent &&
			_child  == node._child;
}

template <typename T>
bool CLinkedList<T>::stNode::operator!=(const stNode& const node) {
	return	_value	!= node._value	||
			_parent != node._parent ||
			_child	!= node._child;
}

template <typename T>
CLinkedList<T>::CIterator::CIterator() {
	_node = nullptr;
}

template <typename T>
CLinkedList<T>::CIterator::CIterator(const stNode* node) {
	_node = (stNode*)node;
}

template <typename T>
CLinkedList<T>::CIterator& CLinkedList<T>::CIterator::operator=(const stNode& node) {
	_node->operator=(node);
	return this;
}

template <typename T>
T& CLinkedList<T>::CIterator::operator*() {
	return _node->value;
}

template <typename T>
CLinkedList<T>::CIterator& CLinkedList<T>::CIterator::operator++() {
	_node = _node->child;
	return *this;
}

template <typename T>
CLinkedList<T>::CIterator& CLinkedList<T>::CIterator::operator--() {
	_node = _node->parent;
	return *this;
}

template <typename T>
bool CLinkedList<T>::CIterator::operator!=(const stNode* const node) {
	return *(this->_node) != *node;
}

template <typename T>
bool CLinkedList<T>::CIterator::operator==(const stNode* const node) {
	return *(this->_node) == *node;
}