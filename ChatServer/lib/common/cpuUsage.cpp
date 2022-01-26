#include "cpuUsage.h"

float CCpuUsage::processorTotal(){ return _processorTotal; }
float CCpuUsage::processorUser(){ return _processorUser; }
float CCpuUsage::processorKernel(){ return _processorKernel; }

float CCpuUsage::processTotal(){ return _processTotal; }
float CCpuUsage::processUser(){ return _processUser; }
float CCpuUsage::processKernel(){ return _processKernel; }

CCpuUsage::CCpuUsage(HANDLE process){

	if(process == INVALID_HANDLE_VALUE){
		process = GetCurrentProcess();
	}

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);	

	_processorNum		= systemInfo.dwNumberOfProcessors;

	_processorTotal		= 0;
	_processorUser		= 0;
	_processorKernel	= 0;

	_processTotal		= 0;
	_processUser		= 0;
	_processKernel		= 0;

	ZeroMemory(&_processorLastKernel,	sizeof(unsigned __int64));
	ZeroMemory(&_processorLastUser,		sizeof(unsigned __int64));
	ZeroMemory(&_processorLastIdle,		sizeof(unsigned __int64));

	ZeroMemory(&_processLastKernel,		sizeof(unsigned __int64));
	ZeroMemory(&_processLastUser,		sizeof(unsigned __int64));
	ZeroMemory(&_processLastTime,		sizeof(unsigned __int64));

	updateCpuTime();

}

void CCpuUsage::updateCpuTime(){

	unsigned __int64 idle;
	unsigned __int64 kernel;
	unsigned __int64 user;

	int getSysTimeError;
	if(GetSystemTimes((FILETIME*)&idle, (FILETIME*)&kernel, (FILETIME*)&user) == false){
		getSysTimeError = GetLastError();
		return ;
	}

	unsigned __int64 kernelDiff = kernel - _processorLastKernel;
	unsigned __int64 userDiff   = user	 - _processorLastUser;
	unsigned __int64 idleDiff   = idle	 - _processorLastIdle;

	unsigned __int64 total		= kernelDiff + userDiff;
	unsigned __int64 timeDiff;


	do {

		if( total == 0 ){

			_processorUser	 = 0;
			_processorKernel = 0;
			_processorTotal	 = 0;
			break;

		}
		
		_processorTotal	 = (float)(total - idleDiff) / total * 100.0f;
		_processorUser	 = (float)userDiff / total * 100.0f;
		_processorKernel = (float)(kernelDiff - idleDiff) / total * 100.0f;

	}while (false);
	
	_processorLastKernel = kernel;
	_processorLastUser   = user;
	_processorLastIdle	 = idle;

	unsigned __int64 noUse;
	unsigned __int64 nowTime;

	GetSystemTimeAsFileTime((FILETIME*)&nowTime);

	GetProcessTimes(_process, (FILETIME*)&noUse, (FILETIME*)&noUse, (FILETIME*)&kernel, (FILETIME*)&user);
	
	timeDiff	= nowTime	- _processLastTime;
	kernelDiff	= kernel	- _processLastKernel;
	userDiff	= user		- _processLastUser;

	total		= kernelDiff + userDiff;

	_processTotal	= (float)total		/ (float)_processorNum / (float)timeDiff * 100.0f;
	_processKernel	= (float)kernelDiff / (float)_processorNum / (float)timeDiff * 100.0f;
	_processUser	= (float)userDiff	/ (float)_processorNum / (float)timeDiff * 100.0f;


	_processLastTime	= nowTime;
	_processLastKernel	= kernel;
	_processLastUser	= user;

}