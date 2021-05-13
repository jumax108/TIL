#include <stdio.h>
#include "myLinkedList.h"

int main() {

	my::linkedList<int> list(1);

	list.insert(new int(1));
	list.insert(new int(2));
	list.insert(new int(3));

	delete list.getFirstNode()->child->value;
	list.erase(list.getFirstNode()->child);

	delete list.getFirstNode()->child->value;
	list.erase(list.getFirstNode()->child);

	delete list.getFirstNode()->value;
	list.erase(list.getFirstNode());

	return 0;
}