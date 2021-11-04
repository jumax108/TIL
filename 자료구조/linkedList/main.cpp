#include <stdio.h>
#include <malloc.h>
#include "linkedList.h"

int main() {

	/*std::list<int> list;

	for (int i = 0; i < 10; i++) {
		list.push_back(i + 1);
	}

	for (std::list<int>::iterator iter = list.begin(); iter != list.end(); ++iter) {

		printf("%d\n", *iter);
	}*/
		
	
	linkedList<int> list;

	list.push_back(1);
	list.push_back(2);
	list.push_back(3);
	list.push_back(4);
	list.push_back(5);
	list.push_back(6);
	list.push_back(7);
	list.push_back(8);
	list.push_back(9);
	list.push_back(10);

	for (linkedList<int>::iterator iter = list.begin(); iter != list.end(); ++iter) {
		printf("%d\n", *iter);
	}

	list.clear();
	list.push_back(5);
	list.push_back(6);
	for (linkedList<int>::iterator iter = list.begin(); iter != list.end(); ++iter) {
		printf("%d\n", *iter);
	}


	return 0;
}