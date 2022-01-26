
#include "chatServer.h"
#include "performanceServer.h"

#define PerformanceDataUpdate(pstPerformanceData, value) \
			pstPerformanceData->sum += value; \
			if(pstPerformanceData->min > value){ \
				pstPerformanceData->min = value; \
			} \
			if(pstPerformanceData->max < value){ \
				pstPerformanceData->max = value; \
			} \

unsigned __stdcall CPerformanceServer::update(void* args){
	CPerformanceServer* server = (CPerformanceServer*)args;
	CChatServer* chatServer = (CChatServer*)args + 1;

	CLockFreeQueue<stMsg>* msgQue = &server->msgQue;
	std::unordered_set<unsigned __int64>* userList = &server->userList;

	stPerformanceData* recvTPS = &server->recvTPS;
	stPerformanceData* sendTPS = &server->sendTPS;
	stPerformanceData* acceptTPS = &server->acceptTPS;
	stPerformanceData* updateTPS = &server->updateTPS;
	stPerformanceData* acceptTotal = &server->acceptTotal;

	stPerformanceData* msgQueSize = &server->msgQueSize;
	stPerformanceData* msgQueCap = &server->msgQueCap;

	stPerformanceData* packetPoolCap = &server->packetPoolCap;
	stPerformanceData* iocpQueueSize = &server->iocpQueueSize;

	stPerformanceData* cpuUsage = &server->cpuUsage;
	stPerformanceData* processCpuKernelUsage = &server->processCpuKernelUsage;
	stPerformanceData* processCpuUserUsage = &server->processCpuUserUsage;

	stPerformanceData* chatRecvMsgNum = &server->chatRecvMsgNum;
	stPerformanceData* chatSendMsgNum = &server->chatSendMsgNum;

	stPerformanceData* sendBytes = &server->sendBytes;
	stPerformanceData* recvBytes = &server->recvBytes;

	stPerformanceData* sessionNum = &server->sessionNum;
	stPerformanceData* userNum = &server->userNum;

	stPerformanceData* processPrivateMemory = &server->processPrivateMemory;
	stPerformanceData* nonPagedPoolMemory = &server->nonPagedPoolMemory;

	int cnt = 0;

	for(;;){

		int queSize = msgQue->getSize();
		while(queSize > 0){

			stMsg msg;
			msgQue->pop(&msg);

			switch(msg.type){
				case 1:{
				
					userList->insert(msg.sessionID);

				} break;
				case 2:{
				
					userList->erase(msg.sessionID);

				} break;
			
			}
		}

		/////////////////////////////////////////////////
		// collect performance data
		/////////////////////////////////////////////////
		
		{
			unsigned __int64 recvTPSValue = chatServer->getRecvTPS();
			PerformanceDataUpdate(recvTPS, recvTPSValue);
		}

		{
			unsigned __int64 sendTPSValue = chatServer->getSendTPS();
			PerformanceDataUpdate(sendTPS, sendTPSValue);
		}

		{
			unsigned __int64 acceptTPSValue = chatServer->getAcceptTPS();
			PerformanceDataUpdate(acceptTPS, acceptTPSValue);
		}

		{
			unsigned __int64 updateTPSValue = chatServer->getUpdateTPS();
			PerformanceDataUpdate(updateTPS, updateTPSValue);
		}
		
		{
			unsigned __int64 acceptTotalValue = chatServer->getUpdateTPS();
			PerformanceDataUpdate(acceptTotal, acceptTotalValue);
		}

		{
			unsigned __int64 sessionNumValue = chatServer->getSessionCount();
			PerformanceDataUpdate(sessionNum, sessionNumValue);
		}

		{
			unsigned __int64 msgQueCapValue = chatServer->msgQueue.getCapacity();
			PerformanceDataUpdate(msgQueCap, msgQueCapValue);
		}

		{
			unsigned __int64 msgQueSizeValue = chatServer->msgQueue.getSize();
			PerformanceDataUpdate(msgQueSize, msgQueSizeValue);
		}

		{
			unsigned __int64 packetPoolCapValue = CPacketPtr::getPacketPoolUsage();
			PerformanceDataUpdate(packetPoolCap, packetPoolCapValue);
		}
		
		{
			unsigned __int64 iocpQueueSizeValue = chatServer->_iocpQueueSize;
			PerformanceDataUpdate(iocpQueueSize, iocpQueueSizeValue);
		}

		{
			unsigned __int64 cpuUsageValue = chatServer->_performanceData.processorTotal();
			PerformanceDataUpdate(cpuUsage, cpuUsageValue);
		}

		{
			unsigned __int64 processCpuKernelUsageValue = chatServer->_performanceData.processKernel();
			PerformanceDataUpdate(processCpuKernelUsage, processCpuKernelUsageValue);
		}

		{
			unsigned __int64 processCpuUserUsageValue = chatServer->_performanceData.processUser();
			PerformanceDataUpdate(processCpuUserUsage, processCpuUserUsageValue);
		}

		{
			unsigned __int64 chatRecvMsgNumValue = chatServer->_chatMsgRecvTPS;
			PerformanceDataUpdate(chatRecvMsgNum, chatRecvMsgNumValue);
		}

		{
			unsigned __int64 chatSendMsgNumValue = chatServer->_chatMsgSendTPS;
			PerformanceDataUpdate(chatSendMsgNum, chatSendMsgNumValue);
		}

		{
			unsigned __int64 sendBytesValue = chatServer->_sendBytes;
			PerformanceDataUpdate(sendBytes, sendBytesValue);
		}

		{
			unsigned __int64 recvBytesValue = chatServer->_recvBytes;
			PerformanceDataUpdate(recvBytes, recvBytesValue);
		}

		{
			unsigned __int64 userNumValue = chatServer->userList.size();
			PerformanceDataUpdate(userNum, userNumValue);
		}
		/////////////////////////////////////////////////

		cnt += 1;
		if(cnt == 5){
			cnt = 0;
			
			/////////////////////////////////////////////////
			// send logic
			/////////////////////////////////////////////////

			/////////////////////////////////////////////////
		}

		Sleep(1000);

	}

	return 0;
}