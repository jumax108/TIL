
#include "chatServer.h"

static constexpr int MSG_CLIENT_JOIN		= 1;
static constexpr int MSG_CLIENT_LEAVE		= 2;
static constexpr int MSG_RECV				= 3;
static constexpr int MSG_HeartBeat_LogIn	= 4;
static constexpr int MSG_HeartBeat_NotLogIn = 5;

static constexpr int TimeOut_LogIn    = 40000; // 40sec
static constexpr int TimeOut_NotLogIn = 5000; // 5sec

//#define TIME_OUT

//static SimpleProfiler sp;

CChatServer::CChatServer(): _userFreeList(HeapCreate(0,0,0), false, false), _msgFreeList(HeapCreate(0,0,0), false, false),
							_performanceData(L"ChatServer") {

	msgEvent = CreateEvent(nullptr, true, false, nullptr);

	updateCnt = 0;

	for(int sectorY = 0 ; sectorY < SECTOR_HEIGHT; ++sectorY){
		for(int sectorX = 0; sectorX < SECTOR_WIDTH; ++sectorX){
			
			stSector* sector = &sectors[sectorY][sectorX];

			int aroundSectorNum = 0;

			//////////////////////////////////////////////////////////
			// border Level이 1이면 테두리, 2이면 꼭지점입니다.
			//////////////////////////////////////////////////////////
			int borderLevel = 0;
			if(sectorY == 0 || sectorY == SECTOR_HEIGHT - 1){
				borderLevel += 1;
			}
			if(sectorX == 0 || sectorX == SECTOR_WIDTH - 1){
				borderLevel += 1;
			}
			//////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////
			// border level을 통해 주변 sector의 수를 알아냅니다.
			//////////////////////////////////////////////////////////
			switch(borderLevel){
				case 0:{
					aroundSectorNum = 8;
				}
				break;
				case 1:{
					aroundSectorNum = 5;
				}
				break;
				case 2:{
					aroundSectorNum = 3;
				}
				break;

			}
			//////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////
			// 주변 섹터의 주소를 저장합니다.
			//////////////////////////////////////////////////////////
			sector->aroundSectorNum = aroundSectorNum;
			sector->aroundSector = (stSector**)malloc(sizeof(stSector*) * aroundSectorNum);

			int sectorCnt = 0;
			for(int otherSectorY = sectorY - 1; otherSectorY <= sectorY + 1; ++otherSectorY){

				if(otherSectorY == -1 || otherSectorY == SECTOR_HEIGHT){
					continue;
				}

				for(int otherSectorX = sectorX - 1; otherSectorX <= sectorX + 1; ++otherSectorX){
				
					if(otherSectorX == -1 || otherSectorX == SECTOR_WIDTH){
						continue;
					}

					if(otherSectorX == sectorX && otherSectorY == sectorY){
						continue;
					}
					
					sector->aroundSector[sectorCnt] = &sectors[otherSectorY][otherSectorX];

					sectorCnt += 1;

				}
			}

			printf("%d ",sectorCnt);

			//////////////////////////////////////////////////////////

		}
		printf("\n");
	}

}

bool CChatServer::onConnectRequest(unsigned int ip, unsigned short port){
	return true;
}

void CChatServer::onClientJoin(unsigned int ip, unsigned short port, unsigned __int64 sessionID){

	stMsg* msg = _msgFreeList.allocObject();

	msg->sessionID = sessionID;
	msg->type = MSG_CLIENT_JOIN;

	msgQueue.push(msg);
//	SetEvent(msgEvent);

}

void CChatServer::onClientLeave(unsigned __int64 sessionID){

	stMsg* msg = _msgFreeList.allocObject();

	msg->sessionID = sessionID;
	msg->type = MSG_CLIENT_LEAVE;

	msgQueue.push(msg);
//	SetEvent(msgEvent);

}

