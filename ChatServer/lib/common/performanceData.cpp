
#pragma once

#include "performanceData.h"

CPerformanceData::CPerformanceData(const wchar_t* processName){
	_processName = processName;

	PdhOpenQuery(NULL, NULL, &_query);

	wchar_t* queryString = new wchar_t[100];
	
	swprintf_s(queryString, 100, L"\\Process(%s)\\Private Bytes", _processName);
	PdhAddCounter(_query, queryString, NULL, &_processPrivateMemoryCounter);

	swprintf_s(queryString, 100, L"\\Process(%s)\\Pool NonPaged Bytes", _processName);
	PdhAddCounter(_query, queryString, NULL, &_processNonPagedMemoryCounter);

	PdhAddCounter(_query, L"\\Memory\\Available MBytes", NULL, &_availableMemoryCounter);

	PdhAddCounter(_query,  L"\\Memory\\Pool Nonpaged Bytes", NULL, &_nonPagedMemoryCounter);

	_processPrivateMemory = 0.0f;
	_processNonPagedMemory = 0.0f;
	_availableMemory = 0.0f;
	_nonPagedMemory = 0.0f;


	delete[] queryString;
}

void CPerformanceData::update(){

	PdhCollectQueryData(_query);

	PDH_FMT_COUNTERVALUE counterValue;

	PdhGetFormattedCounterValue(_processPrivateMemoryCounter, PDH_FMT_LARGE, NULL, &counterValue);
	_processPrivateMemory = counterValue.largeValue;
	
	PdhGetFormattedCounterValue(_processNonPagedMemoryCounter, PDH_FMT_LARGE, NULL, &counterValue);
	_processNonPagedMemory = counterValue.largeValue;

	PdhGetFormattedCounterValue(_availableMemoryCounter, PDH_FMT_LARGE, NULL, &counterValue);
	_availableMemory = counterValue.largeValue;

	PdhGetFormattedCounterValue(_nonPagedMemoryCounter, PDH_FMT_LARGE, NULL, &counterValue);
	_nonPagedMemory = counterValue.largeValue;

	CCpuUsage::updateCpuTime();

}

unsigned __int64 CPerformanceData::getAvailableMemory(){
	return _availableMemory;
}
unsigned __int64 CPerformanceData::getNonPagedMemory(){
	return _nonPagedMemory;
}
unsigned __int64 CPerformanceData::getProcessNonPagedMemory(){
	return _processNonPagedMemory;
}
unsigned __int64 CPerformanceData::getProcessPrivateMemory(){
	return _processPrivateMemory;
}