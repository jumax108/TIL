#include <stdio.h>

#include "tree.h"

int main() {

	CBinaryTree<int> tree;

	tree.insert(5);
	tree.insert(3);
	tree.insert(20);
	tree.insert(1);
	tree.insert(6);
	tree.insert(8);
	tree.insert(9);
	tree.insert(15);
	tree.insert(13);

	tree.erase(20);

	tree.print();

	return 0;
}