void CChatServer::onRecv(unsigned __int64 sessionID, CPacketPtr packet){

	stMsg* msg = _msgFreeList.allocObject();

	msg->sessionID = sessionID;
	msg->packet = packet;
	msg->type = MSG_RECV;

	packet.incRef();

	msgQueue.push(msg);
//	SetEvent(msgEvent);

}

void CChatServer::onSend(unsigned __int64 sessionID, int sendSize){


}

void CChatServer::onError(int errorCode, const wchar_t* errorMsg){

	if(errorCode == 2){
		return ;
	}

	wprintf(L"errorCode: %d\nerrorMsg: %s\n", errorCode, errorMsg);

}

unsigned __stdcall CChatServer::update(void* args){
	
	CChatServer* server = (CChatServer*)args;

	HANDLE msgEvent = server->msgEvent;
	CLockFreeQueue<stMsg*>* msgQueue = &server->msgQueue;

	std::unordered_map<unsigned __int64, stUser*>* userList = &server->userList;
	std::unordered_map<unsigned __int64, stUser*>* notLogInUserList = &server->notLogInUserList;
	CObjectFreeList<stUser>* userFreeList = &server->_userFreeList;

	server->tpsPrintThread = (HANDLE)_beginthreadex(nullptr, 0, tpsFunc, (void*)server, 0, nullptr);
	
	HANDLE threadHandle = (HANDLE)__threadhandle();

	SetProcessAffinityMask(threadHandle, 1);
	SetThreadPriority(threadHandle, THREAD_PRIORITY_HIGHEST);

	// 타임아웃
	#ifdef TIME_OUT
		server->logInHeartBeatSignalThread		= (HANDLE)_beginthreadex(nullptr, 0, logInHearthBeatSignalFunc, (void*)server, 0, nullptr);
		server->notLogInHeartBeatSignalThread	= (HANDLE)_beginthreadex(nullptr, 0, notLogInHearthBeatSignalFunc, (void*)server, 0, nullptr);
	#endif

	for(;;){

		int msgQueueSize = msgQueue->getSize();
		while(msgQueueSize > 0){// && server->updateCnt < 15000){

			stMsg* msg;

			msgQueue->pop(&msg);
			msgQueueSize -= 1;

			switch(msg->type){

				case MSG_CLIENT_JOIN: {

					stUser* user = userFreeList->allocObject();
					user->lastSendTime = GetTickCount64();
					
					notLogInUserList->insert( std::pair<unsigned __int64, stUser*>(msg->sessionID, user) );


				}
				break;

				case MSG_CLIENT_LEAVE: {
				
					std::unordered_map<unsigned __int64, stUser*>::iterator iter = userList->find(msg->sessionID);
					if(iter != userList->end()){

						stUser* user = iter->second;
						if(user->sector != nullptr){
							user->sector->userList.erase(iter->first);
						}

						userFreeList->freeObject(user);
						userList->erase(msg->sessionID);

						break;
					}

					iter = notLogInUserList->find(msg->sessionID);
					if(iter != notLogInUserList->end()){
						userFreeList->freeObject(iter->second);
						notLogInUserList->erase(msg->sessionID);

						break;
					}

					CDump::crash();

				}
				break;

				case MSG_RECV: {

					unsigned short type;
					msg->packet >> type;

					switch(type){

						case REQ_LogIn: {
						
							std::unordered_map<unsigned __int64, stUser*>::iterator findUserIter;
							findUserIter = notLogInUserList->find(msg->sessionID);

							unsigned __int64 sessionID = findUserIter->first;
							stUser* user = findUserIter->second;

							notLogInUserList->erase(sessionID);

							msg->packet >> user->accountNo;
							msg->packet.popData(sizeof(stREQ_LogIn::ID), (unsigned char*)user->id);
							msg->packet.popData(sizeof(stREQ_LogIn::nickName), (unsigned char*)user->nickName);
							msg->packet.popData(sizeof(stREQ_LogIn::sessionKey), (unsigned char*)user->sessionKey);

							user->sector = nullptr;
							userList->insert( std::pair<unsigned __int64, stUser*>(msg->sessionID, user) );

							user->lastSendTime = GetTickCount64();

							CPacketPtrNet packet;

							packet << RES_LogIn;
							packet << (unsigned char)1;
							packet << user->accountNo;

							server->sendPacketPostSend(sessionID, packet);
							packet.decRef();

						}
						break;

						case REQ_Sector_Move: {
						
							std::unordered_map<unsigned __int64, stUser*>::iterator findUserIter;
							findUserIter = userList->find(msg->sessionID);
														
							unsigned __int64 sessionID = findUserIter->first;
							stUser* user = findUserIter->second;

							__int64 accountNo;
							msg->packet >> accountNo;

							if(user->sector != nullptr){
								user->sector->userList.erase(sessionID);
							}
							
							unsigned short sectorX;
							unsigned short sectorY;

							msg->packet >> sectorX;
							msg->packet >> sectorY;

							stSector* sector = &server->sectors[sectorY][sectorX];
							user->sector = sector;
							sector->userList.insert( std::pair<unsigned __int64, stUser*>(sessionID, user) );
												
							CPacketPtrNet packet;

							packet << RES_Sector_Move;
							packet << user->accountNo;
							packet << sectorX;
							packet << sectorY;
							
							server->sendPacketPostSend(sessionID, packet);
							packet.decRef();

							user->lastSendTime = GetTickCount64();
							
						}
						break;

						case REQ_Chat_Message: {
						
							server->_chatMsgRecvNum += 1;

							std::unordered_map<unsigned __int64, stUser*>::iterator findUserIter;
							findUserIter = userList->find(msg->sessionID);
														
							unsigned __int64 sessionID = findUserIter->first;
							stUser* user = findUserIter->second;

							__int64 accountNo;
							unsigned short msgLen;
							msg->packet >> accountNo;
							msg->packet >> msgLen;
														
							CPacketPtrNet packet;

							packet << RES_Chat_Message;
							packet << user->accountNo;
							packet.putData(sizeof(stRES_Chat_Message::id), (unsigned char*)user->id);
							packet.putData(sizeof(stRES_Chat_Message::nickName), (unsigned char*)user->nickName);
							packet << msgLen;
							packet.putData(msgLen, (unsigned char*)msg->packet.getFrontPtr());
				
							//////////////////////////////////////////////////////////////////////////////////////////
							// 유저가 속한 섹터의 주변 섹터에 속한 모든 유저에게 패킷 전송
							//////////////////////////////////////////////////////////////////////////////////////////
							int aroundSectorNum = user->sector->aroundSectorNum;
							stSector** aroundSector = user->sector->aroundSector;

							if(msgQueue->getSize() >= 1000){
								for(int sectorCnt = 0; sectorCnt < aroundSectorNum; ++sectorCnt){
									stSector* sector = aroundSector[sectorCnt];
									std::unordered_map<unsigned __int64, stUser*>* userList = &sector->userList;
									std::unordered_map<unsigned __int64, stUser*>::iterator userIter = userList->begin();
									std::unordered_map<unsigned __int64, stUser*>::iterator userEndIter = userList->end();
									for(; userIter != userEndIter; ++userIter){
										server->sendPacketPostIOCP(userIter->first, packet);
									}
									server->_chatMsgSendNum += userList->size();
								}
							} else {
								for(int sectorCnt = 0; sectorCnt < aroundSectorNum; ++sectorCnt){
									stSector* sector = aroundSector[sectorCnt];
									std::unordered_map<unsigned __int64, stUser*>* userList = &sector->userList;
									std::unordered_map<unsigned __int64, stUser*>::iterator userIter = userList->begin();
									std::unordered_map<unsigned __int64, stUser*>::iterator userEndIter = userList->end();
									for(; userIter != userEndIter; ++userIter){
										server->sendPacketPostSend(userIter->first, packet);
									}
									server->_chatMsgSendNum += userList->size();
								}
							}
							//////////////////////////////////////////////////////////////////////////////////////////
							
							//////////////////////////////////////////////////////////////////////////////////////////
							// 유저가 속한 섹터의 모든 유저에게 패킷 전송
							//////////////////////////////////////////////////////////////////////////////////////////
							stSector* sector = user->sector;
							std::unordered_map<unsigned __int64, stUser*>* userList = &sector->userList;
							std::unordered_map<unsigned __int64, stUser*>::iterator userIter = userList->begin();
							std::unordered_map<unsigned __int64, stUser*>::iterator userEndIter = userList->end();
							if(msgQueue->getSize() >= 1000){
								for(; userIter != userEndIter; ++userIter){
									server->sendPacketPostIOCP(userIter->first, packet);
								}
								server->_chatMsgSendNum += userList->size();
							} else {
								for(; userIter != userEndIter; ++userIter){
									server->sendPacketPostSend(userIter->first, packet);
								}
								server->_chatMsgSendNum += userList->size();
							}
							//////////////////////////////////////////////////////////////////////////////////////////

							packet.decRef();
							
						}
						break;

						// not use
						case 2:
						case 4:
						case 6:
							CDump::crash();
							break;
									
					}
					
					msg->packet.decRef();
				}
				break;
				case MSG_HeartBeat_LogIn:{
					
					unsigned __int64 nowTime = GetTickCount64();

					for(std::unordered_map<unsigned __int64, stUser*>::iterator userIter = userList->begin(); userIter != userList->end(); ++userIter){
						stUser* user = userIter->second;
						if(nowTime - user->lastSendTime >= TimeOut_LogIn){
							server->disconnect(userIter->first);
							userList->erase(userIter);
						}
					}

				}
				break;

				case MSG_HeartBeat_NotLogIn:{
					
					unsigned __int64 nowTime = GetTickCount64();

					for(std::unordered_map<unsigned __int64, stUser*>::iterator userIter = notLogInUserList->begin(); userIter != notLogInUserList->end(); ++userIter){
						stUser* user = userIter->second;
						if(nowTime - user->lastSendTime >= TimeOut_NotLogIn){
							server->disconnect(userIter->first);
							notLogInUserList->erase(userIter);
						}
					}

				}
				break;

				case 6:{
					// not use;
					CDump::crash();
				} break;

			}
			server->updateCnt += 1;
			server->_msgFreeList.freeObject(msg);
		}
		
		Sleep(0);
	}

	return 0;

}


