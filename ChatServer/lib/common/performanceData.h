#pragma once

#include <stdio.h>
#include <pdh.h>
#include "cpuUsage.h"

#pragma comment(lib, "pdh.lib")

class CPerformanceData: public CCpuUsage{

public:

	CPerformanceData(const wchar_t* processName);

	void update();

	unsigned __int64 getProcessPrivateMemory();
	unsigned __int64 getProcessNonPagedMemory();
	unsigned __int64 getAvailableMemory();
	unsigned __int64 getNonPagedMemory();

private:
	
	const wchar_t* _processName;

	PDH_HQUERY _query;

	PDH_HCOUNTER _processPrivateMemoryCounter;
	PDH_HCOUNTER _processNonPagedMemoryCounter;
	
	PDH_HCOUNTER _availableMemoryCounter;
	PDH_HCOUNTER _nonPagedMemoryCounter;

	unsigned __int64 _processPrivateMemory;
	unsigned __int64 _processNonPagedMemory;

	unsigned __int64 _availableMemory;
	unsigned __int64 _nonPagedMemory;

};
