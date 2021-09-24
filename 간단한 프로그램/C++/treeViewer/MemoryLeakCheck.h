#pragma once


namespace __MLC {


#pragma region  struct

	struct MemInfo {

		void* ptr;
		char* fileName;
		int line;

	};

	using valueType = MemInfo;
	using ptrType = MemInfo*;

	struct Node {

		ptrType value;
		Node* parent;
		Node* child;
		int idx;

	};
#pragma endregion

	class Stack {

	public:
		Stack(int cap = 1) {
			arr = (int*)malloc(sizeof(int) * cap);
			this->cap = cap;
		}

		void push(const int in) {
			if (cap == size) {
				resize(cap * 2);
				cap *= 2;
			}

			arr[size] = in;
			size += 1;
		}

		inline void pop() {
			size -= 1;
		}

		inline int front() {
			return arr[size - 1];
		}

		inline int getSize() {
			return size;
		}

#pragma region private func
	private:
		void resize(int nextCap) {
			int* prevArr = arr;

			arr = (int*)malloc(sizeof(int) * nextCap);

			memcpy_s(arr, sizeof(int) * nextCap, prevArr, sizeof(int) * cap);

			free(prevArr);

		}
#pragma endregion

#pragma region private var
	private:
		int* arr;
		int cap;
		int size;
#pragma endregion

	};


	class List {

	public:

		Node* first;

		List(int cap = 1) {
			this->cap = cap;
			allocArr(cap);
		}

		void insert(ptrType value, Node* parent = nullptr) {

			if (notUsedIdx.getSize() == 0) {
				resize(cap * 2);
				cap *= 2;
			}

			int idx = notUsedIdx.front();
			notUsedIdx.pop();

			arr[idx]->value = (ptrType)malloc(sizeof(valueType));
			memcpy_s(arr[idx]->value, sizeof(valueType), value, sizeof(valueType));
			arr[idx]->parent = parent;
			arr[idx]->idx = idx;

			if (parent == nullptr) {
				arr[idx]->child = first;
				if (arr[idx]->child != nullptr)
					arr[idx]->child->parent = arr[idx];
				first = arr[idx];
				return;
			}

			if (parent->child != nullptr) {
				arr[idx]->child = parent->child;
				parent->child->parent = arr[idx];
				parent->child = arr[idx];
			}
		}

		void erase(Node* node) {
			notUsedIdx.push(node->idx);

			Node* parent = node->parent;
			Node* child = node->child;

			if (first == node) {
				first = child;
			}

			if (parent != nullptr) {
				parent->child = child;
			}

			if (child != nullptr) {
				child->parent = parent;
			}
			free(node->value->ptr);

		}

		__declspec(noinline) Node* find(const void* ptr) {
			if (first == nullptr) {
				return nullptr;
			}
			for (Node* iter = first; iter != nullptr; iter = iter->child) {
				if (iter->value->ptr == ptr) {
					return iter;
				}

				if (iter->child == nullptr) {
					break;
				}
			}
			return nullptr;
		}

#pragma region private func

		void allocArr(int nextCap) {

			this->arr = (Node**)malloc(sizeof(Node*) * nextCap);
			ZeroMemory(arr, sizeof(Node*) * nextCap);

			for (int idxCnt = 0; idxCnt < nextCap; idxCnt++) {
				if (idxCnt >= cap) {
					this->arr[idxCnt] = (Node*)malloc(sizeof(Node));
					notUsedIdx.push(idxCnt);
				}
			}
		}

		void resize(int nextCap) {
			Node** beforeArr = arr;

			allocArr(nextCap);
			/*
			for (int idxCnt = 0; idxCnt < cap; idxCnt++) {
				arr[idxCnt]->idx = beforeArr[idxCnt]->idx;
				arr[idxCnt]->parent = beforeArr[idxCnt]->parent;
				arr[idxCnt]->child = beforeArr[idxCnt]->child;
				arr[idxCnt]->value = beforeArr[idxCnt]->value;
			}*/

			memcpy_s(arr, sizeof(Node*) * nextCap, beforeArr, sizeof(Node*) * cap);

			free(beforeArr);
			///ZeroMemory(beforeArr, sizeof(Node*) * cap);

		}

#pragma endregion

#pragma region private var
	private:

		Node** arr;
		int cap;
		Stack notUsedIdx;

#pragma endregion
	};
}

class MemoryLeakChecker {

public:

	MemoryLeakChecker() {
		FILE* logFile;
		tm* now = (tm*)malloc(sizeof(tm));
		__time64_t longTime;
		_time64(&longTime);
		localtime_s(now, &longTime);

		sprintf_s(fileName, "mlcLog%04d%02d%02d_%02d%02d%02d.txt", now->tm_year, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

		fopen_s(&logFile, fileName, "w");
		fclose(logFile);

	}

	~MemoryLeakChecker() {
		FILE* logFile;
		fopen_s(&logFile, fileName, "a");
		for (__MLC::Node* iter = adr.first; iter != nullptr; iter = iter->child) {
			printf_s("[LEAK] %d %s %d\n", iter->value->ptr, iter->value->fileName, iter->value->line);
			fprintf_s(logFile, "[LEAK] 0x%x %s %d\n", iter->value->ptr, iter->value->fileName, iter->value->line);
			if (iter->child == nullptr) {
				break;
			}
		}

	}

	__MLC::List adr;

	char fileName[26];

};

MemoryLeakChecker* mlc = new MemoryLeakChecker();

void* operator new(size_t size, const char* file, int line) {
	void* ptr = malloc(size);

	__MLC::MemInfo* info = (__MLC::MemInfo*)malloc(sizeof(__MLC::MemInfo));
	int fileNameLen = strlen(file);
	info->fileName = (char*)malloc(fileNameLen + 1);
	memcpy_s(info->fileName, fileNameLen, file, fileNameLen);
	info->fileName[fileNameLen] = '\0';

	info->line = line;
	info->ptr = ptr;

	mlc->adr.insert(info, nullptr);

	return ptr;
}

void operator delete(void* ptr, char* file, int line) {}

void operator delete(void* ptr) {
	__MLC::Node* node = nullptr;
	node = mlc->adr.find(ptr);

	if (node == nullptr) {
		// 미할당 구간 해제 시도

		FILE* logFile;
		fopen_s(&logFile, mlc->fileName, "a");
		fprintf_s(logFile, "NOALLOC [0x%x]\n", ptr);
		fclose(logFile);

		return;
	}

	mlc->adr.erase(node);
	free(node->value);
}


#define new new(__FILE__, __LINE__)
