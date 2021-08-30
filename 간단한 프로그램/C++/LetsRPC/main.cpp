#include <stdio.h>
#include <malloc.h>

#include "LinkedList.h"
#include "TextParser.h"
#include "LetsRPC.h"

int main() {

	CLetsRPC rpc;

	rpc.process(L"msg.txt");

	return 0;

}