#include <iostream>
#include <time.h>
#include <stack>
#include "myStack.h"

using namespace std;


int main() {

	const int testSize = 10000000;

	std::stack<int> stdStack;
	my::stack myStack(testSize);
	my::variableLengthStack myVLStack(0);

	time_t start;
	time_t end;

	start = clock();
	for (int i = 0; i < testSize; i++) {
		stdStack.push(i);
	}
	end = clock();

	cout << end - start << "\n";

	start = clock();
	for (int i = 0; i < testSize; i++) {
		myStack.push(i);
	}
	end = clock();

	cout << end - start << "\n";


	start = clock();
	for (int i = 0; i < testSize; i++) {
		myVLStack.push(i);
	}
	end = clock();

	cout << end - start << "\n";

	return 0;
}