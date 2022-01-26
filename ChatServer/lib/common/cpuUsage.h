#pragma once

#include <Windows.h>

class CCpuUsage{

public:

	CCpuUsage(HANDLE process = INVALID_HANDLE_VALUE);

	void  updateCpuTime();

	float processorTotal();
	float processorUser();
	float processorKernel();

	float processTotal();
	float processUser();
	float processKernel();

private:

	HANDLE _process;
	int _processorNum;

	float _processorTotal;
	float _processorUser;
	float _processorKernel;

	float _processTotal;
	float _processUser;
	float _processKernel;

	unsigned __int64 _processorLastKernel;
	unsigned __int64 _processorLastUser;
	unsigned __int64 _processorLastIdle;
	
	unsigned __int64 _processLastKernel;
	unsigned __int64 _processLastUser;
	unsigned __int64 _processLastTime;



};