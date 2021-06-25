#include "MovingStar.h"

bool MovingStar::update() {

	if (x + width >= MAX_WIDTH) {
		return false;
	}


	x += width;
	return true;
}

void MovingStar::render() {
	memset(buffer[y], ' ', x);
	memset(&buffer[y][x], '*', width);
	return;
}