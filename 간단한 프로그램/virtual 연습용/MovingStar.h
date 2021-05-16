#pragma once
#include <malloc.h>
#include <memory.h>
#include "BaseObject.h"
#include "MessageStack.h"

extern cMessageStack msgStack;
extern char buffer[10][31];

class MovingStar :
    public BaseObject
{
public:
    MovingStar(int num, int y) {
        this->width = num;
        this->y = y;
        this->x = 0;
    }

    bool update();
    void render();


#pragma region private var

private:
    const unsigned int MAX_WIDTH = 30;

    int y;
    int width;

#pragma endregion
};

