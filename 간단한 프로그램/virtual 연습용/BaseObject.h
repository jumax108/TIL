#pragma once

class BaseObject
{
public:
	virtual bool update() = 0;
	virtual void render() = 0;
protected:
	int x;
};

