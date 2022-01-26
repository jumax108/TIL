#include "ChatServer.h"
#include "performanceServer.h"

SimpleProfiler sp;
CDump dump;
CLog logger;


int main(){

	CChatServer chatServer;
	HANDLE updateThread;
	chatServer.start(L"lib\\NetServer\\serverConfig.txt");
	updateThread = (HANDLE)_beginthreadex(nullptr, 0, CChatServer::update, (void*)&chatServer, 0, nullptr);

	CPerformanceServer performanceServer;
	performanceServer.start(L"lib\\LanServer\\serverConfig.txt");

	void* args[2] = {&performanceServer, &chatServer};
	performanceServer._updateThread = (HANDLE)_beginthreadex(nullptr, 0, CPerformanceServer::update, (void*)args, 0, nullptr);

	while(1);

	return 0;
}