unsigned __stdcall CChatServer::tpsFunc(void* args){

	CChatServer* server = (CChatServer*)args;

	CPerformanceData* performanceData = &server->_performanceData;
	
	SetThreadPriority((HANDLE)__threadhandle(), THREAD_PRIORITY_HIGHEST);
	for(;;){
	//	return 0;
		wprintf(L"    RECV TPS       : %I64d\n", server->getRecvTPS());
		wprintf(L"    SEND TPS       : %I64d\n", server->getSendTPS());
		wprintf(L"  ACCEPT TPS       : %I64d\n", server->getAcceptTPS());
		wprintf(L"  Update TPS       : %I64d\n\n", server->updateCnt);

		server->updateTPS = server->updateCnt;
		server->updateCnt = 0;

		wprintf(L"ACCEPT Total       : %I64d\n", server->getAcceptTotal());
		wprintf(L"Session Cnt        : %I64d\n\n", server->getSessionCount());


		wprintf(L"Msg Queue Cap      : %I64d\n", server->msgQueue.getCapacity());
		wprintf(L"Msg Queue Size     : %I64d\n\n", server->msgQueue.getSize());
	
		wprintf(L"Packet Pool Cap    : %I64d\n", CPacketPtr::getPacketPoolUsage());
		wprintf(L"Send Queue Max Cap : %d\n", server->_maxSendQueCap);
		wprintf(L"IOCP Queue Size    : %d\n\n", server->_iocpQueueSize);

		performanceData->update();
		
		wprintf(L"Total CPU Usage: %lf%%\n", performanceData->processorTotal());

		wprintf(L"Proc Kernel CPU Usage: %lf%%\n", performanceData->processKernel());
		wprintf(L"Proc User CPU Usage: %lf%%\n\n", performanceData->processUser());

		wprintf(L"Process Private Memory: %I64d MBytes\n", performanceData->getProcessPrivateMemory() / 1024 / 1024);
		wprintf(L"Non Paged Memory: %I64d MBytes\n\n", performanceData->getNonPagedMemory() / 1024 / 1024);
		
		wprintf(L"Send Bytes: %I64d\n", server->_sendBytes);
		wprintf(L"Recv Bytes: %I64d\n\n", server->_recvBytes);

		wprintf(L"Chat Recv Msg Num: %I64d\n", server->_chatMsgRecvNum);
		wprintf(L"Chat Send Msg Num: %I64d\n\n", server->_chatMsgSendNum);

		server->_chatMsgRecvTPS = server->_chatMsgRecvNum;
		server->_chatMsgSendTPS = server->_chatMsgSendTPS;

		server->_chatMsgRecvNum = 0;
		server->_chatMsgSendNum = 0;

		server->updateUserDispersion();

		for(int yCnt = 0; yCnt < 5 ; ++yCnt){
			for(int xCnt = 0; xCnt < 5; ++xCnt){
				wprintf(L"max: %4I64d ", server->_maxUserDispersion[yCnt][xCnt]);
			}
			wprintf(L"\n");
			for(int xCnt = 0; xCnt < 5; ++xCnt){
				wprintf(L"sum: %4I64d ", server->_numUserDispersion[yCnt][xCnt]);
			}
			wprintf(L"\n");
		}

		Sleep(1000);
	}

	return 0;

}

