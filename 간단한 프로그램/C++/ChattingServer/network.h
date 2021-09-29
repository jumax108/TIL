#pragma once
class CNetwork {


public:

	bool init();
	SOCKET acceptConnect(SOCKADDR_IN* clientAddr);


private:

	SOCKET listenSocket;

};