unsigned __stdcall CChatServer::logInHearthBeatSignalFunc(void* args){

	CChatServer* server = (CChatServer*)args;

	for(;;){

		Sleep(TimeOut_LogIn / 2);
		
		stMsg* msg = server->_msgFreeList.allocObject();
		msg->type = MSG_HeartBeat_LogIn;

		server->msgQueue.push(msg);

	}

	return 0;

}

void CChatServer::updateUserDispersion(){
	
	for(int yCnt = 0; yCnt < 5 ; ++yCnt){
		for(int xCnt = 0; xCnt < 5; ++xCnt){
						
			int sectorYEnd = (yCnt+1) * 10;
			int sectorXStart = xCnt * 10;
			int sectorXEnd = sectorXStart + 10;

			__int64 maxNum = 0;
			__int64 userSum = 0;

			for(int sectorYCnt = yCnt * 10; sectorYCnt < sectorYEnd; ++sectorYCnt){
				for(int sectorXCnt = sectorXStart; sectorXCnt < sectorXEnd; ++sectorXCnt){
					int sectorUserNum = sectors[sectorYCnt][sectorXCnt].userList.size();
					if(maxNum < sectorUserNum){
						maxNum = sectorUserNum;
					}
					userSum += sectorUserNum;
				}
			}

			_maxUserDispersion[yCnt][xCnt] = maxNum;
			_numUserDispersion[yCnt][xCnt] = userSum;

		}
	}

}

unsigned __stdcall CChatServer::notLogInHearthBeatSignalFunc(void* args){

	CChatServer* server = (CChatServer*)args;

	for(;;){

		Sleep(TimeOut_NotLogIn / 2);
		
		stMsg* msg = server->_msgFreeList.allocObject();
		msg->type = MSG_HeartBeat_NotLogIn;

		server->msgQueue.push(msg);

	}

	return 0;

}